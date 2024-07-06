#pragma once

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <memory>
#include "WMIManager.h"
#pragma comment(lib, "wbemuuid.lib")
#endif

#include <QDebug>

#ifdef __linux__
#include <fstream>
#include <sstream>
#include <unistd.h>

using ulonglong = unsigned long long;
#endif // __linux__


class MEMInfo
{
public:
	MEMInfo();
	~MEMInfo();

	void updateInfo();

	float totalGB() const;
	float availGB() const;
	float usedGB() const;
	float availPageFileGB() const;
	float totalPageFileGB() const;
	float usedPageFileGB() const;

#ifdef _WIN32
	int speedMHz() const;
#endif // _WIN32


private:
#ifdef __linux__
	ulonglong extractValue(const std::string& line);
#endif // __linux__

#ifdef _WIN32
	void updateSpeedInfo();
	
	int m_speedMHz;
#endif // _WIN32

	float m_totalGB;
	float m_availGB;
	float m_usedGB;
	float m_availPageFileGB;
	float m_totalPageFileGB;
	float m_usedPageFileGB;
};