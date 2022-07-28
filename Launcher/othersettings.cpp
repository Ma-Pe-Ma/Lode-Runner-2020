#include "othersettings.h"
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>
#include "configreader.h"
#include <QGroupBox>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QDebug>

OtherSettings::OtherSettings(QWidget *parent) : QWidget(parent)
{
    QGroupBox* groupBox = new QGroupBox;
    groupBox->setTitle(tr("Other settings:"));

    QHBoxLayout* upper = new QHBoxLayout;
    upper->addWidget(groupBox);
    setLayout(upper);

    QHBoxLayout* qhBoxLayout = new QHBoxLayout;
    groupBox->setLayout(qhBoxLayout);
    //setLayout(qhBoxLayout);

    levelEdit = new LabeledLineEdit;
    levelEdit->setLabelText(tr("Starting level: "));
    levelEdit->setValueText(QLocale::system().toString(ConfigReader::startingLevel));
    levelEdit->setValidator(new QIntValidator(1,150));
    qhBoxLayout->addWidget(levelEdit);
    qhBoxLayout->setAlignment(levelEdit, Qt::AlignLeft);

    playerSpeedEdit = new LabeledLineEdit;
    playerSpeedEdit->setLabelText(tr("Player speed: "));
    playerSpeedEdit->setValueText(QLocale::system().toString(ConfigReader::playerSpeed));
    playerSpeedEdit->setValidator(new QDoubleValidator(0.0f,10.0f,3));
    qhBoxLayout->addWidget(playerSpeedEdit);
    qhBoxLayout->setAlignment(playerSpeedEdit, Qt::AlignLeft);

    enemySpeedEdit = new LabeledLineEdit;
    enemySpeedEdit ->setLabelText(tr("Enemy speed: "));
    enemySpeedEdit->setValueText(QLocale::system().toString(ConfigReader::enemySpeed));
    enemySpeedEdit->setValidator(new QDoubleValidator(0.0f,10.0f,3));
    qhBoxLayout->addWidget(enemySpeedEdit);
    qhBoxLayout->setAlignment(enemySpeedEdit, Qt::AlignLeft);

    fpsEdit = new LabeledLineEdit;
    fpsEdit->setLabelText(tr("FPS: "));
    fpsEdit->setValueText(QLocale::system().toString(ConfigReader::fps));
    fpsEdit->setValidator(new QIntValidator(24,240));
    qhBoxLayout->addWidget(fpsEdit);
    qhBoxLayout->setAlignment(fpsEdit, Qt::AlignLeft);

    recordHeightEdit = new LabeledLineEdit;
    recordHeightEdit->setLabelText(tr("Rec. height: "));
    recordHeightEdit->setValueText(QLocale::system().toString(ConfigReader::recordingHeight));
    recordHeightEdit->setValidator(new QIntValidator(1,3000));
    qhBoxLayout->addWidget(recordHeightEdit);
    qhBoxLayout->setAlignment(recordHeightEdit, Qt::AlignLeft);
}

bool OtherSettings::valuesValid()
{
    bool ok = true;
    int fpsValue = QLocale::system().toInt(fpsEdit->getValue(), &ok);

    if (!ok)
    {
        emit showError(tr("FPS value is not valid!"));
        return false;
    }

    ok = true;
    int startValue = QLocale::system().toInt(levelEdit->getValue(), &ok);

    if (!ok)
    {
        emit showError(tr("Starting level value is not valid!!"));
        return false;
    }

    ok = true;
    float playerValue = QLocale::system().toDouble(playerSpeedEdit->getValue(), &ok);

    if (!ok)
    {
        emit showError(tr("Player speed value is not valid!!"));
        return false;
    }

    ok = true;
    float enemyValue = QLocale::system().toDouble(enemySpeedEdit->getValue(), &ok);

    if (!ok)
    {
        emit showError(tr("Enemy speed value is not valid!"));
        return false;
    }

    ok = true;
    float recordingValue = QLocale::system().toInt(recordHeightEdit->getValue(), &ok);

    if (!ok)
    {
        emit showError(tr("Recording height value is not valid!"));
        return false;
    }

    ConfigReader::fps = fpsValue;
    ConfigReader::startingLevel = startValue;
    ConfigReader::playerSpeed = playerValue;
    ConfigReader::enemySpeed = enemyValue;
    ConfigReader::recordingHeight = recordingValue;

    return true;
}

void OtherSettings::resetSettings()
{
    levelEdit->setValueText(QLocale::system().toString(1));
    fpsEdit->setValueText(QLocale::system().toString(60));
    recordHeightEdit->setValueText(QLocale::system().toString(720));

    playerSpeedEdit->setValueText(QLocale::system().toString(0.9f));
    enemySpeedEdit->setValueText(QLocale::system().toString(0.415f));
}
