#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>
#include <QMessageBox>
#include <QToolTip>
#include "configreader.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
	
    setWindowTitle("Lode Runner Launcher");
    setWindowIcon(QIcon(":/app/Icon"));

    setFixedSize(640, 420);
    setWindowFlags(/*Qt::Dialog |*/ Qt::MSWindowsFixedSizeDialogHint);
    statusBar()->setSizeGripEnabled(false);

    QWidget* centralWidget = new QWidget;
    setCentralWidget(centralWidget);

	verticalLayout = new QVBoxLayout;

    imageLabel = new QLabel;
    QPixmap image(":/app/Title");
    int imageWidth = imageLabel->width();
    int imageHeight= imageLabel->height();

    //imageLabel->setPixmap(image.scaled(imageWidth, imageHeight, Qt::KeepAspectRatio));
    imageLabel->setPixmap(image.scaledToWidth(640));
    imageLabel->setMargin(0);
    //imageLabel->setPixmap(image);
    verticalLayout->addWidget(imageLabel);
    verticalLayout->setAlignment(imageLabel, Qt::AlignHCenter);

    //QHBoxLayout* gameModeLayout = new QHBoxLayout;

    gameModeWidget = new GameModeWidget;
    //gameModeLayout->addWidget(gameModeWidget);

    //verticalLayout->addLayout(gameModeLayout);
    verticalLayout->addWidget(gameModeWidget);

    resolutionWidget = new ResolutionWidget;
    connect(resolutionWidget, &ResolutionWidget::showError, this, &MainWindow::showError);
    verticalLayout->addWidget(resolutionWidget);

    otherSettings = new OtherSettings;
    connect(otherSettings, &OtherSettings::showError, this, &MainWindow::showError);
    verticalLayout->addWidget(otherSettings);


    QHBoxLayout* buttons = new QHBoxLayout;
    buttons->addSpacing(15);

    resetButton = new QPushButton("Reset Settings");
    connect(resetButton, &QPushButton::pressed, this, &MainWindow::resetSettings);
    buttons->addWidget(resetButton);
    buttons->setAlignment(resetButton, Qt::AlignLeft);

    launchButton = new QPushButton("Launch");
    connect(launchButton, &QPushButton::pressed, this, &MainWindow::launchGame);
    buttons->addWidget(launchButton);
    buttons->setAlignment(launchButton, Qt::AlignRight);
    buttons->addSpacing(15);

    verticalLayout->addLayout(buttons);
    centralWidget->setLayout(verticalLayout);
}

void MainWindow::launchGame() {
    gameModeWidget->setSettings();

    if (resolutionWidget->valuesValid() && otherSettings->valuesValid()) {
        if (ConfigReader::write()) {
            QString path = QCoreApplication::applicationDirPath()+"/Lode Runner.exe";
            startup(path.toUtf8().constData());
            QCoreApplication::quit();
        }
    }
}

void MainWindow::resetSettings() {
    gameModeWidget->resetSettings();
    resolutionWidget->resetSettings();
    otherSettings->resetSettings();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::showError(QString text) {
    QMessageBox* qMessageBox = new QMessageBox;
    qMessageBox->setWindowTitle("Invalid value!");
    qMessageBox->setWindowIcon(QIcon(":/app/Icon"));
    qMessageBox->setText(text);
    qMessageBox->show();
}

void MainWindow::startup(LPCSTR lpApplicationName) {
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
