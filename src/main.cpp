#include "game-logic/game.hpp"

#include "data/data.hpp"

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"
#include "net-comms/out-facing/common_cs.hpp"
#include "net-comms/in_convertors.hpp"

#include "ui/screens.hpp"

#include <iostream>

int main(int argc, const char* argv[]) {
    game::Start();

    // auto res = ui::CreateMenu();
    //     // connect players
    // std::unique_ptr<net_comms::CommonCS> transiever;
    // if (res.AreWeServer) {
    //     data::position_t pos = {22, 22};
    //     transiever = std::make_unique<net_comms::Server>(res.PortNumber);
    //     transiever->Send(net_comms::PositionToData(pos));
    // } else {
    //     transiever = std::make_unique<net_comms::Client>(res.IpAddr, res.PortNumber);
    //     auto pos = net_comms::DataToPosition(transiever->Recieve());
    //     std::cout << pos[0] << "," << pos[1] << std::endl;
    // }

    return 0;
}