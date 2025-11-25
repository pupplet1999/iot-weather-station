#ifndef GATHER_WEATHER
#define GATHER_WEATHER

#include <iostream>
#include <fstream> 
#include <sstream>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <limits>
using namespace std;


class CSVFile{
protected:
    ifstream csv_file;
    string csv_name;

public:
    CSVFile(const string& filename) : csv_name(filename){ 
        csv_file.open(csv_name);
        if (!csv_file.is_open()) {
            std::cerr << "Failed to open file: " << csv_name << endl; 
        }
    }

    string getLine(int line_num){
        csv_file.clear(); // clear EOF and fail flags
        csv_file.seekg(0, ios::beg);
        string line;
        for (int i = 0; i < line_num; i++){
            getline(csv_file, line);
        }
        return line;
    }

    ~CSVFile(){
        if (csv_file.is_open()){
            csv_file.close();
        }
    }  
};


struct WeatherRecord {
    string date;
    string hour;
    string location;
    int temp;
    string temp_unit;
    int precip;
    string wind_spd;
    string wind_dir;
    string fc;
};

// class for extracting + handling data from within the weather csv file
class WeatherDataCSV : public CSVFile{
private:
    map<string, int> lineMap; // <location> <yyyy-mm-dd> <hh:mm> -> respective line number

    void createTimeMap(string csv_name){ // gather location+timestamp and store their respective line in a map
        string line;

        int line_num = 1; // data starts at line 1

        while (getline(csv_file, line)){ // parse through entire CSV, stop at end
            // gather timestamp
            int comma_loc = line.find(','); // find comma within line
            string full_ts = line.substr(0, comma_loc); // gather full timestamp -> yyyy-mm-dd hh:mm:ss
            string ts = line.substr(0, full_ts.find(':')); // cutt off minutes and seconds in timestamp -> yyyy-mm-dd hh
            line = line.substr(comma_loc + 1); // start line at comma

            // gather location
            comma_loc = line.find(',');
            string location;
            location = line.substr(0, comma_loc);

            line = line.substr(comma_loc + 1);

            lineMap[location + ' ' + ts] = line_num; // update map with <location> <yyyy-mm-dd> <hh:mm> -> line_num
            line_num++;
        }
        csv_file.clear(); // clear EOF and fail flags
        csv_file.seekg(0, ios::beg); // move the get pointer back to the start
    }

    string extractField(string& line) { // pass by address so it doesn't create a copy of line -> changes line directly
        int comma_loc = line.find(',');
        string field = line.substr(0, comma_loc);
        line = line.substr(comma_loc + 1);
        return field;
    }

public:
    // constructor takes name of CSV file, opens CSV file, creates map
    WeatherDataCSV(const string& filename) : CSVFile(filename) { 
        createTimeMap(csv_name); // create timestamp map
        cout << "\nDEBUG: Dumping map:\n";
        for (const auto& p : lineMap) {
            cout << p.first << " -> " << p.second << "\n";
            }
        cout << "END MAP\n\n";
    }

    WeatherRecord gatherLineData(int line_num) { // gather data of a certain line
        WeatherRecord record;
        // skip to line_numth line
        string line = getLine(line_num);

        // gather date & time
        string timestamp = extractField(line); // gather everything up until comma
        int space_loc = timestamp.find(' ');
        if (space_loc != string::npos) { // if space exists
            record.date = timestamp.substr(0, space_loc); // date = everything before the space
            record.hour = timestamp.substr(space_loc + 1, 2);  // time = hour value, hh
        }

        // gather location
        record.location = extractField(line);

        // gather nominal temperature
        record.temp = stoi(extractField(line));

        // gather temperature unit (F or C)
        record.temp_unit = extractField(line);

        // gather precipiation probability
        record.precip = stoi(extractField(line));

        // gather windspeed
        record.wind_spd = extractField(line);

        // gather wind direction
        record.wind_dir = extractField(line);

        // gather forecast
        record.fc = extractField(line);

        return record;
        }
        
    const map<string, int>& get_map() const { // return reference, method cannot modify the string, 
        return lineMap;
    }
};

class DisplayWeatherCSV{
private:
    WeatherDataCSV& data; // address to hold weather CSV
public:
    DisplayWeatherCSV(WeatherDataCSV& wd) : data(wd) {} // DisplayWeatherData will take a CSV file and assign its address to be data

    void display_info(const string& map_key) {
        WeatherRecord record = data.gatherLineData(data.get_map().at(map_key));
        cout << "Date:           " << record.date << '\n'
             << "Hour:           " << record.hour << '\n'
             << "Location:       " << record.location << '\n'
             << "Temperature:    " << record.temp << " " << record.temp_unit << '\n'
             << "Precipitation:  " << record.precip << '%' << '\n'
             << "Wind Speed:     " << record.wind_spd << '\n'
             << "Wind Direction: " << record.wind_dir << '\n'
             << "Short Forecast: " << record.fc << "\n";
    }
};

class WeatherInputHandler {
private:
    string usr_input;
public:
    string chooseLocation(string chosen_loc) {
        
        transform(chosen_loc.begin(), chosen_loc.end(), chosen_loc.begin(), // convert to lowercase
        [](unsigned char c){ return tolower(c); });  

        // take user's choice and add properly formatted location to key, return formatted location
        if (chosen_loc == "1" || chosen_loc == "charlotte") {
            usr_input += "Charlotte";
            return "Charlotte";
        } else if (chosen_loc == "2" || chosen_loc == "raleigh") {
            usr_input += "Raleigh";
            return "Raleigh";
        } else if (chosen_loc == "3" || chosen_loc == "wilmington") {
            usr_input += "Wilmington";
            return "Wilmington";
        }
        return "";
    }

    bool chooseDate(string chosen_date) {
        if (chosen_date.length() != 10){ // check length
            return false;
        }

        if (chosen_date[4] != '-' || chosen_date[7] != '-') { // check if dashes are in the right spot
            return false; 
        } 

        // make sure numeric
        for (int i = 0; i < 10; i++) { 
            if (i == 4 || i == 7) continue; // skip the dashes
            if (!isdigit(chosen_date[i])) return false;
        }

        // check month/day ranges
        int month = stoi(chosen_date.substr(5,2));
        int day = stoi(chosen_date.substr(8,2));
        if (month < 1 || month > 12) { 
            return false;
        }
        if (day < 1 || day > 31) {
            return false;  
        } 

        usr_input += ' ' + chosen_date;
        return true;
    }

    void chooseTime(string chosen_time) {
        usr_input += ' ' + chosen_time;
    }
    
    bool should_continue() {
        string cont;
        while(true){
            cin >> cont;
            if (cont=="Y" || cont=="y") return true;
            if (cont=="N" || cont=="n") return false;
            cout << "Please enter either y/n:\n";
        }
    }

    const string& getUserKey() const {
        return usr_input;
    }

    string clear_input() {
        usr_input.clear();
        return usr_input;
    }
};
/* commenting all this out until i get it to work!
int main() { 
    WeatherDataCSV weatherCSV("weather_log.csv");
    DisplayWeatherCSV display(weatherCSV);
    WeatherInputHandler inputHandler;

    bool cont_check = true;
    while (cont_check){
        cout << "Choose a location: \n[1] Charlotte\n[2] Raleigh\n[3] Wilmington" << "\n";
        string chosen_loc;
        cin >> chosen_loc;

        // check if valid input, only update if valid
        string loc_result;
        do {
            loc_result = inputHandler.chooseLocation(chosen_loc);
            if (loc_result == "") {
                cout << "Invalid location, try again:" << endl;
                cin >> chosen_loc;
            }
        } while (loc_result == "");

        cout << "Enter date for " << loc_result << " (yyyy-mm-dd):" << "\n";
        string chosen_date;
        cin >> chosen_date;
        while (!inputHandler.chooseDate(chosen_date)){   // update key with date
            cout << "Invalid date, try again (yyyy-mm-dd):" << "\n";
            cin >> chosen_date;
        }                  

        cout << "Enter hour number for " << loc_result << " on " << chosen_date <<  " (hh, 24h):" << "\n";
        string chosen_time;
        cin >> chosen_time;
        inputHandler.chooseTime(chosen_time);            // update key with time
    
        string usr_input = inputHandler.getUserKey();    // get fully constructed key

        map<string, int> lineMap = weatherCSV.get_map(); // gather map
    
        if (lineMap.find(usr_input) != lineMap.end()){   // check if constructed key exists within map
            cout << "\n==========================\n";
            display.display_info(usr_input);             // output weather
            cout << "==========================\n\n";
        }else{
            cout << "Could not find weather for " << loc_result << " on " << chosen_date << " at hour " << chosen_time << ".\n";
        }
        usr_input = inputHandler.clear_input();

        cout << "Would you like to search again? y/n" << "\n";
        cont_check = inputHandler.should_continue();
    }
}
*/
#endif //end lol
