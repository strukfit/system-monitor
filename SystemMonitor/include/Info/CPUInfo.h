#pragma once 

#ifdef _WIN32
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <pdh.h>
#include "WMIManager.h"
#pragma comment(lib, "pdh.lib")
#endif // _WIN32

#ifdef __linux__
//#include <sys/sysinfo.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <set>
#include <filesystem>
#include <algorithm>
#endif // __linux__

#include <string>

using ulong = unsigned long;
using ulonglong = unsigned long long;

class CPUInfo
{
public:
	CPUInfo();
	~CPUInfo();

	void updateInfo();

	std::string modelName() const;

	double usage() const;

	int processCount() const;
	int threadCount() const;
	int handleCount() const;

	ulong baseSpeed() const;
	ulong coreCount() const;
	ulong logicalProcessorCount() const;

private:
#ifdef _WIN32
	void pdhInit();
	void updateCPUBaseSpeed();
	void updateCPUModelName();
	void updateCPUCoreCount();
	void updatePDHInfo();

	PDH_HQUERY m_hQuery;
	PDH_HCOUNTER m_cpuTotal, m_processCounter, m_threadCounter, m_handleCounter;
#endif // _WIN32

#ifdef __linux__
	void updateConstantVariables();
	void updateProcessThreadHandleCount();
	void getCPUTime(ulonglong& idleTime, ulonglong& totalTime);
	void updateCPUUsage();

	ulonglong m_prevIdleTime;
	ulonglong m_prevTotalTime;
#endif // __linux__


	std::string m_modelName;
	double m_usage;

	int m_processCount;
	int m_threadCount;
	int m_handleCount;

	ulong m_baseSpeed;
	ulong m_coreCount;
	ulong m_logicalProcessorCount;
};