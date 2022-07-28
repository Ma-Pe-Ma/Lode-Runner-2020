#ifndef OTHERSETTINGS_H
#define OTHERSETTINGS_H

#include <QWidget>
#include <QLineEdit>
#include "labeledlineedit.h"

class OtherSettings : public QWidget
{
    Q_OBJECT
public:
    explicit OtherSettings(QWidget *parent = nullptr);
    bool valuesValid();
    void resetSettings();
signals:
    void showError(QString);
private:
    LabeledLineEdit* levelEdit;
    LabeledLineEdit* fpsEdit;
    LabeledLineEdit* recordHeightEdit;
    LabeledLineEdit* playerSpeedEdit;
    LabeledLineEdit* enemySpeedEdit;
};

#endif // OTHERSETTINGS_H
