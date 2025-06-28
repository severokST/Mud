#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <unordered_map>

class User;

/**
 * Represents a user session with persistent state
 */
class Session {
public:
    explicit Session(std::shared_ptr<User> user);
    
    // Getters
    std::shared_ptr<User> getUser() const { return m_user; }
    const std::string& getCurrentRoom() const { return m_currentRoom; }
    std::chrono::system_clock::time_point getLastActivity() const { return m_lastActivity; }
    
    // Session state
    void setCurrentRoom(const std::string& room) { m_currentRoom = room; }
    void updateActivity() { m_lastActivity = std::chrono::system_clock::now(); }
    
    // Properties - for storing arbitrary session data
    void setProperty(const std::string& key, const std::string& value);
    std::string getProperty(const std::string& key) const;
    bool hasProperty(const std::string& key) const;
    
    // Serialization
    std::string serialize() const;
    static std::unique_ptr<Session> deserialize(const std::string& data, std::shared_ptr<User> user);

private:
    std::shared_ptr<User> m_user;
    std::string m_currentRoom;
    std::chrono::system_clock::time_point m_lastActivity;
    std::unordered_map<std::string, std::string> m_properties;
};
