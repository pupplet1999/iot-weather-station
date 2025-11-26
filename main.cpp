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

int main (){
    //creating weather station objects
    WeatherStation ILM("Wilmington", "https://api.weather.gov/gridpoints/ILM/92,68/forecast");
    WeatherStation RDU("Raleigh", "https://api.weather.gov/gridpoints/RAH/74,57/forecast");
    WeatherStation CLT("Charlotte", "https://api.weather.gov/gridpoints/GSP/119,65/forecast");

    //making weather data structs first
    WeatherData data1 = ILM.fetchCurrentForecast();
    WeatherData data2 = RDU.fetchCurrentForecast();
    WeatherData data3 = CLT.fetchCurrentForecast();

    //objects for gather_weather (case 2)
    WeatherDataCSV weatherCSV("weather_log.csv");
    DisplayWeatherCSV display(weatherCSV);
    WeatherInputHandler inputHandler;

    cout << "\nWelcome to our Weather Station Device! v1.0";

    int selection = menu(); // call menu and store user selection

    while (selection != 5) { // repeat until quit (selection == 5)
        switch(selection){

            case 1://this pulls data into from the api into the csv file    
                
                //logging the data for each
                ILM.logCurrentForecast(data1);
                RDU.logCurrentForecast(data2); 
                CLT.logCurrentForecast(data3);

                cout << "\nCurrent weather stored for Charlotte, Raleigh, and Wilmington.\n";
                break;

            case 2: // copy pasted from gather_weather main so it does case 2 in the switch owo

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

                /*case 3:
                    cout << "avging function" << endl; // remove
                    //code to run average function...
                    break;

                case 4:
                    cout << "comparison function" << endl; // remove
                    //code to run comparison function...
                    break;*/
        }
        selection = menu(); // prompt user for input again
    }
    return 0;
}