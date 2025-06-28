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

std::shared_ptr<User> UserManager::authenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    auto it = m_users.find(username);
    if (it != m_users.end()) {
        if (it->second->verifyPassword(password)) {
            it->second->updateLastLogin();
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

bool UserManager::userExists(const std::string& username) const {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    return m_users.find(username) != m_users.end();
}

bool UserManager::loadUsers() {
    std::lock_guard<std::mutex> lock(m_usersMutex);
    
    if (!std::filesystem::exists(m_dataDirectory)) {
        return true; // No users to load, but that's okay
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(m_dataDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".user") {
                std::ifstream file(entry.path());
                if (file.is_open()) {
                    std::string content((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
                    file.close();
                    
                    auto user = User::deserialize(content);
                    if (user) {
                        m_users[user->getUsername()] = std::move(user);
                    }
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading users: " << e.what() << std::endl;
        return false;
    }
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
