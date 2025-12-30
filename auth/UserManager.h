#pragma once
#include <string>
#include <vector>
#include "User.h"

class UserManager {
public:
    UserManager(const std::string& usersCsvPath);

    User registerUser(const std::string& fullName,
                      const std::string& email,
                      const std::string& passwordPlain);

    User login(const std::string& username,
               const std::string& passwordPlain);

    bool resetPassword(const std::string& fullName,
                       const std::string& email,
                       const std::string& newPasswordPlain);

private:
    std::string usersCsvPath;
    std::vector<User> users;

    void loadUsers();

    void appendUserToFile(const User& u);
    void writeAllUsersToFile();

    std::string generateUniqueUsername10();

    bool existsDuplicate(const std::string& fullName,
                         const std::string& email);
    bool usernameExists(const std::string& username);
    bool isValidEmail(const std::string& email);

    static std::string hashPassword(const std::string& plain);
};
