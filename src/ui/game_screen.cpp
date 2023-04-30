#include "screens.hpp"
#include "ui_helpers.hpp"

#include "ftxui/component/captured_mouse.hpp"    // for ftxui
#include "ftxui/component/component.hpp"         // for Toggle, Renderer, Vertical
#include "ftxui/component/component_base.hpp"    // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"// for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                // for text, hbox, vbox, Element

namespace ui {
    using namespace ftxui;

    data::position_t GameScreen(const data::board_t& ourBoard, const data::board_t& enemyBoard) {
        auto screen = ScreenInteractive::FitComponent();

        int mouseCol = 0;
        int mouseRow = 0;

        data::position_t lastPos = {0,0};

        auto boardWidth = ourBoard[0].size();
        auto boardHeight = ourBoard.size();


        auto enemyBoardRenderer = Renderer([&]{
            auto boardCanvas = CanvasFromBoard(enemyBoard);
            // highlight hovered
            boardCanvas.DrawText(mouseCol * 2, mouseRow * 4, boardCanvas.GetPixel(mouseCol, mouseRow).character, [](Pixel& p) {
                p.background_color = Color::DarkBlue;
            });
            // highlight selected character
            auto [row, col] = lastPos;
            boardCanvas.DrawText(col * 2, row * 4, boardCanvas.GetPixel(mouseCol, mouseRow).character, [](Pixel& p) {
                p.background_color = Color::Red;
            });
            return canvas(std::move(boardCanvas));
        });

        auto enemyBoardWithMouse = CatchEvent(enemyBoardRenderer, [&](Event e) {
            if (e.is_mouse()) {
                auto& mouse = e.mouse();
                mouseCol = mouse.x - 18; // subtract left side width
                mouseRow = mouse.y - 7; // subtract top side height

                if (mouse.button == Mouse::Left && mouseCol < boardWidth && mouseRow < boardHeight) {
                    lastPos = {static_cast<std::size_t>(mouseRow), static_cast<std::size_t>(mouseCol)};
                }
            }
            return false;
        });

        auto ourBoardWithoutMouse = Renderer([&]{return canvas(CanvasFromBoard(ourBoard));});

        auto buttonCheck = Button("FIRE!", screen.ExitLoopClosure());

        auto components = Container::Horizontal({enemyBoardWithMouse, ourBoardWithoutMouse, buttonCheck});

        auto boardRender = [](const std::string& headerText, ftxui::Component boardWithMouse){
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

        auto rulesRender = []{
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

        auto game_renderer = Renderer(components, [&] {
            return vbox({
                center(text("Battle Screen")),
                separator(),
                hbox({
                    boardRender("Your ships", ourBoardWithoutMouse),
                    separator(),
                    boardRender("Enemy Ships", enemyBoardWithMouse),
                    separator(),
                    vbox({
                        rulesRender(),
                        separator(),
                        buttonCheck->Render(),
                    }),
                }),
            }) | border;
        });
        screen.Loop(game_renderer);

        return lastPos;
    }
}