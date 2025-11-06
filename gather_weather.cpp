#include <iostream>
#include <fstream> 
#include <sstream>
#include <string>

ifstream csvFile;
string line = "";
csvFile.open("weather_log.csv");
getline(csvFile, line);
cout << line << endl;
csvFile.close()

location = line.find(','); // returns numeric location of first comma



