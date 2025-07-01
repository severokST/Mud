#include "UserManager.h"
#include "User.h"
#include <fstream>
#include <filesystem>
#include <iostream>

UserManager::UserManager() : m_dataDirectory("data/users/") {
    // Create data directory if it doesn't exist
    std::filesystem::create_directories(m_dataDirectory);
}

UserManager::~UserManager() {
    saveUsers();
}

std::shared_ptr<User> UserManager::loginUser(const std::string& username, const std::string& password) {
    // Load user if not already loaded
    if (!loadUser(username)) {
        return nullptr; // User does not exist or failed to load
    }

    // Authenticate user
    if (authenticateUser(username, password)) {
        m_users[username]->loginUser();
        return m_users[username];
    }

    // If authentication fails, remove user and return nullptr
    removeUser(username);
    return nullptr;
}

bool UserManager::authenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    auto it = m_users.find(username);
    if (it != m_users.end()) {
        if (it->second->verifyPassword(password)) {
            it->second->loginUser();
            return it->second;
        }
    }
    
    return nullptr;
}

std::shared_ptr<User> UserManager::createUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    // Check if user already exists
    if (m_users.find(username) != m_users.end()) {
        return nullptr;
    }
    
    // Create new user with hashed password
    std::string hashedPassword = hashPassword(password);
    auto user = std::make_shared<User>(username, hashedPassword);
    
    m_users[username] = user;
    
    // Save user to file
    std::string filePath = getUserFilePath(username);
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << user->serialize();
        file.close();
    }
    
    return user;
}

eUserStatus_t UserManager::userExists(const std::string& username) const {
    std::lock_guard<std::mutex> lock(m_usersMutex);

    // Check if user exists in the map if they are already logged in
    if (m_users.find(username) != m_users.end()) {
        return USER_STATUS_ACTIVE; // User is loaded and active
    }

    // Otherwise check if the user file exists
    std::string filePath = getUserFilePath(username);
    if (std::filesystem::exists(filePath)) {
        return USER_STATUS_INACTIVE; // User exists but not loaded
    }

    return USER_STATUS_DOES_NOT_EXIST; // User does not exist
}

/**
 * @brief Loads a user from the file system.
 *  - User is loaded on login or when explicitly requested.
 * 
 * @param username The username of the user to load.
 * @return true if the user was loaded successfully, false otherwise.
 */
bool UserManager::loadUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    // Check if user is already loaded
    if (m_users.find(username) != m_users.end()) {
        return true;
    }
    
    // Load user from file
    std::string filePath = getUserFilePath(username);
    if (!std::filesystem::exists(filePath)) {
        return false; // User file does not exist
    }   

    std::ifstream file(filePath);
    if (file.is_open()) {
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        auto user = User::deserialize(content);
        if (user) {
            m_users[username] = std::move(user);
            return true;
        }
    }

    return false;
}

bool UserManager::removeUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    // Check if user exists
    auto it = m_users.find(username);
    if (it == m_users.end()) {
        return false; // User does not exist
    }
    
    // Remove user from map 
    m_users.erase(it);
    
    return true;
}


bool UserManager::saveUsers() {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    try {
        for (const auto& pair : m_users) {
            std::string filePath = getUserFilePath(pair.first);
            std::ofstream file(filePath);
            if (file.is_open()) {
                file << pair.second->serialize();
                file.close();
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving users: " << e.what() << std::endl;
        return false;
    }
}

std::string UserManager::hashPassword(const std::string& password) const {
    // TODO: Implement proper password hashing (bcrypt, scrypt, etc.)
    // For now, this is just a placeholder (NOT secure for production)
    return password;
}

std::string UserManager::getUserFilePath(const std::string& username) const {
    return m_dataDirectory + username + ".user";
}
