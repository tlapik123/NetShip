#include <iostream>

#include "net-comms/out-facing/client.hpp"
#include "net-comms/out-facing/server.hpp"

#include "net-comms/controller.hpp"



int main(int argc, char** argv) {
    data::position_t pos {22, 42};

    auto res = net_comms::PositionToData(pos);
    auto newPos = net_comms::DataToPosition(res);

    std::cout << "first:" << newPos[0] << " ;second: " << newPos[1] << std::endl;




    // if (argc == 1) {
    //     std::cout << "Creating Server" << std::endl;

    //     net_comms::Server server(4564);
    //     std::cout << "Server created, connection established." << std::endl;

    //     server.Recieve();
    // } else {
    //     std::cout << "Creating Client" << std::endl;

    //     net_comms::Client client("127.0.0.1", 4564);
    //     std::cout << "Client created, connection established." << std::endl;

    //     //client.Send({55, 66, 77, 88});
    // }

    std::cout << "Done" << std::endl;
}
