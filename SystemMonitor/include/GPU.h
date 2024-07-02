#pragma once

#include <QString>
#include <QDebug>
#include <nvml.h>

enum GPUType
{
	NVIDIA, 
	AMD,
	UNKNOWN
};

class GPU
{
public:
	GPU(QString modelName, GPUType type, nvmlDevice_t device = NULL);
	~GPU();

	void updateInfo();

	QString modelName() const;
	unsigned int usage() const;
	unsigned long long memoryUsed() const;
	unsigned long long memoryTotal() const;
	unsigned int temperature() const;

private:
	QString m_modelName;
	GPUType m_type;
	nvmlDevice_t m_device;

	unsigned int m_usage;
	unsigned long long m_memoryUsed;
	unsigned long long m_memoryTotal;
	unsigned int m_temperature;
};