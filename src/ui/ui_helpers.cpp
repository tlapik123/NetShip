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

    Element BoardRender(const std::string& headerText, Component boardWithMouse) {
        return 
        vbox({
            center(text(headerText)),
            separator(),
            hbox({
                vbox({
                    text(" "),
                    separator(),
                    text("01"),
                    text("02"),
                    text("03"),
                    text("04"),
                    text("05"),
                    text("06"),
                    text("07"),
                    text("08"),
                    text("09"),
                    text("10"),
                }),
                separator(),
                vbox({
                    text("ABCDEFGHIJ"),
                    separator(),
                    boardWithMouse->Render(),
                }),
            }),
        });
    };

    Element RulesRender() {
        return
        window(center(text("Fleet:")),
            hbox({
                vbox({
                    text("#"),
                    separator(),
                    text("1"),
                    text("1"),
                    text("1"),
                    text("2"),
                    text("2"),
                }),
                separator(),
                vbox({
                    text("ship"),
                    separator(),
                    text("Aircraft Carrier"),
                    text("Battleship"),
                    text("Cruiser"),
                    text("Destroyer"),
                    text("Submarine"),
                }),
                separator(),
                vbox({
                    text("size"),
                    separator(),
                    text("5"),
                    text("4"),
                    text("3"),
                    text("2"),
                    text("1"),
                }),
            })
        );
    };
}