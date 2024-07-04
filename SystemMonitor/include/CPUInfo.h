#pragma once 

#ifdef _WIN32
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#include "WMIManager.h"
#pragma comment(lib, "pdh.lib")
#endif // _WIN32

#ifdef __unix__
//#include <sys/sysinfo.h>
//#include <fstream>
#endif // __unix__

#include <string>

class CPUInfo
{
public:
	CPUInfo();
	~CPUInfo();

	void updateInfo();

	std::wstring modelName() const;

	double usage() const;

	int processCount() const;
	int threadCount() const;
	int handleCount() const;

	unsigned long baseSpeed() const;
	unsigned long coreCount() const;
	unsigned long logicalProcessorCount() const;

private:
#ifdef _WIN32
	void pdhInit();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_cpuTotal, m_processCounter, m_threadCounter, m_handleCounter;
#endif // _WIN32

	void updateCPUInfo();
	void updateCPUCoreCount();
	void updateCPUBaseSpeed();
	void updateCPUModelName();

	std::wstring m_modelName;
	double m_usage;

	int m_processCount;
	int m_threadCount;
	int m_handleCount;

	unsigned long m_baseSpeed;
	unsigned long m_coreCount;
	unsigned long m_logicalProcessorCount;
};