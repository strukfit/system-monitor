#pragma once

#include <string>
#include <sstream>
#include <iomanip>

using ulonglong = unsigned long long;

class Converter
{
public:
	static std::string convertBytes(ulonglong bytes);
};