#include <iostream>

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"

#include "net-comms/controller.hpp"

#include "ftxui/component/captured_mouse.hpp"    // for ftxui
#include "ftxui/component/component.hpp"         // for Toggle, Renderer, Vertical
#include "ftxui/component/component_base.hpp"    // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"// for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                // for text, hbox, vbox, Element
#include <iostream>

#include "initial_menu.hpp"
#include "placement_menu.hpp"


int main(int argc, const char* argv[]) {
    // NOTE: we make gameBoard one space wider - this will help the ship detection

    data::fleet_rules_t rules = {
        {5, 1},
        {4, 1},
        {3, 1},
        {2, 2},
        {1, 2},
    };

    data::board_t emptyBoard(10, data::board_t::value_type(10, false));

    auto [ourBoard, ourShips] = CreatePlacementMenu(std::move(emptyBoard), rules);

    return 0;
}