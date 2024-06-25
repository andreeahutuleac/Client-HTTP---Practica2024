#include "Weather.h"
#include<iostream>
#include "tinyxml2.h"
#include <sstream>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <json/json.h>
#include <tinyxml2.h>
#include <iomanip>
#include <fstream>

Weather::Weather(const std::string& apiKey, const std::string& cityId)
    :apiKey(apiKey), cityId(cityId),stop_thread(false)
{
//eroare la deschiderea fisierului

    //   std::ifstream cityDataFile("city_data.json");
    // if (!cityDataFile.is_open()) {
    //     throw std::runtime_error("Failed to open city data file.");
    // }

    // Json::Value root;
    // Json::CharReaderBuilder builder;
    // std::string parseErrors;
    // if (!Json::parseFromStream(builder, cityDataFile, &root, &parseErrors)) {
    //     throw std::runtime_error("Failed to parse city data JSON: " + parseErrors);
    // }

    // cityDataFile.close();

    // for (const auto& city : root) {
    //     if (city["name"].asString() == cityName) {
    //         this->cityName = cityName;
    //         this->weather_url = "http://api.openweathermap.org/data/2.5/weather?q=" + cityName + "&appid=" + apiKey + "&units=metric&mode=xml";
    //         return;
    //     }
    // }

    // throw std::runtime_error("City name not found in city data.");

    this->weather_url = "http://api.openweathermap.org/data/2.5/weather?q=" + cityName + "&appid=" + apiKey + "&units=metric&mode=xml";
}

Weather::~Weather()
{
    stop();
}

void Weather::start()
{
    stop_thread=false;
    update_thread=std::thread(&Weather::run, this);
}

void Weather::stop()
{
    stop_thread=true;
    if(update_thread.joinable())
    {
        update_thread.join();
    }
}

WeatherData Weather::getWeatherData()const
{
    std::lock_guard<std::mutex> lock(data_mutex);
    return weather_data;
}

void Weather::run()
{
    while(!stop_thread)
    {
        // if(fetchWeatherData())
        // {
        //     //printWeatherData();
        //     std::this_thread::sleep_for(std::chrono::seconds(60));
        // }
        // else{
        //     std::this_thread::sleep_for(std::chrono::seconds(10));
        // }

        updateWeatherData();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

void Weather::parseJsonResponse(const std::string& jsonResponse)
{
    //std::cout<<"JSON response: "<<jsonResponse<<std::endl;
 
    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    std::istringstream s(jsonResponse);
    
    if (!Json::parseFromStream(builder, s, &root, &errors)) 
    {
        throw std::runtime_error("Failed to parse JSON: " + errors);
    }

    std::lock_guard<std::mutex> lock(data_mutex);

    //temperatura principala
    if (root.isMember("main") && root["main"].isMember("temp")) {
        weather_data.temperature = root["main"]["temp"].asFloat();
    } else {
        std::cerr << "Temperature data not found in JSON response." << std::endl;
        weather_data.temperature = 0.0f; 
    }

    //descriere conditii meteo
   if (root.isMember("weather") && root["weather"].isArray() && root["weather"].size() > 0) {
        weather_data.conditions = root["weather"][0]["description"].asString();
    } else {
        std::cerr << "Weather conditions data not found in JSON response." << std::endl;
        weather_data.conditions = "Unknown"; 
    }

    //viteza vant
     if (root.isMember("wind") && root["wind"].isMember("speed")) {
        weather_data.wind_speed = root["wind"]["speed"].asFloat();
    } else {
        std::cerr << "Wind speed data not found in JSON response." << std::endl;
        weather_data.wind_speed = 0.0f; 
    }

    //temperatura maxima
     if (root.isMember("main") && root["main"].isMember("temp_max")) {
        weather_data.max_temperature = root["main"]["temp_max"].asFloat();
    } else {
        std::cerr << "Max Temperature data not found in JSON response." << std::endl;
        weather_data.max_temperature = 0.0f; 
    }

    //temperatura minima
    if (root.isMember("main") && root["main"].isMember("temp_min")) {
        weather_data.min_temperature = root["main"]["temp_min"].asFloat();
    } else {
        std::cerr << "Min Temperature data not found in JSON response." << std::endl;
        weather_data.min_temperature = 0.0f;
    }

    // Umiditate
    if (root.isMember("main") && root["main"].isMember("humidity")) {
        weather_data.humidity = root["main"]["humidity"].asInt();
    } else {
        std::cerr << "Humidity data not found in JSON response." << std::endl;
        weather_data.humidity = 0; 
    }

    // Presiune
     if (root.isMember("main") && root["main"].isMember("pressure")) {
        weather_data.pressure = root["main"]["pressure"].asInt();
    } else {
        std::cerr << "Pressure data not found in JSON response." << std::endl;
        weather_data.pressure = 0; 
    }

    // Vizibilitate
    if (root.isMember("visibility")) {
        weather_data.visibility = root["visibility"].asFloat() / 1000.0; 
    } else {
        std::cerr << "Visibility data not found in JSON response." << std::endl;
        weather_data.visibility = 0.0f; 
    }

    // Punct de roua
       if (root.isMember("main") && root["main"].isMember("dew_point")) {
        weather_data.dew_point = root["main"]["dew_point"].asFloat();
    } else {
        std::cerr << "Dew point data not found in JSON response." << std::endl;
        weather_data.dew_point = 0.0f; 
    }

    // Indice UV
     if (root.isMember("uvi")) {
        weather_data.uv_index = root["uvi"].asInt();
    } else {
        std::cerr << "UV Index data not found in JSON response." << std::endl;
        weather_data.uv_index = 0; 
    }
}


void Weather::parseXmlResponse(const std::string& xmlResponse)
{
    //std::cout << "XML response: " << xmlResponse << std::endl;

    tinyxml2::XMLDocument doc;
    if (doc.Parse(xmlResponse.c_str()) != tinyxml2::XML_SUCCESS)
    {
        throw std::runtime_error("Failed to parse XML response.");
    }

    std::lock_guard<std::mutex> lock(data_mutex);

    //temperatura principala
    tinyxml2::XMLElement* temperatureElem = doc.FirstChildElement("current")->FirstChildElement("temperature");
  if (temperatureElem)
    {
        const char* temperatureValue = temperatureElem->Attribute("value");
        if (temperatureValue)
        {
            weather_data.temperature = std::stof(temperatureValue);
        }
        else
        {
            std::cerr << "Temperature value attribute not found in XML response." << std::endl;
            weather_data.temperature = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Temperature element not found in XML response." << std::endl;
        weather_data.temperature = 0.0f; 
    }

    //conditii meteo
    tinyxml2::XMLElement* weatherElem = doc.FirstChildElement("current")->FirstChildElement("weather");
    if (weatherElem)
    {
        const char* descriptionValue = weatherElem->Attribute("value");
        if (descriptionValue)
        {
            weather_data.conditions = descriptionValue;
        }
        else
        {
            std::cerr << "Weather description attribute not found in XML response." << std::endl;
            weather_data.conditions = "Unknown"; 
        }
    }
    else
    {
        std::cerr << "Weather element not found in XML response." << std::endl;
        weather_data.conditions = "Unknown";
    }

    //viteza vant
    tinyxml2::XMLElement* windSpeedElem = doc.FirstChildElement("current")->FirstChildElement("wind")->FirstChildElement("speed");
     if (windSpeedElem)
    {
        const char* windSpeedValue = windSpeedElem->Attribute("value");
        if (windSpeedValue)
        {
            weather_data.wind_speed = std::stof(windSpeedValue);
        }
        else
        {
            std::cerr << "Wind speed value attribute not found in XML response." << std::endl;
            weather_data.wind_speed = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Wind element or speed element not found in XML response." << std::endl;
        weather_data.wind_speed = 0.0f;
    }

    //temperatura maxima
    tinyxml2::XMLElement* temperatureMaxElem = doc.FirstChildElement("current")->FirstChildElement("temperature")->FirstChildElement("max");
   if (temperatureMaxElem)
    {
        const char* maxTempValue = temperatureMaxElem->Attribute("value");
        if (maxTempValue)
        {
            weather_data.max_temperature = std::stof(maxTempValue);
        }
        else
        {
            std::cerr << "Max temperature element not found in XML response." << std::endl;
            weather_data.max_temperature = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Max temperature element not found in XML response." << std::endl;
        weather_data.max_temperature = 0.0f;
    }
    
    // Umiditate
     tinyxml2::XMLElement* humidityElem = doc.FirstChildElement("current")->FirstChildElement("humidity");
    if (humidityElem)
    {
        const char* humidityValue = humidityElem->Attribute("value");
        if (humidityValue)
        {
            weather_data.humidity = std::stoi(humidityValue);
        }
        else
        {
            std::cerr << "Humidity value attribute not found in XML response." << std::endl;
            weather_data.humidity = 0; 
        }
    }
    else
    {
        std::cerr << "Humidity element not found in XML response." << std::endl;
        weather_data.humidity = 0; 
    }

    //temperatura minima
    tinyxml2::XMLElement* temperatureMinElem = doc.FirstChildElement("current")->FirstChildElement("temperature")->FirstChildElement("min");
    if (temperatureMinElem)
    {
        const char* minTempValue = temperatureMinElem->Attribute("value");
        if (minTempValue)
        {
            weather_data.min_temperature = std::stof(minTempValue);
        }
        else
        {
            std::cerr << "Min Temperature data not found in XML response." << std::endl;
            weather_data.min_temperature = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Min temperature element not found in XML response." << std::endl;
        weather_data.min_temperature = 0.0f; 
    }


     // Presiune
    tinyxml2::XMLElement* pressureElem = doc.FirstChildElement("current")->FirstChildElement("pressure");
    if (pressureElem)
    {
        const char* pressureValue = pressureElem->Attribute("value");
        if (pressureValue)
        {
            weather_data.pressure = std::stoi(pressureValue);
        }
        else
        {
            std::cerr << "Pressure value attribute not found in XML response." << std::endl;
            weather_data.pressure = 0; 
        }
    }
    else
    {
        std::cerr << "Pressure element not found in XML response." << std::endl;
        weather_data.pressure = 0; 
    }

    // Vizibilitate
    tinyxml2::XMLElement* visibilityElem = doc.FirstChildElement("current")->FirstChildElement("visibility");
    if (visibilityElem)
    {
        const char* visibilityValue = visibilityElem->Attribute("value");
        if (visibilityValue)
        {
            weather_data.visibility = std::stof(visibilityValue) / 1000.0; 
        }
        else
        {
            std::cerr << "Visibility value attribute not found in XML response." << std::endl;
            weather_data.visibility = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Visibility element not found in XML response." << std::endl;
        weather_data.visibility = 0.0f; 
    }

    // Punct de roua
    tinyxml2::XMLElement* dewPointElem = doc.FirstChildElement("current")->FirstChildElement("dewpoint");
    if (dewPointElem)
    {
        const char* dewPointValue = dewPointElem->Attribute("value");
        if (dewPointValue)
        {
            weather_data.dew_point = std::stof(dewPointValue);
        }
        else
        {
            std::cerr << "Dew point value attribute not found in XML response." << std::endl;
            weather_data.dew_point = 0.0f; 
        }
    }
    else
    {
        std::cerr << "Dew point element not found in XML response." << std::endl;
        weather_data.dew_point = 0.0f; 
    }

    // Indice UV
    tinyxml2::XMLElement* uvIndexElem = doc.FirstChildElement("current")->FirstChildElement("uvi");
     if (uvIndexElem)
    {
        const char* uvIndexValue = uvIndexElem->Attribute("value");
        if (uvIndexValue)
        {
            weather_data.uv_index = std::stoi(uvIndexValue);
        }
        else
        {
            std::cerr << "UV Index value attribute not found in XML response." << std::endl;
            weather_data.uv_index = 0; 
        }
    }
    else
    {
        std::cerr << "UV Index element not found in XML response." << std::endl;
        weather_data.uv_index = 0; 
    }
}

std::string Weather::detectResponseType(const std::string& response) {
    
    if (response.find("{") != std::string::npos && response.find("}") != std::string::npos) {
        return "json";
    }
    else if (response.find("<") != std::string::npos && response.find(">") != std::string::npos) {
        return "xml";
    }
    return "unknown";
}


bool Weather::fetchWeatherData()
{
    HttpClient client;
    HttpOptions options;
    std::string response;

    try
    {
        response = client.request("GET", weather_url, options);
        std::string responseType = detectResponseType(response);
        
        if (responseType == "json")
        {
            parseJsonResponse(response);
        }
        else if (responseType == "xml")
        {
            parseXmlResponse(response);
        }
        else
        {
            std::cerr << "Unknown response type." << std::endl;
            return false;
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error fetching weather data: " << e.what() << std::endl;
        return false;
    }
}

void Weather::printWeatherData() const
{
    std::lock_guard<std::mutex> lock(data_mutex);

    std::cout << "City: " << cityId << std::endl;
    std::cout << "Weather Data:" << std::endl;
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Temperature: " << weather_data.temperature << " °C" << std::endl;
    std::cout << "Feels like: " << weather_data.feels_like << "°C" << std::endl;
    std::cout << "Weather conditions: " << weather_data.conditions << std::endl;
    std::cout << "Wind speed: " << weather_data.wind_speed << " m/s" << std::endl;
    std::cout << "Max Temperature: " << weather_data.max_temperature << " °C" << std::endl;
    std::cout << "Min Temperature: " << weather_data.min_temperature << " °C" << std::endl;
    std::cout << "Humidity: " << weather_data.humidity << "%" << std::endl;
    std::cout << "Pressure: " << weather_data.pressure << " hPa" << std::endl;
    std::cout << "Visibility: " << weather_data.visibility << " km" << std::endl;
    std::cout << "UV Index: " << weather_data.uv_index << std::endl;
    std::cout << "Dew point: " << weather_data.dew_point << "°C" << std::endl;
}

void Weather::updateWeatherData() {
    if (fetchWeatherData()) {
        std::cout << "Weather data updated." << std::endl;
    } else {
        std::cerr << "Failed to update weather data." << std::endl;
    }
}