#include "MudServer.h"
#include <iostream>
#include <csignal>
#include <memory>

std::unique_ptr<MudServer> g_server;

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down server..." << std::endl;
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handling for graceful shutdown
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Parse command line arguments
    int port = 8080;
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception& e) {
            std::cerr << "Invalid port number: " << argv[1] << std::endl;
            return 1;
        }
    }
    
    std::cout << "Starting MUD Server..." << std::endl;
    
    // Create and start server
    g_server = std::make_unique<MudServer>(port);
    
    if (!g_server->start()) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }
    
    // Run server
    g_server->run();
    
    std::cout << "Server shut down complete." << std::endl;
    return 0;
}
