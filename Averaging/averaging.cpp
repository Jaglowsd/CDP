#include "averaging.h"

Averaging::Averaging(QVBoxLayout *parent) {
    minTime = new QLineEdit();
    minTime->setReadOnly(true);
    minTime->setStyleSheet("border: 1px inset gray;");
    minTime->setStyleSheet("color: gray");

    scaledTime = new QLineEdit();
    scaledTime->setPlaceholderText(tr("time scale"));

    units = new QComboBox;
    units->addItem(tr("sec"));
    units->addItem(tr("min"));
    units->addItem(tr("hour"));
    units->addItem(tr("day"));
    units->setFixedWidth(50);

    QLabel *note = new QLabel(tr("<b> Averaging: </b>"));

    note->setToolTip("<ul><li>Min time is the minimum time allowed to format the given files (Value changes depending on the files given)"
                     "</li> \n<li>Time scale is a multiplier for the time that the files should be formatted to</li> "
                     "\n<li>Select the units the time should be formatted to</li></ul>");
    //note->setStyleSheet("border: 1px solid black");
    note->setFixedWidth(85);
    QLabel *minLabel = new QLabel(tr("Minimum Time:"));
    minLabel->setStyleSheet("color: gray");
    minLabel->setFixedWidth(80);
    QLabel *timeLabel = new QLabel(tr("Time Scale:"));
    timeLabel->setFixedWidth(80);
    QLabel *unitsLabel = new QLabel(tr("Units:"));
    unitsLabel->setFixedWidth(80);

    QHBoxLayout *labels = new QHBoxLayout();
    QSpacerItem *labelSpacer1 = new QSpacerItem(0,0);
    QSpacerItem *labelSpacer2 = new QSpacerItem(260,0);
    labels->addWidget(note);
    labels->addSpacerItem(labelSpacer1);
    labels->addWidget(minLabel);
    labels->addWidget(timeLabel);
    labels->addWidget(unitsLabel);
    labels->addSpacerItem(labelSpacer2);

    QHBoxLayout *layout = new QHBoxLayout();
    QSpacerItem *spacer = new QSpacerItem(300, 0);
    QSpacerItem *spacer2 = new QSpacerItem(100, 0);
    layout->setContentsMargins(0,0,0,0);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addSpacerItem(spacer2);
    layout->addWidget(minTime);
    layout->addWidget(scaledTime);
    layout->addWidget(units);
    layout->addSpacerItem(spacer);

    parent->addLayout(labels);
    parent->addLayout(layout);
}

void Averaging::setMinTime(QString str) {
    minTime->setText(str);
}

int Averaging::getScaledTime() {
    if(scaledTime->text() != "")
        return scaledTime->text().toInt();
    else
        return -1;
}

QString Averaging::getUnits() {
    int index = units->currentIndex();
    return units->itemText(index);
}
