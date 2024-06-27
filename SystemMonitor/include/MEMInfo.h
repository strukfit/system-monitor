#pragma once

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <memory>
#include "WMIManager.h"
#pragma comment(lib, "wbemuuid.lib")
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif


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
	float speedMHz() const;

private:
	void updateSpeedInfo();

	float m_totalGB;
	float m_availGB;
	float m_usedGB;
	float m_availPageFileGB;
	float m_totalPageFileGB;
	float m_usedPageFileGB;
	DWORD m_speedMHz;
};