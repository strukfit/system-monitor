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

#ifdef __linux__
#include <fstream>
#include <sstream>
#include <QDebug>
#endif // __linux__


#include <string>
#include <iostream>

using byte = unsigned char;
using ulonglong = unsigned long long;

class DiskInfo
{
public:
#ifdef WIN32
	DiskInfo(const char diskLetter);

	std::wstring diskLetter() const;
#endif // WIN32

#ifdef __linux__
	DiskInfo(std::string device);

	static void setUpdateInterval(int seconds);

	std::string device() const;
#endif // __linux__

	~DiskInfo();

	void updateInfo();

	std::wstring modelName() const;

	byte activeTime() const;
	ulonglong readSpeed() const;
	ulonglong writeSpeed() const;
	double avgResponseTime() const;

	ulonglong totalUsedBytes() const;
	ulonglong totalBytes() const;
	ulonglong totalFreeBytes() const;

private:
#ifdef _WIN32
	void pdhInit();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_readCounter, m_writeCounter, m_responseTimeCounter;
	
	const std::wstring m_diskLetter;
	void updateActiveTime();
#endif // _WIN32

#ifdef __linux__
	std::string m_device;

	static uint m_updateIntervalS;

	ulonglong m_prevSectorsRead;
	ulonglong m_prevSectorsWritten;
#endif // __linux__

	void updateModelName();

	std::wstring m_modelName;

	byte m_activeTime;
	ulonglong m_readSpeed;
	ulonglong m_writeSpeed;
	double m_avgResponseTime;

	ulonglong m_totalUsedBytes;
	ulonglong m_totalBytes;
	ulonglong m_totalFreeBytes;
};