#pragma once

#include "board.hpp"
#include "io.hpp"


namespace NetShip {
    void Play() {
        // enemy plays
            auto pos = NetworkGetPos();
            // check if position hits anything
            // send the result back to the second player
            // delete the ship if needed
        // we play 
            // get input from user
            // change input to position
            // send position wia network
            // get response
            // mark the shot on the screen
            // mark if the whole ship died
        // repeat

        // we need to keep
            // where did we hit
            // list of our ships
        // for user experience
            // where are our ships and where did enemy hit
            // what is a hit and what is a miss on the "hit map"
    }
}