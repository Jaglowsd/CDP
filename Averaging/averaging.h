#ifndef AVERAGING_H
#define AVERAGING_H

#include <QObject>
#include <QLineEdit>
#include <QString>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDebug>

class Averaging : public QWidget{

    Q_OBJECT

public:
    Averaging(QVBoxLayout *parent);

    QLineEdit *minTime;
    QLineEdit *scaledTime;
    QComboBox *units;

    void setMinTime(QString); // set min time to be displayed
    int getScaledTime(); // Retrieve scaled time from line edit
    QString getUnits(); // Retrieve untis from combo box
};

#endif // AVERAGING_H
