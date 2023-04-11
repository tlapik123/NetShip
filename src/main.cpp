#include <iostream>

#include "net-comms/client.hpp"
#include "net-comms/server.hpp"

#include <asio/ip/tcp.hpp>

using asio::ip::tcp;

void CreateServer() {
    try {
        asio::io_context io_context;

        // Listener
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 1234));
        auto socket = acceptor.accept();

        std::cout << "Connection established" << std::endl;

        std::array<std::byte, 20> data;
        // read some data
        auto sizeRead = socket.read_some(asio::buffer(data));
        if (sizeRead == 0) {
            std::cerr << "some error occured" << std::endl;
        }
        std::string message(reinterpret_cast<const char*>(data.data()), sizeRead);
        std::cout << "Message recieved: " << message << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void CreateClient() {
    asio::io_context io_context;
    tcp::socket socket(io_context);
    socket.connect(tcp::endpoint(tcp::v4(), 1234));

    std::string message = "Hello, world!";

    socket.write_some(asio::buffer(message));
}


int main(int argc, char** argv) {
    if (argc == 1) {
        std::cout << "Creating Server" << std::endl;

        net_comms::Server server(4564);
        std::cout << "Server created, connection established." << std::endl;

        server.Recieve();
    } else {
        std::cout << "Creating Client" << std::endl;

        net_comms::Client client("127.0.0.1", 4564);
        std::cout << "Client created, connection established." << std::endl;

        client.Send();
    }

    std::cout << "Done" << std::endl;
}
