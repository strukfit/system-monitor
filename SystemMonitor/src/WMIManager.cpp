#include "WMIManager.h"

#ifdef _WIN32

thread_local IWbemLocator* WMIManager::m_pLoc = nullptr;
thread_local IWbemServices* WMIManager::m_pSvc = nullptr;

WMIManager::WMIManager()
{
}

WMIManager::~WMIManager()
{
    if(m_pSvc) m_pSvc->Release();
    if(m_pLoc) m_pLoc->Release();
}

void WMIManager::initWMI()
{
    HRESULT hr;

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&m_pLoc));
    if (FAILED(hr)) return;

    hr = m_pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &m_pSvc);
    if (FAILED(hr)) return;

    hr = CoSetProxyBlanket(m_pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hr)) return;
}

void WMIManager::execQuery(const std::wstring& query, const std::wstring& property, std::vector<WMIValue>& results)
{
    initWMI();

    HRESULT hr;

    IEnumWbemClassObject* pEnumerator = nullptr;
    hr = m_pSvc->ExecQuery(
        bstr_t("WQL"),
        bstr_t(query.c_str()),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr,
        &pEnumerator);

    if (FAILED(hr)) return;

    // Iterate over results
    IWbemClassObject* pclsObj = nullptr;
    ULONG uReturn = 0;

    while (pEnumerator)
    {
        hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

        if (0 == uReturn)
            break;
        
        VARIANT vtProp;
        hr = pclsObj->Get(property.c_str(), 0, &vtProp, nullptr, nullptr);
        if (FAILED(hr))
        {
            pclsObj->Release();
            return;
        }

        WMIValue value;
        switch (vtProp.vt)
        {
        case VT_BSTR:
            value = std::wstring(vtProp.bstrVal);
            break;
        case VT_I4:
            value = static_cast<int>(vtProp.intVal);
            break;
        case VT_R4:
            value = static_cast<float>(vtProp.fltVal);
            break;
        case VT_I8:
            value = static_cast<ULONGLONG>(vtProp.llVal);
            break;
        default:
            break;
        }

        results.push_back(value);

        VariantClear(&vtProp);
        pclsObj->Release();
    }

    // Cleanup
    pEnumerator->Release();
}

#endif
