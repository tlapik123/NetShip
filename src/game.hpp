#pragma once

#include <string>
#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"
#include "net-comms/out-facing/common_cs.hpp"
#include "net-comms/in_convertors.hpp"

namespace game {
    void Start() {
        // initialize menu

        // get all the data needed from menu
        std::string enemyIpAddress;
        bool areWeServer;

        // connect players
        std::unique_ptr<net_comms::CommonCS> transiever;
        if (areWeServer) {
            transiever = std::make_unique<net_comms::Server>();
        } else {
            transiever = std::make_unique<net_comms::Client>();
        }

        // initialize game board
        board_t ourBoard;
        board_t enemyBoard;

        ships_t ourShips;

        int numOfEnemyShips; // initialize to the number of enemy ships
        int numOfOurShips; // initialize to the number of enemy ships

        // start the game with required data
        bool wePlay = areWeServer;
        while(true) {
            if (wePlay) {
                // get move from terminal
                data::position_t pos;
                auto [x, y] = pos;

                transiever->Send(net_comms::PositionToData(pos));
                // TODO: there will need to be some transform function for the data to bool (hit/miss)
                auto answer = net_comms::DataToHitStatus(transiever->Recieve());

                switch (answer) {
                case data::HitStatus::Sunken: 
                    enemyBoard[x][y] = true;
                    --enemyShips;
                    // update the enemy board on "screen"
                    // did we win?
                    if (numOfEnemyShips == 0) {
                        // We won! do something on screen
                        return;
                    }
                    break;
                case data::HitStatus::Hit:
                    enemyBoard[x][y] = true;
                    // update the enemy board on "screen"
                    break;
                case data::HitStatus::Miss:
                    // update the enemy board on "screen"
                    break;
                }
            } else {
                // get position from the opponent
                auto pos = net_comms::DataToPosition(transiever->Recieve());
                auto [x, y] = pos;

                // check if it hit anything
                data::HitStatus hitStatus = data::HitStatus::Miss;
                if (ourBoard[x][y]) {
                    hitStatus = data::HitStatus::Hit;
                    // remove the target from board
                    ourBoard[x][y] = false;
                    --numOfOurShips;
                    // update our own screen with the visual


                    bool sunken = false;
                    // update our ships
                    foreach(const auto& positions : ourShips) {
                        if (positions.contains(pos)) {
                            positions.erase(pos);
                            if (positions.empty()) {
                                sunken = true;
                                ourShips.erase(position);
                                // update our own screen with the visual

                                break;
                            }
                        }
                    }

                    // did ve lose?
                    if (numOfOurShips == 0) {
                        // we lost
                        // update our own screen with the visual
                    }
                }
                // signal to the opponent the status of the hit
                transiever->Send(net_comms::HitStatusToData(hitStatus));
            }
        }
    }
}