//vibecode.cpp
// Created by gavga on 5/18/2026.

#include <crow.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <utility>

using json = nlohmann::json;


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total = size * nmemb;
    output->append((char*)contents, total);
    return total;
}


std::string fetchURL(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "User-Agent: FCC-Student-App");

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        curl_easy_perform(curl);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}


std::pair<int, std::string> parseWeather(const std::string& raw) {
    if (raw.empty()) return {0, "No data"};

    auto j = json::parse(raw);

    int temp = j["properties"]["periods"][0]["temperature"];
    std::string condition = j["properties"]["periods"][0]["shortForecast"];

    return {temp, condition};
}


int main() {

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() {

        // Weather API endpoints
        std::string fresnoURL =
            "https://api.weather.gov/gridpoints/HNX/53,95/forecast";

        std::string nyURL =
            "https://api.weather.gov/gridpoints/OKX/33,37/forecast";

        // Fetch data
        std::string fresnoRaw = fetchURL(fresnoURL);
        std::string nyRaw = fetchURL(nyURL);

        // Parse JSON
        auto [fTemp, fCond] = parseWeather(fresnoRaw);
        auto [nTemp, nCond] = parseWeather(nyRaw);

        // Build HTML safely (NO raw strings, NO special characters)
        std::string html =
            "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "<title>Weather Dashboard</title>"
            "<style>"
            "body{font-family:Arial;margin:40px;background:white;color:black;}"
            "h1{font-size:24px;margin-bottom:30px;}"
            ".city{margin-bottom:20px;border-bottom:1px solid #ddd;padding-bottom:10px;}"
            ".name{font-weight:bold;font-size:18px;}"
            ".temp{font-size:20px;margin-top:5px;}"
            ".cond{font-size:14px;color:#444;}"
            "</style>"
            "</head>"
            "<body>"

            "<h1>Weather Dashboard</h1>"

            "<div class='city'>"
            "<div class='name'>Fresno, CA</div>"
            "<div class='temp'>" + std::to_string(fTemp) + " F</div>"
            "<div class='cond'>" + fCond + "</div>"
            "</div>"

            "<div class='city'>"
            "<div class='name'>New York, NY</div>"
            "<div class='temp'>" + std::to_string(nTemp) + " F</div>"
            "<div class='cond'>" + nCond + "</div>"
            "</div>"

            "</body>"
            "</html>";

        return html;
    });

    app.port(8080).multithreaded().run();
}