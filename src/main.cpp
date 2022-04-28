#include <iostream>

#include "io.hpp"
#include "board.hpp"
#include "game.hpp"

int main () {
    NetShip::bool_2d_vec hits {
        {true, false},
        {true, false},
        {true, false},
        {false, false},
        {false, false},
        {false, false},
    };
    for (auto&& x : NetShip::FindSunkShip(hits, {1,0})) {
        std::cout << x.first << x.second << std::endl;
    }
}