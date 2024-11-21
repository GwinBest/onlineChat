#include <functional>


#include "../src/userData/user.h"
#include "../src/userData/userCredentialsFile.h"
#include "../src/userData/userRepository.h"


#include "../src/gui/login/loginPage.h"

#include <QApplication>

#include "gui/mainWindow.h"

UserData::User currentUser;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyle("fusion");
    Gui::MainWindow window;
    window.show();

    return app.exec();
}