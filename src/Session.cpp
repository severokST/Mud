#include "Session.h"
#include "User.h"
#include <sstream>

Session::Session(std::shared_ptr<User> user) 
    : m_user(user), m_currentRoom("starting_room") {
    updateActivity();
}

void Session::setProperty(const std::string& key, const std::string& value) {
    m_properties[key] = value;
    updateActivity();
}

std::string Session::getProperty(const std::string& key) const {
    auto it = m_properties.find(key);
    return (it != m_properties.end()) ? it->second : "";
}

bool Session::hasProperty(const std::string& key) const {
    return m_properties.find(key) != m_properties.end();
}

std::string Session::serialize() const {
    std::ostringstream oss;
    
    auto lastActivityTimeT = std::chrono::system_clock::to_time_t(m_lastActivity);
    
    oss << m_currentRoom << "\n"
        << lastActivityTimeT << "\n"
        << m_properties.size() << "\n";
    
    for (const auto& pair : m_properties) {
        oss << pair.first << "\n" << pair.second << "\n";
    }
    
    return oss.str();
}

std::unique_ptr<Session> Session::deserialize(const std::string& data, std::shared_ptr<User> user) {
    std::istringstream iss(data);
    std::string currentRoom;
    std::time_t lastActivityTimeT;
    size_t propertyCount;
    
    if (!std::getline(iss, currentRoom) ||
        !(iss >> lastActivityTimeT) ||
        !(iss >> propertyCount)) {
        return nullptr;
    }
    
    // Skip the newline after propertyCount
    iss.ignore();
    
    auto session = std::make_unique<Session>(user);
    session->m_currentRoom = currentRoom;
    session->m_lastActivity = std::chrono::system_clock::from_time_t(lastActivityTimeT);
    
    // Load properties
    for (size_t i = 0; i < propertyCount; ++i) {
        std::string key, value;
        if (std::getline(iss, key) && std::getline(iss, value)) {
            session->m_properties[key] = value;
        }
    }
    
    return session;
}
