#include <iostream>
#include <kissnet.hpp>

#include <atomic>
#include <list>
#include <thread>
#include <vector>

namespace
{
    std::list<kissnet::tcp_socket>& connections()
    {
        auto static connections_ = new std::list<kissnet::tcp_socket>;
        return *connections_;
    }

    std::vector<std::thread>& workers()
    {
        auto static workers_ = new std::vector<std::thread>;
        return *workers_;
    }

    constexpr kissnet::port_t port  = 12321;
    std::atomic<bool> running       = true;
    using socket_cref = std::reference_wrapper<const kissnet::tcp_socket>;
    using socket_list = std::list<socket_cref>;
}

void listen(kissnet::tcp_socket& socket);
void run(const std::string &listen_address, kissnet::port_t liste_port);
void send(const kissnet::buffer<4096>&, size_t length, const socket_list&);

void run(const std::string &listen_address, kissnet::port_t listen_port)
{
    kissnet::tcp_socket server(kissnet::endpoint(listen_address, listen_port));
    server.bind();
    server.listen();

    while (running)
    {
        auto &socket = connections().emplace_back(server.accept());
        std::cout << "detected connection: "
                  << socket.get_recv_endpoint().address << ":"
                  << socket.get_recv_endpoint().port << std::endl;

        workers().emplace_back([&]
        {
            listen(socket);

            std::cout << "detected disconnect\n";
            if (const auto socket_iter =
                    std::find(connections().begin(), connections().end(), std::ref(socket));
                    socket_iter != connections().end())
            {
                std::cout << "closing socket...\n";
                connections().erase(socket_iter);
            }
        });
        workers().back().detach();
    }
    server.close();
}

void listen(kissnet::tcp_socket& socket)
{
    bool continue_receiving = true;

    kissnet::buffer<4096> static_buffer;
    while (continue_receiving)
    {
        if (auto [size, valid] = socket.recv(static_buffer); valid)
        {
            if (valid.value == kissnet::socket_status::cleanly_disconnected)
            {
                continue_receiving = false;
            }

            if (size < static_buffer.size()) static_buffer[size] = std::byte{0};
            std::cout << reinterpret_cast<const char *>(static_buffer.data()) << "\n";
            send(static_buffer, static_buffer.size (),{socket});
        }
        else
        {
            continue_receiving = false;
            socket.close();
        }
    }
}

//void send(const kissnet::buffer<4096>&, const socket_list&);
void send(const kissnet::buffer<4096> &buffer, size_t length, const socket_list &exclude = {})
{
    for (auto &socket : connections())
    {
        if (auto it = std::find(exclude.cbegin(), exclude.cend(), socket);
        it == exclude.cend())
        {
            socket.send(buffer, length);
        }
    }
}

int main()
{
    run("0.0.0.0", port);
    delete &connections();
    delete &workers();
    return EXIT_SUCCESS;
}
