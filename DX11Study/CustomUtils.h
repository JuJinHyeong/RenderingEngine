#pragma once
#include <vector>
#include <string>

std::vector<std::string> TokenizeQuated(const std::string& input);

std::wstring ToWide(const std::string& narrow);

std::string toNarrow(const std::wstring& wide);