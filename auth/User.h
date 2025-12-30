#pragma once
#include <string>

class User {
public:
    User();

    User(const std::string& username,
         const std::string& fullName,
         const std::string& email,
         const std::string& passwordHash)
        : username(username),
          fullName(fullName),
          email(email),
          passwordHash(passwordHash) {}

    const std::string& getUsername() const { return username; }
    const std::string& getFullName() const { return fullName; }
    const std::string& getEmail() const { return email; }
    const std::string& getPasswordHash() const { return passwordHash; }

    void setPasswordHash(const std::string& newHash) { passwordHash = newHash; }
private:
    std::string username;
    std::string fullName;
    std::string email;
    std::string passwordHash;
};
