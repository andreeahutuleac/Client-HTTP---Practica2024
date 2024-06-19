#include "HttpClient.h"
#include "HttpOptions.h"
#include <iostream>
#include <stdexcept>

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    headers=nullptr;
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

HttpClient::~HttpClient() {
    if (headers) {
        curl_slist_free_all(headers);
    }
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}


HttpClient HttpClient::withOptions(const HttpOptions& options) {
   HttpClient newClient(*this); // Clonare obiect curent
    newClient.headers = NULL; // Reset headerele pentru noul obiect
    if (!options.getHeaders().empty()) {
        for (const auto& header : options.getHeaders()) {
            std::string headerString = header.first + ": " + header.second;
            newClient.headers = curl_slist_append(newClient.headers, headerString.c_str());
        }
        curl_easy_setopt(newClient.curl, CURLOPT_HTTPHEADER, newClient.headers);
    }
    return newClient;
}

void HttpClient::setHeader(const std::string& header) {
    headers = curl_slist_append(headers, header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

std::string HttpClient::request(const std::string& method, const std::string& url, const HttpOptions& options) {
    if (!curl) {
        throw std::runtime_error("CURL is not initialized");
    }

    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//optiuni din httpOptions
    if (!options.getHeaders().empty()) {
        struct curl_slist *headers = NULL;
        for (const auto& header : options.getHeaders()) {
            std::string headerString = header.first + ": " + header.second;
            headers = curl_slist_append(headers, headerString.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    if (method == "HEAD") {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    }

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(res));
    }

    long httpCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    std::cout << method << " request successful, response code: " << httpCode << std::endl;

    return readBuffer;
}


size_t HttpClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

size_t HttpClient::HeaderCallback(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::map<std::string, std::string>* headersMap = static_cast<std::map<std::string, std::string>*>(userp);

    std::string header(reinterpret_cast<char*>(buffer), size * nmemb);
    size_t separator = header.find(':');
    if (separator != std::string::npos) {
        std::string key = header.substr(0, separator);
        std::string value = header.substr(separator + 2); // +2 pentru a sari peste ": "
        (*headersMap)[key] = value;
    }

    return size * nmemb;
}

Json::Value HttpClient::parseJsonResponse(const std::string& jsonResponse) {
    Json::CharReaderBuilder builder;
    Json::Value root;
    std::string errors;

    std::istringstream jsonStream(jsonResponse);
    if (!Json::parseFromStream(builder, jsonStream, &root, &errors)) {
        throw std::runtime_error("Failed to parse JSON: " + errors);
    }

    return root;
}


// std::string HttpClient::get(const std::string& url, const HttpOptions& options) {
//      if (!curl) {
//         throw std::runtime_error("CURL is not initialized");
//     }

//    std::string readBuffer;
//      if (curl) {
//         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//         // Aplicare opțiuni din HttpOptions
//         if (!options.getHeaders().empty()) {
//             struct curl_slist *headers = NULL;
//             for (const auto& header : options.getHeaders()) {
//                 std::string headerString = header.first + ": " + header.second;
//                 headers = curl_slist_append(headers, headerString.c_str());
//             }
//             curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//         }

//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             throw std::runtime_error(curl_easy_strerror(res));
//         } else {
//             long httpCode;
//             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
//             if (httpCode == 200) {
//                 std::cout << "GET request successful, response code: " << httpCode << std::endl;
//             } else {
//                 std::cout << "GET request failed, response code: " << httpCode << std::endl;
//             }
//         }
//     }
//     return readBuffer;
// }

// std::string HttpClient::post(const std::string& url, const std::string& data, const HttpOptions& options) {
//     std::string readBuffer;
//     if (curl) {
//         // Setare antete din HttpOptions
//         struct curl_slist *headers = NULL;
//         for (const auto& header : options.getHeaders()) {
//             std::string headerString = header.first + ": " + header.second;
//             headers = curl_slist_append(headers, headerString.c_str());
//         }

//         curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//         curl_easy_setopt(curl, CURLOPT_POST, 1L);
//         curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//         CURLcode res = curl_easy_perform(curl);
//         if (res != CURLE_OK) {
//             throw std::runtime_error(curl_easy_strerror(res));
//         } else {
//             long httpCode;
//             curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
//             if (httpCode == 201) {
//                 std::cout << "POST request successful, response code: " << httpCode << std::endl;
//             } else {
//                 std::cout << "POST request failed, response code: " << httpCode << std::endl;
//             }
//         }

//         curl_slist_free_all(headers);
//     }
    
//     return readBuffer;
// }

// std::string HttpClient::put(const std::string& url, const std::string& data, const HttpOptions& options) {
//     if (!curl) {
//         throw std::runtime_error("CURL is not initialized");
//     }

//     std::string readBuffer;
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
//     curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//     if (!options.getHeaders().empty()) {
//         struct curl_slist *headers = NULL;
//         for (const auto& header : options.getHeaders()) {
//             std::string headerString = header.first + ": " + header.second;
//             headers = curl_slist_append(headers, headerString.c_str());
//         }
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     }

//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         throw std::runtime_error(curl_easy_strerror(res));
//     }

//     long httpCode;
//     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

//     if (httpCode == 200 || httpCode == 204) {
//         std::cout << "PUT request successful, response code: " << httpCode << std::endl;
//     } else {
//         throw std::runtime_error("Failed to PUT, HTTP response code: " + std::to_string(httpCode));
//     }


//     return readBuffer;
// }

// std::string HttpClient::del(const std::string& url, const HttpOptions& options) {
//     if (!curl) {
//         throw std::runtime_error("CURL is not initialized");
//     }

//     std::string readBuffer;
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//    if (!options.getHeaders().empty()) {
//         struct curl_slist *headers = NULL;
//         for (const auto& header : options.getHeaders()) {
//             std::string headerString = header.first + ": " + header.second;
//             headers = curl_slist_append(headers, headerString.c_str());
//         }
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     }

//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         throw std::runtime_error(curl_easy_strerror(res));
//     }

//     long httpCode;
//     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

//     if (httpCode == 200 || httpCode == 204) {
//         std::cout << "DELETE request successful, response code: " << httpCode << std::endl;
//     } else {
//         throw std::runtime_error("Failed to DELETE, HTTP response code: " + std::to_string(httpCode));
//     }
    
//     return readBuffer;
// }

// std::string HttpClient::head(const std::string& url, const HttpOptions& options) {
//     if (!curl) {
//         throw std::runtime_error("CURL is not initialized");
//     }

//     std::string readBuffer;
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

//    if (!options.getHeaders().empty()) {
//         struct curl_slist *headers = NULL;
//         for (const auto& header : options.getHeaders()) {
//             std::string headerString = header.first + ": " + header.second;
//             headers = curl_slist_append(headers, headerString.c_str());
//         }
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     }

//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         throw std::runtime_error(curl_easy_strerror(res));
//     }

//     long httpCode;
//     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

//     if (httpCode == 200) {
//         std::cout << "HEAD request successful, response code: " << httpCode << std::endl;
//     } else {
//         throw std::runtime_error("Failed to HEAD, HTTP response code: " + std::to_string(httpCode));
//     }

//     return readBuffer;

// }

// std::string HttpClient::options(const std::string& url, const HttpOptions& options) {
//     if (!curl) {
//         throw std::runtime_error("CURL is not initialized");
//     }

//     std::string readBuffer;
//     curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
//     curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
//     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

//     // Aplicare opțiuni din HttpOptions
//     if (!options.getHeaders().empty()) {
//         struct curl_slist *headers = NULL;
//         for (const auto& header : options.getHeaders()) {
//             std::string headerString = header.first + ": " + header.second;
//             headers = curl_slist_append(headers, headerString.c_str());
//         }
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//     }

//     CURLcode res = curl_easy_perform(curl);
//     if (res != CURLE_OK) {
//         throw std::runtime_error(curl_easy_strerror(res));
//     }

//     long httpCode;
//     curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
//     if (httpCode == 200 ||  httpCode==204) {
//         std::cout << "OPTIONS request successful, response code: " << httpCode << std::endl;
//     } else {
//         throw std::runtime_error("Failed to OPTIONS, HTTP response code: " + std::to_string(httpCode));
//     }

//     return readBuffer;
// }