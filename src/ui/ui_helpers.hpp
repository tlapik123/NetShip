#pragma once

#include "data/data.hpp"

#include <ftxui/component/component.hpp>      // for Toggle, Renderer, Vertical
#include <ftxui/dom/elements.hpp>             // for text, hbox, vbox, Element

namespace ui {
    /**
     * @brief Render the gameboard with the "chess like" sides
     * 
     * @param headerText Header text for the board.
     * @param board Board componenet to render.
    */
    ftxui::Element BoardRender(const std::string& headerText, ftxui::Component board);

    /**
     * @brief Render the rules of the game as a window element.
    */
    ftxui::Element RulesRender();
}