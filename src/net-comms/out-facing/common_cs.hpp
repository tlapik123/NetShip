#pragma once

#include <asio/ip/tcp.hpp>

namespace net_comms {
    class CommonCS {
    protected: 
        asio::io_context mIOContext;
        asio::ip::tcp::socket mEstablishedSocket{mIOContext};

    public:
        void Send(std::vector<std::byte> dataToSend);
        std::vector<std::byte> Receive();
    };
}