#include "gamemodewidget.h"
#include <QRadioButton>
#include <QVBoxLayout>
#include "configreader.h"

GameModeWidget::GameModeWidget(QWidget *parent) : QWidget(parent)
{
    this->coverGroup = createCoverGroup();
    this->gameGroup = createGameGroup();

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(gameGroup);
    hLayout->addWidget(coverGroup);

    setLayout(hLayout);
}

QGroupBox* GameModeWidget::createGameGroup()
{
    QGroupBox* gameGroup = new QGroupBox;
    gameGroup->setTitle(tr("Levelset:"));

    originalButton = new QRadioButton(tr("Original"));
    championshipButton = new QRadioButton(tr("Championship"));
    connect(originalButton, &QRadioButton::toggled, this, &GameModeWidget::originalChecked);
    this->coverGroup->setEnabled(false);

    if (ConfigReader::levelset)
    {
        championshipButton->setChecked(true);
    }
    else
    {
        originalButton->setChecked(true);
    }

    QHBoxLayout *vbox = new QHBoxLayout;
    vbox->addWidget(originalButton);
    vbox->addWidget(championshipButton);
    vbox->addStretch(1);
    gameGroup->setLayout(vbox);

    return gameGroup;
}

QGroupBox* GameModeWidget::createCoverGroup()
{
    QGroupBox* coverGroup = new QGroupBox;
    coverGroup->setTitle(tr("Cover:"));

    classicButton = new QRadioButton(tr("Classic"));
    usButton = new QRadioButton(tr("US"));

    if (ConfigReader::usCover)
    {
        usButton->setChecked(true);
    }
    else
    {
        classicButton->setChecked(true);
    }

    QHBoxLayout *vbox = new QHBoxLayout;
    vbox->addWidget(classicButton);
    vbox->addWidget(usButton);
    vbox->addStretch(1);
    coverGroup->setLayout(vbox);

    return coverGroup;
}

void GameModeWidget::originalChecked(bool checked)
{
    this->coverGroup->setEnabled(checked);

    if (checked)
    {
        //this->coverGroup->show();
    }
    else
    {
        //this->coverGroup->hide();
    }
}

void GameModeWidget::resetSettings()
{
    originalButton->setChecked(true);
    classicButton->setChecked(true);
}

void GameModeWidget::setSettings()
{
    if(originalButton->isChecked())
    {
        ConfigReader::levelset = false;
    }
    else
    {
        ConfigReader::levelset = true;
    }

    if(classicButton->isChecked())
    {
        ConfigReader::usCover = false;
    }
    else
    {
        ConfigReader::usCover = true;
    }
}
