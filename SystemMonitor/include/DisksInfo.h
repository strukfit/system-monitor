#pragma once

#include <vector>
#include <memory>
#include "Disk.h"


class DisksInfo
{
public:
	DisksInfo(std::shared_ptr<WMIManager> p_WMIManager);
	~DisksInfo();

	void updateInfo();
	std::vector<Disk> allDisks() const;

private:
	std::vector<Disk> m_allDisks;
};