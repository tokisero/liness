#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QString>
#include "UserPanell.h"
#include "ManagerException.h"
#include "DataBase.h"

class LoginPage : public QWidget {
    Q_OBJECT

public:
    explicit LoginPage(DataBase* db, QWidget* parent);
    ~LoginPage() = default;

signals:
    void adminLoginSuccessful();
    void userLoginSuccessful(int userId);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void checkInputFields();

private:
    QLineEdit* usernameLineEdit;
    QLineEdit* passwordLineEdit;
    QPushButton* registerButton;
    QPushButton* loginButton;
    DataBase* database;
};
