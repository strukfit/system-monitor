#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include "Disk.h"


class DisksInfo
{
public:
	DisksInfo();
	~DisksInfo();

	void updateInfo();
	const std::vector<Disk>& allDisks() const;
	
private:
	std::vector<Disk> m_allDisks;
};