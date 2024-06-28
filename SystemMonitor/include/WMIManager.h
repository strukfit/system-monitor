#pragma once

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include <variant>
#include <string>
#include <atomic>
#include <mutex>
#include <iostream>

#include <QObject>

#pragma comment(lib, "wbemuuid.lib")


using WMIValue = std::variant<std::wstring, ULONGLONG, float, int>;

class WMIManager
{
public:
	WMIManager();
	~WMIManager();
	
	static void execQuery(const std::wstring& query, const std::wstring& property, std::vector<WMIValue>& results);

private:	
	//static void servicesInit();

	//static IWbemServices* m_pSvc;
	//static IStream* m_pSvcStream;
	//static std::mutex m_mutex;
};
#endif
