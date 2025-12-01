#include "api_pull.hpp" // pulls weather from api, stores it into CSV
#include "gather_weather.hpp" //added gather weather lol

// menu funtion
int menu(){
    int selection; // int for menu selection

    // just a simple menu prompt 
         cout << "\n-------------------------------------------"
         << "\nPlease select from the following options:\n\n"
         << "[1] Store this hour's weather from API\n"
         << "[2] Search from stored weather data\n"
         << "[3] Average weather from location\n"
         << "[4] Compare averages across locations\n"
         << "[5] Quit\n";
     
    cin >> selection;

    while (cin.fail() || !(selection >= 1 && selection <= 5)) { // check if num between 1-5
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "\nInvalid entry, please enter a number 1-5\n";
        cin >> selection; 
    }

    return selection;
}

int main () {
    //creating weather station objects
    WeatherStation ILM("Wilmington", "https://api.weather.gov/gridpoints/ILM/92,68/forecast");
    WeatherStation RDU("Raleigh", "https://api.weather.gov/gridpoints/RAH/74,57/forecast");
    WeatherStation CLT("Charlotte", "https://api.weather.gov/gridpoints/GSP/119,65/forecast");
    //initialize weather data structs
    weatherRecord data1;
    weatherRecord data2;
    weatherRecord data3;
    //objects for gather_weather (case 2)
    WeatherDataCSV weatherCSV(CSV_PATH); //only if you use clion, otherwise the commented out line under this works
    //WeatherDataCSV weatherCSV("weather_log.csv");
    DisplayWeatherCSV display(weatherCSV);
    WeatherInputHandler inputHandler;

    cout << "\nWelcome to our Weather Station Device! v1.0";

    int selection = menu(); // call menu and store user selection

    while (selection != 5) { // repeat until quit (selection == 5)
        switch(selection) {
            case 1: {
                //this pulls data into from the api into the csv file
                //make the weather data structs
                data1 = ILM.fetchCurrentForecast();
                data2 = RDU.fetchCurrentForecast();
                data3 = CLT.fetchCurrentForecast();
                //logging the data for each
                ILM.logCurrentForecast(data1);
                RDU.logCurrentForecast(data2); 
                CLT.logCurrentForecast(data3);

                cout << "\nCurrent weather stored for Charlotte, Raleigh, and Wilmington.\n";
                break;
            }
            case 2: {
                // copy pasted from gather_weather main so it does case 2 in the switch owo

                bool cont_check = true;
                while (cont_check){
                    cout << "\nChoose a location: \n[1] Charlotte\n[2] Raleigh\n[3] Wilmington\n[4] Quit" << "\n";
                    string chosen_loc;
                    cin >> chosen_loc;

                    // check if valid input, only update if valid
                    string loc_result;
                    do {
                        loc_result = inputHandler.chooseLocation(chosen_loc);
                        if (loc_result == "") {
                            cout << "\nInvalid location, try again:\n" << endl;
                            cin >> chosen_loc;
                        }
                    } while (loc_result == "");

                    if (loc_result == "quit") break; // exit 

                    cout << "\nEnter date for " << loc_result << " (yyyy-mm-dd):" << "\n";
                    string chosen_date;
                    cin >> chosen_date;
                    while (!inputHandler.chooseDate(chosen_date)){   // update key with date
                        cout << "\nInvalid date, try again (yyyy-mm-dd):" << "\n";
                        cin >> chosen_date;
                    }                  

                    cout << "\nEnter hour number for " << loc_result << " on " << chosen_date <<  " (hh, 24h):" << "\n";
                    string chosen_time;
                    cin >> chosen_time;
                    while (!inputHandler.chooseTime(chosen_time)){   // update key with time
                        cout << "\nInvalid time, try again (0-24, hh)" << "\n";
                        cin >> chosen_time;
                    } 
                
                    string usr_input = inputHandler.getUserKey();    // get fully constructed key
                    cout << "DEBUG USER KEY = [" << usr_input << "]\n";
                    map<string, int> lineMap = weatherCSV.get_map(); // gather map
                
                    if (lineMap.find(usr_input) != lineMap.end()){   // check if constructed key exists within map
                        display.display_info(usr_input);             // output weather
                    }else{
                        cout << "\nCould not find weather for " << loc_result << " on " << chosen_date << " at hour " << chosen_time << ".\n";
                    }
                    usr_input = inputHandler.clear_input();

                    cout << "\nWould you like to search again? y/n" << "\n";
                    cont_check = inputHandler.should_continue();
                }
                break;
            }
            case 3: {
                bool cont_check = true;

                while (cont_check) {

                    cout << "\nChoose a location to average: \n"
                            "[1] Charlotte\n"
                            "[2] Raleigh\n"
                            "[3] Wilmington\n"
                            "[4] Quit\n";

                    string chosen_loc;
                    cin >> chosen_loc;

                    // validate location using the inputHandler
                    string loc_result;
                    do {
                        loc_result = inputHandler.chooseLocation(chosen_loc);
                        if (loc_result == "") {
                            cout << "\nInvalid location, try again:\n";
                            cin >> chosen_loc;
                        }
                    } while (loc_result == "");

                    if (loc_result == "quit")
                        break;

                    // get the average for the city and print a table
                    avgResult result = computeAverages(weatherCSV, loc_result);

                    cout << "\nAverages for " << result.location << ":\n";
                    cout << "+----------------------+------------------------------+\n";
                    cout << "|   Average Temp (F)   |  Average Precip Prob (%)     |\n";
                    cout << "+----------------------+------------------------------+\n";
                    cout << "|      " << result.avgTemp
                         << "           |           " << result.avgPrecip
                         << "                |\n";
                    cout << "+----------------------+------------------------------+\n";
                    cout << "\nWould you like to average another location? y/n\n";
                    cont_check = inputHandler.should_continue();
                }

                break;
            }
            case 4:{
                bool cont_check = true;
                while (cont_check) {
                    cout << "\nChoose TWO locations to compare:\n"
                    "[1] Charlotte\n"
                    "[2] Raleigh\n"
                    "[3] Wilmington\n"
                    "[4] Quit\n";

                    string loc1, loc2;
                    //make sure first location is valid
                    cout << "\nSelect first location (or quit): ";
                    cin >> loc1;

                    string loc1_result;
                    do {
                        loc1_result = inputHandler.chooseLocation(loc1);
                        if (loc1_result == "") {
                            cout << "\nInvalid location, try again:\n";
                            cin >> loc1;
                        }
                    } while (loc1_result == "");

                    if (loc1_result == "quit") break;

                    inputHandler.clear_input(); //reset the input handler
                    // make sure second location is valid
                    cout << "\nSelect second location (or quit): ";
                    cin >> loc2;

                    string loc2_result;
                    do {
                        loc2_result = inputHandler.chooseLocation(loc2);
                        if (loc2_result == "") {
                            cout << "\nInvalid location, try again:\n";
                            cin >> loc2;
                        }
                    } while (loc2_result == "");

                    if (loc2_result == "quit") break;

                    inputHandler.clear_input(); // reset input handler again
                    // get averages from each city and display them together.
                    avgResult A = computeAverages(weatherCSV, loc1_result);
                    avgResult B = computeAverages(weatherCSV, loc2_result);

                    cout << "\nComparison of Averages:\n";
                    cout << "+--------------------------+----------------------+----------------------+\n";
                    cout << "|         Metric           |     " << A.location
                         << "        |     " << B.location << "        |\n";
                    cout << "+--------------------------+----------------------+----------------------+\n";
                    cout << "| Average Temp (F)         |      " << A.avgTemp
                         << "          |      " << B.avgTemp << "          |\n";
                    cout << "| Average Precip (%)       |      " << A.avgPrecip
                         << "          |      " << B.avgPrecip << "          |\n";
                    cout << "+--------------------------+----------------------+----------------------+\n";
                    //put it in percentage form how much hotter city A is from city B
                    double tempDiff = A.avgTemp - B.avgTemp;
                    double tempDiffWithPerc = (tempDiff / B.avgTemp) * 100.0;
                    if (tempDiff > 0) {
                        cout << "\nOn average, " << A.location
                             << " is " << tempDiff << "°F (" << tempDiffWithPerc
                             << "%) warmer than " << B.location << ".\n";
                    } else if (tempDiff < 0) {
                        cout << "\nOn average, " << B.location
                             << " is " << tempDiff << "°F (" << tempDiffWithPerc
                             << "%) warmer than " << A.location << ".\n";
                    } else
                        cout << "\nBoth locations have the exact same average temperature.\n";
                }
                    break;
            }
        }
                selection = menu(); // prompt user for input again
        }
        return 0;

}