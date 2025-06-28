#pragma once

#include <memory>
#include <unordered_map>
#include <mutex>

class Session;
class User;

/**
 * Manages user sessions - creation, storage, and retrieval
 */
class SessionManager {
public:
    SessionManager();
    ~SessionManager();

    // Session operations
    std::shared_ptr<Session> createSession(std::shared_ptr<User> user);
    std::shared_ptr<Session> getSession(const std::string& username);
    void removeSession(const std::string& username);
    
    // Data persistence
    bool loadSession(const std::string& username);
    bool saveSession(const std::string& username);
    bool saveAllSessions();

private:
    std::string getSessionFilePath(const std::string& username) const;
    
    mutable std::mutex m_sessionsMutex;
    std::unordered_map<std::string, std::shared_ptr<Session>> m_sessions;
    std::string m_dataDirectory;
};
