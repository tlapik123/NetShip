#include "game.hpp"

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"
#include "net-comms/out-facing/common_cs.hpp"
#include "net-comms/in_convertors.hpp"

#include "ui/screens.hpp"

#include <thread>
#include <iostream>

namespace game {
    using namespace data;
    namespace {
        /**
         * @brief Get the coordinates of the sunken ship
         * 
         * @param board Board to search the ship in.
         * @param pos Positions of the last hit.
         * @return Coordinates of the sunken ship. 
        */
        ship_coords_t GetSunkShipCoords(const board_t& board, position_t pos) {
            auto [row, col] = pos;

            auto height = board.size();
            auto width = board[0].size();

            constexpr std::array rowOffsets = {-1, 0, 0, 1};
            constexpr std::array colOffsets = { 0,-1, 1, 0};
            
            ship_coords_t shipCoords;

            auto isThereShipCheck = [&](idx_t row, idx_t col){
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
        coords_t GetShipBoarderCoords(const ship_coords_t& shipCoords, std::size_t boardSize) {
            constexpr std::array rowOffsets = {-1, -1, -1,  0,  0,  1, 1, 1};
            constexpr std::array colOffsets = {-1,  0,  1, -1,  1, -1, 0, 1};

            coords_t boarderCoords;

            for (auto [row, col] : shipCoords) {
                for (std::size_t i = 0; i < rowOffsets.size(); ++i) {
                    std::size_t currRow = row + rowOffsets[i];
                    std::size_t currCol = col + colOffsets[i];
                    // check the for out of range
                    if (currRow > boardSize || currCol > boardSize) {
                        continue;
                    }

                    position_t currPos = {currRow, currCol};
                    if (shipCoords.contains(currPos)) continue;
                    boarderCoords.push_back(std::move(currPos));
                }
            }
            return boarderCoords;
        }


        /**
         * @brief Convert the logic board to the display board to visualize it.
         * 
         * @param board logic board to convert
         * @return Display board with true on logic board as Ship on it.
        */
        display_board_t DisplayBoardFromBoard(const board_t& board) {
            auto height = board.size();
            auto width = board[0].size();

            display_board_t retDisplBoard(
                height, 
                display_board_t::value_type(
                    width, 
                    DisplayType::Empty
                )
            );

            for(std::size_t i = 0; i < height; ++i) {
                for(std::size_t j = 0; j < width; ++j) {
                    if(board[i][j]) retDisplBoard[i][j] = DisplayType::Ship;
                }
            }
            return retDisplBoard;
        }
 
        /**
         * @brief Gets the number of ships from the rules given.
         * @param rules Rules of the game.
         * @return Number of ships.
        */
        num_of_ships_t GetNumberOfShipsFromRules(const fleet_rules_t& rules) {
            num_of_ships_t retNumOfShips = 0;
            for (const auto& [_, numOfShips] : rules) {
                retNumOfShips += numOfShips;
            }
            return retNumOfShips;
        }

        /**
         * @brief Mark the ship at pos as dead. Change the display board with this information.
         * 
         * @param board Board to search the ship in.
         * @param pos Position of the last hit.
         * @param outDisplBoard Display board to change.
        */
        void MarkDeadShip(const board_t board, position_t pos, display_board_t& outDisplBoard) {
            auto sunkShipCoords = GetSunkShipCoords(board, pos);
            auto sunkShipBorder = GetShipBoarderCoords(sunkShipCoords, board.size());
            for(auto [row, col] : sunkShipCoords) {
                outDisplBoard[row][col] = DisplayType::Sunken;
            }
            for (auto [row, col] : sunkShipBorder) {
                outDisplBoard[row][col] = DisplayType::Miss;
            }
        }

        fleet_rules_t Rules {
            {5, 1}, // one ship of size 5
            {4, 1}, // one ship of size 4
            {3, 1}, // one ship of size 4
            {2, 2}, // two ships of size 2
            {1, 2}, // two ships of size 1
        };
        // count number of ships from the rules
        num_of_ships_t DefaultNumOfShips = GetNumberOfShipsFromRules(Rules);
    }

    void Start() {
        // initial menu
        auto res = ui::CreateMenu();
        // connect players
        std::unique_ptr<net_comms::CommonCS> transceiver;
        if (res.AreWeServer) {
            std::cout << "Waiting for client..." << std::endl;
            transceiver = std::make_unique<net_comms::Server>(res.PortNumber);
        } else {
            transceiver = std::make_unique<net_comms::Client>(res.IpAddr, res.PortNumber);
        }
        std::cout << "Connected" << std::endl;


        board_t emptyBoard(10, board_t::value_type(10, false));
        board_t enemyBoard = emptyBoard;

        auto [ourBoard, ourShips] = ui::CreatePlacementMenu(std::move(emptyBoard), Rules);

        auto ourDisplBoard = DisplayBoardFromBoard(ourBoard);
        auto enemyDisplBoard = DisplayBoardFromBoard(enemyBoard);

        int numOfEnemyShips = DefaultNumOfShips;
        int numOfOurShips = DefaultNumOfShips;

        // start the game with required data
        bool wePlay = res.AreWeServer;
        while(numOfEnemyShips != 0 && numOfOurShips != 0) {
            if (wePlay) {
                std::cout << "We are playing" << std::endl;
                // get move from terminal
                position_t pos = ui::GameScreen(ourDisplBoard, enemyDisplBoard);
                auto [x, y] = pos;

                transceiver->Send(net_comms::PositionToData(pos));
                auto answer = net_comms::DataToHitStatus(transceiver->Receive());

                switch (answer) {
                case HitStatus::Sunken: 
                    enemyBoard[x][y] = true;
                    --numOfEnemyShips;
                    // mark the around of ship dead for display
                    MarkDeadShip(enemyBoard, pos, enemyDisplBoard);
                    std::cout << "You have sunk the enemy ship!" << std::endl;
                    break;
                case HitStatus::Hit:
                    enemyBoard[x][y] = true;
                    enemyDisplBoard[x][y] = DisplayType::Hit;
                    std::cout << "You have hit the enemy ship!" << std::endl;
                    break;
                case HitStatus::Miss:
                    enemyDisplBoard[x][y] = DisplayType::Miss;
                    std::cout << "You have missed :(" << std::endl;
                    break;
                }
            } else {
                std::cout << "Enemy is playing" << std::endl;
                // get position from the opponent
                auto pos = net_comms::DataToPosition(transceiver->Receive());
                auto [x, y] = pos;

                // check if it hit anything
                HitStatus hitStatus = HitStatus::Miss;
                if (ourBoard[x][y]) {
                    hitStatus = HitStatus::Hit;
                    // remove the target from board
                    ourBoard[x][y] = false;

                    ourDisplBoard[x][y] = DisplayType::Hit;
                    std::cout << "Enemy hit one of your ships!" << std::endl;

                    // update our ships
                    for(auto it = ourShips.begin(); it != ourShips.end(); ++it) {
                        auto& positions = *it;
                        if (positions.contains(pos)) {
                            positions.erase(pos);
                            if (positions.empty()) {
                                --numOfOurShips;
                                hitStatus = HitStatus::Sunken;
                                ourShips.erase(it);
                                std::cout << "The hit was a sunk! You are one ship short now." << std::endl;
                            }
                            break; // this break is important since we could have invalidated the iterator!
                        }
                    }
                } else {
                    ourDisplBoard[x][y] = DisplayType::Miss;
                    std::cout << "The enemy have missed your ships!" << std::endl;
                }
                // signal to the opponent the status of the hit
                transceiver->Send(net_comms::HitStatusToData(hitStatus));
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