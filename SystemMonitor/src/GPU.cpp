#include "GPU.h"

GPU::GPU(QString modelName):
	m_modelName(modelName),
	m_type()
{
	if (modelName.contains("NVIDIA", Qt::CaseInsensitive) || 
		modelName.contains("GeForce", Qt::CaseInsensitive))
	{
		m_type = NVIDIA;
	}
	else if (modelName.contains("AMD", Qt::CaseInsensitive) ||
		modelName.contains("Radeon", Qt::CaseInsensitive))
	{
		m_type = AMD;
	}
}

GPU::~GPU()
{
}
