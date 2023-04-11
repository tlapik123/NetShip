#include "server.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;

    Server::Server(int portNumber) : mAcceptor{mIOContext, tcp::endpoint(tcp::v4(), portNumber)},
                                     mEstablishedSocket{mAcceptor.accept()} {}

    void Server::Send() {}
    void Server::Recieve() {
        std::array<std::byte, 20> data;
        // read some data
        auto sizeRead = mEstablishedSocket.read_some(asio::buffer(data));
        if (sizeRead == 0) {
            std::cerr << "some error occured" << std::endl;
        }
        std::string message(reinterpret_cast<const char*>(data.data()), sizeRead);
        std::cout << message << std::endl;
    }
}// namespace net_comms