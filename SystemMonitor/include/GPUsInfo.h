#pragma once

#include <vector>
#include <memory>

#include "GPU.h"

class GPUsInfo
{
public:
	GPUsInfo();
	~GPUsInfo();

	void updateInfo();
	const std::vector<std::unique_ptr<GPU>>& allGPUs() const;

private:
	void initNvidiaCards();
	void initAmdCards();

	std::vector<std::unique_ptr<GPU>> m_allGPUs;
};