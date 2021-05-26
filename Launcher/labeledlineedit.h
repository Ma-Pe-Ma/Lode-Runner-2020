#ifndef LABELEDLINEEDIT_H
#define LABELEDLINEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>

class LabeledLineEdit : public QWidget {
    Q_OBJECT
public:
    explicit LabeledLineEdit(QWidget *parent = nullptr);
    void setLabelText(QString);
    void setValueText(QString);
    void setValidator(QValidator*);
    QString getValue();
signals:

private:
    QLabel* label;
    QLineEdit* lineEdit;
};

#endif // LABELEDLINEEDIT_H
