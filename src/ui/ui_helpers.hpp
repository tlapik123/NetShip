#pragma once

#include "data/data.hpp"

#include <ftxui/dom/elements.hpp>  // for text, hbox, vbox, Element

namespace ui {
    /**
     * @brief Create canvas object from board data.
     * @param board Board data
     * @return Canvas created from board data.
    */
    ftxui::Canvas CanvasFromBoard(const data::board_t& board);
}