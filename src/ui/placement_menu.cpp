#include "screens.hpp"
#include "ui_helpers.hpp"

#include <ftxui/component/captured_mouse.hpp>  // for ftxui
#include <ftxui/component/component.hpp>       // for Toggle, Renderer, Vertical
#include <ftxui/component/screen_interactive.hpp>  // for Component, ScreenInteractive
#include <ftxui/dom/elements.hpp>  // for text, hbox, vbox, Element

namespace ui {
    using namespace ftxui;

    // anonymous namespace for internal linkage of helper functions
    namespace {
        /**
         * @brief Check if the ship size and count is valid
         * @param ships Ships that we have located.
         * @param conf Current rules that we need to obey.
         * @return 
         * true: we obey the rules
         * false: we dont obey the rules
        */
        bool AreFleetNumbersValid(const data::ships_t& ships, const data::fleet_rules_t& conf) {
            data::fleet_rules_t currFleet;
            for (auto&& shipLoc : ships) {
                currFleet[shipLoc.size()]++;
            }
            return currFleet == conf;
        }

        /**
         * @brief Check if the ships coordinates are valid (enough space around them, not overlapping, etc)
         * 
         * @param ships ship locations
         * @return 
         * true: the coordinates/locations are valid
         * false: the coordinates aren't valid 
        */
        bool AreShipCoordsValid(const data::ships_t& ships, std::size_t boardSize) {
            constexpr std::array rowOffsets = {-1, -1, -1, 0, 0, 1, 1, 1};
            constexpr std::array colOffsets = {-1, 0, 1, -1, 1, -1, 0, 1};
            data::ship_coords_t allTheBorders;
            // check it for border violations (ships too close) (russia already failed this one)
            for (const auto& shipCoords : ships) {
                data::ship_coords_t currShipBorders;
                for (const auto& coord : shipCoords) {
                    if (allTheBorders.contains(coord)) {
                        return false;
                    } else {
                        // add the borders
                        auto [row, col] = coord;
                        for (std::size_t i = 0; i < rowOffsets.size(); ++i) {
                            // yes we can overflow here, but it won't matter to the correctness
                            currShipBorders.insert({row + rowOffsets[i], col + colOffsets[i]});
                        }
                    }
                }
                allTheBorders.insert(currShipBorders.begin(), currShipBorders.end());
            }
            return true;
        }


        /**
         * @brief Get ship locations from the raw board data
         * This function uses multiple passes and little tricks to get the result as easily as it can.
         * 
         * @param board Board data with ships as "true"
         * @return data::ships_t of ship locations - these are not checked, so the configuration may be invalid.
        */
        data::ships_t ShipsFromBoardUnchecked(const data::board_t& board) {
            /**
             * We will need a "one bigger" gameBoard for better ship detection.
             * This is needed because of easiry "edge" case handling
             * Make copy of current board and make it bigger
            */
            data::board_t biggerGameBoard = board;
            // add last element to each row
            for(std::size_t i = 0; i < biggerGameBoard.size(); ++i) {
                biggerGameBoard[i].push_back(false);
            }
            // add one more row
            biggerGameBoard.push_back(data::board_t::value_type(biggerGameBoard[0].size(), false));
            
            std::unordered_set<data::idx_t> tilesUsed;
            data::ships_t retShips;

            auto boardWidth = biggerGameBoard[0].size();
            auto boardHeight = biggerGameBoard.size();

            std::size_t lastShipSize = 0;
            data::ship_coords_t tmpShipCoords;

            /** @brief Detect ships with some size contraint*/
            auto detectShipFunc = [&](std::size_t row, std::size_t col, std::size_t minSize) {
                if (biggerGameBoard[row][col]) {
                    ++lastShipSize;
                    // add position to the current ship positions
                    tmpShipCoords.insert({row, col});
                } else {
                    if (lastShipSize >= minSize) {
                        retShips.push_back(std::move(tmpShipCoords));
                    }
                    tmpShipCoords.clear();
                    lastShipSize = 0;
                }
            };

            // special case to detect ships of size 1
            data::ship_coords_t sizeOneShipsHorizontal;
            data::position_t lastCoords;
            for (std::size_t row = 0; row < boardWidth; ++row) {
                lastShipSize = 0;
                for (std::size_t col = 0; col < boardWidth; ++col) {
                    if (biggerGameBoard[row][col]) {
                        ++lastShipSize;
                        // add position to the current ship positions
                        lastCoords = {row, col};
                    } else {
                        if (lastShipSize == 1) {
                            sizeOneShipsHorizontal.insert(std::move(lastCoords));
                        }
                        lastShipSize = 0;
                    }
                }
            }

            // detect ships of size 1 vertically - and cross check with horizontal - only add ships that are in both
            data::ships_t sizeOneShipsVertical;
            for (std::size_t col = 0; col < boardWidth; ++col) {
                lastShipSize = 0;
                for (std::size_t row = 0; row < boardWidth; ++row) {
                    if (biggerGameBoard[row][col]) {
                        ++lastShipSize;
                        // add position to the current ship positions
                        lastCoords = {row, col};
                    } else {
                        if (lastShipSize == 1 && sizeOneShipsHorizontal.contains(lastCoords)) {
                            // only push those that are *really* of size one (both vertical and horizontal)
                            retShips.push_back({lastCoords});
                        }
                        lastShipSize = 0;
                    }
                }
            }

            // detect horizontal ships
            for (std::size_t row = 0; row < boardWidth; ++row) {
                lastShipSize = 0; // size cant span across borders
                for (std::size_t col = 0; col < boardWidth; ++col) {
                    detectShipFunc(row, col, 2);
                }
            }

            // detect vertical ships
            for (std::size_t col = 0; col < boardWidth; ++col) {
                lastShipSize = 0; // size cant span across borders
                for (std::size_t row = 0; row < boardWidth; ++row) {
                    detectShipFunc(row, col, 2);
                }
            }

            return retShips;
        }
    }

    PlacementData CreatePlacementMenu(data::board_t&& gameBoard, const data::fleet_rules_t& fleetConfig) {
        auto screen = ScreenInteractive::FitComponent();

        int mouseX = 0;
        int mouseY = 0;
        bool mouseLeft = false;
        bool mouseRight = false;

        auto boardWidth = gameBoard[0].size();
        auto boardHeight = gameBoard.size();


        auto board_renderer = Renderer([&] {
            auto boardCanvas = CanvasFromBoard(gameBoard);
            // highlight selected character
            boardCanvas.DrawText(mouseX * 2, mouseY * 4, boardCanvas.GetPixel(mouseX, mouseY).character, [](Pixel& p) {
                p.background_color = Color::DarkBlue;
            });

            // check mouse clicks
            if (mouseLeft && mouseX < boardWidth && mouseY < boardHeight) {
                gameBoard[mouseX][mouseY] = true;
            } else if (mouseRight && mouseX < boardWidth && mouseY < boardHeight) {
                gameBoard[mouseX][mouseY] = false;
            }
            return canvas(std::move(boardCanvas));
        });
        auto board_with_mouse = CatchEvent(board_renderer, [&](Event e) {
            if (e.is_mouse()) {
                auto& mouse = e.mouse();
                mouseX = mouse.x - 4;// subtract left side width
                mouseY = mouse.y - 5;// subtract top side height
                mouseLeft = mouse.button == Mouse::Left;
                mouseRight = mouse.button == Mouse::Right;
            }
            return false;
        });

        auto buttonCheck = Button("Check configuration", screen.ExitLoopClosure());

        auto components = Container::Horizontal({board_with_mouse, buttonCheck});

        auto game_renderer = Renderer(components, [&] {
            return vbox({center(text("Place your Ships")),
                        separator(),
                        hbox({vbox({
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
                                    board_with_mouse->Render(),
                            }),
                            separator(),
                            // TODO: this is hardcoded for now, fix this in some later release
                            vbox({window(center(text("Fleet:")),
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
                                            })),
                                    separator(),
                                    buttonCheck->Render()})})}) |
                border;
        });
        screen.Loop(game_renderer);

        // NOTE: the compiler should hopefully optimize out the tail recursion.

        // Check if the configuration is valid
        auto uncheckedShips = ShipsFromBoardUnchecked(gameBoard);
        if (!AreShipCoordsValid(uncheckedShips, boardWidth)) {
            std::cout << "Ships are too close together - There needs to be 1 block border between ships!" << std::endl;
            std::cout << "Try again:" << std::endl;
            std::cout << std::endl;
            return CreatePlacementMenu(std::move(gameBoard), fleetConfig);
        }
        if (!AreFleetNumbersValid(uncheckedShips, fleetConfig)) {
            std::cout << "There are too many of some ship or too little or there is some ship with size larger/smaller than allowed!" << std::endl;
            std::cout << "Try again:" << std::endl;
            std::cout << std::endl;
            return CreatePlacementMenu(std::move(gameBoard), fleetConfig);
        }

        return {gameBoard, uncheckedShips};
    }
}
