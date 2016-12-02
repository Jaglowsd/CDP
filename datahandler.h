#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "fileselect.h"

#include <QVector>
#include <QString>
#include <QObject>
#include <QStringList>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <QCharRef>

class DataHandler : public QObject
{
    Q_OBJECT

public:
    DataHandler(QVector<FileSelect*>*, QVector<int>*, QStringList*, int, QString, int, QString); // Constructor

    QVector<QVector<QString>> fileData;
    QVector<QVector<QString>> fileHeaders;
    QVector<QDateTime> startTimes;

    QVector<FileSelect*> *fSList;
    QStringList *fileTypes;
    QVector<int> *resolutions;
    int timeResolution;
    QString saveDestination;
    QString units;
    int timeScale;

    QDateTime startTime;
    QDateTime endTimeBlock;

    bool readFiles(); // Open files and send instructions for reading file type
    void recordFileData(QTextStream*, QRegularExpression); // Saves data by header and data
    bool pullStartTime(QString, int); // Reads first line of data of each data set and finds the earliest time, set as start time
    bool printHeaders(); // Prints file name and headers of given files
    int countCommas(QString); // How many commas needed for even spacing for each file
    bool findStartTime();
    int scaledByTimeResolution();

    bool findTimeBlock();

};

#endif // DATAHANDLER_H
