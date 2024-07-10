#include "Info/GPUInfo.h"

#ifdef _WIN32
GPUInfo::GPUInfo(QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice) :
    m_modelName(modelName),
    m_type(type),
    m_nvmlDevice(nvmlDevice),
    m_AdlxGpuPtr(nullptr),
    m_usage(0),
    m_memoryUsed(0),
    m_memoryTotal(0),
    m_temperature(0)
{
}

GPUInfo::GPUInfo(QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices) :
    m_modelName(modelName),
    m_type(type),
    m_nvmlDevice(NULL),
    m_AdlxGpuPtr(adlxGpuPtr),
    m_perfMonitoringServices(perfMonitoringServices),
    m_usage(0),
    m_memoryUsed(0),
    m_memoryTotal(0),
    m_temperature(0)
{
    ADLX_RESULT res;

    res = m_perfMonitoringServices->GetSupportedGPUMetrics(m_AdlxGpuPtr, &m_gpuMetricsSupport);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get supported GPU metrics";
        return;
    }

    res = m_perfMonitoringServices->GetCurrentGPUMetrics(m_AdlxGpuPtr, &m_gpuMetrics);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get current GPU metrics";
        return;
    }
}
#endif // _WIN32

#ifdef __linux__
GPUInfo::GPUInfo(std::string index, gpu::Type type) :
    m_index(index),
    m_modelName(""),
    m_type(type),
    m_usage(0),
    m_memoryUsed(0),
    m_memoryTotal(0),
    m_temperature(0)
{
    // Update constant variables
    updateModelName();
}
#endif // __linux__

GPUInfo::~GPUInfo()
{
}

#ifdef _WIN32
void GPUInfo::updateUsageNvidia()
{
    nvmlReturn_t result;

    nvmlUtilization_t utilization;
    result = nvmlDeviceGetUtilizationRates(m_nvmlDevice, &utilization);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get utilization info: " << nvmlErrorString(result);
        return;
    }
    m_usage = utilization.gpu;
}
#endif // _WIN32

#ifdef _WIN32
void GPUInfo::updateMemoryNvidia()
{
    nvmlReturn_t result;

    nvmlMemory_t memory;
    result = nvmlDeviceGetMemoryInfo(m_nvmlDevice, &memory);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get memory info: " << nvmlErrorString(result);
        return;
    }
    m_memoryUsed = memory.used;
    m_memoryTotal = memory.total;
}
#endif // _WIN32

#ifdef _WIN32
void GPUInfo::updateTemperatureNvidia()
{
    nvmlReturn_t result;
    result = nvmlDeviceGetTemperature(m_nvmlDevice, NVML_TEMPERATURE_GPU, &m_temperature);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get temperature info: " << nvmlErrorString(result);
        return;
    }
}
#endif // _WIN32

void GPUInfo::updateUsageAMD()
{
#ifdef _WIN32
    ADLX_RESULT res;

    // Check GPU usage support status
    adlx_bool supported = false;
    res = m_gpuMetricsSupport->IsSupportedGPUUsage(&supported);
    if (ADLX_FAILED(res))
    {
        return;
    }

    if (!supported)
    {
        qDebug() << "GPU usage metric reporting is not supported on GPU";
        return;
    }

    adlx_double usage;
    res = m_gpuMetrics->GPUUsage(&usage);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get GPU usage";
        return;
    }

    m_usage = static_cast<unsigned int>(usage);
#endif // _WIN32
}

void GPUInfo::updateMemoryAMD()
{
#ifdef _WIN32
    ADLX_RESULT res;

    adlx_uint totalVRAM;
    res = m_AdlxGpuPtr->TotalVRAM(&totalVRAM);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get GPU total VRAM";
        return;
    }

    m_memoryTotal = static_cast<unsigned long long>(totalVRAM * 1024 * 1024);
    
    // Display GPU VRAM support status
    adlx_bool supported = false;
    res = m_gpuMetricsSupport->IsSupportedGPUVRAM(&supported);
    if (ADLX_FAILED(res))
    {
        qDebug() << "GPU VRAM metric reporting is not supported on GPU";
    }

    if (!supported)
    {
        qDebug() << "GPU VRAM metric reporting is not supported on GPU";
        return;
    }

    adlx_int VRAM = 0;
    res = m_gpuMetrics->GPUVRAM(&VRAM);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get GPU VRAM";
        return;
    }

    m_memoryUsed = static_cast<unsigned long long>(VRAM * 1024 * 1024);
#endif // _WIN32
}

void GPUInfo::updateTemperatureAMD()
{
#ifdef _WIN32
    ADLX_RESULT res;

    // Check GPU temperature support status
    adlx_bool supported = false;
    res = m_gpuMetricsSupport->IsSupportedGPUTemperature(&supported);
    if (ADLX_FAILED(res))
    {
        qDebug() << "GPU temperature metric reporting is not supported on GPU";
    }

    if (!supported)
    {
        qDebug() << "GPU temperature metric reporting is not supported on GPU";
        return;
    }

    adlx_double temperature = 0;
    res = m_gpuMetrics->GPUTemperature(&temperature);
    if (ADLX_FAILED(res))
    {
        qDebug() << "Failed to get GPU temperature";
        return;
    }

    m_temperature = static_cast<unsigned int>(temperature);
#endif // _WIN32
}

#ifdef __linux__
void GPUInfo::updateModelName()
{
    // Get GPU model name
    std::string modelNameStr = exec(("nvidia-smi --query-gpu=name --format=csv,noheader --id=" + m_index).c_str());
    m_modelName = QString::fromStdString(modelNameStr);
}

void GPUInfo::updateInfoNvidia()
{
    std::string gpuInfo = exec(("nvidia-smi --query-gpu=utilization.gpu,memory.total,memory.used,temperature.gpu --format=csv,noheader,nounits --id=" + m_index).c_str());
    
    std::replace(gpuInfo.begin(), gpuInfo.end(), ',', ' ');
    std::stringstream ss(gpuInfo);
    ss >> m_usage >> m_memoryTotal >> m_memoryUsed >> m_temperature;

    // Mbytes to Bytes
    m_memoryTotal = m_memoryTotal * 1024 * 1024;
    m_memoryUsed = m_memoryUsed * 1024 * 1024;
}

std::string GPUInfo::exec(const char* cmd)
{
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}
#endif // __linux__


void GPUInfo::updateInfo()
{
    switch (m_type)
    {
    case gpu::NVIDIA:
#ifdef _WIN32
        updateUsageNvidia();
        updateMemoryNvidia();
        updateTemperatureNvidia();
#endif // _WIN32

#ifdef __linux__
        updateInfoNvidia();
#endif // __linux__
        break;
    case gpu::AMD:
        updateUsageAMD();
        updateMemoryAMD();
        updateTemperatureAMD();
        break;
    default:
        break;
    }
}

QString GPUInfo::modelName() const
{
    return m_modelName;
}

uint GPUInfo::usage() const
{
    return m_usage;
}

ulonglong GPUInfo::memoryUsed() const
{
    return m_memoryUsed;
}

ulonglong GPUInfo::memoryTotal() const
{
    return m_memoryTotal;
}

uint GPUInfo::temperature() const
{
    return m_temperature;
}
