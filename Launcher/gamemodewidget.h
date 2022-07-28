#ifndef GAMEMODEWIDGET_H
#define GAMEMODEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>

class GameModeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameModeWidget(QWidget *parent = nullptr);
    void resetSettings();
    void setSettings();

signals:
private:
    QGroupBox* gameGroup;
    QGroupBox* coverGroup;

    QGroupBox* createGameGroup();
    QGroupBox* createCoverGroup();

    QRadioButton* originalButton;
    QRadioButton* championshipButton;

    QRadioButton *classicButton ;
    QRadioButton *usButton;

private slots:
    void originalChecked(bool);
};

#endif // GAMEMODEWIDGET_H
