#ifndef UPDATEWINDOW_H
#define UPDATEWINDOW_H

#include "fileselect.h"
#include "datahandler.h"

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QString>
#include <QFile>
#include <QIODevice>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QTime>
#include <QDateTime>

class UpdateWindow: public QObject
{
    Q_OBJECT

public:
    UpdateWindow(); // Default Constructor
    UpdateWindow(QWidget *parent);

    QVector<int> resolutions = QVector<int>(6, -1);
    QVector<FileSelect*> fSList;
    QVector<QString> firstHeaders;
    QVector<QString> allHeaders;
    QStringList fileTypes;
    DataHandler *dataHandler;
    QWidget *parent;
    int timeResolution;
    QVector<QString> startTimes;
    QString saveDestination;

    bool createDataHandler(int, QString);
    bool handleData(); // Begins process of handling all data in DataHandler.cpp

    void addFileObject(FileSelect* fs); // Add file select object to list
    void removeFileObject(); // Remove file select object from list

    void setTimeResolution(int);
    int getTimeResolution();
    void setSaveDestination(QString);

    int findTimesToCompare(QString, QTextStream*); // Gathers times for comparison
    int compareTimes(QString, QString, int); // Compares two date time values to find time resolution

    void clear(); // Called when remove button is signaled to clear all changes made to the window
                    // Sets all resolutions to -1, indicating there is no min time to be displayed

    void update(QString); // When textChanged() on a line edit objects is signaled
                          // Caller is update(QString str) in MainWindow

    void minimumTime(); // Find the minimum resolution time to display
    bool duplicates(QString); // Remove duplicate files
    int GCD(int, int); // Find Greated Common Divisor
    int LCM(int, int); // Find the Least Common Multiple
};

#endif // UpdateWindow_H
