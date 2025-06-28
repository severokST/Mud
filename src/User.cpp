#include "User.h"
#include <sstream>
#include <cstring>
#include <iomanip>

User::User(const std::string& username, const std::string& passwordHash)
    : m_username(username), m_passwordHash(passwordHash) {
    m_createdTime = std::chrono::system_clock::now();
    m_lastLoginTime = m_createdTime;
}

bool User::verifyPassword(const std::string& password) const {
    // TODO: Implement proper password hashing and verification
    // For now, this is a simple comparison (NOT secure for production)
    return m_passwordHash == password;
}

void User::updateLastLogin() {
    m_lastLoginTime = std::chrono::system_clock::now();
}

std::string User::serialize() const {
    std::ostringstream oss;
    
    auto createdTimeT = std::chrono::system_clock::to_time_t(m_createdTime);
    auto lastLoginTimeT = std::chrono::system_clock::to_time_t(m_lastLoginTime);
    
    oss << m_username << "\n"
        << m_passwordHash << "\n"
        << createdTimeT << "\n"
        << lastLoginTimeT << "\n";
    
    return oss.str();
}

std::unique_ptr<User> User::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string username, passwordHash;
    std::time_t createdTimeT, lastLoginTimeT;
    
    if (!std::getline(iss, username) || 
        !std::getline(iss, passwordHash) ||
        !(iss >> createdTimeT) ||
        !(iss >> lastLoginTimeT)) {
        return nullptr;
    }
    
    auto user = std::make_unique<User>(username, passwordHash);
    user->m_createdTime = std::chrono::system_clock::from_time_t(createdTimeT);
    user->m_lastLoginTime = std::chrono::system_clock::from_time_t(lastLoginTimeT);
    
    return user;
}
