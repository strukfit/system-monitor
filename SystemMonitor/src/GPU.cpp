#include "GPU.h"

#ifdef _WIN32
GPU::GPU(QString modelName, gpu::Type type, nvmlDevice_t nvmlDevice) :
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

GPU::GPU(QString modelName, gpu::Type type, IADLXGPUPtr adlxGpuPtr, IADLXPerformanceMonitoringServicesPtr perfMonitoringServices) :
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

GPU::GPU()
{
}

GPU::~GPU()
{
}

void GPU::updateUsageNvidia()
{
#ifdef _WIN32
    nvmlReturn_t result;

    nvmlUtilization_t utilization;
    result = nvmlDeviceGetUtilizationRates(m_nvmlDevice, &utilization);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get utilization info: " << nvmlErrorString(result);
        return;
    }
    m_usage = utilization.gpu;
#endif // _WIN32
}

void GPU::updateMemoryNvidia()
{
#ifdef _WIN32
    nvmlReturn_t result;

    nvmlMemory_t memory;
    result = nvmlDeviceGetMemoryInfo(m_nvmlDevice, &memory);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get memory info: " << nvmlErrorString(result);
        return;
    }
    m_memoryUsed = memory.used;
    m_memoryTotal = memory.total;
#endif // _WIN32
}

void GPU::updateTemperatureNvidia()
{
#ifdef _WIN32
    nvmlReturn_t result;
    result = nvmlDeviceGetTemperature(m_nvmlDevice, NVML_TEMPERATURE_GPU, &m_temperature);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get temperature info: " << nvmlErrorString(result);
        return;
    }
#endif // _WIN32
}

void GPU::updateUsageAMD()
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

void GPU::updateMemoryAMD()
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

void GPU::updateTemperatureAMD()
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

void GPU::updateInfo()
{
    switch (m_type)
    {
    case gpu::NVIDIA:
        updateUsageNvidia();
        updateMemoryNvidia();
        updateTemperatureNvidia();
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

QString GPU::modelName() const
{
    return m_modelName;
}

unsigned int GPU::usage() const
{
    return m_usage;
}

unsigned long long GPU::memoryUsed() const
{
    return m_memoryUsed;
}

unsigned long long GPU::memoryTotal() const
{
    return m_memoryTotal;
}

unsigned int GPU::temperature() const
{
    return m_temperature;
}
