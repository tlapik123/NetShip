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

        return lastPos;
    }
}