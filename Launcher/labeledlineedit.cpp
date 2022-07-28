#include "labeledlineedit.h"
#include <QHBoxLayout>

LabeledLineEdit::LabeledLineEdit(QWidget *parent) : QWidget(parent)
{
    label = new QLabel;
    lineEdit = new QLineEdit;
    lineEdit->setFixedWidth(30);

    QHBoxLayout* horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(label);
    horizontalLayout->addWidget(lineEdit);

    setLayout(horizontalLayout);
}

void LabeledLineEdit::setLabelText(QString text)
{
    label->setText(text);
}

void LabeledLineEdit::setValueText(QString text)
{
    lineEdit->setText(text);
}

void LabeledLineEdit::setValidator(QValidator* validator)
{
    lineEdit->setValidator(validator);
}

QString LabeledLineEdit::getValue()
{
    return lineEdit->text();
}
