#include "datahandler.h"
#include "updatewindow.h"

DataHandler::DataHandler(QVector<FileSelect*> *fs, QVector<int> *res, int time, QString save, int timeScale, QString units)
{
    fSList = fs;
    resolutions = res;
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
                    /*
                    if(!(pullStartTime(fileType, i)))
                    {
                        file.close();
                        return false;
                    }
                    */ // Commented out because startTime is not pulled until after date has been added
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
    fileHeaders.push_back(vec);
    vec.clear();

    while(!in->atEnd())
    {
        QString line = in->readLine();
        vec.push_back(line);
    } // Store all data for given file
    fileData.push_back(vec);
    fileTypes.push_back(fileType);
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
            dt = dt.addYears(100); //  Add 100 because dd/MM/yy format does not specify the century, so QDateTime assumes 19**
            startTimes.push_back(dt);
            return true;
        }
    }
    else if(fileType == "Apollo")
    {
        QString line = fileData[index][0];
        re = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9],[0-3][0-9]/[0-1][0-9]/[1][0-9]"); // HH:MM:SS, dd/MM/yy
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch) {
            line = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(line, "hh:mm:ss,dd/MM/yy");
            dt = dt.addYears(100);
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

void DataHandler::addDateApollo()
{
    QVector<QString>::iterator vItr;
    QDateTime fictionalDate = startTimeBlock;
    int timeRes = 0;

    for(int i = 0; i < fSList->length(); i++)
    {
        if(fSList->at(i)->getFileType() == "Apollo")
        {
            vItr = fileHeaders[i].begin();
            *vItr = *vItr + ",Date";

            vItr = fileData[i].begin();
            while(vItr != fileData[i].end())
            {
                fictionalDate = fictionalDate.addSecs(timeRes);
                *vItr = *vItr + "," + fictionalDate.toString("dd/MM/yy");
                timeRes = 6;
                vItr++;
            }// Modify recorded data to add Date header and date data
            pullStartTime("Apollo", i);
        }
    } // Look for all Apollo files
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
                    QString date = "Time Block";

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
        stream1 << startTimeBlock.toString("dd/MM/yy hh:mm:ss") << " - " << endTimeBlock.toString("dd/MM/yy hh:mm:ss") << ",";
        stream1.flush();
        stream2 << startTimeBlock.toString("dd/MM/yy hh:mm:ss") << " - " << endTimeBlock.toString("dd/MM/yy hh:mm:ss") << ",";
        stream2.flush();

        int temp = 0;
        int longestFileLength;
        int longestFileIndex;
        int longestRange = 0;


        // Move to separate function
        QVector<QVector<QString>> inRangeData;
        for(int k = 0; k < fileData.size(); k++)
        {
            QVector<QString> l;
            inRangeData.push_back(l);
        } // Create inner vectors

        QVector<QString>::iterator longestFileItr;
        QList<QVector<QString>::iterator> itrStorage;

        for(int i = 0; i < fileData.length(); i++)
        {
            QVector<QString>::iterator vItr = fileData[i].begin();
            itrStorage.push_back(vItr);

            if(fileData.at(i).length() > temp)
            {
                longestFileItr = vItr;
                temp = fileData.at(i).length();
                longestFileLength = temp;
                longestFileIndex = i;
            } // Searching for longest file

        } // Setting up file iterators

        bool isInRange;
        bool dataToExamine = true;
        int count = 0;
        while(dataToExamine)
        {
            temp = 0;
            count  = 0;
            for(int j = 0; j < fileData.length(); j++)
            {
                isInRange = true;
                QVector<QString>::iterator vItr = itrStorage.at(j);
                while(isInRange && vItr != fileData.at(j).end())
                {
                    isInRange = checkRange(fileTypes.at(j), vItr, j);
                    if(isInRange)
                    {
                        inRangeData[j].push_back(*vItr);
                        if(inRangeData[j].length() > temp)
                        {
                            temp = inRangeData[j].length();
                            longestRange = temp;

                        } // Track longest vector of data

                        vItr++;

                    } // If data is in interval

                } // While data is in range and not at the end of the file, keeping adding items to block

                itrStorage[j] = vItr;

            } // Iterate through all files for a time block

            addDataToBlockBefore(&inRangeData, &fileBefore, longestRange);
            addDataToBlockAfter(&inRangeData, &fileAfter);

            newTimeBlock();

            for(int e = 0; e < inRangeData.length(); e++)
                inRangeData[e].clear();

            for(int n = 0; n < fileData.length(); n++)
            {
                if(itrStorage.at(n) != fileData.at(n).end())
                    count++;
            } // Checks if any data is left to process
            if(count == 0)
                dataToExamine = false;

            if(dataToExamine)
            {
                stream1 << startTimeBlock.toString("dd/MM/yy hh:mm:ss") << " - " << endTimeBlock.toString("dd/MM/yy hh:mm:ss") << ",";
                stream1.flush();
                stream2 << startTimeBlock.toString("dd/MM/yy hh:mm:ss") << " - " << endTimeBlock.toString("dd/MM/yy hh:mm:ss") << ",";
                stream2.flush();
            } // Print new time block if there is data left to output

        } // Iterate until all data has been output to files

        fileBefore.close();
        fileAfter.close();

        return true;

    } // Make sure both save files open
    else
        return false;
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
    endTimeBlock = endTimeBlock.addSecs(timeResolution);
}

bool DataHandler::checkRange(QString fileType, QVector<QString>::iterator vItr, int index)
{
    QRegularExpression re;

    if(fileType == "O3" || fileType == "NOx")
    {
        QString line = *vItr;
        re = QRegularExpression("[0-3][0-9]/[0-1][0-9]/[1][0-9],[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // DD/MM/YY, HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(data, "dd/MM/yy,hh:mm:ss");
            dt = dt.addYears(100);
            if(dt < startTimeBlock)
                return false;
            else if (dt > endTimeBlock)
                return false;
            else
                return true;
        }

    }
    else if(fileType == "Apollo")
    {
        QString line = *vItr;
        re = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9],[0-3][0-9]/[0-1][0-9]/[1][0-9]"); // HH:MM:SS, dd/MM/yy
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(data, "hh:mm:ss,dd/MM/yy");
            dt = dt.addYears(100);
            if(dt < startTimeBlock)
                return false;
            else if (dt > endTimeBlock)
                return false;
            else
                return true;
        }
    }
    else if (fileType == "Kestral")
    {
        QString line = *vItr;
        re = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // YYYY-MM-DD HH:MM:SS
        QRegularExpressionMatch match = re.match(line);
        bool hasMatch = match.hasMatch();
        if(hasMatch)
        {
            QString data = match.captured(0);
            QDateTime dt;
            dt = dt.fromString(data, "yyyy-MM-dd hh:mm:ss");
            if(dt < startTimeBlock)
                return false;
            else if (dt > endTimeBlock)
                return false;
            else
                return true;
        }
    }
    else
    {
        QString line = *vItr;
        QDateTime dt = startTimes[index];

        QStringList query = line.split(",");
        QString timeRes = query[0];
        int sec = timeRes.toInt();

        dt = dt.addSecs(sec);

        if(dt < startTimeBlock)
            return false;
        else if (dt > endTimeBlock)
            return false;
        else
            return true;
    } // Particle file

    return false;
}

void DataHandler::addDataToBlockBefore(QVector<QVector<QString>>* data, QFile* fB, int longestRange)
{
    QTextStream sB(fB);

    // Before
    for(int j = 0; j < longestRange; j++)
    {
        for(int i = 0; i < data->length(); i++)
        {
            if(data->at(i).length() > j)
            {
                sB << data->at(i).at(j) << ",";
                sB.flush();
                sB << ",";
            }
            else
            {
                QString blank = "";
                int commas = countCommas(fileData[i][0]);
                for(int l = 0; l <= commas; l++)
                    blank += ",";
                sB << blank << ",";
                sB.flush();
            } // The number of commas needed to space empty data properly
        }
        sB << "\n";
        if((j + 1) < longestRange)
            sB << ",";
        sB.flush();
    }
}

void DataHandler::addDataToBlockAfter(QVector<QVector<QString>>* data, QFile* fA)
{
    QTextStream sA(fA);
    QVector<QStringList> sL;
    QString str = "";

    // After
    for(int j = 0; j < data->length(); j++)
    {
        sL.clear();
        str = "";
        for(int i = 0; i < data->at(j).length(); i++)
        {
            // If query is not the first, Date, or Time, we can average
            QStringList query = data->at(j).at(i).split(",");
            sL.push_back(query);
        }

        if(!sL.empty())
        {
            for(int column = 0; column < sL[0].length(); column++)
            {
                for(int row = 0; row < sL.length(); row++)
                {
                    if(!edgeCases(&sL, row, column))
                    {
                        // Items that need to be averaged
                        str = str + averageData(&sL, column);
                        if(column + 1 != sL[0].length())
                            str = str + ",";
                        row = sL.length();
                    }
                    else
                    {
                        // Comment out the second str line to remove ranges of non-averaged data
                        str = str + sL.at(0).at(column);
                        str = str + " -> " + sL.at(sL.length() - 1).at(column);
                        if(column + 1 != sL[0].length())
                            str = str + ",";
                        row = sL.length();
                    }
                }
            }
            sA << str << ",";
            sA.flush();
            sA << ",";
        }
        else
        {
            QString blank = "";
            int commas = countCommas(fileData[j][0]);
            for(int l = 0; l <= commas; l++)
                blank += ",";
            sA << blank << ",";
            sA.flush();
        }
    }
    sA << "\n";
    sA.flush();
}

bool DataHandler::edgeCases(QVector<QStringList> *sL, int row, int column)
{
    if(column == 0)
        return true;
    else
    {
        QString line = sL->at(row).at(column);
        QRegularExpressionMatch match;
        bool hasMatch;

        QRegularExpression reDate1 = QRegularExpression("[0-3][0-9]/[0-1][0-9]/[1][0-9]"); // DD/MM/YY
        match = reDate1.match(line);
        hasMatch = match.hasMatch();
        if(hasMatch)
            return true;

        QRegularExpression reDate2 = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9]"); // YYYY-MM-DD
        match = reDate2.match(line);
        hasMatch = match.hasMatch();
        if(hasMatch)
            return true;

        QRegularExpression reTime1 = QRegularExpression("[0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // HH:MM:SS
        match = reTime1.match(line);
        hasMatch = match.hasMatch();
        if(hasMatch)
            return true;
    }
    return false;
}

QString DataHandler::averageData(QVector<QStringList> *sL, int column)
{
    double sum = 0;
    QString line;

    for(int i = 0; i < sL->length(); i++)
    {
        line = sL->at(i).at(column);
        sum += line.toDouble();
    }

    double average = sum / sL->length();
    return line = QString::number(average);
}

void DataHandler::newTimeBlock()
{
    startTimeBlock = endTimeBlock;
    startTimeBlock = startTimeBlock.addSecs(1);
    endTimeBlock = startTimeBlock;
    endTimeBlock = endTimeBlock.addSecs(timeResolution); // Establish new time block

}
