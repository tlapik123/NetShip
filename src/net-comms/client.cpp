#include "client.hpp"

#include <iostream>

namespace net_comms {
    using asio::ip::tcp;
    
    Client::Client(std::string ipAdress, int portNumber) {
        mEstablishedSocket.connect(tcp::endpoint(asio::ip::address::from_string(ipAdress), portNumber));
    }
    void Client::Send(){
        std::string message = "Hello, world!";

        mEstablishedSocket.write_some(asio::buffer(message));
    }
    void Client::Recieve(){
        // std::array<std::byte, 20> data;
        // // read some data
        // auto sizeRead = socket.read_some(asio::buffer(data));
        // if (sizeRead == 0) {
        //     std::cerr << "some error occured" << std::endl;
        // }
        // std::string message(reinterpret_cast<const char*>(data.data()), sizeRead);
    }
}