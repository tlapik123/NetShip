#pragma once

#include "data/data.hpp"

#include <string>

namespace ui {
    /** @brief Structure to hold return data from the Placement menu */
    struct PlacementData {
        data::board_t board;
        data::ships_t ships;
    };

    struct MenuData {
        bool AreWeServer;
        int PortNumber;
        std::string IpAddr;
    };

    /**
     * @brief Create the visual menu and let the user interact with it.
     * IF the user fails the placement call yourself again.
     * @param gameBoard To use as canvas and let user place ships on it.
     * @param fleetConfig Rules of the game (how many ships and of what size).
     * @return PlacementData (board and sthip) from the menu.
    */
    PlacementData CreatePlacementMenu(data::board_t&& gameBoard, const data::fleet_rules_t& fleetConfig);

    /**
     * @brief Create initial menu to get settings.
     * @param defaultPortNum Default port number.
     * @return Data from the menu.
    */
    MenuData CreateMenu(int defaultPortNum = 4242);

    data::position_t GameScreen(const data::board_t& ourBoard, const data::board_t& enemyBoard);
}