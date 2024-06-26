#pragma once

#ifdef _WIN32
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <memory>

#include "WMIManager.h" 

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "pdh.lib")
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

#include <string>
#include <QMessageBox>

class Disk
{
public:
	Disk(const char diskLetter, std::shared_ptr<WMIManager> p_WMIManager);
	~Disk();

	void updateInfo();

	std::wstring diskLetter() const;

	byte activeTime() const;
	float readSpeed() const;
	float writeSpeed() const;
	float avgResponseTime() const;

	ULONGLONG totalUsedBytes() const;
	ULONGLONG totalBytes() const;
	ULONGLONG totalFreeBytes() const;

private:
	void updateActiveTime();

	std::shared_ptr<WMIManager> m_WMIManager;

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_activeTimeCounter;

	const std::wstring m_diskLetter;
	const std::string m_diskModel;

	byte m_activeTime;
	float m_readSpeed;
	float m_writeSpeed;
	float m_avgResponseTime;

	ULONGLONG m_totalUsedBytes;
	ULONGLONG m_totalBytes;
	ULONGLONG m_totalFreeBytes;
};