#include <iostream>

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"

#include "net-comms/controller.hpp"

#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Toggle, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, vbox, Element
#include <iostream>
 
#include "initial_menu.hpp"

int main(void) {
    auto res = CreateMenu();
    
    std::cout << res.AreWeServer << std::endl;
    std::cout << res.IpAddr << std::endl;
    std::cout << res.PortNumber << std::endl;

    return EXIT_SUCCESS;
}
