#pragma once

#include "board.hpp"
#include "io.hpp"


namespace NetShip {
    class Game {
    public:
        Game(int numOfShips, std::size_t width, std::size_t heigth)
            : mEnemyShipsRemaining(numOfShips), mShots(heigth, std::vector<bool>(width, false)){};

        void Play(bool first) {
            while (mEnemyShipsRemaining || mMyShips.size()) {
                if (first) {
                    OurMove();
                    EnemyMove();
                } else {
                    EnemyMove();
                    OurMove();
                }
            }
        }

    private:
        void EnemyMove() {
            auto pos = NetGetPos();
            auto optShip = TryFindHitShip(mMyShips, pos);
            // check if position hits anything
            if (!optShip) {
                NetSendRes(ShotResult::Miss);
                // propagate message to the player
                return;
            } 
            auto ship = *optShip;
            mMyShips.erase(ship);
            --ship.mHitPoints;

            if (ship.mHitPoints == 0) {
                NetSendRes(ShotResult::Sink);
            } else {
                NetSendRes(ShotResult::Hit);
                mMyShips.insert(ship);
            }
            // propagate message to the player
        }

        void OurMove() {
            auto pos = UserGetPos(std::cin);
            auto shotRes = NetSendPosGetRes(pos);
            switch (shotRes) {
                case ShotResult::Hit:
                    /* code */
                    break;
                case ShotResult::Miss:
                    /* code */
                    break;
                case ShotResult::Sink:
                    /* code */
                    break;
            }
            // mark the shot on the screen
            // mark if the whole ship died
        }

        // for user experience
        // where are our ships and where did enemy hit
        // what is a hit and what is a miss on the "hit map"

        ship_set mMyShips;
        bool_2d_vec mShots;
        int mEnemyShipsRemaining;
    };
}// namespace NetShip