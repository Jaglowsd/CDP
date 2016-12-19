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
    bool handleData();

    void addFileObject(FileSelect* fs);
    void removeFileObject();

    void setTimeResolution(int);
    int getTimeResolution();
    void setSaveDestination(QString);

    int findTimesToCompare(QString, QTextStream*);
    int compareTimes(QString, QString, int);

    void clear();

    void update(QString);
    void minimumTime();
    bool duplicates(QString);
    int GCD(int, int);
    int LCM(int, int);
};

#endif // UpdateWindow_H
