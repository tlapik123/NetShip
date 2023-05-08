#include "screens.hpp"
#include "ui_helpers.hpp"

#include "ftxui/component/captured_mouse.hpp"    // for ftxui
#include "ftxui/component/component.hpp"         // for Toggle, Renderer, Vertical
#include "ftxui/component/component_base.hpp"    // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"// for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                // for text, hbox, vbox, Element

namespace ui {
    using namespace ftxui;
    using namespace data;

    namespace {
        Canvas CanvasFromDisplayBoard(const display_board_t& board) {
            auto width = board[0].size();
            auto height = board.size();

            auto boardCanvas = ftxui::Canvas(width * 2, height * 4);
            for (std::size_t row = 0; row < height; ++row) {
                auto canvasHeight = row * 4;
                for (std::size_t col = 0; col < width; ++col) {
                    auto canvasWidth = col * 2;
                    auto val = board[row][col];
                    switch (val) {
                    case DisplayType::Empty: 
                        boardCanvas.DrawText(canvasWidth, canvasHeight, " ", [](Pixel& p) {
                            p.background_color = Color::BlueLight;
                        });
                        break;
                    case DisplayType::Miss: 
                        boardCanvas.DrawText(canvasWidth, canvasHeight, "M", [](Pixel& p) {
                            p.background_color = Color::Blue;
                            p.bold = true;
                        });
                        break;
                    case DisplayType::Ship:
                        boardCanvas.DrawText(canvasWidth, canvasHeight, "S", [](Pixel& p) {
                            p.foreground_color = Color::Black;
                            p.background_color = Color::GrayLight;
                            p.bold = true;
                        });
                        break;
                    case DisplayType::Hit: 
                        boardCanvas.DrawText(canvasWidth, canvasHeight, "H", [](Pixel& p) {
                            p.foreground_color = Color::Black;
                            p.background_color = Color::Red;
                            p.bold = true;
                        });
                        break;
                    case DisplayType::Sunken: 
                        boardCanvas.DrawText(canvasWidth, canvasHeight, " ", [](Pixel& p) {
                            p.foreground_color = Color::Red;
                            p.background_color = Color::Red;
                        });
                        break;
                    }
                }
            }
            return boardCanvas;
        }
    }

    position_t GameScreen(const display_board_t& ourBoard, const display_board_t& enemyBoard) {
        auto screen = ScreenInteractive::FitComponent();

        int mouseCol = 0;
        int mouseRow = 0;

        int lastPosRow = -1;
        int lastPosCol = -1;

        auto boardWidth = ourBoard[0].size();
        auto boardHeight = ourBoard.size();


        auto enemyBoardRenderer = Renderer([&]{
            auto boardCanvas = CanvasFromDisplayBoard(enemyBoard);
            // highlight hovered
            boardCanvas.DrawText(mouseCol * 2, mouseRow * 4, boardCanvas.GetPixel(mouseCol, mouseRow).character, [](Pixel& p) {
                p.background_color = Color::DarkBlue;
            });
            // highlight selected character
            if (lastPosCol != -1) {
                boardCanvas.DrawText(lastPosCol * 2, lastPosRow * 4, boardCanvas.GetPixel(lastPosCol, lastPosRow).character, [](Pixel& p) {
                    p.background_color = Color::Yellow;
                });
            }
            return canvas(std::move(boardCanvas));
        });

        auto enemyBoardWithMouse = CatchEvent(enemyBoardRenderer, [&](Event e) {
            if (e.is_mouse()) {
                auto& mouse = e.mouse();
                mouseCol = mouse.x - 18; // subtract left side width
                mouseRow = mouse.y - 7; // subtract top side height

                if (mouse.button == Mouse::Left && mouseCol < boardWidth && mouseRow < boardHeight) {
                    lastPosRow = mouseRow;
                    lastPosCol = mouseCol;
                }
            }
            return false;
        });

        auto ourBoardWithoutMouse = Renderer([&]{return canvas(CanvasFromDisplayBoard(ourBoard));});

        auto buttonCheck = Button("FIRE!", screen.ExitLoopClosure());

        auto components = Container::Horizontal({enemyBoardWithMouse, ourBoardWithoutMouse, buttonCheck});

        auto game_renderer = Renderer(components, [&] {
            return vbox({
                center(text("Battle Screen")),
                separator(),
                hbox({
                    BoardRender("Your ships", ourBoardWithoutMouse),
                    separator(),
                    BoardRender("Enemy Ships", enemyBoardWithMouse),
                    separator(),
                    vbox({
                        RulesRender(),
                        separator(),
                        buttonCheck->Render(),
                    }),
                }),
            }) | border;
        });
        screen.Loop(game_renderer);

        return {static_cast<std::size_t>(lastPosRow), static_cast<std::size_t>(lastPosCol)};
    }
}