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
    std::string conditions;
    float wind_speed;
    float max_temperature;
    float min_temperature;
    int humidity;
    int pressure;
    float visibility;
    int uv_index;
    float dew_point;
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

private:
    std::string apiKey;
    std:: string cityId;
    std:: string cityName;
    std::string weather_url;

    std::string body;
    WeatherData weather_data;

    mutable std::mutex data_mutex;
    bool stop_thread;
    std::thread update_thread;

    void parseJsonResponse(const std::string& response);
    void parseXmlResponse(const std::string& xmlResponse);
    std::string detectResponseType(const std::string& response);
    bool fetchWeatherData();
    void run();
   
};


#endif // WEATHER_H