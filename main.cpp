#include <iostream>
#include <curl/curl.h>
#include <string>
#include "HttpClient.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {

    //verificare conexiune cu un server din internet
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.google.com");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode == 200) {
                std::cout << "Conexiune reusita: " << readBuffer << std::endl;
            } else {
                std::cout << "Conexiune nereusita, cod de răspuns: " << httpCode << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    //functiile de GET si POST
    try
    {
        HttpClient client;

        std::string getUrl="https://jsonplaceholder.typicode.com/posts/1";
        std::string getResponse = client.get(getUrl);
        std::cout << "GET Response: " << getResponse << std::endl;
    
        std::string postUrl = "https://jsonplaceholder.typicode.com/posts";
        std::string postData = R"({"title": "foo", "body": "bar", "userId": 1})";
        std::string postResponse = client.post(postUrl, postData);
        std::cout << "POST Response: " << postResponse << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    curl_global_cleanup();
    


    return 0;
}
