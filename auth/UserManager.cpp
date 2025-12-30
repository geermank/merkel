#include "UserManager.h"
#include "../CSVReader.h"
#include <fstream>
#include <random>
#include <stdexcept>
#include <iostream>

UserManager::UserManager(const std::string& usersCsvPath)
    : usersCsvPath(usersCsvPath) {
    loadUsers();
}

void UserManager::loadUsers() {
    users = CSVReader::readUsersCSV(usersCsvPath);
}

std::string UserManager::hashPassword(const std::string& plain) {
    std::hash<std::string> hasher;
    return std::to_string(hasher(plain));
}

bool UserManager::existsDuplicate(const std::string& fullName, const std::string& email) {
    for (const User& u : users) {
        if (u.getFullName() == fullName && u.getEmail() == email) return true;
    }
    return false;
}

bool UserManager::usernameExists(const std::string& username) {
    for (const User& u : users) {
        if (u.getUsername() == username) return true;
    }
    return false;
}

bool UserManager::isValidEmail(const std::string &email) {
    // simple/naive email validation. There are much more cases to validate
    // but for the sake of simplicity, just verify the string contains an @ symbol
    for (const char& c : email) {
        if (c == '@') {
            return true;
        }
    }
    return false;
}

std::string UserManager::generateUniqueUsername10() {
    // this code was obtained by using stackoverflow
    // https://stackoverflow.com/questions/35978987/how-to-generate-n-digit-random-numbers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 9);

    while (true) {
        std::string id;
        for (int i = 0; i < 10; i++) {
            id.push_back(char('0' + dist(gen)));
        }
        if (id[0] == '0') continue;
        if (!usernameExists(id)) return id;
    }
}

void UserManager::appendUserToFile(const User& u) {
    // used the help of stackoverflow:
    // https://stackoverflow.com/questions/71883343/how-to-write-to-file-in-c
    // but use app to actually append data
    std::ofstream usersFile(usersCsvPath, std::ios::app);
    if (!usersFile.is_open())
    {
        std::cout << "Can't open the users file to append new user" << std::endl;
        throw std::exception();
    }

    usersFile << CSVReader::userToCSV(u) << std::endl;
}

void UserManager::writeAllUsersToFile()
{
    // use truc mode to delete and rewrite
    std::ofstream usersFile(usersCsvPath, std::ios::trunc);
    if (!usersFile.is_open()) {
        std::cout << "Can't open the users file to rewrite" << std::endl;
        throw std::exception();
    }

    for (const User& u : users) {
        usersFile << CSVReader::userToCSV(u) << std::endl;
    }
}

User UserManager::registerUser(const std::string& fullName,
                               const std::string& email,
                               const std::string& passwordPlain) {
    if (fullName.empty() || email.empty() || passwordPlain.empty()) {
        std::cout << "Invalid input, it cannot be empty" << std::endl;
        throw std::exception();
    }
    if (!isValidEmail(email)) {
        std::cout << "Invalid email" << std::endl;
        throw std::exception();
    }

    if (existsDuplicate(fullName, email)) {
        std::cout << "Name and email already registered" << std::endl;
        throw std::exception();
    }

    std::string username = generateUniqueUsername10();
    std::string passHash = hashPassword(passwordPlain);

    User u{username, fullName, email, passHash};
    users.push_back(u);
    appendUserToFile(u);
    return u;
}

User UserManager::login(const std::string& username,
                        const std::string& passwordPlain) {
    std::string passwordHash = hashPassword(passwordPlain);
    for (const User& u : users) {
        if (u.getUsername() == username && u.getPasswordHash() == passwordHash) {
            return u;
        }
    }
    std::cout << "There is no user matching your credentials. Please, check them and try again." << std::endl;
    throw std::exception();
}

bool UserManager::resetPassword(const std::string& fullName,
                                const std::string& email,
                                const std::string& newPasswordPlain) {
    if (fullName.empty() || email.empty() || newPasswordPlain.empty()) {
        std::cout << "Invalid input, it cannot be empty" << std::endl;
        return false;
    }

    bool updated = false;
    for (User& u : users) {
        if (u.getFullName() == fullName && u.getEmail() == email) {
            u.setPasswordHash(hashPassword(newPasswordPlain));
            updated = true;
            break;
        }
    }

    if (updated) writeAllUsersToFile();
    return updated;
}
