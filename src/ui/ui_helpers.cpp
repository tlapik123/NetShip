#include "ui_helpers.hpp"

namespace ui {
    using namespace ftxui;

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