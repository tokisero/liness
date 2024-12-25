#ifndef MANAGEREXCEPTION_H
#define MANAGEREXCEPTION_H

#include <exception>
#include <QString>
#include <QMessageBox>
#include <string>

class ManagerException : public std::exception {
public:
    ManagerException(const QString& message) : message(message.toStdString()) {}
    virtual ~ManagerException() noexcept = default;

    const char* what() const noexcept override {
        return message.c_str();
    }
    virtual void showErrorMessage() const = 0;
protected:
    std::string message;
};


class DatabaseException : public ManagerException {
public:
    DatabaseException(const QString& message) : ManagerException(message) {}
    virtual ~DatabaseException() noexcept = default;

    void showErrorMessage() const {
        QMessageBox::critical(nullptr, "Database error", what());
    }
};

class InputException : public ManagerException {
public:
    InputException(const QString& message) : ManagerException(message) {}
    virtual ~InputException() noexcept = default;

    void showErrorMessage() const {
        QMessageBox::critical(nullptr, "Input error", what());
    }
};

class GeneralException : public ManagerException {
public:
    GeneralException(const QString& message) : ManagerException(message) {}
    virtual ~GeneralException() noexcept = default;

    void showErrorMessage() const {
        QMessageBox::warning(nullptr, "Error", what());
    }
};

class Warning : public ManagerException {
public:
    Warning(const QString& message) : ManagerException(message) {}
    virtual ~Warning() noexcept = default;

    void showErrorMessage() const {
        QMessageBox::warning(nullptr, "Warning: ", what());
    }
};

class LoginException : public ManagerException {
public:
    LoginException(const QString& message) : ManagerException(message) {}
    virtual ~LoginException() noexcept = default;

    void showErrorMessage() const {
        QMessageBox::warning(nullptr, "Login failed", what());
    }
};

#endif // MANAGEREXCEPTION_H
