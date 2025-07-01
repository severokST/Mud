#include "MudServer.h"
#include "Manager.h"
#include "ClientConnection.h"
#include "UserManager.h"
#include "SessionManager.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

MudServer::MudServer(int port) 
    : m_port(port), m_serverSocket(-1), m_running(false) {
    m_userManager = std::make_unique<UserManager>();
    m_sessionManager = std::make_unique<SessionManager>();

    // Register managers
    RegisterManager(ManagerType::UserManager, m_userManager.get());
    RegisterManager(ManagerType::SessionManager, m_sessionManager.get());
}

MudServer::~MudServer() {
    stop();
}

bool MudServer::start() {
    // Create socket
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Error setting socket options" << std::endl;
        close(m_serverSocket);
        return false;
    }

    // Bind socket
    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    if (bind(m_serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Error binding socket to port " << m_port << std::endl;
        close(m_serverSocket);
        return false;
    }

    // Listen for connections
    if (listen(m_serverSocket, 10) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
        close(m_serverSocket);
        return false;
    }

    m_running = true;
    std::cout << "MUD Server started on port " << m_port << std::endl;
    return true;
}

void MudServer::stop() {
    if (!m_running) return;
    
    m_running = false;
    
    if (m_serverSocket >= 0) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }
    
    if (m_acceptThread.joinable()) {
        m_acceptThread.join();
    }
    
    cleanup();
    
    // Save all data
    m_userManager->saveUsers();
    m_sessionManager->saveAllSessions();
    
    std::cout << "MUD Server stopped" << std::endl;
}

void MudServer::run() {
    if (!m_running) return;
    
    m_acceptThread = std::thread(&MudServer::acceptConnections, this);
    
    // Main server loop - could add periodic tasks here
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Clean up disconnected clients
        cleanup();
    }
}

void MudServer::acceptConnections() {
    while (m_running) {
        struct sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        
        int clientSocket = accept(m_serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);
        
        if (clientSocket < 0) {
            if (m_running) {
                std::cerr << "Error accepting client connection" << std::endl;
            }
            continue;
        }
        
        std::cout << "New client connected" << std::endl;
        
        // Create new client connection
        auto connection = std::make_unique<ClientConnection>(clientSocket);
        connection->start();
        
        std::lock_guard<std::mutex> lock(m_connectionsMutex);
        m_connections.push_back(std::move(connection));
    }
}

void MudServer::cleanup() {
    std::lock_guard<std::mutex> lock(m_connectionsMutex);
    
    auto it = m_connections.begin();
    while (it != m_connections.end()) {
        if (!(*it)->isConnected()) {
            std::cout << "Cleaning up disconnected client" << std::endl;
            it = m_connections.erase(it);
        } else {
            ++it;
        }
    }
}
