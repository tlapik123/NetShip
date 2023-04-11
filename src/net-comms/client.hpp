#pragma once

#include <asio/ip/tcp.hpp>

namespace net_comms {
    class Client {        
        asio::io_context mIOContext;
        asio::ip::tcp::socket mEstablishedSocket{mIOContext};

    public:
        Client(std::string ipAdress, int portNumber);

        void Send();
        void Recieve();
    };
}