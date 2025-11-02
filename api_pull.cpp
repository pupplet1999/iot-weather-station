#include<iostream>//libraires
#include<string>
#include<curl/curl.h> //needed to make request from the NWS api
#include<nlohmann/json.hpp>//this is to parse json files
#include<fstream>//for loggin to a file
#include<ctime>// so we can log time
#include<iomanip>
#include<sys/stat.h>//for file status check

using namespace std;//bc im lazy
using json = nlohmann::json; // make sure json



size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) { //this is the callback function, it gets the size of the request from the API, we need size_t for 64 bit integer sizes incase the download is huge
    size_t totalSize = size * nmemb; // size of the total download
    string* str = (string*)userp; // casting userp to string pointer 
    str->append((char*)contents, totalSize); // add the bytes to the string
    return totalSize; // returns how many bytes we need to process

} //this whole thing makes usable data for us to use

bool fileExists(string& filename) {//function to check if a file exists or not
    struct stat buffer; //stat recieves information about the file
    return (stat(filename.c_str(), &buffer) == 0); //it needs to return a 0 which means it exists otherwise it will return -1 and that is FALSE
}

string APIrequest (string url) { //function for api calls, we can change the url

    CURL* curl; // making curl object
    CURLcode res;
    string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT); //initializing curl
    curl = curl_easy_init(); // easy handle to use

    if(curl){
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());//pulls data for UNCW lat, long... this sets the options for the data pull (only use 4 decimals). ALSO has to be c string for url
        
        //all this is for the header for the request
        struct curl_slist* headers = NULL; //creating a linked list to send with my request
        headers = curl_slist_append(headers, "User-Agent: IoTWeatherStation (jessica.m.horncastle@gmail.com)"); // my user agent authentication header for the request
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // setting the header with my request

        //writing the response into the readBuffer string
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); //sets the writecallback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        //do the request
        res = curl_easy_perform(curl);

        //error handling
        if(res != CURLE_OK) {
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl; //sends error out
        }

        //cleanup for curl
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);


    }

    curl_global_cleanup();
    return readBuffer; //returns the data as a string

}  


struct WeatherData { //struct to store and move the data more easily, (dont have to pass like eight variables)
    string timestamp;
    string location;
    int temperature;
    string temperatureUnit;
    int precipitationProbability;
    string windSpeed;
    string windDirection;
    string shortForecast;

};

class WeatherStation { //weather station class
private:
    string location;//these can be private, these arent going to change
    string url;

public:
    string response; //

    WeatherStation(string location, string url){//constructor for each location object that we will make for different places
        this->location = location;
        this->url = url;
    }

    WeatherData fetchCurrentForecast(){//method that returns the data into a struct called weatherdata, we can move this package around easier instead of like 7 variables
        //api call and response
        string response = APIrequest(url);
        //parse the json
        json forecastData = json::parse(response); //parses the json that is returned 
        //pulls the correct section, which is the first forecast
        auto periods = forecastData["properties"]["periods"];
        auto current = periods[0]; //grabs the first element (the current forecast)

        //all the timestamp crap, quite annoying and i dont really understand how this works too well tbh
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ltm);

        WeatherData data;//making the data struct and storing it
        data.timestamp = timestamp;
        data.location = location;
        data.temperature = current["temperature"];
        data.temperatureUnit = current["temperatureUnit"];
        data.precipitationProbability = current["probabilityOfPrecipitation"]["value"];
        data.windSpeed = current["windSpeed"];
        data.windDirection = current["windDirection"];
        data.shortForecast = current["shortForecast"];

        return data; //returns this struct with all the data stored properly into the struct
    }

    void logCurrentForecast(WeatherData& data){// function takes the api response for the supplied response and a location name
        
        string filename = "weather_log.csv";//file name
        bool isNewFile = !fileExists(filename);//makes us a bool for if the log exists

        //time to append everything into the CSV
        ofstream csvFile("weather_log.csv", ios::app);//opens and appends


        if (isNewFile){//checking if the file is new and needs a header
            csvFile << "timestamp,location,temperature,temperatureUnit,precipitationProbability,windSpeed,windDirection,shortForecast\n";// appending a header row to the CSV
        }

        csvFile << data.timestamp << "," // appends the data we want
                << data.location << ","
                << data.temperature << ","
                << data.temperatureUnit << ","
                << data.precipitationProbability << ","
                << data.windSpeed << ","
                << data.windDirection << ","
                << data.shortForecast << "\n";
        csvFile.close();//closes the file


    }
};

int main (){//main

    //weather station objects
    WeatherStation ILM("Wilmington", "https://api.weather.gov/gridpoints/ILM/92,68/forecast");
    WeatherStation RDU("Raleigh", "https://api.weather.gov/gridpoints/RAH/74,57/forecast");
    WeatherStation CLT("Charlotte", "https://api.weather.gov/gridpoints/GSP/119,65/forecast");

    
    //making the weather data structs for each station then logging the data for each
    WeatherData data1 = ILM.fetchCurrentForecast();
    ILM.logCurrentForecast(data1);

    WeatherData data2 = RDU.fetchCurrentForecast();
    RDU.logCurrentForecast(data2);

    WeatherData data3 = CLT.fetchCurrentForecast();
    CLT.logCurrentForecast(data3);

    return 0;
}