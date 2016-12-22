#include "mainwindow.h"
#include "fileselect.h"
#include "averaging.h"

MainWindow::MainWindow() {
    resizeTimer = new QTimer(this);

    mainWidget = new QWidget;
    setCentralWidget(mainWidget);

    mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(10,10,10,10);

    mainWidget->setLayout(mainLayout);
    layoutList = new QList<QHBoxLayout*>();

    updateWindow = new UpdateWindow(mainWidget);

    QLabel *fileLabel = new QLabel(tr("File Path:"));
    fileLabel->setFixedWidth(470);
    QLabel *typeLabel = new QLabel(tr("File Type:"));
    QLabel *timeLabel = new QLabel(tr("Time Res:"));
    QLabel *saveLabel = new QLabel(tr("<b>Note: </b>Original name, YourFile.csv, indicates after averaging and "
                                      "modified path, YourFile_before.csv indicates before averaging."));

    removeFile = new QPushButton(tr("Remove Last File"), this);
    addFile = new QPushButton(tr("Add Additional File"), this);
    saveButton  = new QPushButton(tr("Choose Save File Destination ... "), this);
    saveButton->setFixedWidth(200);
    clearButton = new QPushButton(tr("Clear All"));
    csvButton = new QPushButton(tr("Create File"));

    QFrame* spacer1 = new QFrame();
    spacer1->setFrameShape(QFrame::HLine);
    QFrame* spacer2 = new QFrame();
    spacer2->setFrameShape(QFrame::HLine);

    saveBar = new QLineEdit(this);
    saveBar->setPlaceholderText(tr("Save File Path"));
    saveBar->setFixedWidth(400);

    QHBoxLayout *labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->addWidget(fileLabel, 0, Qt::AlignBottom);
    labelLayout->addWidget(typeLabel, 0, Qt::AlignBottom);
    labelLayout->addWidget(timeLabel, 0, Qt::AlignBottom);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(10,0,10,0);
    buttonLayout->setSpacing(25);
    buttonLayout->setSizeConstraint(QLayout::SetFixedSize);
    buttonLayout->addWidget(removeFile, Qt::AlignTop);
    buttonLayout->addWidget(addFile, Qt::AlignTop);

    QHBoxLayout *lineLayout1 = new QHBoxLayout();
    lineLayout1->addWidget(spacer1);

    QHBoxLayout *saveLayout = new QHBoxLayout();
    saveLayout->setContentsMargins(0,0,0,0);
    saveLayout->setSizeConstraint(QLayout::SetFixedSize);
    saveLayout->addWidget(saveBar, Qt::AlignRight);
    saveLayout->addWidget(saveButton, 0, Qt::AlignRight);

    QHBoxLayout *saveNoteLayout = new QHBoxLayout();
    saveNoteLayout->addWidget(saveLabel, 0, Qt::AlignTop);

    QHBoxLayout *lineLayout2 = new QHBoxLayout();
    lineLayout2->addWidget(spacer2);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QSpacerItem *emptySpacer = new QSpacerItem(375, 0);
    bottomLayout->setContentsMargins(0,0,0,0);
    bottomLayout->setSizeConstraint(QLayout::SetFixedSize);
    bottomLayout->addSpacerItem(emptySpacer);
    bottomLayout->addWidget(clearButton);
    bottomLayout->addWidget(csvButton);

    mainLayout->addLayout(buttonLayout);

    mainLayout->addLayout(labelLayout);

    FileSelect *newFileSelect = new FileSelect(mainLayout, 2);
    updateWindow->addFileObject(newFileSelect);
    fileSelectList.push_back(newFileSelect);

    FileSelect *newFileSelect2 = new FileSelect(mainLayout, 3);
    updateWindow->addFileObject(newFileSelect2);
    fileSelectList.push_back(newFileSelect2);

    mainLayout->insertLayout(5, lineLayout1);

    mainLayout->insertLayout(6, saveLayout);

    mainLayout->insertLayout(7, saveNoteLayout);

    mainLayout->insertLayout(8, lineLayout2);

    averaging = new Averaging(mainLayout);

    mainLayout->addLayout(bottomLayout);

    labelLayoutPosition = mainLayout->children().indexOf(labelLayout)+3;

    createActions();
    createMenus();
    createSigToSlot();

    setWindowTitle(tr("CDP"));
    setMaximumSize(650, 368);
    resize(650, 160);
} // Constructor

QVBoxLayout* MainWindow::getMainLayout() {
    return mainLayout;
}

void MainWindow::createActions() {
    selectAction = new QAction(tr("&Select File"), this);
    selectAction->setShortcut(tr("Ctrl+F"));
    selectAction->setStatusTip(tr("Select a file to format"));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit the application"));

    importAction = new QAction(tr("&Export"), this);
    importAction->setShortcut(tr("Ctrl+I"));
    importAction->setStatusTip(tr("Export .csv file to excel"));

    helpAction = new QAction(tr("&Documentation"), this);
    helpAction->setShortcut(tr("Ctrl+D"));
    helpAction->setStatusTip(tr("Guide on how to use the application"));
}

void MainWindow::createMenus() {
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(selectAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(importAction);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
}

void MainWindow::createSigToSlot() {
    connect(selectAction, SIGNAL(triggered()), this, SLOT(selectFile()));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exit()));
    connect(importAction, SIGNAL(triggered()), this, SLOT(exPort()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));

    connect(removeFile, SIGNAL(pressed()), this, SLOT(removeFileHandler()));
    connect(addFile, SIGNAL(pressed()), this, SLOT(fileHandler()));
    connect(this, SIGNAL(addFileSelectHandler(QVBoxLayout*,  int)),
            this, SLOT(createSelectFile(QVBoxLayout*, int)));
    connect(saveButton, SIGNAL(pressed()), this, SLOT(saveLocation()));
    connect(csvButton, SIGNAL(pressed()), this, SLOT(createCSV()));
    connect(clearButton, SIGNAL(pressed()), this, SLOT(clearAll()));

    connect(resizeTimer, SIGNAL(timeout()), this, SLOT(resizeTimeout()));
}

void MainWindow::removeFileHandler()
{
   int howManyFSObjects = 3; // By default at least two file select objects should be present
   if(fileSelectList.length() >= howManyFSObjects)
    {
       if(!fileSelectList.empty())
       {
           QVector<FileSelect*>::iterator itr;
           for(itr = fileSelectList.begin(); itr!= fileSelectList.end(); itr++){}
           itr--;
           (*itr)->setFileName("remove");
           updateWindow->removeFileObject();
           fileSelectList.pop_back();
           delete *itr;
       }
        QList<QWidget*> child = mainWidget->findChildren<QWidget*>();
        for(int i = 0; i <= 3; i++) {
            QWidget *ch = child.back();
            child.pop_back();
            delete ch;
        }
        labelLayoutPosition--;
        resize(650, 188);
    }
}

void MainWindow::resizeEvent( QResizeEvent * event )
{
    resizeTimer->stop();
    resizeTimer->start(RESIZE_TIMEOUT);
}

void MainWindow::resizeTimeout()
{
    resizeTimer->stop();
    resize(650, 188);
}

void MainWindow::fileHandler() { emit addFileSelectHandler(mainLayout, labelLayoutPosition); labelLayoutPosition++; }
void MainWindow::createSelectFile(QVBoxLayout *layout, int position)
{
    if(fileSelectList.size() <= 5)
    {
        FileSelect *newFileSelect = new FileSelect(layout, position);
        fileSelectList.push_back(newFileSelect);
        updateWindow->addFileObject(newFileSelect);
    }
    else
        QMessageBox::warning(this, tr("Oops"), tr("Maximum number of files reached."));
} // Create new file select object and add it to the window

void MainWindow::selectFile() {
    QString fileName = QFileDialog::getOpenFileName
            (this, tr("Select .csv file"), "", tr("Comma Separated Values (*.csv)"));
    qDebug() << fileName;
    QFile file(fileName);
    if(fileName == "")
    {
        return;
    }
    else {
        if(file.open(QIODevice::ReadOnly))
        {
                for(QVector<FileSelect*>::iterator itr = fileSelectList.begin(); itr != fileSelectList.end(); itr++)
                {
                    if(((*itr)->fileName->text() == ""))
                    {
                        (*itr)->setFileName(fileName);
                        file.close();
                        return;
                    }
                }
        QMessageBox::warning(this, tr("Oops"), tr("Click Add File to add another file path."));
        file.close();

        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file \n Check file path and try again"));
            return;
        }
    }

} // Add file path from file menu

void MainWindow::saveLocation() {
    QString fileName = QFileDialog::getOpenFileName
            (this, tr("Select .csv file"), "", tr("Comma Separated Values (*.csv)"));
    if(fileName == "")
        return;
    else
        saveBar->setText(fileName);
} // Path for save file

void MainWindow::clearAll() {
    for(QVector<FileSelect*>::iterator itr = fileSelectList.begin(); itr != fileSelectList.end(); itr++) {
        (*itr)->fileName->setText("");
        int index = (*itr)->fileType->findText(("O3"));
        if(index != -1)
            (*itr)->fileType->setCurrentIndex(index);
        (*itr)->timeResolution->setText("");
    }
    saveBar->setText("");
    averaging->scaledTime->setText("");
    averaging->setMinTime("");
    updateWindow->clear();
} // Set all currently displayed widgets to their respective default state

void MainWindow::createCSV() { // Check that the time scale is at least the minimum!
    // Check everything is filled out properly here
    bool isEmpty = false;
    bool isWorking = false;
    int fSListLength = 0;
    int fileCount = 0;
    int timeScale;

    if(averaging->getScaledTime() == -1)
        timeScale = 0;
    else
        timeScale = averaging->getScaledTime();

    for(QVector<FileSelect*>::iterator itr = fileSelectList.begin(); itr != fileSelectList.end(); itr++)
    {
        if((*itr)->fileName->text() == "")
        {
            isEmpty = true;
            fSListLength++;
        }
        else
            fileCount++;
    }

    if(fSListLength == fileSelectList.length() || fileCount <= 1)
    {
        QMessageBox::warning(this, tr("Oops"), tr("At least two files are needed to perform time formatting."));
        return;
    }
    else if(isEmpty)
    {
       QMessageBox messageBox;
       messageBox.setWindowTitle(tr("CDP"));
       messageBox.setText(tr("Empty file boxes have been detected. \nWould you like to proceed with the given files?"));
       messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
       messageBox.setDefaultButton(QMessageBox::No);
       if(messageBox.exec() == QMessageBox::Yes)
       {
           if(saveBar->text() != "")
               updateWindow->setSaveDestination(saveBar->text());
           else
           {
               QMessageBox::warning(this, tr("Oops"), tr("A save destination must be specified."));
               return;
           }
           isWorking = updateWindow->createDataHandler(timeScale, averaging->getUnits());
           if(!isWorking)
               QMessageBox::critical(this, tr("ERROR"), tr("Something went wrong when creating your csv file."));
           else
               QMessageBox::warning(this, tr("Done!"), tr("Your csv files have been successfully created!"));
           messageBox.close();
       }
       else
           messageBox.close();
    }
    else
    {
        if(saveBar->text() != "")
            updateWindow->setSaveDestination(saveBar->text());
        else
        {
            QMessageBox::warning(this, tr("Oops"), tr("A save destination must be specified."));
            return;
        }

        isWorking = updateWindow->createDataHandler(timeScale, averaging->getUnits());
        if(!isWorking)
            QMessageBox::critical(this, tr("ERROR"), tr("Something went wrong when creating your csv file."));
        else
            QMessageBox::warning(this, tr("Done!"), tr("Your csv files have been successfully created!"));
    }

    // Display something

}

void MainWindow::update(QString str)
{
    updateWindow->update(str); // Allow updateWindow object to handle changes made to fileSelect line edit widgets
    int resolution = updateWindow->getTimeResolution(); // Post: updateWindow->update(), an updated min tim res is available to be displayed
    if(resolution != -1)
    {
        if(resolution > 60)
        {
            resolution /= 60;
            averaging->setMinTime(QString::number(resolution));
            int index = averaging->units->findText(("min"));
            if(index != -1)
                averaging->units->setCurrentIndex(index);
        }
        else
        {
                averaging->setMinTime(QString::number(resolution));
        }
    } // If !(-1) convert to proper unit of time and display min res time

    else
    {
        averaging->setMinTime("");
    } // -1 indicates there are no file paths, so place an empty string

} // SLOT for when textChanged() is signaled

void MainWindow::exPort() {
    return;
}

void MainWindow::help() {
    QFile file("help.txt");
    if(file.open(QIODevice::ReadOnly))
    {
        return;
    }
} // Open help document

void MainWindow::exit() {
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("CDP"));
    messageBox.setText(tr("Do you really want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if(messageBox.exec() == QMessageBox::Yes)
        close();
} // close application
