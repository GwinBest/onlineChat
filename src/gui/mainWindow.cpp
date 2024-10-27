#include "mainWindow.h"

#include "gui/login/loginPage.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , _ui(new Ui::MainWindowClass())
{
    _ui->setupUi(this);

    _ui->stackedWidget->addWidget(new Gui::LoginPage());
}

MainWindow::~MainWindow()
{
    delete _ui;
}
