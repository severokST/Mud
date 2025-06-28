#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

class User;
class Session;

/**
 * Represents a client connection via telnet
 */
class ClientConnection {
public:
    explicit ClientConnection(int socket);
    ~ClientConnection();

    // Delete copy constructor and assignment operator
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection& operator=(const ClientConnection&) = delete;

    void start();
    void stop();
    
    void sendMessage(const std::string& message);
    bool isConnected() const { return m_connected; }
    
    int getSocket() const { return m_socket; }

private:
    void handleInput();
    void processCommand(const std::string& command);
    void handleLogin();
    void handleAuthentication(const std::string& input);
    void handleGameCommands(const std::string& command);
    
    std::string readLine();
    void sendPrompt();

    int m_socket;
    std::atomic<bool> m_connected;
    std::thread m_inputThread;
    
    std::shared_ptr<User> m_currentUser;
    std::shared_ptr<Session> m_currentSession;
    
    enum class ConnectionState {
        LOGIN_PROMPT,
        PASSWORD_PROMPT,
        NEW_USER_CONFIRMATION,
        NEW_PASSWORD_PROMPT,
        AUTHENTICATED
    } m_state;

    std::string m_pendingUsername;
    std::string m_inputBuffer;
};
