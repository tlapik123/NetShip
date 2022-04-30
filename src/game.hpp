#pragma once

#include "board.hpp"
#include "io.hpp"


namespace NetShip {
    class Game {
    public:
        Game(int numOfShips, std::size_t width, std::size_t heigth) : mEnemyShipsRemaining(numOfShips){};

        void InitializePlayingField() {
            // TODO how will the map size be handeled?
        }

        void Play() {

            // TODO separate who is starting and who not
            while (mEnemyShipsRemaining || mMyShips.size()) {
                EnemyMove();
                OurMove();
            }
        }

    private:
        void EnemyMove() {
            auto pos = NetGetPos();
            auto optShip = TryFindHitShip(mMyShips, pos);
            // check if position hits anything
            if (!optShip) {
                // send no hit back to the second player
                // return
            }
            auto ship = *optShip;
            mMyShips.erase(ship);
            --ship.mHitPoints;

            if (ship.mHitPoints == 0) {
                // send sink back to the second player
            } else {
                // send hit back to the second player
                mMyShips.insert(ship);
            }
        }

        void OurMove() {
            auto pos = UserGetPos(std::cin);
            auto shotRes = NetSendPos(pos);
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