#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

#include "gamemodewidget.h"
#include "resolutionwidget.h"
#include "othersettings.h"
#include "Windows.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVBoxLayout* verticalLayout;
    QLabel* imageLabel;
    GameModeWidget* gameModeWidget;
    ResolutionWidget* resolutionWidget;
    OtherSettings* otherSettings;

    QPushButton* resetButton;
    QPushButton* launchButton;

    void startup(LPCSTR lpApplicationName);


private slots:
    void launchGame();
    void resetSettings();
    void showError(QString);

signals:

};
#endif // MAINWINDOW_H
