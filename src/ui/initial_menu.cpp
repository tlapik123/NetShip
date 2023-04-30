#include <ftxui/component/captured_mouse.hpp>  // for ftxui
#include <ftxui/component/component.hpp>       // for Toggle, Renderer, Vertical
#include <ftxui/component/component_base.hpp>  // for ComponentBase
#include <ftxui/component/screen_interactive.hpp>  // for Component, ScreenInteractive
#include <ftxui/dom/elements.hpp>  // for text, hbox, vbox, Element

#include <charconv>

#include "screens.hpp"

namespace ui {
    using namespace ftxui; // TODO: remove this

    MenuData CreateMenu(int defaultPortNum) {
        MenuData retData;

        std::cout << "-----------------------------------------------------------------------------------" << std::endl;
        std::cout << "There is no going back now, you have started this game, you are going to finish it!" << std::endl;
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;

        auto screen = ScreenInteractive::TerminalOutput();

        std::vector<std::string> serverOrClient = {
            "Server",
            "Client",
        };

        std::string portNumber = std::to_string(defaultPortNum);
        std::string serverIp = "127.0.0.1";
        int zeroForServer = 0;

        Component toggleClientOrServer = Toggle(&serverOrClient, &zeroForServer);
        Component inputPortNumber = Input(&portNumber, "port number");
        Component inputServerIp = Input(&serverIp, "server IPv4");

        std::string labelPlay = "Play";
        auto buttonPlay = Button(&labelPlay, screen.ExitLoopClosure());

        auto component = Container::Vertical({
            toggleClientOrServer,
            inputPortNumber,
            inputServerIp,
            buttonPlay,
        });

        auto renderer = Renderer(component, [&] {
            return vbox({
                text("Choose your options:"),
                text(""),
                hbox(text(" * Are we a client or server?                                        : "), toggleClientOrServer->Render()),
                hbox(text(" * What is the port number for the connection?                       : "), inputPortNumber->Render()),
                hbox(text(" * What is the ipv4 of the server? (leave blank if we are server)    : "), inputServerIp->Render()),
                separator(),
                hbox(buttonPlay->Render()),
                text(""),
            });
        });
        screen.Loop(renderer);

        // bind data to structure
        retData.IpAddr = serverIp;
        retData.AreWeServer = zeroForServer == 0 ? true : false;

        auto[ptr, ec] { std::from_chars(portNumber.data(), portNumber.data() + portNumber.size(), retData.PortNumber)};


        if (ec == std::errc::invalid_argument) {
            std::cout << "That port string isn't a number." << std::endl;
            std::cout << "Try again:" << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            return CreateMenu();
        } else if (ec == std::errc::result_out_of_range) {
            std::cout << "This number is larger than an int." << std::endl;
            std::cout << "Try again:" << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;
            return CreateMenu();
        }

        return retData;
    }
}
