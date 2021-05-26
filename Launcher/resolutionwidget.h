#ifndef RESOLUTIONWIDGET_H
#define RESOLUTIONWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include "labeledlineedit.h"
#include <QRadioButton>

class ResolutionWidget : public QWidget {
    Q_OBJECT
public:
    explicit ResolutionWidget(QWidget *parent = nullptr);
    bool valuesValid();
    void resetSettings();
signals:
    void showError(QString);

private slots:
    void customToggled(bool);

private:
    QRadioButton *radio1;
    QRadioButton *radio2;
    QRadioButton *radio3;
    QRadioButton *radio4;
    QRadioButton *radio5;

    LabeledLineEdit* widthEdit;
    LabeledLineEdit* heightEdit;
    QGroupBox* resolutionGroup;

};

#endif // RESOLUTIONWIDGET_H
