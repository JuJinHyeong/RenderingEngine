#include "ApiService.h"
#include <iostream>

size_t ApiService::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
	size_t newLength = size * nmemb;
	try {
		userp->append((char*)contents, newLength);
	}
	catch (std::bad_alloc& e) {
		return 0;
	}
	return newLength;
}

std::string ApiService::httpGetRequest(const std::string& url, void* CallbackFunction) {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CallbackFunction == nullptr ? WriteCallback : CallbackFunction);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		res = curl_easy_perform(curl);

		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}

		curl_easy_cleanup(curl);
	}
	return readBuffer;
}

std::string ApiService::httpPostRequest(const std::string& url, const json& body, void* CallbackFunction) {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	std::string postData = body.dump();

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}

		curl_slist_free_all(headers);
		curl_easy_cleanup(curl);
	}
	return readBuffer;
}