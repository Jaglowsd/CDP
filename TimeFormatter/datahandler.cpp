#include "datahandler.h"
#include "updatewindow.h"

DataHandler::DataHandler(QVector<FileSelect*> *fs, QVector<int> *res, QStringList *types, int time, QString save, int timeScale, QString units)
{
    fSList = fs;
    resolutions = res;
    fileTypes = types;
    timeResolution = time;
    reFormatSave(save);
    this->timeScale = timeScale;
    this->units = units;
}

void DataHandler::reFormatSave(QString save)
{
    QStringList query = save.split(".");
    saveBeforeAverage = query[0] + "_before." + query[1];
    saveAfterAverage = save;
}

bool DataHandler::readFiles() {

    QRegularExpression reHeader;

    for(int i = 0; i < fSList->length(); i++)
    {

        QString fileType = fSList->at(i)->getFileType();
        if(fSList->at(i)->getFileName() != "")
        {
            QFile file (fSList->at(i)->getFileName());

            if(file.open(QIODevice::ReadOnly))
            {
                QTextStream in(&file);

                if(fileType == "O3")
                {
                    reHeader = QRegularExpression("Int32,Decimal,Decimal,Decimal,Decimal,Date,Time");
                    recordFileData(&in, reHeader, fileType);

                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    file.close();
                }
                else if(fileType == "NOx")
                {
                    reHeader = QRegularExpression("Int32,Decimal,Decimal,Decimal,Int32,Int32,Decimal,Decimal,Decimal,Int32,Int32,Decimal,Decimal,"
                                            "Decimal,Decimal,Decimal,Date,Time,Int32"); // Final header in file
                    recordFileData(&in, reHeader, fileType);
                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    file.close();
                    // Here we could log that the file was sucessfully read
                }
                else if (fileType == "Apollo")
                {
                    reHeader = QRegularExpression("Num,Temperature,Humidity,CO2,Dust concentration,time"); // Final header in file
                    recordFileData(&in, reHeader, fileType);
                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    file.close();
                }
                else if (fileType == "Kestral")
                {
                    reHeader = QRegularExpression("YYYY"); // Final header in file
                    recordFileData(&in, reHeader, fileType);
                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    file.close();
                }
                else if (fileType == "Particle")
                {
                    reHeader = QRegularExpression("Elapsed Time"); // Final header in file
                    recordFileData(&in, reHeader, fileType);
                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    file.close();
                }
                else
                    return false;
            }
            else
                return false;
        }
    }

    return true;
}

void DataHandler::recordFileData(QTextStream *in, QRegularExpression reHeader, QString fileType)
{
    bool hasMatch = false;
    QVector<QString> vec;

    while(!hasMatch)
    {
        QString line = in->readLine();
        line = line.trimmed();
        vec.push_back(line);
        QRegularExpressionMatch match = reHeader.match(line);
        hasMatch = match.hasMatch();
    }// Store all headers for given file
    //fileHeaders.push_back(vec);
    fileHeaders.push_back(vec);
    vec.clear();

    while(!in->atEnd())
    {
        QString line = in->readLine();
        vec.push_back(line);
    } // Store all data for given file
    fileData.push_back(vec);
    fileDataMap.insert(fileType, vec);
}

bool DataHandler::pullStartTime(QString fileType, int index)
{
    QRegularExpression re;
    QRegularExpression reDate;

    if(fileType == "O3" || fileType == "NOx")
    {
        QString line = fileData[index][0];
        re = QRegularExpression("[0-3][0-9]/[0-1][0-9]/[1][0-9],[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // DD/MM/YY, HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch) {
            line = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "dd/MM/yy,hh:mm:ss");
            dt = dt.addYears(100);
            startTimes.push_back(dt);
            return true;
        }
    }
    else if(fileType == "Apollo") // Need the date when the data was recorded!
    {
        QString line = fileData[index][0];
        re = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch) {
            line = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "hh:mm:ss");
            startTimes.push_back(dt);
            return true;
        }
    }
    else if(fileType == "Kestral")
    {
        QString line = fileData[index][0];
        re = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // YYYY-MM-DD HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch) {
            line = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "yyyy-MM-dd hh:mm:ss");
            startTimes.push_back(dt);
            return true;
        }
    }
    else if(fileType == "Particle")
    {
        re = QRegularExpression("Test Start Time"); // Explicit start time
        reDate = QRegularExpression("Test Start Date"); // Explicit start date

        for(int i = 0; i <= fileHeaders[index].length(); i++)
        {
            QString line1 = fileHeaders[index][i];
            QString line2 = "";
            if(i != fileHeaders.length())
                line2 = fileHeaders[index][i+1];

            QRegularExpressionMatch match = re.match(line1);
            QRegularExpressionMatch match2 = reDate.match(line2);
            bool hasMatch = match.hasMatch();
            bool hasAnotherMatch = match2.hasMatch();
            if(hasMatch && hasAnotherMatch) {
                line1 = line1.section(',', 1, 1);
                line2 = line2.section(',', 1, 1);

                QString dateTime = line2 + " " + line1;

                QDateTime dt;
                dt = dt.fromString(dateTime, "yyyy/MM/dd hh:mm:ss");
                startTimes.push_back(dt);
                return true;
            }
        }
        return false;
    }
    else
        return false;

    return false;
}

bool DataHandler::findStartTime()
{
    QDateTime earliest = QDateTime::currentDateTime().addYears(2); // Add two years to ensure an earlier time than when data was recorded

    for(QVector<QDateTime>::iterator itr = startTimes.begin(); itr != startTimes.end(); itr++)
    {
        QDateTime temp = *itr;

        if(earliest.msecsTo(temp) <= 0)
            earliest = temp;
    }

    startTimeBlock = earliest;
    if(startTimeBlock < QDateTime::currentDateTime().addYears(1))
        return true;
    else
        return false;
}

bool DataHandler::printHeaders()
{
    int headerCount = 0;
    int longestHeader = 0;
    int stringLength;
    int i;
    int temp;

    QVector<int> commas;

    for(QVector<QVector<QString>>::iterator itr = fileHeaders.begin(); itr != fileHeaders.end(); itr++)
    {
        std::reverse(itr->begin(), itr->end());
    }

    for(i = 0; i < fileHeaders.length(); i++)
    {
        temp = fileHeaders[i].length();
        if(temp > headerCount)
            headerCount = temp;
    }

    for(int m = 0; m < fileHeaders.length(); m++)
    {
        longestHeader = 0;
        for(int n = 0; n < fileHeaders[m].length(); n++)
        {
            int maxChar = 0;
            temp = fileHeaders[m][n].length();
            maxChar = countCommas(fileHeaders[m][n]);

            if(maxChar > longestHeader)
                longestHeader = maxChar;
        }
        commas.push_back(longestHeader + 1);
    }

    QFile fileAfter(saveAfterAverage);
    QFile fileBefore(saveBeforeAverage);
    if(fileAfter.open(QIODevice::WriteOnly) && fileBefore.open(QIODevice::WriteOnly))
    {
        QTextStream stream1(&fileAfter);
        QTextStream stream2(&fileBefore);

        for(i = 0; i < fSList->length(); i++)
        {
            QString fileName = fSList->at(i)->getFileName();
            QFileInfo fName = QFileInfo(fileName);
            if(fileName != "")
            {
                fileName = ",From " + fName.baseName();

                for(int a = 0; a < commas[i]; a++)
                    fileName += ",";
                stream1 << fileName;
                stream2 << fileName;
            }

        }
        stream1 << "\n";
        stream2 << "\n";
        stream1.flush();
        stream2.flush();

        int headersRemaining = headerCount-1;

        for(int l = headerCount; l > 0; l--)
        {
            for(int k = 0; k < fileHeaders.size(); k++)
            {
                if(fileHeaders[k].size()-1 < headersRemaining)
                {
                    QString s;
                    for(int b = 1; b <= commas[k]; b++)
                        s += ",";
                    s = "," + s;
                    stream1 << s;
                    stream2 << s;
                }
                else
                {
                    QString header;
                    QString date = "Date Time";

                    if(fileHeaders[k].indexOf(fileHeaders[k][headersRemaining]) == 0 && k == 0)
                    {
                        stream1 << date;
                        stream2 << date;
                    }

                    header = fileHeaders[k][headersRemaining];
                    int count = countCommas(header);

                    stringLength = commas[k] - count;
                    if(stringLength > 0)
                    {
                        for(int c = 1; c <= stringLength; c++)
                            header += ",";
                    }
                    header = "," + header;
                    stream1 << header;
                    stream2 << header;
                }

            }
            stream1 << "\n";
            stream2 << "\n";
            headersRemaining--;
        }
        fileAfter.close();
        fileBefore.close();
    }
    else
        return false;
    return true;
}

int DataHandler::countCommas(QString str)
{
    int commaCount = 0;

    for(int i = 0; i < str.length(); i++)
    {
        QCharRef ch = str[i];
        if(ch == ',')
            commaCount++;
    }
    return commaCount;
}

void DataHandler::scaledByTimeResolution()
{
    if(timeScale != 0)
    {
        UpdateWindow *uw = new UpdateWindow();
        if(units == "sec")
        {
            if(timeResolution%timeScale == 0)
                timeResolution = timeScale;
            else
                timeResolution = uw->LCM(timeResolution,timeScale);
        }
        else if(units == "min")
        {
            timeScale = timeScale*60;
            if(timeResolution%timeScale == 0)
                timeResolution = timeScale;
            else
                timeResolution = uw->LCM(timeResolution,timeScale);
        }
        else if(units == "hour")
        {
            timeScale = timeScale*3600;
            if(timeResolution%timeScale == 0)
                timeResolution = timeScale;
            else
                timeResolution = uw->LCM(timeResolution,timeScale);
        }
        else if(units == "day")
            timeScale = timeScale*82400;
            if(timeResolution%timeScale == 0)
                timeResolution = timeScale;
            else
                timeResolution = uw->LCM(timeResolution,timeScale);
        delete uw;
    }
    endTimeBlock = startTimeBlock;
    endTimeBlock.addSecs(timeResolution);
}

bool DataHandler::gatherTimeBlockData()
{
    scaledByTimeResolution();
    QFile fileBefore(saveBeforeAverage);
    QFile fileAfter(saveAfterAverage);
    if(fileAfter.open(QIODevice::Append) &&
            fileBefore.open(QIODevice::Append))
    {
        QTextStream stream1(&fileBefore);
        QTextStream stream2(&fileAfter);
        stream1 << startTimeBlock.toString("dd/MM/yy hh:mm:ss");
        stream1.flush();
        stream2 << startTimeBlock.toString("dd/MM/yy hh:mm:ss");
        stream2.flush();

        bool isInRange = true;
        int temp = 0;
        int longestFileLength;
        int longestFileIndex;
        QMap<QString,QVector<QString>>::iterator longestFileItr;

        QList<QMap<QString,QVector<QString>>::iterator> fileItrStorage;
        for(int i = 0; i < fSList->length(); i++)
        {
           QMap<QString,QVector<QString>>::iterator itr = fileDataMap.begin();
            fileItrStorage.push_back(itr);
            if(itr->length() > temp)
            {
                longestFileItr = itr;
                temp = itr->length();
                longestFileLength = temp;
                longestFileIndex = i;
            }
        } // Setting up file iterators

            /*
            while(longestFileItr != fSList[longestFileIndex].end())
            {
                for(int j = 0; j < fSList->length(); j++)
                {
                    itr = itrStorage[j];
                    if(itr != fSList[j].end())
                    {
                        while(isInRange)
                        {
                            isInRange = checkRange(itr);
                            if(isInRange)
                            {
                                addDataToBlock(itr);
                                itr++;
                            }
                        }
                    } // Ensure that file iterators are not at the end

                if(itr->length() == longestFileLength)
                    longestFileItr = itr;

                } // Iterate through all files for a time block

            startTimeBlock.addSecs(1);
            endBlockTime = startTimeBlock;
            endBlockTime.addSecs(timeResolution); // Establish new time block

            } // Upper bound using the longest file
            */

        return true;
    }
    else
        return false;
}

bool DataHandler::checkRange(QMap<QString,QVector<QString>>::iterator itr)
{
    /*
    QRegularExpression reHeader;

    if(*(itr.second) == "O3" || *(itr.second) == "NOx")
    {
        QString line = itr.first;
        re = QRegularExpression("[0-3][0-9]/[0-1][0-9]/[1][0-9],[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // DD/MM/YY, HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "dd/MM/yy,hh:mm:ss");
            dt = dt.addYears(100);
            if( // make comparisons between startTimeBlock and endTimeBlock)
        }

    }
    else if(*(itr.second) == "Apollo")
    {
        QString line = itr.first;
        re = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // HH:MM:SS  Still need a date for apollo!
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "hh:mm:ss");
            if( // make comparisons between startTimeBlock and endTimeBlock)
        }
    }
    else if (*(itr.second)   == "Kestral")
    {
        QString line = itr.first;
        re = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // YYYY-MM-DD HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "yyyy-MM-dd hh:mm:ss");
            if( // make comparisons between startTimeBlock and endTimeBlock)
        }
    }
    else
    {
       // Has elapsed time but not actual time...use start time for particle file and add 20 from there?,
       // How to access start time for particle when there is not way to differentiate
       // Before adding to startTimes vector, save elsewhere or...make a separate function for particle,
       // copy the way its done in pull start time
    }

     */
    return true;
}
