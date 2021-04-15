#include <atomic>
#include <iostream>
#include <thread>
#include <kissnet.hpp>

kissnet::tcp_socket connect(const std::string &server_address, kissnet::port_t server_port);
void input(kissnet::tcp_socket &socket);
void run(kissnet::tcp_socket&&);

namespace
{
    std::atomic<bool> connected = false;
    std::atomic<bool> running   = false;
}

kissnet::tcp_socket connect(const std::string &server_address, kissnet::port_t server_port)
{
    kissnet::tcp_socket client_socket(kissnet::endpoint{server_address, server_port});
    client_socket.connect(0);
    connected = true;
    return client_socket;
}

void input(kissnet::tcp_socket &socket)
{
    while (running)
    {
        std::string str;
        std::getline(std::cin, str);
        std::cin.clear();

        if (connected)
        {
            socket.send(reinterpret_cast<std::byte *>(str.data()), str.length());
        }
    }
}

void run(kissnet::tcp_socket&& socket)
{
    std::thread input_thread ([&]
                              {
                                  input(socket);
                              });

    running = true;
    input_thread.detach();

    while (running && connected)
    {
        kissnet::buffer<4096> static_buffer;
        while (connected)
        {
            if (auto [size, valid] = socket.recv(static_buffer); valid)
            {
                if (valid.value == kissnet::socket_status::cleanly_disconnected)
                {
                    connected = false;
                    std::cout << "clean disconnection" << std::endl;
                    socket.close();
                    break;
                }

                if (size < static_buffer.size()) static_buffer[size] = std::byte{0};
                std::cout << reinterpret_cast<const char *>(static_buffer.data()) << '\n';
            }
            else
            {
                connected = false;
                std::cout << "disconnected" << std::endl;
                socket.close();
            }
        }
    }
}

// program entrypoint
int main(int /*argc*/, char* /*argv*/ [])
{
    // connect to the server's socket
    auto socket = connect("127.0.0.1", 12321);

    std::string str {"hello computer!"};

    socket.send(reinterpret_cast<std::byte*>(str.data()), str.length());
    run(std::move(socket));
    return EXIT_SUCCESS;
}
