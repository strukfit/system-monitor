#include "MEMInfo.h"

MEMInfo::MEMInfo():
    m_totalGB(0.f),
    m_availGB(0.f),
    m_usedGB(0.f),
    m_availPageFileGB(0.f),
    m_totalPageFileGB(0.f),
    m_usedPageFileGB(0.f)
{
    updateInfo();
}

MEMInfo::~MEMInfo()
{
}

void MEMInfo::updateInfo()
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memStatus);

    m_totalGB = memStatus.ullTotalPhys / 1024.f / 1024.f / 1024.f;
    m_availGB = memStatus.ullAvailPhys / 1024.f / 1024.f / 1024.f;
    m_usedGB = m_totalGB - m_availGB;

    m_availPageFileGB = memStatus.ullAvailPageFile / 1024.f / 1024.f / 1024.f;
    m_totalPageFileGB = memStatus.ullTotalPageFile / 1024.f / 1024.f / 1024.f;
    m_usedPageFileGB = m_totalPageFileGB - m_availPageFileGB;

    updateSpeedInfo();
}

void MEMInfo::updateSpeedInfo()
{
    HRESULT hr;

    IWbemLocator* pLoc = nullptr;
    IWbemServices* pSvc = nullptr;

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLoc));
    if (FAILED(hr)) return;

    hr = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
    if (FAILED(hr))
    {
        pLoc->Release();
        return;
    }

    hr = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hr))
    {
        pSvc->Release();
        pLoc->Release();
        return;
    }

    // Query WMI for memory speed
    IEnumWbemClassObject* pEnumerator = nullptr;
    hr = pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t("SELECT * FROM Win32_PhysicalMemory"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator);

    if (FAILED(hr))
    {
        pSvc->Release();
        pLoc->Release();
        return;
    }

    // Iterate over results
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    UINT speed = 0;
    while (pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
            break;

        VARIANT vtProp;
        hr = pclsObj->Get(L"Speed", 0, &vtProp, nullptr, nullptr);
        if (FAILED(hr))
        {
            pclsObj->Release();
            pSvc->Release();
            pLoc->Release();
            return;
        }

        if(vtProp.uintVal > speed)
            speed = vtProp.uintVal;
        
        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    pSvc->Release();
    pLoc->Release();
    pEnumerator->Release();

    m_speedMHz = speed;
}

float MEMInfo::totalGB() const
{
    return m_totalGB;
}

float MEMInfo::availGB() const
{
    return m_availGB;
}

float MEMInfo::usedGB() const
{
    return m_usedGB;
}

float MEMInfo::availPageFileGB() const
{
    return m_availPageFileGB;
}

float MEMInfo::totalPageFileGB() const
{
    return m_totalPageFileGB;
}

float MEMInfo::usedPageFileGB() const
{
    return m_usedPageFileGB;
}

float MEMInfo::speedMHz() const
{
    return m_speedMHz;
}
