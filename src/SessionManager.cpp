#include "SessionManager.h"
#include "Session.h"
#include "User.h"
#include <fstream>
#include <filesystem>
#include <iostream>

SessionManager::SessionManager() : m_dataDirectory("data/sessions/") {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories(m_dataDirectory);
}

SessionManager::~SessionManager() {
    saveAllSessions();
}

std::shared_ptr<Session> SessionManager::createSession(std::shared_ptr<User> user) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    
    const std::string& username = user->getUsername();
    
    // Try to load existing session first
    if (loadSession(username)) {
        auto it = m_sessions.find(username);
        if (it != m_sessions.end()) {
            return it->second;
        }
    }
    
    // Create new session
    auto session = std::make_shared<Session>(user);
    m_sessions[username] = session;
    
    return session;
}

std::shared_ptr<Session> SessionManager::getSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    
    auto it = m_sessions.find(username);
    if (it != m_sessions.end()) {
        return it->second;
    }
    
    return nullptr;
}

void SessionManager::removeSession(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    
    // Save session before removing
    saveSession(username);
    
    m_sessions.erase(username);
}

bool SessionManager::loadSession(const std::string& username) {
    std::string filePath = getSessionFilePath(username);
    
    if (!std::filesystem::exists(filePath)) {
        return false; // No saved session
    }
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        // Note: This is incomplete - we need the User object to deserialize
        // In a real implementation, you'd pass the UserManager here
        // For now, just return false indicating no session was loaded
        return false;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading session for " << username << ": " << e.what() << std::endl;
        return false;
    }
}

bool SessionManager::saveSession(const std::string& username) {
    auto session = getSession(username);
    if (!session) {
        return false;
    }
    
    try {
        std::string filePath = getSessionFilePath(username);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        
        file << session->serialize();
        file.close();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving session for " << username << ": " << e.what() << std::endl;
        return false;
    }
}

bool SessionManager::saveAllSessions() {
    std::lock_guard<std::mutex> lock(m_sessionsMutex);
    
    bool allSuccessful = true;
    for (const auto& pair : m_sessions) {
        if (!saveSession(pair.first)) {
            allSuccessful = false;
        }
    }
    
    return allSuccessful;
}

std::string SessionManager::getSessionFilePath(const std::string& username) const {
    return m_dataDirectory + username + ".session";
}
