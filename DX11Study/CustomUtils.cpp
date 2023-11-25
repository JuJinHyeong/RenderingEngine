#include "CustomUtils.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

std::vector<std::string> TokenizeQuated(const std::string& input) {
    std::istringstream stream;
    stream.str(input);
    std::vector<std::string> tokens;
    std::string token;

    while (stream >> std::quoted(token)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::wstring ToWide(const std::string& narrow) {
    wchar_t wide[512] = L"";
    mbstowcs_s(nullptr, wide, narrow.c_str(), _TRUNCATE);
    return wide;
}

std::string toNarrow(const std::wstring& wide) {
    char narrow[512] = "";
    wcstombs_s(nullptr, narrow, wide.c_str(), _TRUNCATE);
    return narrow;
}

std::vector<std::string> SplitString(const std::string& s, const std::string& delim) {
    std::vector<std::string> strings;
    SplitStringIter(s, delim, std::back_inserter(strings));
    return strings;
}

bool StringContains(std::string_view haystack, std::string_view needle) {
    return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end()) != haystack.end();
}
