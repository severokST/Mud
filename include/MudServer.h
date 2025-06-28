#pragma once

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

class ClientConnection;
class UserManager;
class SessionManager;

/**
 * Main server class that handles telnet connections and manages the MUD server
 */
class MudServer {
public:
    explicit MudServer(int port = 8080);
    ~MudServer();

    // Delete copy constructor and assignment operator
    MudServer(const MudServer&) = delete;
    MudServer& operator=(const MudServer&) = delete;

    bool start();
    void stop();
    void run();

private:
    void acceptConnections();
    void handleClient(int clientSocket);
    void cleanup();

    int m_port;
    int m_serverSocket;
    std::atomic<bool> m_running;
    
    std::unique_ptr<UserManager> m_userManager;
    std::unique_ptr<SessionManager> m_sessionManager;
    
    std::vector<std::unique_ptr<ClientConnection>> m_connections;
    std::mutex m_connectionsMutex;
    
    std::thread m_acceptThread;
};
