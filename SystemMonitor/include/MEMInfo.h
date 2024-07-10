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
#endif // __linux__

using ulonglong = unsigned long long;

class MEMInfo
{
public:
	MEMInfo();
	~MEMInfo();

	void updateInfo();

	ulonglong total() const;
	ulonglong avail() const;
	ulonglong used() const;
	ulonglong availPageFile() const;
	ulonglong totalPageFile() const;
	ulonglong usedPageFile() const;

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

	ulonglong m_total;
	ulonglong m_avail;
	ulonglong m_used;
	ulonglong m_availPageFile;
	ulonglong m_totalPageFile;
	ulonglong m_usedPageFile;
};