#pragma once

#include <string>
#include <memory>
#include <chrono>

/**
 * Represents a user account in the MUD system
 */
class User {
public:
    User(const std::string& username, const std::string& passwordHash);
    
    // Getters
    const std::string& getUsername() const { return m_username; }
    const std::string& getPasswordHash() const { return m_passwordHash; }
    
    std::chrono::system_clock::time_point getCreatedTime() const { return m_createdTime; }
    std::chrono::system_clock::time_point getLastLoginTime() const { return m_lastLoginTime; }
    
    // Authentication
    bool verifyPassword(const std::string& password) const;
    void updateLastLogin();
    
    // Serialization
    std::string serialize() const;
    static std::unique_ptr<User> deserialize(const std::string& data);

private:
    std::string m_username;
    std::string m_passwordHash;
    std::chrono::system_clock::time_point m_createdTime;
    std::chrono::system_clock::time_point m_lastLoginTime;
};
