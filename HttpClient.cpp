#include "HttpClient.h"
#include <iostream>
#include <stdexcept>

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

HttpClient::~HttpClient() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

size_t HttpClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string HttpClient::get(const std::string& url) {
    std::string readBuffer;
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode == 200) {
                std::cout << "GET request successful, response code: " << httpCode << std::endl;
            } else {
                std::cout << "GET request failed, response code: " << httpCode << std::endl;
            }
        }
    }
    return readBuffer;
}

std::string HttpClient::post(const std::string& url, const std::string& data) {
    std::string readBuffer;
    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            throw std::runtime_error(curl_easy_strerror(res));
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode == 201) {
                std::cout << "POST request successful, response code: " << httpCode << std::endl;
            } else {
                std::cout << "POST request failed, response code: " << httpCode << std::endl;
            }
        }

        curl_slist_free_all(headers); // Eliberare antet
    }
    return readBuffer;
}
