#include "fileselect.h"

FileSelect::FileSelect(QVBoxLayout *parent, int position) {
    fileName = new QLineEdit(this);
    fileName->setReadOnly(true);
    fileName->setPlaceholderText(tr("File Name"));

    browse  = new QPushButton(tr("Browse ... "), this);

    connect(browse, SIGNAL(pressed()), this, SLOT(fileDialogHandler()));
    connect(fileName, SIGNAL(textChanged(QString)), parent->parent()->parent(), SLOT(update(QString)));

    fileType = new QComboBox(this);
    fileType->setFixedWidth(75);
    fileType->addItem(tr("O3"));
    fileType->addItem(tr("NOx"));
    fileType->addItem(tr("Apollo"));
    fileType->addItem(tr("Particle"));
    fileType->addItem(tr("Kestral"));

    timeResolution = new QLineEdit(this);
    timeResolution->setFixedWidth(75);
    timeResolution->setPlaceholderText(tr("10"));


    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(5);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(fileName);
    layout->addWidget(browse);
    layout->addSpacing(20);
    layout->addWidget(fileType);
    layout->addWidget(timeResolution);

    parent->insertLayout(position, layout);
}

QString FileSelect::getFileName() { return fileName->text(); }
QString FileSelect::getFileType() { int index = fileType->currentIndex();
                                    return fileType->itemText(index); }
QString FileSelect::getTimeResolution() { return timeResolution->text(); }

void FileSelect::setFileName(QString filePath) { fileName->setText(filePath); }

void FileSelect::fileDialogHandler() {
    QString fileName = QFileDialog::getOpenFileName
            (this, tr("Select .csv file"), "", tr("Comma Separated Values (*.csv)"));
    QFile file(fileName);
    if(fileName == "")
        return;
    else
    {
        if(file.open(QIODevice::ReadOnly))
        {
            this->fileName->setText(fileName);
            file.close();
        } else
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
    }
}
