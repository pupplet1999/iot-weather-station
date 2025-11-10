#include <iostream>
#include <fstream> 
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
using namespace std;

// FIXME make sure date is in format mm/dd/yyyy, currently in mm/d/yyyy
class CSV{
private:
    ifstream csvFile;
    map<string, int> lineMap;
    int line_num = 0,
        comma_loc = 0,
        temp,
        precip;

    string csv_name,
           map_key,
           line,
           timestamp,
           date,
           time,
           location,
           temp_unit,
           wind_spd,
           wind_dir,
           fc,
           usr_input;

    void createTimeMap(string csv_name){ // gather all timestamps and store their line in a map
        csvFile.open(csv_name);  
        if (!csvFile.is_open()) {
            std::cerr << "Failed to open file: " << csv_name << endl; 
        }

        while (getline(csvFile, line)){ // parse through entire CSV 
            // FIXME: slightly repeated code
            // gather timestamp
            comma_loc = line.find(','); // find comma within line
            timestamp = line.substr(0, comma_loc); // gather everything up until comma
            line = line.substr(comma_loc + 1); // start line at comma

            // gather location
            comma_loc = line.find(',');
            location = line.substr(0, comma_loc);
            line = line.substr(comma_loc + 1);

            map_key = timestamp + " " + location;
            
            cout << map_key << endl;

            line_num++;
            lineMap[map_key] = line_num; // update map with timestamp + location's line_num
        }
        csvFile.close();
    }

    void gatherLineData(int line_num){ // gather data of a certain line
        csvFile.clear(); // clear EOF and fail flags
        csvFile.seekg(0, ios::beg); // move the get pointer back to the start

        // skip to line_numth line
        for(int i = 0; i < line_num; i++){
            getline(csvFile, line);
        }

        // gather date & time
        comma_loc = line.find(','); // find comma within line
        timestamp = line.substr(0, comma_loc); // gather everything up until comma
        int space_loc = timestamp.find(' ');
        if (space_loc != string::npos) {
            date = timestamp.substr(0, space_loc);       // date = everything before the space
            time = timestamp.substr(space_loc + 1);      // time = everything after the space
        }
        line = line.substr(comma_loc + 1); // start line at comma

        // gather location
        comma_loc = line.find(',');
        location = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);

        // gather nominal temperature
        comma_loc = line.find(',');
        temp = stoi(line.substr(0, comma_loc));
        line = line.substr(comma_loc + 1);

        // gather temperature unit (F or C)
        comma_loc = line.find(',');
        temp_unit = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);

        // gather precipiation probability
        comma_loc = line.find(',');
        precip = stoi(line.substr(0, comma_loc));
        line = line.substr(comma_loc + 1);

        // gather windspeed
        comma_loc = line.find(',');
        wind_spd = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);

        // gather wind direction
        comma_loc = line.find(',');
        wind_dir = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);

        // gather forecast
        comma_loc = line.find(',');
        fc = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);
        }

public:
    CSV(string n) : csv_name(n){ // constructor takes name of CSV file, opens CSV file
        createTimeMap(csv_name); // create timestamp map
        csvFile.open(csv_name);  
        if (!csvFile.is_open()) {
            std::cerr << "Failed to open file: " << csv_name << endl; 
        }
    }
    
    string get_date(string map_key){
        gatherLineData(lineMap[map_key]); // if adding a parameter for what line to read, throw it in here
        return date;
    }

    string get_time(string map_key){
        gatherLineData(lineMap[map_key]);
        return time;
    }
    
    string get_location(string map_key){
        gatherLineData(lineMap[map_key]);
        return location;
    }

    int get_temp(string map_key){
        gatherLineData(lineMap[map_key]);
        return temp;
    }

    string get_temp_unit(string map_key){
        gatherLineData(lineMap[map_key]);
        return temp_unit;
    }

    int get_precip(string map_key){
        gatherLineData(lineMap[map_key]);
        return precip;
    }

    string get_wind_spd(string map_key){
        gatherLineData(lineMap[map_key]);
        return wind_spd;
    }

    string get_wind_dir(string map_key){
        gatherLineData(lineMap[map_key]);
        return wind_dir;
    }

    string get_fc(string map_key){
        gatherLineData(lineMap[map_key]);
        return fc;
    }

    void display_info(string map_key) {
        cout << "Date:           " << get_date(map_key) << '\n'
             << "Time:           " << get_time(map_key) << '\n'
             << "Location:       " << get_location(map_key) << '\n'
             << "Temperature:    " << get_temp(map_key) << " " << get_temp_unit(map_key) << '\n'
             << "Precipitation:  " << get_precip(map_key) << '%' << '\n'
             << "Wind Speed:     " << get_wind_spd(map_key) << '\n'
             << "Wind Direction: " << get_wind_dir(map_key) << '\n'
             << "Short Forecast: " << get_fc(map_key) << "\n\n";
    }

    string chooseLocation(string chosen_loc){
        transform(chosen_loc.begin(), chosen_loc.end(), chosen_loc.begin(),
        [](unsigned char c){ return tolower(c); }); // converts to lowercase: makes input capitalization not matter

        if (chosen_loc == "1" || chosen_loc == "charlotte"){
            usr_input += "Charlotte";
            return "Charlotte";
        } else if (chosen_loc == "2" || chosen_loc == "raleigh"){
            usr_input += "Raleigh";
            return "Raleigh";
        } else if (chosen_loc == "3" || chosen_loc == "wilmington"){
            usr_input += "Wilmington";
            return "Wilmington";
        }
        return 0;
    }

    void chooseDate(string chosen_date){
        // any necessary conversions to chosen_date can go here
        usr_input += ',' + chosen_date;

    }

    void chooseTime(string chosen_time){
        // any necessary conversions to chosen_time can go here
        usr_input += ',' + chosen_time;
        cout << "key: " << usr_input << endl;
    }
            
    ~CSV(){
        csvFile.close();
    }
};

int main(){ // FIXME currently throws an error if user entered incorrectly formatted key
    string chosen_loc,
           location,
           chosen_date,
           chosen_time;
    CSV weatherCSV("weather_log.csv");

    cout << "^^^ Temporary database above for reference of how key should be entered ^^^" << "\n";

    cout << "Choose a location: \n[1] Charlotte\n[2] Raleigh\n[3] Wilmington" << "\n";
    getline(cin, chosen_loc);
    location = weatherCSV.chooseLocation(chosen_loc); // gather user's desired location, update key with such location

    cout << "Enter date for " << location << " (yyyy-mm-dd):" << "\n";
    getline(cin, chosen_date);
    weatherCSV.chooseDate(chosen_date);

    cout << "Enter time for " << location << " on " << chosen_date <<  " (hh-mm, 24h):" << "\n";
    getline(cin, chosen_time);
    weatherCSV.chooseTime(chosen_time);
}
