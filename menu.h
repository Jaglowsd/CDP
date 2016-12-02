#ifndef MENU_H
#define MENU_H

#include <QMainWindow>
#include <QVBoxLayout>
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


class Menu : public QMainWindow
{
    Q_OBJECT

public:
    Menu(QWidget *parent); // Constructor

private slots:
    void selectFile();
    void exit();
    void import();
    void help(); // Registered slots for the menu

private:
    QMenu *fileMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;

    QAction *selectAction;
    QAction *exitAction;
    QAction *importAction;
    QAction *helpAction;

}; // Class declaration for a notepad

#endif // MENU_H
