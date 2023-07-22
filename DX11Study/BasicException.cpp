#include "BasicException.h"
#include <sstream>

BasicException::BasicException(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* BasicException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << "\n" << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* BasicException::GetType() const noexcept
{
	return nullptr;
}

int BasicException::GetLine() const noexcept
{
	return 0;
}

const std::string& BasicException::GetFile() const noexcept
{
	return file;
}

std::string BasicException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << "\n"
		<< "[Line] " << line;
	return oss.str();
}
