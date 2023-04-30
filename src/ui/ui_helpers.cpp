
#include "ui_helpers.hpp"

namespace ui {
    using namespace ftxui;

    Canvas CanvasFromBoard(const data::board_t& board) {
        auto width = board[0].size();
        auto height = board.size();

        auto boardCanvas = ftxui::Canvas(width * 2, height * 4);
        for (std::size_t row = 0; row < height; ++row) {
            auto canvasHeight = row * 4;
            for (std::size_t col = 0; col < width; ++col) {
                auto canvasWidth = col * 2;
                if (board[row][col]) {
                    boardCanvas.DrawText(canvasWidth, canvasHeight, "*", [](Pixel& p) {
                        p.foreground_color = Color::Red;
                        p.background_color = Color::GrayLight;
                        p.bold = true;
                    });
                } else {
                    boardCanvas.DrawText(canvasWidth, canvasHeight, " ", [](Pixel& p) {
                        p.background_color = Color::BlueLight;
                    });
                }
            }
        }
        return boardCanvas;
    }
}