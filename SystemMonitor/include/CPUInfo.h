#pragma once 

#ifdef _WIN32
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#include "WMIManager.h"
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

	std::wstring modelName() const;

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
	void updateCPUModelName();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_cpuTotal, m_processCounter, m_threadCounter, m_handleCounter;

	std::wstring m_modelName;
	double m_usage;

	LONG m_processCount;
	LONG m_threadCount;
	LONG m_handleCount;

	DWORD m_baseSpeed;
	DWORD m_coreCount;
	DWORD m_logicalProcessorCount;
};