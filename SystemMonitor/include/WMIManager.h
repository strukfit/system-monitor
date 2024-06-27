#pragma once

#ifdef _WIN32
#include <windows.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include <variant>
#include <string>

#pragma comment(lib, "wbemuuid.lib")


using WMIValue = std::variant<std::wstring, ULONGLONG, float, int>;

class WMIManager
{
public:
	WMIManager();
	~WMIManager();
	
	static void execQuery(const std::wstring& query, const std::wstring& property, std::vector<WMIValue>& results);

private:
	static void initWMI();

	static thread_local IWbemLocator* m_pLoc;
	static thread_local IWbemServices* m_pSvc;

};
#endif
