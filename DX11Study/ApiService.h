#pragma once
#include <curl/curl.h>
#include <string>
#include "json.hpp"

class ApiService {
	using json = nlohmann::json;
public:
	static std::string httpGetRequest(const std::string& url, void* CallbackFunction = nullptr);
	static std::string httpPostRequest(const std::string& url, const json& body, void* CallbackFunction = nullptr);
private:
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
};