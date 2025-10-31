#include<iostream>//libraires
#include<string>
#include<curl/curl.h> //needed to make request from the NWS api
#include<nlohmann/json.hpp>//this is to parse json files
#include<fstream>//for loggin to a file
#include<ctime>// so we can log time
#include<iomanip>
#include<sys/stat.h>//for file status check



using namespace std;//bc im lazy
using json = nlohmann::json; // make sure jspon

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



void logCurrentForecast(string response, string location){//takes the api response for the supplied

    json forecastData = json::parse(response); //parses the json that is returned 
    
    auto periods = forecastData["properties"]["periods"];
    auto current = periods[0]; //grabs the first element (the current forecast)

    //pulling all the data into variables 
    int temperature = current["temperature"];
    string temperatureUnit = current["temperatureUnit"];
    int precipitationProbability = current["probabilityOfPrecipitation"]["value"];//nested list
    string windSpeed = current["windSpeed"];
    string windDirection = current["windDirection"];
    string shortForecast = current["shortForecast"];

    //setup for the timestamp in CSV
    time_t now = time(0); //returns number of seconds since Unix epoch lol, so goofy, learned something new
    tm *ltm = localtime(&now); //localtime() converts now into a struct with fields to fill out
    char timestamp[20]; //array of characters for the timestamp that needs to be converted to a nice string
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", ltm);// makes a nice and neat string
    
    string filename = "weather_log.csv";//file name
    bool isNewFile = !fileExists(filename);//makes us a bool for if the log exists

    //time to append everything into the CSV
    ofstream csvFile("weather_log.csv", ios::app);//opens and appends

    

    

    if (isNewFile){//checking if the file is new and needs a header
        csvFile << "timestamp,location,temperature,temperatureUnit,precipitationProbability,windSpeed,windDirection,shortForecast\n";// appending a header row to the CSV
    }

    csvFile << timestamp << "," // appends the data we want
            << location << ","
            << temperature << ","
            << temperatureUnit << ","
            << precipitationProbability << ","
            << windSpeed << ","
            << windDirection << ","
            << shortForecast << "\n";
    csvFile.close();//closes the file


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


int main (){//main
    string url = "https://api.weather.gov/gridpoints/ILM/92,68/forecast";

    string response = APIrequest(url); //turns the api response into a string

    string location = "ILM";

    logCurrentForecast(response, location); //this takes hte api request and just a location then 




    return 0;
}