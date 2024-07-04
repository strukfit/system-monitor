#pragma once

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <memory>
#include <future>

#include "WMIManager.h" 

#pragma comment(lib, "pdh.lib")
#endif

#ifdef __unix__

#endif

#include <string>
#include <iostream>

using byte = unsigned char;
using ulonglong = unsigned long long;

class Disk
{
public:
	Disk(const char diskLetter);
	~Disk();

	void updateInfo();

	std::wstring diskLetter() const;
	std::wstring modelName() const;

	byte activeTime() const;
	long readSpeed() const;
	long writeSpeed() const;
	double avgResponseTime() const;

	ulonglong totalUsedBytes() const;
	ulonglong totalBytes() const;
	ulonglong totalFreeBytes() const;

private:
#ifdef _WIN32
	void pdhInit();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_readCounter, m_writeCounter, m_responseTimeCounter;
#endif // _WIN32

	void updateActiveTime();
	void updateModelName();

	const std::wstring m_diskLetter;
	std::wstring m_modelName;

	byte m_activeTime;
	long m_readSpeed;
	long m_writeSpeed;
	double m_avgResponseTime;

	ulonglong m_totalUsedBytes;
	ulonglong m_totalBytes;
	ulonglong m_totalFreeBytes;
};