#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <curl/curl.h>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    std::string get(const std::string& url);
    std::string post(const std::string& url, const std::string& data);

private:
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    CURL* curl;
};

#endif
