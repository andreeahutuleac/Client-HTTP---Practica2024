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
    :apiKey(apiKey), cityId(cityId),cityListFilePath("city_list.txt"), countryListFilePath("country_list.txt"), stop_thread(false)
{

    this->weather_url = "http://api.openweathermap.org/data/2.5/weather?id=" + cityId + "&appid=" + apiKey + "&units=metric";//&mode=xml";
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


void Weather::parseJsonResponse(const std::string& jsonResponse)
{
    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;
    std::istringstream s(jsonResponse);
    
    if (!Json::parseFromStream(builder, s, &root, &errors)) 
    {
        throw std::runtime_error("Failed to parse JSON: " + errors);
    }

    std::lock_guard<std::mutex> lock(data_mutex);

    // weather_data.timezone = root["timezone"].asString();
    // weather_data.timezone_offset = root["timezone_offset"].asInt64();

    // if (root.isMember("main"))
    // {
    //     if (root["main"].isMember("temp"))
    //         weather_data.temperature = root["main"]["temp"].asFloat();

    //     if (root["main"].isMember("feels_like"))
    //         weather_data.feels_like = root["main"]["feels_like"].asFloat();

    //     if (root["main"].isMember("temp_max"))
    //         weather_data.max_temperature = root["main"]["temp_max"].asFloat();

    //     if (root["main"].isMember("temp_min"))
    //         weather_data.min_temperature = root["main"]["temp_min"].asFloat();

    //     if (root["main"].isMember("humidity"))
    //         weather_data.humidity = root["main"]["humidity"].asInt();

    //     if (root["main"].isMember("pressure"))
    //         weather_data.pressure = root["main"]["pressure"].asInt();
    // }

    // if (root.isMember("weather") && root["weather"].isArray() && root["weather"].size() > 0)
    // {
    //     weather_data.conditions = root["weather"][0]["description"].asString();
    // }

    // if (root.isMember("wind") && root["wind"].isMember("speed"))
    // {
    //     weather_data.wind_speed = root["wind"]["speed"].asFloat();
    // }

    // if (root.isMember("visibility"))
    // {
    //     weather_data.visibility = root["visibility"].asFloat() / 1000.0f;
    // }

    // if (root.isMember("dew_point"))
    // {
    //     weather_data.dew_point = root["dew_point"].asFloat();
    // }

    // if (root.isMember("uvi"))
    // {
    //     weather_data.uv_index = root["uvi"].asInt();
    // }

  

    weather_data.temperature = root["main"]["temp"].asFloat();
    weather_data.feels_like = root["main"]["feels_like"].asFloat();
    weather_data.max_temperature = root["main"]["temp_max"].asFloat();
    weather_data.min_temperature = root["main"]["temp_min"].asFloat();
    weather_data.humidity = root["main"]["humidity"].asInt();
    weather_data.pressure = root["main"]["pressure"].asInt();
    weather_data.wind_speed = root["wind"]["speed"].asFloat();
    weather_data.wind_deg = root["wind"]["deg"].asInt();
    weather_data.visibility = root["visibility"].asFloat() / 1000.0f;
    weather_data.uv_index = root["current"]["uvi"].asFloat();
    weather_data.dew_point = root["current"]["dew_point"].asFloat();
    weather_data.conditions = root["weather"][0]["description"].asString();
    weather_data.timezone = root["timezone"].asString();
    weather_data.sunrise = root["current"]["sunrise"].asInt64();
    weather_data.sunset = root["current"]["sunset"].asInt64();
    weather_data.dt = root["current"]["dt"].asInt64();
    weather_data.timezone_offset = root["timezone_offset"].asInt64();
    weather_data.clouds = root["clouds"]["all"].asInt();

    readCitiesFromJson(jsonFilePath);
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

    tinyxml2::XMLElement* currentElem = doc.FirstChildElement("current");

    if (currentElem)
    {
        tinyxml2::XMLElement* temperatureElem = currentElem->FirstChildElement("temperature");
        if (temperatureElem)
        {
            weather_data.temperature = temperatureElem->FloatAttribute("value");
            weather_data.max_temperature = temperatureElem->FloatAttribute("max");
            weather_data.min_temperature = temperatureElem->FloatAttribute("min");
        }

        tinyxml2::XMLElement* feelsLikeElem = currentElem->FirstChildElement("feels_like");
        if (feelsLikeElem)
        {
            weather_data.feels_like = feelsLikeElem->FloatAttribute("value");
        }

        tinyxml2::XMLElement* humidityElem = currentElem->FirstChildElement("humidity");
        if (humidityElem)
        {
            weather_data.humidity = humidityElem->IntAttribute("value");
        }

        tinyxml2::XMLElement* pressureElem = currentElem->FirstChildElement("pressure");
        if (pressureElem)
        {
            weather_data.pressure = pressureElem->IntAttribute("value");
        }

        tinyxml2::XMLElement* windElem = currentElem->FirstChildElement("wind");
        if (windElem)
        {
            tinyxml2::XMLElement* windSpeedElem = windElem->FirstChildElement("speed");
            if (windSpeedElem)
            {
                weather_data.wind_speed = windSpeedElem->FloatAttribute("value");
            }

            tinyxml2::XMLElement* windDegElem = windElem->FirstChildElement("direction");
            if (windDegElem)
            {
                weather_data.wind_deg = windDegElem->IntAttribute("value");
            }
        }

        tinyxml2::XMLElement* cloudsElem = currentElem->FirstChildElement("clouds");
        if (cloudsElem)
        {
            weather_data.clouds = cloudsElem->IntAttribute("value");
        }

        tinyxml2::XMLElement* visibilityElem = currentElem->FirstChildElement("visibility");
        if (visibilityElem)
        {
            weather_data.visibility = visibilityElem->FloatAttribute("value") / 1000.0f;
        }

        tinyxml2::XMLElement* uvElem = currentElem->FirstChildElement("uvi");
        if (uvElem)
        {
            weather_data.uv_index = uvElem->FloatAttribute("value");
        }

        tinyxml2::XMLElement* dewPointElem = currentElem->FirstChildElement("dew_point");
        if (dewPointElem)
        {
            weather_data.dew_point = dewPointElem->FloatAttribute("value");
        }

        tinyxml2::XMLElement* weatherElem = currentElem->FirstChildElement("weather");
        if (weatherElem)
        {
            weather_data.conditions = weatherElem->Attribute("value");
        }

          tinyxml2::XMLElement* timezoneElem = currentElem->FirstChildElement("timezone");
        if (timezoneElem)
        {
            weather_data.timezone = timezoneElem->GetText();
        }

        tinyxml2::XMLElement* dtElem = currentElem->FirstChildElement("dt");
        if (dtElem)
        {
            weather_data.dt = dtElem->Int64Text();
        }

        tinyxml2::XMLElement* sunriseElem = currentElem->FirstChildElement("sunrise");
        if (sunriseElem)
        {
            weather_data.sunrise = sunriseElem->Int64Text();
        }

        tinyxml2::XMLElement* sunsetElem = currentElem->FirstChildElement("sunset");
        if (sunsetElem)
        {
            weather_data.sunset = sunsetElem->Int64Text();
        }

        tinyxml2::XMLElement* timezoneOffsetElem = currentElem->FirstChildElement("timezone_offset");
        if (timezoneOffsetElem)
        {
            weather_data.timezone_offset = timezoneOffsetElem->Int64Text();
        }
    }

    readCitiesFromJson(jsonFilePath);
}

void Weather::readCitiesFromJson(const std::string& jsonFilePath)
{
    std::ifstream ifs(jsonFilePath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open JSON file: " << jsonFilePath << std::endl;
        return;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    if (!Json::parseFromStream(builder, ifs, &root, &errs)) {
        std::cerr << "Failed to parse JSON file: " << errs << std::endl;
        ifs.close();
        return;
    }

    ifs.close();

    for (const auto& city : root) {
        int id = city["id"].asInt();
        if (std::to_string(id) == cityId) {
            weather_data.cityName = city["name"].asString();
            weather_data.country = city["country"].asString();
            break;
        }
    }
}

void Weather::readCityFromFile(const std::string &cityId, const std::string &cityListFilePath)
{
    std::ifstream file(cityListFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open city list file: " << cityListFilePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id, name, country;
        if (std::getline(ss, id, '\t') && std::getline(ss, name, '\t') && std::getline(ss, country, '\t')) {
            if (id == cityId) {
                weather_data.cityName = name;
                weather_data.country = country;
                file.close();
                return;
            }
        }
    }

    file.close();
}

void Weather::readPrintCountriesFromFile(const std::string &countryListFilePath)
{
    std::ifstream file(countryListFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open city list file: " << countryListFilePath << std::endl;
        return;
    }

      std::map<std::string, std::string> countries;

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string code, name;
            if (ss >> code >> std::ws && std::getline(ss, name)) {
                countries[code] = name;
            }
        }

    file.close();

    for (const auto& pair : countries) {
            std::cout << pair.first << " - " << pair.second << std::endl;
        }
}


void Weather::setCityId(const std::string &newCityId)
{
    this->cityId=newCityId;
    weather_url="http://api.openweathermap.org/data/2.5/weather?id=" + cityId + "&appid=" + apiKey + "&units=metric";

    readCityFromFile(newCityId, this->cityListFilePath);
}

void Weather::setCityName(const std::string &newCityName)
{
    this->city_name=newCityName;
}

void Weather::setCountryName(const std::string &newCountryName)
{
    this->country_name=newCountryName;
}

std::string Weather::findCityIdByName(const std::string &cityName, const std::string& cityListFilePath)
{
       std::ifstream file(cityListFilePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open city list file: " << cityListFilePath << std::endl;
        return "";
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    if (!Json::parseFromStream(builder, file, &root, &errs)) {
        std::cerr << "Failed to parse JSON file: " << errs << std::endl;
        file.close();
        return "";
    }

    file.close();

    for (const auto& city : root) {
        std::string cityNameFromFile = city["name"].asString();
        if (cityNameFromFile == cityName) {
            int cityId = city["id"].asInt();
            return std::to_string(cityId);
        }
    }

    std::cerr << "City with name '" << cityName << "' not found in the city list." << std::endl;
    return "";

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

    std::cout<<"Fetching data from URL: " << weather_url << std::endl;
    
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

    std::cout<<std::endl;
    std::cout << "City: " << this->city_name <<", "<<this->country_name<< std::endl;
    std::cout << "Weather Data:" << std::endl<<std::endl;
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "Temperature: " << weather_data.temperature << "°C" << std::endl;
    std::cout << "Feels Like: " << weather_data.feels_like << "°C" << std::endl;
    std::cout << "Max Temperature: " << weather_data.max_temperature << "°C" << std::endl;
    std::cout << "Min Temperature: " << weather_data.min_temperature << "°C" << std::endl;
    std::cout << "Humidity: " << weather_data.humidity << "%" << std::endl;
    std::cout << "Pressure: " << weather_data.pressure << " hPa" << std::endl;
    std::cout << "Dew Point: " << weather_data.dew_point << "°C" << std::endl;
    std::cout << "UV Index: " << weather_data.uv_index << std::endl;
    std::cout << "Visibility: " << weather_data.visibility << " km" << std::endl;
    std::cout << "Wind Speed: " << weather_data.wind_speed << " m/s" << std::endl;
    std::cout << "Wind Direction: " << weather_data.wind_deg << "°" << std::endl;
    std::cout << "Clouds: " << weather_data.clouds << "%" << std::endl;
    std::cout << "Conditions: " << weather_data.conditions << std::endl;
    std::cout << "Timezone: " << weather_data.timezone << std::endl;
    std::cout << "Sunrise: " << weather_data.sunrise << std::endl;
    std::cout << "Sunset: " << weather_data.sunset << std::endl;
    std::cout << "Data Time: " << weather_data.dt << std::endl;
    std::cout << "Timezone Offset: " << weather_data.timezone_offset << " seconds" << std::endl;

}

void Weather::updateWeatherData()
{
    if (fetchWeatherData())
    {
        std::cout << "Weather data updated." << std::endl;
        printWeatherData();
    }
    else
    {
        std::cerr << "Failed to update weather data." << std::endl;
    }
}

void Weather::run()
{
    while(!stop_thread)
    {
        updateWeatherData();
        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}
