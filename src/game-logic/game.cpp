#include "game.hpp"

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"
#include "net-comms/out-facing/common_cs.hpp"
#include "net-comms/in_convertors.hpp"

#include "ui/screens.hpp"

#include <thread>
#include <iostream>

namespace game {
    namespace {
        /**
         * @brief Get the coordinates of the sunken ship
         * 
         * @param board Board to search the ship in.
         * @param pos Positions of the last hit.
         * @return Coordinates of the sunken ship. 
        */
        data::ship_coords_t GetSunkShipCoords(const data::board_t& board, data::position_t pos) {
            auto [row, col] = pos;

            auto height = board.size();
            auto width = board[0].size();

            constexpr std::array rowOffsets = {-1, 0, 0, 1};
            constexpr std::array colOffsets = { 0,-1, 1, 0};
            
            data::ship_coords_t shipCoords;

            auto isThereShipCheck = [&](data::idx_t row, data::idx_t col){
                return row < height && col < width && board[row][col];
            };

            for (std::size_t i = 0; i < rowOffsets.size(); ++i) {
                // there is a ship in this direction
                auto rowOffset = rowOffsets[i];
                auto colOffset = colOffsets[i];
                auto rowCheck = row+rowOffset;
                auto colCheck = col+colOffset;
                
                if (isThereShipCheck(rowCheck, colCheck)) {
                    // pursue that direction
                    while(isThereShipCheck(rowCheck, colCheck)) {
                        rowCheck += rowOffset;
                        colCheck += colOffset;
                    }
                    // go one back to the last good position
                    rowCheck -= rowOffset;
                    colCheck -= colOffset;
                    // the last good position is now in col/row Check variable
                    // go backwards and log all the positions
                    while (isThereShipCheck(rowCheck, colCheck)) {
                        shipCoords.insert({rowCheck, colCheck});
                        rowCheck -= rowOffset;
                        colCheck -= colOffset;
                    }
                    return shipCoords;
                }
            }
            return {pos};
        }

        /**
         * @brief Get coordinates around the ship.
         * 
         * @param shipCoords To get the boarder around.
         * @param boardSize Size of the board to check against.
         * @return Coordinates of the boarder around the ship.
        */
        data::coords_t GetShipBoarderCoords(const data::ship_coords_t& shipCoords, std::size_t boardSize) {
            constexpr std::array rowOffsets = {-1, -1, -1,  0,  0,  1, 1, 1};
            constexpr std::array colOffsets = {-1,  0,  1, -1,  1, -1, 0, 1};

            data::coords_t boarderCoords;

            for (auto [row, col] : shipCoords) {
                for (std::size_t i = 0; i < rowOffsets.size(); ++i) {
                    std::size_t currRow = row + rowOffsets[i];
                    std::size_t currCol = col + colOffsets[i];
                    // check the for out of range
                    if (currRow > boardSize || currCol > boardSize) {
                        continue;
                    }

                    data::position_t currPos = {currRow, currCol};
                    if (shipCoords.contains(currPos)) continue;
                    boarderCoords.push_back(std::move(currPos));
                }
            }
            return boarderCoords;
        }


        /**
         * @brief Gets the number of ships from the rules given.
         * @param rules Rules of the game.
         * @return Number of ships.
        */
        data::num_of_ships_t GetNumberOfShipsFromRules(const data::fleet_rules_t& rules) {
            data::num_of_ships_t retNumOfShips = 0;
            for (const auto& [_, numOfShips] : rules) {
                retNumOfShips += numOfShips;
            }
            return retNumOfShips;
        }

        data::fleet_rules_t Rules {
            {5, 1}, // one ship of size 5
            {4, 1}, // one ship of size 4
            {3, 1}, // one ship of size 4
            {2, 2}, // two ships of size 2
            {1, 2}, // two ships of size 1
        };
        // TODO: count number of ships from the rules
        data::num_of_ships_t DefaultNumOfShips = GetNumberOfShipsFromRules(Rules);
    }

    void Start() {
        // initial menu
        auto res = ui::CreateMenu();
        // connect players
        std::unique_ptr<net_comms::CommonCS> transiever;
        if (res.AreWeServer) {
            std::cout << "Waiting for client..." << std::endl;
            transiever = std::make_unique<net_comms::Server>(res.PortNumber);
        } else {
            transiever = std::make_unique<net_comms::Client>(res.IpAddr, res.PortNumber);
        }
        std::cout << "Connected" << std::endl;


        data::board_t emptyBoard(10, data::board_t::value_type(10, false));
        data::board_t enemyBoard = emptyBoard;

        auto [ourBoard, ourShips] = ui::CreatePlacementMenu(std::move(emptyBoard), Rules);

        int numOfEnemyShips = DefaultNumOfShips;
        int numOfOurShips = DefaultNumOfShips;

        // start the game with required data
        bool wePlay = res.AreWeServer;
        while(numOfEnemyShips != 0 && numOfOurShips != 0) {
            if (wePlay) {
                std::cout << "We are playing" << std::endl;
                // get move from terminal
                data::position_t pos = ui::GameScreen(ourBoard, enemyBoard);
                auto [x, y] = pos;

                transiever->Send(net_comms::PositionToData(pos));
                auto answer = net_comms::DataToHitStatus(transiever->Recieve());

                switch (answer) {
                case data::HitStatus::Sunken: 
                    enemyBoard[x][y] = true;
                    --numOfEnemyShips;
                    // mark the around of ship dead
                    // TODO: change ui board not logic board
                    // {
                    // auto sunkShipCoords = GetSunkShipCoords(enemyBoard, pos);
                    // auto sunkShipBorder = GetShipBoarderCoords(sunkShipCoords, enemyBoard.size());
                    // for (auto [row, col] : sunkShipBorder) {
                    //     enemyBoard[row][col] = true;
                    // }
                    // }
                    std::cout << "You have sunk the enemy ship!" << std::endl;
                    break;
                case data::HitStatus::Hit:
                    enemyBoard[x][y] = true;
                    std::cout << "You have hit the enemy ship!" << std::endl;
                    break;
                case data::HitStatus::Miss:
                    std::cout << "You have missed :(" << std::endl;
                    break;
                }
            } else {
                std::cout << "Enemy is playing" << std::endl;
                // get position from the opponent
                auto pos = net_comms::DataToPosition(transiever->Recieve());
                auto [x, y] = pos;

                // check if it hit anything
                data::HitStatus hitStatus = data::HitStatus::Miss;
                if (ourBoard[x][y]) {
                    hitStatus = data::HitStatus::Hit;
                    // remove the target from board
                    ourBoard[x][y] = false;

                    std::cout << "Enemy hit one of your ships!" << std::endl;

                    // update our ships
                    for(auto it = ourShips.begin(); it != ourShips.end(); ++it) {
                        auto& positions = *it;
                        if (positions.contains(pos)) {
                            positions.erase(pos);
                            if (positions.empty()) {
                                --numOfOurShips;
                                hitStatus = data::HitStatus::Sunken;
                                ourShips.erase(it);
                                std::cout << "The hit was a sunk! You are one ship short now." << std::endl;
                            }
                            break; // this break is important since we could have invalidated the iterator!
                        }
                    }
                } else {
                    std::cout << "The enemy have missed your ships!" << std::endl;
                }
                // signal to the opponent the status of the hit
                transiever->Send(net_comms::HitStatusToData(hitStatus));
            }
            wePlay = !wePlay;
        }

        // did we win?
        if (numOfEnemyShips == 0) {
            std::cout << "You have WON!" << std::endl;
            return;
        }
        // did ve lose?
        if (numOfOurShips == 0) {
            std::cout << "You have lost the game, better luck next time!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return;
        }
    }
}