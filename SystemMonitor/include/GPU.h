#pragma once

#include <QString>

enum GraphicsCardType
{
	NVIDIA, 
	AMD
};

class GPU
{
public:
	GPU(QString modelName);
	~GPU();

private:
	QString m_modelName;
	GraphicsCardType m_type;
};