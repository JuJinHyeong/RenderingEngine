#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

std::vector<std::string> TokenizeQuated(const std::string& input);

std::wstring ToWide(const std::string& narrow);

std::string toNarrow(const std::wstring& wide);

template<class Iter>
void SplitStringIter(const std::string& s, const std::string& delim, Iter out) {
	if (delim.empty()) {
		*out++ = s;
	}
	else {
		size_t a = 0, b = s.find(delim);
		for (; b != std::string::npos;
			a = b + delim.length(), b = s.find(delim, a)) {
			*out++ = std::move(s.substr(a, b - a));
		}
		*out++ = std::move(s.substr(a, s.length() - a));
	}
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim);

bool StringContains(std::string_view haystack, std::string_view needle);

template<typename T>
void SetIfChanged(T& dest, const T& src) {
	if (dest != src) {
		dest = src;
	}
}
