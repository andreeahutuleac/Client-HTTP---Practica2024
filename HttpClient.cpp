#include "HttpClient.h"
#include "HttpOptions.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    headers=nullptr;
    cookieFile=nullptr;
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

HttpClient::~HttpClient() {
    if (headers) {
        curl_slist_free_all(headers);
    }
    if (cookieFile) {
        fclose(cookieFile);
    }
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
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
        std::string value = header.substr(separator + 1); // +1 pentru a sari peste ": "
        (*headersMap)[key] = value;
    }

    return size * nmemb;
}


HttpClient HttpClient::withOptions(const HttpOptions& options) {
    HttpClient newClient(*this); // Clonare obiect curent
    newClient.headers = nullptr; // Reset headere pentru noul obiect
    newClient.cookieFile=nullptr;
    
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

void HttpClient::setCookieFile(const std::string& cookieFilePath) 
{
    cookieFile=fopen(cookieFilePath.c_str(),"wb");
     if (!cookieFile) {
        throw std::runtime_error("Failed to open cookie file");
    }
    //fisier de unde se vor citi cookie urile pt cererile ulterioare
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookieFilePath.c_str());
    //fisier in care se vor salva cookie-urile primite de la server
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookieFilePath.c_str());
}

void HttpClient::setTimeout(long timeout)
{
    curl_easy_setopt(curl,CURLOPT_TIMEOUT,timeout);
}

void HttpClient::setRetry(int retries)
{
    this->retries=retries;
}

void HttpClient::enableDebugging(bool enabled)
{
    this->debug=enabled;
    curl_easy_setopt(curl, CURLOPT_VERBOSE,enabled ? 1L : 0L);
}

void HttpClient::logDebugInfo(const std::string& message)
{
    if(debug)
    {
        std::cerr<<"DEBUG: "<<message<<std::endl;
    }
}

void HttpClient::performRequestWithRetries(const std::string& method, const std::string& url, std::string& readBuffer)
{
    CURLcode res;
    int attempt=0;
    while(attempt<retries)
    {
        readBuffer.clear();
        curl_easy_setopt(curl,CURLOPT_CUSTOMREQUEST,method.c_str());
        curl_easy_setopt(curl,CURLOPT_URL,url.c_str());
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,WriteCallback);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,&readBuffer);
        res=curl_easy_perform(curl);

        if(res==CURLE_OK)
        {
            break;
        }

        logDebugInfo("Retry attempt " + std::to_string(attempt+1) + std::string(curl_easy_strerror(res)));
        attempt++;

    }

    if(res != CURLE_OK)
    {
        throw std::runtime_error("Failed to perform request: "+ std::string(curl_easy_strerror(res)));
    }

}

void HttpClient::setProxy(const std::string& proxyAddress, int proxyPort) 
{
    curl_easy_setopt(curl, CURLOPT_PROXY, proxyAddress.c_str());
    curl_easy_setopt(curl, CURLOPT_PROXYPORT, proxyPort);
}

void HttpClient::setCachePaths(const std::string& cacheDirectory) 
{
    std::string cookieFilePath = cacheDirectory + "/cookies";
    std::string sslCertPath = cacheDirectory + "/client.pem";

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookieFilePath.c_str());
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookieFilePath.c_str());
    curl_easy_setopt(curl, CURLOPT_SSLCERT, sslCertPath.c_str());
    curl_easy_setopt(curl, CURLOPT_SSLKEY, sslCertPath.c_str());
}


std::string HttpClient::request(const std::string& method, const std::string& url, const HttpOptions& options) {
    if (!curl) {
        throw std::runtime_error("CURL is not initialized");
    }

    //setare proxy si cache path - DE IMPLEMENTAT
    
    std::string readBuffer;

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    //setare fisier cookie daca este configurat
    if (cookieFile) {
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookieFile);
    }

    //gestionarea redirectarilor
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    

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

    //gestionare autentificare basic
    if(options.hasUsernameAndPassword())
    {
        std::string authString=options.getUsername() + ":" + options.getPassword();
        curl_easy_setopt(curl, CURLOPT_HTTPAUTH,CURLAUTH_BASIC);
        curl_easy_setopt(curl,CURLOPT_USERPWD,authString.c_str());
    }

    //prelucrare retry
    performRequestWithRetries(method,url,readBuffer);

    //cererile HTTP
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw std::runtime_error("Failed to perform request: " + std::string(curl_easy_strerror(res)));
    }

    long httpCode;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    //gestionarea erorilor si a codurilor de status
    if (httpCode >= 400) {
        throw std::runtime_error("HTTP request failed with status code: " + std::to_string(httpCode));
    }

    std::cout << method << " request successful, response code: " << httpCode << std::endl;

    return readBuffer;
}

Json::Value HttpClient::parseJsonResponse(const std::string& jsonResponse) {
    Json::CharReaderBuilder builder;
    Json::Value jsonData;
    std::string errors;

    std::istringstream jsonStream(jsonResponse);
    if (!Json::parseFromStream(builder, jsonStream, &jsonData, &errors)) {
        throw std::runtime_error("Failed to parse JSON: " + errors);
    }

    return jsonData;
}

//afisare de cookie uri
void HttpClient::logCookies() {
    if (!cookieFile) {
        std::cerr << "Cookie file is not initialized" << std::endl;
        return;
    }

    FILE* file = cookieFile;
    if (file) {
        char buffer[4096]; 
        std::cout << "Current cookies:" << std::endl;
        
        while (fgets(buffer, sizeof(buffer), file)) {
            std::cout << buffer; 
        }

        std::cout << std::endl;
    } else {
        std::cerr << "Failed to open cookie file for reading" << std::endl;
    }
}

//suport Http si gestioanre certificari SSL
void HttpClient::setHttpSettings()
{
    //verificare certificat server
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,1L);

    //verificare nume de domeniu
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,2L);

    //setare cale catre fisier CA
    curl_easy_setopt(curl,CURLOPT_CAINFO,"/etc/ssl/certs/ca-certificates.crt");
   
    //versiune ssl/tls
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
}



void HttpClient::setCacheSettings(bool enableCache, const std::string& cacheDirectory)
{
    //daca este activata
    if(enableCache)
    {
        setCachePaths(cacheDirectory);
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE,8192L); //dim buffer cache
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT,0L); //nu se conecteaza datele noi la cache
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0L); //se permite reutilizarea conexiunilor existente
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);//se activeaza mesajele verbose pentru debugging cache
        curl_easy_setopt(curl, CURLOPT_NETRC, 1L);//se permite utilizarea fisierului .netrc pentru autentificare

        //setez directorul cache
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, (cacheDirectory + "/cookies").c_str());
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, (cacheDirectory + "/cookies").c_str());
        curl_easy_setopt(curl, CURLOPT_SSLCERT, (cacheDirectory + "/client.pem").c_str());
        curl_easy_setopt(curl, CURLOPT_SSLKEY, (cacheDirectory + "/client.pem").c_str());

        //activez cache
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

    } else{
        //daca nu este activata, se dezactiveaza setarile de cache
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 16384L);
        curl_easy_setopt(curl, CURLOPT_FRESH_CONNECT, 1L);//conectare la date noi, fara cache
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1L);//nu se reutilizeaza conexiunile existente
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);//dezactivare mesaje verbose
        curl_easy_setopt(curl, CURLOPT_NETRC, 0L);//dezactivare .netrc

        //dezactivare cache
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    }
}

void HttpClient::setProxySettings(bool enableProxy, const std::string& proxyAddress)
{
    if(enableProxy)
    {
        //activare proxy
        curl_easy_setopt(curl,CURLOPT_PROXY,proxyAddress.c_str());
        curl_easy_setopt(curl,CURLOPT_PROXYPORT,8080L);//port proxy
    } else{
        //dezactivare proxy
        curl_easy_setopt(curl,CURLOPT_PROXY,"");
        curl_easy_setopt(curl,CURLOPT_PROXYPORT,0L);
    }
}