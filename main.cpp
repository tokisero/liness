#include <QApplication>
#include "LoginPage.h"
#include "AdminPanell.h"
#include "UserPanell.h"
#include "DataBase.h"

template <typename CurrentPanelType, typename NewPanelType>
void switchPanel(CurrentPanelType* currentPanel, NewPanelType* newPanel) {
    currentPanel->hide();
    newPanel->show();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    DataBase db("flight.db");
    LoginPage* loginPage = new LoginPage(&db, nullptr);
    AdminPanell adminPanel(&db);

    UserPanell* userPanel = nullptr;

    QObject::connect(loginPage, &LoginPage::adminLoginSuccessful, [&]() {
        switchPanel(loginPage, &adminPanel);
    });

    QObject::connect(loginPage, &LoginPage::userLoginSuccessful, [&](int userId) {
        if (userPanel) {
            delete userPanel;
            userPanel = nullptr;
        }

        userPanel = new UserPanell(&db, userId, nullptr);
        QObject::connect(userPanel, &UserPanell::logoutRequested, [&]() {
            switchPanel(userPanel, loginPage);
            delete userPanel;
            userPanel = nullptr;
        });

        switchPanel(loginPage, userPanel);
    });

    QObject::connect(&adminPanel, &AdminPanell::logoutRequested, [&]() {
        switchPanel(&adminPanel, loginPage);
    });

    loginPage->show();
    return app.exec();
}
