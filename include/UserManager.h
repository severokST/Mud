#pragma once

#include "Typedef.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

class User;

/**
 * Manages user accounts - loading, saving, authentication
 */
class UserManager {
public:
    UserManager();
    ~UserManager();

    // User operations
    std::shared_ptr<User> authenticateUser(const std::string& username, const std::string& password);
    std::shared_ptr<User> createUser(const std::string& username, const std::string& password);
    eUserStatus_t userExists(const std::string& username) const;
    
    // Data persistence
    bool loadUser(const std::string& username);
    bool saveUsers();

private:
    std::string hashPassword(const std::string& password) const;
    std::string getUserFilePath(const std::string& username) const;
    bool removeUser(const std::string& username);
    
    mutable std::mutex m_usersMutex;
    std::unordered_map<std::string, std::shared_ptr<User>> m_users;
    std::string m_dataDirectory;
};
