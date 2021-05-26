#include "resolutionwidget.h"
#include <QRadioButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include "configreader.h"

ResolutionWidget::ResolutionWidget(QWidget *parent) : QWidget(parent) {
    QHBoxLayout* upper = new QHBoxLayout;

    resolutionGroup = new QGroupBox;
    resolutionGroup->setTitle("Resolution:");

    QHBoxLayout *hbox = new QHBoxLayout;
    resolutionGroup->setLayout(hbox);

    radio1 = new QRadioButton(tr("1500×900"));
    radio2 = new QRadioButton(tr("750×450"));
    radio3 = new QRadioButton(tr("3000×1800"));
    radio4 = new QRadioButton(tr("1750×1050"));
    radio5 = new QRadioButton(tr("Custom: "));

    connect(radio5, &QRadioButton::toggled, this, &ResolutionWidget::customToggled);

    hbox->setSpacing(20);

    hbox->addWidget(radio1);
    hbox->addWidget(radio2);
    hbox->addWidget(radio3);
    hbox->addWidget(radio4);
    hbox->addWidget(radio5);

    hbox->addSpacing(-25);

    widthEdit = new LabeledLineEdit;
    widthEdit->setLabelText("W: ");
    widthEdit->setValueText(QLocale::system().toString(ConfigReader::width));
    widthEdit->setValidator(new QIntValidator(1,6000));
    hbox->addWidget(widthEdit);
    hbox->setAlignment(widthEdit, Qt::AlignLeft);
    hbox->addSpacing(-25);

    heightEdit = new LabeledLineEdit;
    heightEdit->setLabelText("× H: ");
    heightEdit->setValueText(QLocale::system().toString(ConfigReader::height));
    heightEdit->setValidator(new QIntValidator(1,6000));
    hbox->addWidget(heightEdit);
    hbox->setAlignment(heightEdit, Qt::AlignLeft);
    hbox->addStretch();

    upper->addWidget(resolutionGroup);
    setLayout(upper);
    //setLayout(hbox);

    customToggled(false);

    switch (ConfigReader::resolutionMode) {
        case 0:
        radio1->setChecked(true);
        break;

        case 1:
        radio2->setChecked(true);
        break;

        case 2:
        radio3->setChecked(true);
        break;

        case 3:
        radio4->setChecked(true);
        break;

        case 4:
        customToggled(true);
        radio5->setChecked(true);
        break;
    }
}


void ResolutionWidget::customToggled(bool toggled) {
    heightEdit->setEnabled(toggled);
    widthEdit->setEnabled(toggled);
}

bool ResolutionWidget::valuesValid() {
    if(radio1->isChecked()){
        ConfigReader::resolutionMode = 0;
    }
    else if(radio2->isChecked()){
        ConfigReader::resolutionMode = 1;
    }
    else if(radio3->isChecked()){
        ConfigReader::resolutionMode = 2;
    }
    else if(radio4->isChecked()){
        ConfigReader::resolutionMode = 3;
    }
    else if (radio5->isChecked()) {
        ConfigReader::resolutionMode = 4;

        bool ok = true;
        int widthValue = QLocale::system().toInt(widthEdit->getValue(), &ok);

        if (!ok) {
            emit showError("Width value is not correct!");
            return false;
        }
        else {
            ConfigReader::width = widthValue;
        }

        ok = true;
        int heightValue = QLocale::system().toInt(heightEdit->getValue(), &ok);

        if (!ok) {
            emit showError("Height value is not correct!");
            return false;
        }
        else {
            ConfigReader::height = heightValue;
        }
    }

    return true;
}

void ResolutionWidget::resetSettings() {
    radio4->setChecked(true);
    widthEdit->setValueText(QLocale::system().toString(1280));
    heightEdit->setValueText(QLocale::system().toString(720));
}
