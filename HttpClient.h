#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <curl/curl.h>
#include <map>
#include "HttpOptions.h"
#include "json/json.h"


class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // std::string get(const std::string& url,const HttpOptions& options = HttpOptions());
    // std::string post(const std::string& url, const std::string& data,const HttpOptions& options = HttpOptions());
    // std:: string put(const std::string& url, const std::string& data,const HttpOptions& options = HttpOptions());
    // std::string del(const std::string& url,const HttpOptions& options = HttpOptions());
    // std::string head(const std::string& url,const HttpOptions& options = HttpOptions());
    // std::string options(const std::string& url, const HttpOptions& options=HttpOptions());


    std::string request(const std::string& method, const std::string& url, const HttpOptions& options=HttpOptions());
    HttpClient withOptions(const HttpOptions& options); 
    void setHeader(const std::string& header);
    Json::Value parseJsonResponse(const std::string& jsonResponse);


private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t HeaderCallback(void* buffer, size_t size, size_t nmemb, void* userp);
    
    CURL* curl;
    std::string baseUri;
    struct curl_slist* headers;
};

#endif
