#include "updatewindow.h"

UpdateWindow::UpdateWindow(){}
UpdateWindow::UpdateWindow(QWidget *parent){
    timeResolution = 0;
    this->parent = parent; // Have a reference to mainWidget from MainWindow in order to display inputDialog
    fileTypes << "O3" << "NOx" << "Apollo" << "Particle" << "Kestral"; // Specifiy file types
    firstHeaders = {"202/205 Flow No Analog In,,,,,,", "Model 405", "Num,Temperature,Humidity,CO2,Dust concentration,time", "Instrument Name,Optical Particle Sizer",
               "FORMATTED DATE-TIME,DT,MG,TR,WS,CW,HW,TP,WC,RH,HI,DP,WB,BP,AL,DA" };
}

bool UpdateWindow::createDataHandler(int timeScale, QString units)
{
    dataHandler = new DataHandler(&fSList, &resolutions, &fileTypes, timeResolution, saveDestination, timeScale, units);
    bool isSuccess = handleData();
    return isSuccess;
}

bool UpdateWindow::handleData()
{
    bool success = dataHandler->readFiles();
    if(success)
    {
        success = dataHandler->findStartTime();
        if(success)
        {
            success = dataHandler->printHeaders();
            if(success)
            {
                //success = dataHandler->gatherTimeBlockData();

                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;
}

void UpdateWindow::addFileObject(FileSelect *fs) { fSList.push_back(fs); } // Recording all fileSelect objects
void UpdateWindow::removeFileObject() { fSList.pop_back(); } // Remove fileSelect objects when remove button on MainWindow is signaled

void UpdateWindow::setTimeResolution(int time) { timeResolution = time; }
int UpdateWindow::getTimeResolution() { return timeResolution; }

void UpdateWindow::setSaveDestination(QString str) { saveDestination = str; } // Save location

void UpdateWindow::update(QString str)
{
    if(str != "")
    {
        if(str == "remove")
        {
            resolutions[fSList.length()-1] = -1; // Last item is always removed when remove button is signaled,
                                                 // so we set it to -1 to indicate no time resolution
            minimumTime(); // Finds the minimum time after removing the last file

        } // "remove" string is programmatically specified when the remove button is signaled
        else
        {
            bool duplicate = duplicates(str); // Check for duplicate files
            if(!duplicate)
            {
                QFile file(str);
                if(file.open(QIODevice::ReadOnly))
                {
                    QTextStream in(&file);
                    for(int i = 0; i != fSList.length(); i++)
                    {
                        if(((fSList[i])->fileName->text() == str))
                        {
                            bool isGiven;
                            QString text = QInputDialog::getItem(parent, tr("File Type"),
                                                                 tr("File Type:"), fileTypes, false,
                                                                 0, &isGiven);
                            // Read the first line of the file to match a predefined header
                            QString firstLine = in.readLine();
                            firstLine = firstLine.trimmed();

                            if (isGiven && !text.isEmpty())
                            {
                                // Set the combox box to match the selected file type
                                int index = fSList[i]->fileType->findText(text);
                                fSList[i]->fileType->setCurrentIndex(index);

                                if(firstLine == firstHeaders[index])
                                {
                                    int difference = findTimesToCompare(fileTypes[index], &in);
                                    resolutions[i] = difference;

                                    QString s = QString::number(difference);
                                    fSList[i]->timeResolution->setText(s);

                                    minimumTime();
                                    file.close();
                                } // Read the first line of the selected file to detect what type of file it is
                                  // Compare that line from the list of known headers
                                  // Resolution's indices correspond to fSList's so we can use index i as the loaction to add the time resolution returned
                                  // by findTimesToCompare
                                  // Display the time resolution to the current fileSelect object's timeResolution widget

                                else
                                {
                                    int dex = firstHeaders.indexOf(firstLine);

                                    if(dex != -1)
                                    {
                                        QMessageBox::warning(parent, tr("Oops"), tr("Auto file type detects that the file type given does not match the file."
                                                                                   "\nChanging file type to match given file."));
                                        fSList[i]->fileType->setCurrentIndex(dex);

                                        int difference = findTimesToCompare(fileTypes[dex], &in);
                                        resolutions[i] = difference;

                                        QString s = QString::number(difference);
                                        fSList[i]->timeResolution->setText(s);

                                        minimumTime();
                                        file.close();
                                    } // Automatically changes file type if a user happens to mis-click or picks the wrong file type

                                    else
                                    {
                                        QMessageBox::warning(parent, tr("Oops"), tr("No compatible file type found for the given file path."
                                                                                    "\nGo to help for more information."));
                                        (fSList[i])->fileName->setText("");
                                        file.close();

                                    } // If the file type could not be matched to the given file, direct the user to the help page

                                } // Similar to what happens above, but in the case the user selects an incorrect file type, the program
                                  // will automatically change for them provided the file given has a predefined header

                            }
                            else
                            {
                                QMessageBox::critical(parent, tr("Oops"), tr("File type must be specified."));
                                fSList[i]->fileName->setText("");
                                file.close();
                                return;
                            } // User is required to input a file type for easier data processing
                              // Set the chosen file type in the file type comboBox

                        } // Find the position of the file select that was just modified to place the file type in the
                          // appropriate location

                    } // Iterate through all fileSelect objects to find the one that was just modified by comparing its file name widget to the
                      // string that is returned from textChanged()

                } // Check that the file can be opened
                else
                {
                    QMessageBox::information(0, "error", file.errorString());
                } // A precaution, but based on the current structure this line should never execute unless it is caused by
                  // an issue in the user's file explorer

            } // Only adjust the min time when the file given is not a duplicate of an existing file

        } // Strings that are legitimate file names will access these statements because file dialogs will not allow anything but a valid file path

    } // Empty string indicates that work is done elsewhere (i.e. duplicates or update)

} // When textChanged() on line edit objects is signaled
  // Caller is update(QString str) in MainWindow

bool UpdateWindow::duplicates(QString str) {
    int position = -1;
    if(str != "" && str != "remove")
    {
        for(int i = 0; i != fSList.length(); i++)
        {
            if(((fSList[i])->fileName->text() == str))
            {
                int temp = position;
                position = i;
                if(temp != -1) {
                    QMessageBox::warning(parent, tr("Oops"), tr("No duplicate files allowed."));
                    (fSList[temp])->fileName->setText("");
                    (fSList[temp])->fileType->setCurrentIndex(0);
                    (fSList[temp])->timeResolution->setText("");
                    (fSList[position])->fileName->setText("");
                    (fSList[position])->fileType->setCurrentIndex(0);
                    (fSList[position])->timeResolution->setText("");
                    qDebug() << resolutions;
                    resolutions[temp] = -1;
                    resolutions[position] = -1;
                    qDebug() << resolutions;
                    minimumTime();
                    return true;

                } // When temp is no longer -1, at least two of the same file name have been found
                  // Clear their data and update the minimum time

            } // When a fileName widget matches the string, we record the position to later clear data from that fileSelect object

        } // Iterate through the file select objects in order to find identical file paths

    }// Empty strings indicate that the string was programatically changed and therefore can be ignored
     // Remove strings will never enter this function but a detail to watch for

    return false;

} // Check for duplicate file names, true for no duplicates, false for duplicates

int UpdateWindow::findTimesToCompare(QString type, QTextStream *in){
    QRegularExpression re;
    QString matched = "";
    QString temp = "";
    int count = 0;

    if(type == "O3" || type == "NOx"){
        re = QRegularExpression("[0-3][0-9]/[0-1][0-9]/[1][0-9],[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // DD/MM/YY, HH:MM:SS
        while(count <= 1)
        {
            QString str = in->readLine();
            QRegularExpressionMatch match = re.match(str);
            bool hasMatch = match.hasMatch();
            if(hasMatch) {
                temp = matched;
                matched = match.captured(0);
                count++;
            }
        }
        int difference = compareTimes(matched, temp, 0);
        return difference;
    }
    else if (type == "Apollo") {
        re = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // HH:MM:SS
        while(count <= 1)
        {
            QString str = in->readLine();
            QRegularExpressionMatch match = re.match(str);
            bool hasMatch = match.hasMatch();
            if(hasMatch) {
                temp = matched;
                matched = match.captured(0);
                count++;
            }
        }
        int difference = compareTimes(matched, temp, -1);
        return difference;
    }
    else if (type == "Kestral") {
        re = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // YYYY-MM-DD, HH:MM:SS
        while(count <= 1)
        {
            QString str = in->readLine();
            QRegularExpressionMatch match = re.match(str);
            bool hasMatch = match.hasMatch();
            if(hasMatch) {
                temp = matched;
                matched = match.captured(0);
                count++;
            }
        }
        int difference = compareTimes(matched, temp, 1);
        return difference;
    }
    else if (type == "Particle") {
        re = QRegularExpression("Sample Interval"); // Explicitly gives time resolution
        while(count <= 0)
        {
            QString str = in->readLine();
            QRegularExpressionMatch match = re.match(str);
            bool hasMatch = match.hasMatch();
            if(hasMatch) {
                temp = matched;
                matched = str;
                count++;
            }
        }
        int difference = compareTimes(matched, temp, 2);
        return difference;
    }
    else
        return - 1;
}

int UpdateWindow::compareTimes(QString str1, QString str2, int instruction) {
    QString subStr1 = str1;
    QString subStr2 = str2;

    if(instruction == 0) {
        subStr1 = str1.section(',', 1, 1);
        subStr2 = str2.section(',', 1, 1);
    } // Instruction 0 indicates O3 or NOx

    else if(instruction == 1) {
        subStr1 = str1.section(' ', 1, 1);
        subStr2 = str2.section(' ', 1, 1);
    } // Instruction 1 indicates Kestral

    else if(instruction == 2) {
        subStr1 = str1.section(',', 1, 1);
        subStr1 = subStr1.section(':', 2,2);

        int difference = subStr1.toInt();
        return difference;
    } // Instruction 2 indicates Particle

    QTime time1;
    QTime time2;

    time1 = time1.fromString(subStr1);
    time2 = time2.fromString(subStr2);

    int difference = abs(time1.secsTo(time2));

    return difference;

} // If Instruction is not 0, 1, or 2, then it is Apollo because no splitting of strings is necessary

void UpdateWindow::minimumTime() {
    int resolution;
    QVector<int> lcm = QVector<int>();
    // Re-calculate minTime
    for(int i = 0; i <= 5; i++)
    {
        if(resolutions[i] != -1)
            lcm.push_back(resolutions[i]);
    }
    int lcmSize = lcm.length();
    switch (lcmSize) {
        case 1: resolution = lcm[0]; break;
        case 2: resolution = LCM(lcm[0],lcm[1]); break;
        case 3: resolution = LCM(LCM(lcm[0],lcm[1]),lcm[2]); break;
        case 4: resolution = LCM(LCM(LCM(lcm[0],lcm[1]), lcm[2]), lcm[3]); break;
        case 5: resolution = LCM(LCM(LCM(LCM(lcm[0],lcm[1]), lcm[2]), lcm[3]), lcm[4]); break;
        case 6: resolution = LCM(LCM(LCM(LCM(LCM(lcm[0],lcm[1]), lcm[2]), lcm[3]), lcm[4]), lcm[5]); break;
        default: resolution = -1;
    }
    setTimeResolution(resolution); // Possibility that the time resolution is >= 60 so conversions can take place
} // Find the minimum resolution time to display

int UpdateWindow::GCD(int a, int b) {
   int GCD, temp;

    if (a < b)
    {
        temp = a;
        a = b;
        b = temp;
    }

    while (b != 0)
    {
        temp = b;
        b = a % b;
        a = temp;
    }

       GCD = temp;

   return GCD;
}
int UpdateWindow::LCM(int a, int b) {
    int LCM = (a*b) / (GCD(a,b));
    return LCM;
}

void UpdateWindow::clear() {
    for(int i = 0; i <= 5; i++)
        resolutions[i] = -1;
} // Called when remove button is signaled to clear all changes made to the window
  // Sets all resolutions to -1, indicating there is no min time to be displayed
