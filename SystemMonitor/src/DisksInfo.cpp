#include "DisksInfo.h"


DisksInfo::DisksInfo()
{
	DWORD drivesMask = GetLogicalDrives();
	for (char letter = 'A'; letter <= 'Z'; ++letter)
	{
		// If the bit is set, the disk exists
		if (drivesMask & 1)
			m_allDisks.push_back(Disk(letter));
		drivesMask >>= 1;
	}
	updateInfo();
}

DisksInfo::~DisksInfo() {}

void DisksInfo::updateInfo()
{
    std::vector<std::future<void>> futures;

    for (auto& disk : m_allDisks)
    {
        futures.emplace_back(std::async(std::launch::async, [&disk]() {
			disk.updateInfo();
		}));
    }

	for (auto& future : futures)
		future.get();
}

const std::vector<Disk>& DisksInfo::allDisks() const
{
	return m_allDisks;
}
