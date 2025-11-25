#include "api_pull.hpp" // pulls weather from api, stores it into CSV
#include "gather_weather.hpp" //addsd gather weather lol



//menu funtion

void menu(){

    //just a simple menu prompt
    cout << "Welcome to our Weather Station Device! v1.0" << endl;
    cout << "Please Select from the following options:" << endl;
    cout << "1 - Collect from API" << endl;
    cout << "2 - Search for historical weather forcasts" << endl;
    cout << "3 - Average weather from location" << endl;
    cout << "4 - Compare averages across locations" << endl;

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

    int selection; //int for menu selection

    menu();

    cin >> selection;


    switch(selection){

        case 1://this pulls data into from the api into the csv file    
            
            //logging the data for each
            ILM.logCurrentForecast(data1);
            RDU.logCurrentForecast(data2); 
            CLT.logCurrentForecast(data3);

            break;

        case 2: // copy pasted from gather_weather main so it does case 2 in the switch owo



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
            break;
        //case 3:
        //code to run the averaging function
        //    break;

        //case 4:
        //code to run comparison function
        //    break;




        

        
    }

    
    return 0;
}