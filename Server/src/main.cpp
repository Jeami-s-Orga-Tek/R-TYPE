/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include <boost/asio.hpp>
#include <csignal>
#include <memory>
#include "net/UdpServer.hpp"
#include "util/Log.hpp"

static std::unique_ptr<boost::asio::io_context> g_io;
static std::unique_ptr<RtypeServer::UdpServer> g_server;

void signalHandler(int signum) {
    RtypeServer::infof("Signal %d received, shutting down server...", signum);
    if (g_io) {
        g_io->stop();
    }
}

int main(int argc, char* argv[])
{
    try {
        uint16_t port = 4242;
        if (argc >= 2) {
            try {
                port = static_cast<uint16_t>(std::stoi(argv[1]));
            } catch (const std::exception& e) {
                std::cerr << "Invalid port number: " << argv[1] << std::endl;
                return 1;
            }
        }

        RtypeServer::infof("Starting R-Type Server on port %u", port);
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
        g_io = std::make_unique<boost::asio::io_context>();
        g_server = std::make_unique<RtypeServer::UdpServer>(*g_io, port);

        RtypeServer::infof("Server initialized successfully");
        RtypeServer::infof("Press Ctrl+C to stop the server");
        g_io->run();
        RtypeServer::infof("Server stopped gracefully");
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
    return 0;
}
