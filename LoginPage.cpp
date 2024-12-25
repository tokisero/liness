#include <QMessageBox>
#include <QLabel>
#include <QIcon>
#include "LoginPage.h"

LoginPage::LoginPage(DataBase* db, QWidget* parent)
    : database(db), QWidget(parent) {
    setWindowTitle("Login");
    setFixedSize(400, 300);

    QLabel* titleLabel = new QLabel("Welcome to Flight Manager", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin-bottom: 20px;");

    usernameLineEdit = new QLineEdit(this);
    usernameLineEdit->setPlaceholderText("Username");
    usernameLineEdit->setStyleSheet(
        "border: 1px solid gray; border-radius: 5px; padding: 5px; font-size: 14px;");

    passwordLineEdit = new QLineEdit(this);
    passwordLineEdit->setPlaceholderText("Password");
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setStyleSheet(
        "border: 1px solid gray; border-radius: 5px; padding: 5px; font-size: 14px;");

    loginButton = new QPushButton("Login", this);
    loginButton->setEnabled(false);
    loginButton->setStyleSheet(
        "background-color: #0c73fe; color: white; border: none; border-radius: 5px; padding: 10px;"
        "font-size: 14px;"
        "hover { background-color: #45a049; }");

    registerButton = new QPushButton("Register", this);
    registerButton->setEnabled(false);
    registerButton->setStyleSheet(
        "background-color: #0c73fe; color: white; border: none; border-radius: 5px; padding: 10px;"
        "font-size: 14px;"
        "hover { background-color: #007bb5; }");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(10);
    layout->setContentsMargins(30, 20, 30, 20);
    layout->addWidget(titleLabel);
    layout->addWidget(usernameLineEdit);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    setLayout(layout);

    // Подключение сигналов
    connect(loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(usernameLineEdit, &QLineEdit::textChanged, this, &LoginPage::checkInputFields);
    connect(passwordLineEdit, &QLineEdit::textChanged, this, &LoginPage::checkInputFields);
}

void LoginPage::onLoginClicked() {
    try {
        QString username = usernameLineEdit->text();
        QString password = passwordLineEdit->text();

        if (database->authenticateUser(username, password)) {
            int userId = database->getUserIdByUsername(username);
            int role = database->getUserRole(username);
            if (role == 0) {
                emit userLoginSuccessful(userId);
            } else if (role == 1) {
                emit adminLoginSuccessful();
            }
        } else {
            throw LoginException("Invalid username or password.");
        }
        passwordLineEdit->clear();
    } catch (const ManagerException& ex) {
        ex.showErrorMessage();
    }
}

void LoginPage::onRegisterClicked() {
    try {
        QString username = usernameLineEdit->text();
        QString password = passwordLineEdit->text();
        if (!username.isEmpty() && !password.isEmpty()) {
            database->registerUser(username, password, 0);
        } else {
            throw LoginException("Please enter valid username and password.");
        }
        passwordLineEdit->clear();
    } catch (const ManagerException& ex) {
        ex.showErrorMessage();
    }
}

void LoginPage::checkInputFields() {
    bool isUsernameEmpty = usernameLineEdit->text().trimmed().isEmpty();
    bool isPasswordEmpty = passwordLineEdit->text().trimmed().isEmpty();
    loginButton->setEnabled(!isUsernameEmpty && !isPasswordEmpty);
    registerButton->setEnabled(!isUsernameEmpty && !isPasswordEmpty);
}
