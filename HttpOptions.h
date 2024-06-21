#ifndef HTTPOPTIONS_H
#define HTTPOPTIONS_H

#include <string>
#include <curl/curl.h>
#include <map>

class HttpOptions {
public:
    HttpOptions& setBaseUri(const std::string& baseUri);
    HttpOptions& setHeader(const std::string& headerName, const std::string& headerValue);
   
    const std::string& getBaseUri() const;
    const std::map<std::string, std::string>& getHeaders() const;
   
    bool hasHeader(const std::string& headerName) const;
    const std::string& getHeader(const std::string& headerName) const;
    void removeHeader(const std::string& headerName);

    void setUsername(const std::string& username);
    std::string getUsername()const;

    void setPassword(const std::string& password);
    std::string getPassword()const;

    bool hasUsernameAndPassword()const;

private:
    std::string baseUri;
    std::map<std::string, std::string> headers;
    std::string username;
    std::string password;
};

#endif