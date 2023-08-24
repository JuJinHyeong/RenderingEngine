#include "CustomUtils.h"
#include <sstream>
#include <iomanip>

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
