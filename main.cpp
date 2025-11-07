#include "api_pull.hpp" // pulls weather from api, stores it into CSV

int main (){
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