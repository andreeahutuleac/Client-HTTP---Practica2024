#include <iostream>
#include <curl/curl.h>
#include <string>
#include "HttpClient.h"
#include <jsoncpp/json/json.h>
#include<iomanip>

#include "Weather.h"


//scriere raspuns in buffer
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {

    //verificare conexiune cu un server din internet

    //initializare curl
    // curl_global_init(CURL_GLOBAL_DEFAULT);
    // CURL* curl = curl_easy_init();
  
    // if(curl) {
    //     //configurare url + callback pt scrierea raspunsului
    //     curl_easy_setopt(curl, CURLOPT_URL, "https://openweathermap.org/");
    //     std::string readBuffer;
    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    
        
    //     //cerere HTTP
    //     CURLcode res = curl_easy_perform(curl);
    //     if(res != CURLE_OK) {
    //         std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    //     } else {
    //         long httpCode;
    //         curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    //         if (httpCode == 200) {
    //             std::cout << "Conexiune reusita: " << readBuffer << std::endl;
    //         } else {
    //             std::cout << "Conexiune nereusita, cod de răspuns: " << httpCode << std::endl;
    //         }
    //     }
    //     curl_easy_cleanup(curl);
    // }

    // curl_global_cleanup();

    // // //cereri HTTP
    // try
    // {
    //     //initializare client
    //     HttpClient client;
    //     //activare setari https
    //     client.setHttpSettings();
    //     //setare fisier cookie
    //     client.setCookieFile("cookie.txt");
    //     //10 sec timeout
    //     client.setTimeout(10);
    //     //3 retries
    //     client.setRetry(3);
    //     //activare debugging
    //     client.enableDebugging(true);
        

    //     //activare cache si setare dire cache


    //     //activare proxy si setare adresa proxy
       

    //     //setare a antetelor personalizate
    //     std::map<std::string, std::string> headers;
    //     HttpOptions options;
    //     options.setBaseUri("https://openweathermap.org/");
    //     options.setHeader("Content-Type", "application/json");
    //     options.setHeader("Authorization", "Bearer token123");

    //     options.setUsername("username");
    //     options.setPassword("parola");

    // //   // GET
    //     std::string getUrl = "https://openweathermap.org/";
    //     std::string getResponse = client.request("GET", getUrl,options);
    //     std::cout << "GET Response: " << getResponse << std::endl;

    // //     // POST
    //     std::string postUrl = "https://openweathermap.org/";
    //     std::string postData = R"({"title": "foo", "body": "bar", "userId": 1})";
    //     options.setHeader("Content-Lenght",std::to_string(postData.length()));
    //     std::string postResponse = client.request("POST", postUrl, options);
    //     std::cout << "POST Response: " << postResponse << std::endl;

    //     // PUT
    //     std::string putUrl = "https://openweathermap.org/";
    //     std::string putData = R"({"id": 1, "title": "foo", "body": "bar", "userId": 1})";
    //     options.setHeader("Content-Lenght",std::to_string(postData.length()));
    //     std::string putResponse = client.request("PUT", putUrl, options);
    //     std::cout << "PUT Response: " << putResponse << std::endl;

    //     // DELETE
    //     std::string delUrl = "https://openweathermap.org/";
    //     std::string delResponse = client.request("DELETE", delUrl,options);
    //     std::cout << "DELETE Response: " << delResponse << std::endl;

    //     // HEAD
    //     std::string headUrl = "https://openweathermap.org/";
    //     std::string headResponse = client.request("HEAD", headUrl,options);
    //     std::cout << "HEAD Response: " << headResponse << std::endl;

    //     // OPTIONS
    //     std::string optionsUrl = "https://openweathermap.org/";
    //     std::string optionsResponse = client.request("OPTIONS", optionsUrl,options);
    //     std::cout << "OPTIONS Response: " << optionsResponse << std::endl;

    //     //JSON response
    //     Json::Value jsonResponse = client.parseJsonResponse(getResponse);
    //     std::string title = jsonResponse["title"].asString();
    //     std::cout << "Parsed title: " << title << std::endl;

    //     //afisare cookie-uri
    //     client.logCookies();

    // } catch (const std::exception& e) {
    //     std::cerr << "Error: " << e.what() << std::endl;
    // }

    // curl_global_cleanup();
    

    const std::string apiKey="2e141feae1407d873cf806d90627a0e4";
    //const std::string cityId="683506";
    std::string cityListFilePath="/home/andreea/Desktop/git/Client-HTTP---Practica2024/city_list.txt";
    std::string countryListFilePath="/home/andreea/Desktop/git/Client-HTTP---Practica2024/country_list.txt";

   // Weather weather(apiKey,cityId);

    Weather weather(apiKey, "");

    std::string cityName,countryName;
    std::cout << "Introduceti numele orasului: ";
    std::getline(std::cin, cityName);

    weather.readPrintCountriesFromFile(countryListFilePath);

    std::cout << "Introduceti acronimul tarii: ";
    std::getline(std::cin, countryName);
  

    std::string cityIdStr = weather.findCityIdByName(cityName, cityListFilePath);
    
    if (cityIdStr.empty()) {
        std::cerr << "Could not find city ID for '" << cityName << "'. Exiting." << std::endl;
        return 1;
    }

      
    weather.setCityName(cityName);
    weather.setCountryName(countryName);
    weather.setCityId(cityIdStr);
    weather.start();


    std::this_thread::sleep_for(std::chrono::seconds(5));

    weather.stop();

    return 0;
}
