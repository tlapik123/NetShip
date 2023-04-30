
#include "ui_helpers.hpp"

namespace ui {
    using namespace ftxui;

    Canvas CanvasFromBoard(const data::board_t& board) {
        auto width = board[0].size();
        auto height = board.size();

        auto boardCanvas = ftxui::Canvas(width * 2, height * 4);
        for (int i = 0; i < width; ++i) {
            auto canvasWidth = i * 2;
            for (int j = 0; j < height; ++j) {
                auto canvasHeight = j * 4;
                if (board[i][j]) {
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