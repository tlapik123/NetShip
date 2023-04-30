#include "game.hpp"

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"
#include "net-comms/out-facing/common_cs.hpp"
#include "net-comms/in_convertors.hpp"

#include "ui/screens.hpp"

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
            constexpr std::array rowOffsets = {-1, -1, -1, 0, 0, 1, 1, 1};
            constexpr std::array colOffsets = {-1, 0, 1, -1, 1, -1, 0, 1};

            data::coords_t boarderCoords;

            for (auto [row, col] : shipCoords) {
                for (std::size_t i = 0; i < rowOffsets.size(); ++i) {
                    auto currRow = row + rowOffsets[i];
                    auto currCol = col + colOffsets[i];
                    // check the for out of range
                    if (currRow < 0 || currRow > boardSize ||
                        currCol < 0 || currCol > boardSize) {
                        continue;
                    }

                    data::position_t currPos = {currRow, currCol};
                    if (shipCoords.contains(currPos)) continue;
                    boarderCoords.push_back(std::move(currPos));
                }
            }
            return boarderCoords;
        }

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

        data::fleet_rules_t rules = {
            {5, 1}, // one ship of size 5
            {4, 1}, // one ship of size 4
            {3, 1}, // one ship of size 4
            {2, 2}, // two ships of size 2
            {1, 2}, // two ships of size 1
        };
        constexpr int DefaultNumOfShips = 7; // TODO: hardcoded for no

        data::board_t enemyBoard = emptyBoard;

        auto [ourBoard, ourShips] = ui::CreatePlacementMenu(std::move(emptyBoard), rules);

        int numOfEnemyShips = DefaultNumOfShips;
        int numOfOurShips = DefaultNumOfShips;

        // start the game with required data
        bool wePlay = res.AreWeServer;
        while(true) {
            if (wePlay) {
                std::cout << "We are playing rn" << std::endl;
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
                    {
                    auto sunkShipCoords = GetSunkShipCoords(enemyBoard, pos);
                    auto sunkShipBorder = GetShipBoarderCoords(sunkShipCoords, enemyBoard.size());
                    for (auto [row, col] : sunkShipCoords) {
                        enemyBoard[row][col] = true;
                    }
                    for (auto [row, col] : sunkShipBorder) {
                        enemyBoard[row][col] = true;
                    }
                    }
                    // TODO some animation

                    // did we win?
                    if (numOfEnemyShips == 0) {
                        // We won! do something on screen
                        return;
                    }
                    break;
                case data::HitStatus::Hit:
                    enemyBoard[x][y] = true;
                    // TODO some animation
                    break;
                case data::HitStatus::Miss:
                    // TODO some animation
                    break;
                }
                std::cout << "number of enemy ships: " << numOfEnemyShips << std::endl;
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
                    // update our own screen with the visual


                    bool sunken = false;
                    // update our ships
                    for(auto& positions : ourShips) {
                        if (positions.contains(pos)) {
                            positions.erase(pos);
                            if (positions.empty()) {
                                sunken = true;
                                --numOfOurShips;
                                hitStatus = data::HitStatus::Sunken;
                                // TODO: fix the erase on the list
                                // ourShips.erase(position);
                                // update our own screen with the visual
                                auto sunkShipCoords = GetSunkShipCoords(enemyBoard, pos);
                                auto sunkShipBorder = GetShipBoarderCoords(sunkShipCoords, enemyBoard.size());

                                break; // this break is important since we have invalidated the iterator!
                            }
                        }
                    }

                    // did ve lose?
                    if (numOfOurShips == 0) {
                        // we lost
                        // update our own screen with the visual
                        return;
                    }
                } else {
                    // show miss on the board
                }
                // signal to the opponent the status of the hit
                transiever->Send(net_comms::HitStatusToData(hitStatus));
            }
            wePlay = !wePlay;
        }
    }
}