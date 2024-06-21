#include "HttpOptions.h"

HttpOptions& HttpOptions::setBaseUri(const std::string& baseuri)
{
    this->baseUri=baseUri;
    return *this;
}

HttpOptions& HttpOptions::setHeader(const std::string& headerName, const std::string& headerValue) {
    this->headers[headerName] = headerValue;
    return *this;
}

const std::string& HttpOptions::getBaseUri() const {
    return baseUri;
}

const std::map<std::string, std::string>& HttpOptions::getHeaders() const {
    return headers;
}

bool HttpOptions::hasHeader(const std::string& headerName) const {
    return headers.find(headerName) != headers.end();
}

const std::string& HttpOptions::getHeader(const std::string& headerName) const {
    static const std::string emptyString;
    auto it = headers.find(headerName);
    if (it != headers.end()) {
        return it->second;
    }
    return emptyString;
}

void HttpOptions::removeHeader(const std::string& headerName) {
    headers.erase(headerName);
}

void HttpOptions::setUsername(const std::string& username) {
    this->username = username;
}

std::string HttpOptions::getUsername() const {
    return username;
}

void HttpOptions::setPassword(const std::string& password) {
    this->password = password;
}

std::string HttpOptions::getPassword() const {
    return password;
}

bool HttpOptions::hasUsernameAndPassword() const {
    return !username.empty() && !password.empty();
}