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
    DataHandler(QVector<FileSelect*>*, QVector<int>*, QStringList*, int, QString, int, QString); // Constructor

    QVector<QVector<QString>> fileData; // Data stored for each file
    QMap<QString, QVector<QString>> fileDataMap;
    QVector<QVector<QString>> fileHeaders; // Headers stored for each file
    QMap<QString, QVector<QString>> fileHeadersMap;
    QVector<QDateTime> startTimes; // Earliest date time for each file

    QVector<FileSelect*> *fSList;
    QStringList *fileTypes;
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
    bool printHeaders(); // Prints file name and headers of given files
    int countCommas(QString); // How many commas needed for even spacing for each file
    bool findStartTime(); // Find the earliest time among all files
    void scaledByTimeResolution(); // Modify time resolution to match user input time scale
    bool gatherTimeBlockData(); // determine data that fits into a time block
    bool checkRange(QMap<QString, QVector<QString>>::iterator); // Check if file iterator satisfies time block
    bool addDataToBlock(QMap<QString, QVector<QString>>::iterator); // Adds data to time block/prints block to save files

};

#endif // DATAHANDLER_H
