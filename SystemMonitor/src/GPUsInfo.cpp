#include "GPUsInfo.h"

GPUsInfo::GPUsInfo()
{
    nvmlReturn_t result;
    unsigned int device_count;

    // Initialize NVML library
    result = nvmlInit();
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to initialize NVML: " << nvmlErrorString(result);
        return;
    }

    // Get the number of devices in the system
    result = nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to get device count: " << nvmlErrorString(result);
        nvmlShutdown();
        return;
    }

    // Loop over each device and get its name and handle
    for (unsigned int i = 0; i < device_count; ++i) {
        nvmlDevice_t device;
        char name[NVML_DEVICE_NAME_BUFFER_SIZE];

        // Get the handle for the device
        result = nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get handle for device " << i << ": " << nvmlErrorString(result);
            continue;
        }

        // Get the name of the device
        result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
        if (NVML_SUCCESS != result) {
            qDebug() << "Failed to get name for device " << i << ": " << nvmlErrorString(result);
            continue;
        }

        GPUType type = gpuType(name);

        std::unique_ptr<GPU> gpu;
        if (type == NVIDIA)
        {
            gpu = std::make_unique<GPU>(name, type, device);
        }
        else
        {
            gpu = std::make_unique<GPU>(name, type);
        }

        m_allGPUs.push_back(std::move(gpu));
        
    }

    // Shutdown NVML library
    result = nvmlShutdown();
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to shutdown NVML: " << nvmlErrorString(result);
        return;
    }
}

GPUsInfo::~GPUsInfo()
{
}

void GPUsInfo::updateInfo()
{
    for (const auto& gpu : m_allGPUs)
    {
        gpu->updateInfo();
    }
}

GPUType GPUsInfo::gpuType(QString modelName)
{
    if (modelName.contains("NVIDIA", Qt::CaseInsensitive) ||
        modelName.contains("GeForce", Qt::CaseInsensitive))
    {
        return NVIDIA;
    }
    else if (modelName.contains("AMD", Qt::CaseInsensitive) ||
        modelName.contains("Radeon", Qt::CaseInsensitive))
    {
        return AMD;
    }
    else
    {
        return UNKNOWN;
    }
}

const std::vector<std::unique_ptr<GPU>>& GPUsInfo::allGPUs() const
{
	return m_allGPUs;
}
