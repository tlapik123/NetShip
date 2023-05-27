#include "common_cs.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;

    void CommonCS::Send(std::vector<std::byte> dataToSend) {
        mEstablishedSocket.write_some(asio::buffer(dataToSend));
    }
    std::vector<std::byte> CommonCS::Receive() {
        std::vector<std::byte> data(256);
        auto sizeRead = mEstablishedSocket.read_some(asio::buffer(data));
        return data;
    }
}// namespace net_comms