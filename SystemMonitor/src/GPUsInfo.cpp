#include "GPUsInfo.h"

GPUsInfo::GPUsInfo()
{
    initNvidiaCards();
    initAmdCards();
}

GPUsInfo::~GPUsInfo()
{
    // Shutdown NVML library
    nvmlReturn_t result;
    result = nvmlShutdown();
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to shutdown NVML: " << nvmlErrorString(result);
        return;
    }
}

void GPUsInfo::updateInfo()
{
    for (const auto& gpu : m_allGPUs)
    {
        gpu->updateInfo();
    }
}

const std::vector<std::unique_ptr<GPU>>& GPUsInfo::allGPUs() const
{
	return m_allGPUs;
}

void GPUsInfo::initNvidiaCards()
{
    nvmlReturn_t result;

    // Initialize NVML library
    result = nvmlInit();
    if (NVML_SUCCESS != result) {
        qDebug() << "Failed to initialize NVML: " << nvmlErrorString(result);
        return;
    }

    unsigned int device_count;

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

        m_allGPUs.push_back(std::make_unique<GPU>(static_cast<QString>(name), gpu::NVIDIA, device));
    }
}

void GPUsInfo::initAmdCards()
{
}
