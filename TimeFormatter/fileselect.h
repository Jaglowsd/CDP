#ifndef FILESELECT_H
#define FILESELECT_H

#include <QObject>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QIODevice>
#include <QTextStream>
#include <QMessageBox>
#include <QAbstractButton>
#include <QDebug>
#include <QVector>
#include <QList>
#include <QComboBox>

class FileSelect : public QWidget{

    Q_OBJECT

public:
    FileSelect() {} // Default constructor
    FileSelect(QVBoxLayout *parent, int position); // Constructor

    QPushButton *browse;
    QLineEdit *timeResolution;
    QLineEdit *fileName;
    QComboBox *fileType;

    QString getTimeResolution(); // Get time resolution from timeResolution line edit
    QString getFileType(); // Get file type from fileType line edit
    QString getFileName(); // Get file name from fileName line edit

    void setFileName(QString); // Set file name onto fileName line edit

private slots:
    void fileDialogHandler(); // Open file dialog
};

#endif // FILESELECT_H
