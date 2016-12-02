#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fileselect.h"
#include "averaging.h"
#include "updatewindow.h"

#include <QThread>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QAction>
#include <QSizePolicy>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>
#include <QObject>
#include <QMessageBox>
#include <QString>
#include <QTextStream>
#include <QIODevice>
#include <QFileDialog>
#include <QFile>
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QObjectList>
#include <iterator>
#include <QLineEdit>
#include <QPushButton>
#include <QVector>
#include <QFrame>
#include <QEventLoop>
#include <QApplication>
#include <QInputDialog>
#include <QTimer>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    QVBoxLayout *mainLayout;

    int labelLayoutPosition;

    QVector<FileSelect*> fileSelectList;
    QList<QHBoxLayout*> *layoutList;

    Averaging *averaging;

    UpdateWindow *updateWindow;

    static const int RESIZE_TIMEOUT = 250; // 1/4 second in milliseconds
    QTimer* resizeTimer;

public slots:
    void update(QString); // Called when fileSelect objects file names are modified

signals:
    void addFileSelectHandler(QVBoxLayout*, int);

private slots:
    void selectFile();
    void exit();
    void exPort();
    void help(); // Registered slots for the menu

    void fileHandler();
    void createSelectFile(QVBoxLayout*, int); // Slots for adding files

    void removeFileHandler(); // slot for removing file select objects

    void saveLocation();

    void clearAll();
    void createCSV();

    void resizeTimeout(); // Resizing window timer

private:
    void createActions();
    void createMenus();
    void createSigToSlot();

    void resizeEvent( QResizeEvent * event );

    QFile *logger;

    QVBoxLayout* getMainLayout();

    QWidget *mainWidget;

    QMenu *fileMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;

    QLineEdit *saveBar;
    QPushButton *addFile;
    QPushButton *removeFile;
    QPushButton *saveButton;
    QPushButton *clearButton;
    QPushButton *csvButton;

    QAction *selectAction;
    QAction *exitAction;
    QAction *importAction;
    QAction *helpAction;
};

#endif // MAINWINDOW_H
