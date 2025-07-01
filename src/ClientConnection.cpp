#include "ClientConnection.h"
#include "Manager.h"
#include "UserManager.h"
#include "SessionManager.h"
#include "User.h"
#include "Session.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/socket.h>

ClientConnection::ClientConnection(int socket) 
    : m_socket(socket), m_connected(true), m_state(ConnectionState::LOGIN_PROMPT) {
}

ClientConnection::~ClientConnection() {
    stop();
}

void ClientConnection::start() {
    sendMessage("Welcome to the MUD Server!\n");
    sendMessage("Please enter your username: ");
    
    m_inputThread = std::thread(&ClientConnection::handleInput, this);
}

void ClientConnection::stop() {
    if (!m_connected) return;
    
    m_connected = false;
    
    if (m_socket >= 0) {
        close(m_socket);
        m_socket = -1;
    }
    
    if (m_inputThread.joinable()) {
        m_inputThread.join();
    }
}

void ClientConnection::sendMessage(const std::string& message) {
    if (!m_connected || m_socket < 0) return;
    
    ssize_t sent = send(m_socket, message.c_str(), message.length(), 0);
    if (sent < 0) {
        std::cerr << "Error sending message to client" << std::endl;
        m_connected = false;
    }
}

void ClientConnection::handleInput() {
    while (m_connected) {
        std::string input = readLine();
        if (input.empty() && !m_connected) {
            break;
        }
        
        if (!input.empty()) {
            processCommand(input);
        }
    }
}

std::string ClientConnection::readLine() {
    std::string line;
    char buffer[1024];
    
    while (m_connected) {
        ssize_t received = recv(m_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (received <= 0) {
            m_connected = false;
            break;
        }
        
        buffer[received] = '\0';
        m_inputBuffer += buffer;
        
        // Look for complete line (ending with \n or \r\n)
        size_t pos = m_inputBuffer.find('\n');
        if (pos != std::string::npos) {
            line = m_inputBuffer.substr(0, pos);
            m_inputBuffer = m_inputBuffer.substr(pos + 1);
            
            // Remove trailing \r if present
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            
            break;
        }
    }
    
    return line;
}

void ClientConnection::processCommand(const std::string& command) {
    switch (m_state) {
        case ConnectionState::LOGIN_PROMPT:
        case ConnectionState::PASSWORD_PROMPT:
        case ConnectionState::NEW_USER_CONFIRMATION:
        case ConnectionState::NEW_PASSWORD_PROMPT:
            handleAuthentication(command);
            break;
            
        case ConnectionState::AUTHENTICATED:
            handleGameCommands(command);
            break;
    }
}

void ClientConnection::handleAuthentication(const std::string& input) {
    // This is a simplified authentication flow
    // In a real implementation, you'd integrate with UserManager and SessionManager

    UserManager* userManager = static_cast<UserManager*>(GetManager(ManagerType::UserManager));

    // Check if UserManager is available - this is crucial for authentication
    if (!userManager) {
        sendMessage("Server error: UserManager not available.\n");
        m_connected = false;
        return;
    }

    switch (m_state) {
        case ConnectionState::LOGIN_PROMPT:
            m_pendingUsername = input;

            // Check if user exists
            if (userManager->userExists(m_pendingUsername)) {
                sendMessage("Welcome back. Please enter your password: ");
                m_state = ConnectionState::PASSWORD_PROMPT;
            } else {
                sendMessage("User does not exist. Would you like to create a new account? (yes/no): ");
                m_state = ConnectionState::NEW_USER_CONFIRMATION;
            }
            break;
            
        case ConnectionState::PASSWORD_PROMPT:
            m_currentUser = userManager->authenticateUser(m_pendingUsername, input);
            if (m_currentUser) {
                sendMessage("Login successful!\n");
                sendMessage("Welcome back, " + m_pendingUsername + "!\n");
                m_state = ConnectionState::AUTHENTICATED;
            } else {
                sendMessage("Invalid password. Please try again: ");
            }
            break;

        case ConnectionState::NEW_USER_CONFIRMATION:
            if (input == "yes") {
                sendMessage("Please enter your desired password: ");
                m_state = ConnectionState::NEW_PASSWORD_PROMPT;
            } else {
                m_state = ConnectionState::LOGIN_PROMPT;
                sendMessage("Please enter another username: ");
            }
            break;
            
        case ConnectionState::NEW_PASSWORD_PROMPT:
            userManager->createUser(m_pendingUsername, input);
            sendMessage("Password set successfully! You can now log in.\n");
            m_state = ConnectionState::LOGIN_PROMPT;
            break;
            
        default:
            break;
    }
}

void ClientConnection::handleGameCommands(const std::string& command) {
    if (command.empty()) {
        sendPrompt();
        return;
    }
    
    // Basic command parsing
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    // Convert to lowercase for comparison
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "quit" || cmd == "exit") {
        sendMessage("Goodbye!\n");
        m_connected = false;
    } else if (cmd == "help") {
        sendMessage("Available commands:\n");
        sendMessage("  help    - Show this help message\n");
        sendMessage("  look    - Look around\n");
        sendMessage("  say     - Say something\n");
        sendMessage("  quit    - Quit the game\n");
    } else if (cmd == "look") {
        sendMessage("You are in a simple room. There's not much to see yet.\n");
    } else if (cmd == "say") {
        std::string message;
        std::getline(iss, message);
        if (!message.empty()) {
            sendMessage("You say: " + message + "\n");
        } else {
            sendMessage("Say what?\n");
        }
    } else {
        sendMessage("Unknown command. Type 'help' for available commands.\n");
    }
    
    sendPrompt();
}

void ClientConnection::sendPrompt() {
    sendMessage("> ");
}
