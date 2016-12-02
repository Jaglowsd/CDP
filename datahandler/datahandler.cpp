#include "datahandler.h"

DataHandler::DataHandler(QVector<FileSelect*> *fs, QVector<int> *res, QStringList *types, int time, QString str, int timeScale, QString units)
{
    fSList = fs;
    resolutions = res;
    fileTypes = types;
    timeResolution = time;
    saveDestination = str;
    this->timeScale = timeScale;
    this->units = units;
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
                    recordFileData(&in, reHeader);
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
                    recordFileData(&in, reHeader);
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
                    recordFileData(&in, reHeader);
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
                    recordFileData(&in, reHeader);
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
                    recordFileData(&in, reHeader);
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

void DataHandler::recordFileData(QTextStream *in, QRegularExpression re)
{
    bool hasMatch = false;
    QVector<QString> vec;

    while(!hasMatch)
    {
        QString line = in->readLine();
        line = line.trimmed();
        vec.push_back(line);
        QRegularExpressionMatch match = re.match(line);
        hasMatch = match.hasMatch();
    }
    fileHeaders.push_back(vec);
    vec.clear();

    while(!in->atEnd())
    {
        QString line = in->readLine();
        vec.push_back(line);
    }
    fileData.push_back(vec);
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
        re = QRegularExpression("[1-2][0-9][0-9][0-9]-[0-1][0-9]-[0-3][0-9] [0-2][0-9]:[0-5][0-9]:[0-5][0-9]"); // YYYY-MM-DD, HH:MM:SS
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
    QDateTime earliest = QDateTime::currentDateTime().addYears(2);

    for(QVector<QDateTime>::iterator itr = startTimes.begin(); itr != startTimes.end(); itr++)
    {
        QDateTime temp = *itr;

        if(earliest.msecsTo(temp) <= 0)
            earliest = temp;
    }

    startTime = earliest;
    if(startTime < QDateTime::currentDateTime().addYears(1))
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

    QFile file(saveDestination);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        for(i = 0; i < fSList->length(); i++)
        {
            QString fileName = fSList->at(i)->getFileName();
            QFileInfo fName = QFileInfo(fileName);
            if(fileName != "")
            {
                fileName = ",From " + fName.baseName();

                for(int a = 0; a < commas[i]; a++)
                    fileName += ",";
                stream << fileName;
            }

        }
        stream << "\n";
        stream.flush();

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
                    stream << s;
                }
                else
                {
                    QString str;
                    QString date = "Date Time";

                    if(fileHeaders[k].indexOf(fileHeaders[k][headersRemaining]) == 0)
                        stream << date;

                    str = fileHeaders[k][headersRemaining];
                    int count = countCommas(str);

                    stringLength = commas[k] - count;
                    if(stringLength > 0)
                    {
                        for(int c = 1; c <= stringLength; c++)
                            str += ",";
                    }
                    str = "," + str;
                    stream << str;
                }

            }
            stream << "\n";
            headersRemaining--;
        }
        file.close();
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


int DataHandler::scaledByTimeResolution()
{
    endTimeBlock = startTime;
    if(timeScale != 0)
        timeResolution = timeResolution * timeScale;

    if(units == "sec")
    {
        endTimeBlock.addSecs(timeResolution);
    }
    else if(units == "min")
    {
        endTimeBlock.addSecs(timeResolution*60);
    }
    else if(units == "hour")
    {
        endTimeBlock.addSecs(timeResolution*3600);
    }
    else if(units == "day")
        endTimeBlock.addSecs(timeResolution*82400);
    else
        return -1;

    return -1;
}
