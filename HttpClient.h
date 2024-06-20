#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <curl/curl.h>
#include <map>
#include "HttpOptions.h"
#include "jsoncpp/json/json.h"
#include <fstream>
#include<sstream>
#include <cstdio>
#include "json.h"

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    HttpClient withOptions(const HttpOptions& options); 
    void setHeader(const std::string& header);
    void setCookieFile(const std::string& cookieFilePath);
    std::string request(const std::string& method, const std::string& url, const HttpOptions& options=HttpOptions());
    Json::Value parseJsonResponse(const std::string& jsonResponse);

    void logCookies();
    void setHttpSettings();

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t HeaderCallback(void* buffer, size_t size, size_t nmemb, void* userp);
    
    CURL* curl;
    std::string baseUri;
    struct curl_slist* headers;
    FILE* cookieFile;
};

#endif
