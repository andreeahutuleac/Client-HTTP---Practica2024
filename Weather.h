#ifndef WEATHER_H
#define WEATHER_H

#include <string>
#include<thread>
#include<mutex>
#include<ctime>
#include <atomic>
#include "HttpClient.h"
#include<jsoncpp/json/json.h>
#include "tinyxml2.h"
#include <tinyxml2.h>

struct WeatherData
{
    float temperature;
    float feels_like;
    float max_temperature;
    float min_temperature;
    int humidity;
    int pressure;
    float wind_speed;
    int wind_deg;
    float visibility;
    float uv_index;
    float dew_point;
    std::string conditions;
    std::string cityName;
    std::string country;
    std::string country_code;
    std::string timezone;
    std::string base;
    std::string name;
    int64_t sunrise;
    int64_t sunset;
    int64_t dt;
    int64_t timezone_offset;
    int clouds;
    std::string weather_overview;
    float latitude;
    float longitude;
    float wind_gust;
    int wind_direction;
    float rain_volume;
};

class Weather
{
public:
    Weather(const std::string& apiKey, const std::string& cityId);
    ~Weather();

    void start();
    WeatherData getWeatherData() const;
    void stop();
   
    void printWeatherData() const;
    void updateWeatherData();

    void setCityId(const std::string& newCityId);
    void setCityName(const std::string& newCityName);
    void setCountryName(const std::string& newCountryName);
    std:: string findCityIdByName(const std::string& cityName, const std::string& cityListFilePath);
   
    void readPrintCountriesFromFile(const std::string& countryListFilePath);

private:
    std::string apiKey;
    std:: string cityId;
    std:: string city_name;
    std:: string country_name;
    std::string weather_url;
    std::string uv_url;
    std::string overview_url;
    std::string dew_point_url;
    std::string cityListFilePath;
    std::string countryListFilePath;

    std::string body;
    WeatherData weather_data;

    mutable std::mutex data_mutex;
    bool stop_thread;
    std::thread update_thread;

    void parseJsonResponse(const std::string& response);
    void parseXmlResponse(const std::string& xmlResponse);
    std::string detectResponseType(const std::string& response);
    void fetchWeatherOverview(float lat, float lon);
    bool fetchWeatherData();
    void run();

    std::string jsonFilePath;
    void readCitiesFromJson(const std::string& jsonFilePath);
    void readCityFromFile(const std::string& cityId, const std::string& cityListFilePath);
   

};


#endif // WEATHER_H