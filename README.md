# CDP
Comma Separate Value Data Processor

\Overview

This is the original and only copy of this project which is entirely based in Qt, a cross-platform application framework, and is written in C++.  Comma-Separated Value Data Processor (CDP) is a desktop application that was designed to manage large csv file data sets. Generally speaking, comma-separated values (csv) files stores tabular data. Each line has a record of data that reflects the headers of the file. The most important piece of data that CDP expects is a time stamp that records the time and date indicating when the data was acquired. The goal is to bring together unique files and match them together based on time stamps of collected data. CDP has a simple interface that only accepts input csv files and returns two files: one of the files outputs blocks of data that fit within a certain time interval and the second returns those blocks of data averaged together into one line. 

\Distribution
…in progress

\Procedure Overview

CDP provides a simple interface that is intuitive to the user provided they know the objective of the application. So, after the user has selected the files they choose to compile the following procedure is what CDP follows (not including initialization of UI):

1)	Validates user’s input
  a.	Checks user’s file selection
  b.	Checks user’s save file
2)	Reads the files provided by the user
  a.	Cross checks that files provided are valid for data processing
  b.	Stores the headers and data from files individually
3)	Identify beginning time for each file
  a.	Locate start time of each file
  b.	Extract time 
  c.	Compare them to find the earliest time
4)	Print headers in single output
  a.	Run sub-routine to determine formatting of file based on commas in all files
  b.	Print headers directly to output files
5)	Determine time interval 
  a.	Calculate minimum time
  b.	Scale minimum time to users specified scalar
6)	Compile data that fits in a time block
  a.	Calculate time block
  b.	Use start time and apply the time interval to the time stamp to construct a time block
7)	Print data blocks to one of the outputs
  a.	Output selected data for a time block before averaging
8)	Average all data in a time block
  a.	Average all corresponding cells of data
  b.	Print averaged data to second output file

\Procedure Details

1)	Validating users input:
The user is required to provide at least two files in order to format them together. So, a simple check if there are two files is done. Additionally, the user must specify a file they wish to save the output files to. There are additional fields that the user can specify but they are not required to. CDP will use default settings if nothing is specified.

2)	Reads the files provided by the user: 
One hurdle in designing CDP is that csv files are by no means universal. The only expectation is that the data is in a tabular form separated by commas. As a result, CDP is limited to the files is can process. A bank of expected files is created, so that when files are input CDP can cross check that the files provided are in the set of files that CDP can handle. The only limitation is memory; otherwise the bank could theoretically hold all possible file types and handle all permutations of files. Sadly, CDP is limited to a certain set of files. Once the files are validated, CDP opens all files and stores the headers of the file and data separately. 

3)	Identify beginning time for each file: 
Each file has lines of data and in each line of data exists a time stamp indicating when the data was recorded. We search for the earliest time, typically the first line of data recorded, using regular expressions. Store the earliest time for a particular file. After collecting all the start times find the time that is the earliest relative to the rest based on day, hour, minute, and second.

4)	Print headers in single output:
As previously stated the data and headers were stored individually. Now, we can access the headers without having to encounter any data to complicate the process of printing the headers. Each of the headers has text and commas that follow it. The goal is to format all the headers for each file by maintaining correct spacing of each set of data. A sub-routine runs to find out how to manage the spacing based on the header that uses the most spacing (i.e. number of cells in a csv translator). Once the number of commas is known CDP can print each header of each file with correct spacing.

5)	Determine time interval:
On the interface, there is a field titled, “minimum time” which is the minimum size of the time interval. By size, I mean the amount of time in the interval. This can be calculated by first finding the time resolution of each file. Time resolution means the interval of time that passes before another line of data is collected. For example, 20 seconds must pass after the initial and every other line of data is collected for a particular source of data. Then, once time resolutions are discovered, we can find the minimum amount of time that must pass for all files to collect data. Unfortunately, simply finding the minimum time for one line of data for each file to be collected can causes other files to have data overlapping in several time intervals. In order to alleviate overlapping intervals, using all time resolutions of files, we can calculated a least common multiple of the values and find a time that sufficiently fits lines of data without causing start time overlap in other time intervals. Next, the user can specify how to scale the minimum time using units of seconds, minutes, hours, and days so, we apply the scale time to the minimum time which finally determines the time interval that will be used for the remainder of the process.
 
6)	Compile data that fits in a time block:
Now that a time interval has been found, time stamps for lines of data can have the time interval be applied to it to construct a time block. For each of the files, we begin verifying if a line of data fits in the time block which can be done by comparing times and discovering if the start time of a line of data is still within the block. Only start times cannot overlap into other block, but end times can overlap into other time block. Each line that can satisfy the block is temporarily stored. 

7)	Print data blocks to one of the outputs:
Prior to processing the data, the stored data is going to be printed to the second output file that allows the user to locate the origin of a processed line of. The file will look similar to the files that were originally given. The additions are going to be date time headers at the beginning of each time block that the time stamps associated with it.

8)	Average all data in a time interval:
Before constructing the next time block CDP looks at the temporarily stored data for each header with data that fits in a time block and averages data in each column to produce a single line of output that is printed to output file specified by the user. This is repeated until all data is processed. For example, an Ozone csv file has headers titled decimal that contain double values. The column of data under the header decimal is averaged but only for data that fits in a time block. After the temporarily stored data is averaged, the next time block can be constructed by adding the time interval to the current time blocks time stamp. Steps 6-8 repeat for the entirety of the data sets.


