#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "fileselect.h"

#include <QVector>
#include <QMap>
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
    DataHandler(QVector<FileSelect*>*, QVector<int>*, int, QString, int, QString); // Constructor

    QVector<QVector<QString>> fileData; // Data stored for each file
    QMap<QString, QVector<QString>> fileDataMap; // Key is the filetype and value is the data
    QVector<QVector<QString>> fileHeaders; // Headers stored for each file
    QVector<QDateTime> startTimes; // Earliest date time for each file
    QList<QString> fileTypes;

    QVector<FileSelect*> *fSList;
    QVector<int> *resolutions;
    int timeResolution;
    QString saveAfterAverage;
    QString saveBeforeAverage;
    QString units;
    int timeScale;

    QDateTime startTimeBlock;
    QDateTime endTimeBlock;

    void reFormatSave(QString); // Modify save file name to differentiate between before/after averaging output

    bool readFiles(); // Open files and send instructions for reading file type
    void recordFileData(QTextStream*, QRegularExpression, QString); // Saves data by header and data
    bool pullStartTime(QString, int); // Stores start time for each file

    bool findStartTime(); // Find the earliest time among all files
    void addDateApollo(); // Add date header and date data to Apollo files

    bool printHeaders(); // Prints file name and headers of given files
    int countCommas(QString); // How many commas needed for even spacing in output 

    bool gatherTimeBlockData(); // Determine data that fits into a time block
    void scaledByTimeResolution(); // Modify time resolution to match user input time scale
    bool checkRange(QString, QVector<QString>::iterator, int); // Check if current line of data satisfies time block

    void addDataToBlockBefore(QVector<QVector<QString>>*, QFile*, int); // Adds data to time block/prints block to before averaging save file

    void addDataToBlockAfter(QVector<QVector<QString>>*, QFile*); // Averages data in time block/prints to after output file
    bool edgeCases(QVector<QStringList>*, int, int); // Handles data that does not need to be averaged: sample number, date, and time
    QString averageData(QVector<QStringList>*, int); // Averages column of data

    void newTimeBlock(); // Establish new time block

};

#endif // DATAHANDLER_H
