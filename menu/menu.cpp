#include "menu.h"

Menu::menu(QWidget *parent) {
    selectAction = new QAction(tr("&Select File"), this);
    selectAction->setShortcut(tr("Ctrl+F"));
    selectAction->setStatusTip(tr("Select a file to format"));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));

    importAction = new QAction(tr("&Import"), this);
    importAction->setShortcut(tr("Ctrl+I"));
    importAction->setStatusTip(tr("Import .csv file to excel"));

    helpAction = new QAction(tr("&Documentation"), this);
    helpAction->setShortcut(tr("Ctrl+D"));
    helpAction->setStatusTip(tr("Guide on how to use the application"));


    connect(selectAction, SIGNAL(triggered()), this, SLOT(selectFile()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));
    connect(importAction, SIGNAL(triggered()), this, SLOT(import()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    //QMenuBar *menuBar = new QMenuBar(0);

    fileMenu = parent->menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(selectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(importAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
} // Conctructor funciton for notepad

void Menu::selectFile() {
    QString fileName = QFileDialog::getOpenFileName
            (this, tr("Select .csv file"), "", tr("Comma Separated Values (*.csv)"));
    QFile file(fileName);
    if(fileName == "") {
        /*if(!file.open(QIODevice::ReadOnly)) {
            QMessageBox::(this, tr("Error"), tr("Could not open file"));
            return;
        }*/
        return;
    }
    else {
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            // Put this in the file box on a separate widget??
            file.close();
        } else
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
    }

}

void Menu::import() {
    return;
}

void Menu::help() {
    QFile file("help.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        return;
    }
}

void Menu::exit() {
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("Time Formatter"));
    messageBox.setText(tr("Do you really want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if(messageBox.exec() == QMessageBox::Yes)
        close();
}
