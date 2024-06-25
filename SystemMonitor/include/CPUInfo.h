#pragma once 

#ifdef _WIN32
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#pragma comment(lib, "pdh.lib")
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif


class CPUInfo
{
public:
	CPUInfo();
	~CPUInfo();

	void updateInfo();

	double usage() const;

	LONG processCount() const;
	LONG threadCount() const;
	LONG handleCount() const;

	DWORD baseSpeed() const;
	DWORD coreCount() const;
	DWORD logicalProcessorCount() const;

private:
	void pdhInit();
	void updateCPUInfo();
	void updateCPUCoreCount();
	void updateCPUBaseSpeed();

	PDH_HQUERY cpuQuery;
	PDH_HCOUNTER cpuTotal, processCounter, threadCounter, handleCounter;

	double m_usage;

	LONG m_processCount;
	LONG m_threadCount;
	LONG m_handleCount;

	DWORD m_baseSpeed;
	DWORD m_coreCount;
	DWORD m_logicalProcessorCount;
};