#include "GPU.h"

GPU::GPU(QString modelName, gpu::Type type, nvmlDevice_t device):
    m_modelName(modelName),
    m_type(type),
    m_device(device),
    m_usage(0),
    m_memoryUsed(0),
    m_memoryTotal(0),
    m_temperature(0)
{
    /*if (m_type == NVIDIA)
    {
        nvmlReturn_t result;
        result = nvmlInit();
        if (result != NVML_SUCCESS) {
            qDebug() << "Failed to initialize NVML: " << nvmlErrorString(result);
            return;
        }
    }*/
}

GPU::~GPU()
{
    /*if (m_type == NVIDIA)
    {
        nvmlReturn_t result;
        result = nvmlShutdown();
        if (result != NVML_SUCCESS) {
            qDebug() << "Failed to shutdown NVML: " << nvmlErrorString(result);
            return;
        }
    }*/
}

void GPU::updateUsageNvidia()
{
    nvmlReturn_t result;

    nvmlUtilization_t utilization;
    result = nvmlDeviceGetUtilizationRates(m_device, &utilization);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get utilization info: " << nvmlErrorString(result);
        return;
    }
    m_usage = utilization.gpu;
}

void GPU::updateMemoryNvidia()
{
    nvmlReturn_t result;

    nvmlMemory_t memory;
    result = nvmlDeviceGetMemoryInfo(m_device, &memory);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get memory info: " << nvmlErrorString(result);
        return;
    }
    m_memoryUsed = memory.used;
    m_memoryTotal = memory.total;
}

void GPU::updateTemperatureNvidia()
{
    nvmlReturn_t result;
    result = nvmlDeviceGetTemperature(m_device, NVML_TEMPERATURE_GPU, &m_temperature);
    if (result != NVML_SUCCESS) {
        qDebug() << "Failed to get temperature info: " << nvmlErrorString(result);
        return;
    }
}

void GPU::updateInfo()
{
	if (m_type == gpu::NVIDIA)
	{
        updateUsageNvidia();
        updateMemoryNvidia();
        updateTemperatureNvidia();
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
