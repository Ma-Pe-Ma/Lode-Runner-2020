#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>
#include "configreader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(654, 440);
    //setWindowFlags(/*Qt::Dialog |*/ Qt::MSWindowsFixedSizeDialogHint);    

    connect(ui->resolutionWidget, &ResolutionWidget::showError, this, &MainWindow::showError);
    connect(ui->otherSettings, &OtherSettings::showError, this, &MainWindow::showError);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showError(QString text)
{
    QMessageBox* qMessageBox = new QMessageBox;
    qMessageBox->setWindowTitle("Invalid value!");
    qMessageBox->setWindowIcon(QIcon(":/app/Icon"));
    qMessageBox->setText(text);
    qMessageBox->show();
}

void MainWindow::on_launchButton_clicked()
{
    ui->gameModeWidget->setSettings();

    if (ui->resolutionWidget->valuesValid() && ui->otherSettings->valuesValid())
    {
        if (ConfigReader::write())
        {
            QString path = QCoreApplication::applicationDirPath()+"/Lode Runner.exe";
            startup(path.toUtf8().constData());
            QCoreApplication::quit();
        }
    }
}

void MainWindow::on_resetButton_clicked()
{
    ui->gameModeWidget->resetSettings();
    ui->resolutionWidget->resetSettings();
    ui->otherSettings->resetSettings();
}

void MainWindow::startup(LPCSTR lpApplicationName)
{
    // additional information
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcessA
    (
        lpApplicationName,   // the path
        NULL,                // Command line
        NULL,                   // Process handle not inheritable
        NULL,                   // Thread handle not inheritable
        FALSE,                  // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE,     // Opens file in a separate console
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi           // Pointer to PROCESS_INFORMATION structure
    );

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
