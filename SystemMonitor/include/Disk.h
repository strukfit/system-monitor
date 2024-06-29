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
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

#include <string>
#include <QObject>
#include <iostream>

class Disk
{
public:
	Disk(const char diskLetter);
	~Disk();

	void updateInfo();

	std::wstring diskLetter() const;

	byte activeTime() const;
	LONG readSpeed() const;
	LONG writeSpeed() const;
	double avgResponseTime() const;

	ULONGLONG totalUsedBytes() const;
	ULONGLONG totalBytes() const;
	ULONGLONG totalFreeBytes() const;

private:
	void pdhInit();
	void updateActiveTime();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_readCounter, m_writeCounter, m_responseTimeCounter;

	const std::wstring m_diskLetter;
	const std::string m_diskModel;

	byte m_activeTime;
	LONG m_readSpeed;
	LONG m_writeSpeed;
	double m_avgResponseTime;

	ULONGLONG m_totalUsedBytes;
	ULONGLONG m_totalBytes;
	ULONGLONG m_totalFreeBytes;
};