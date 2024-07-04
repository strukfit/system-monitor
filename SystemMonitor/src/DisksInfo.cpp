#include "DisksInfo.h"


DisksInfo::DisksInfo()
{
#ifdef _WIN32
	DWORD drivesMask = GetLogicalDrives();
	for (char letter = 'A'; letter <= 'Z'; ++letter)
	{
		// If the bit is set, the disk exists
		if (drivesMask & 1)
			m_allDisks.push_back(std::make_unique<Disk>(letter));
		drivesMask >>= 1;
	}
#endif // _WIN32

	updateInfo();
}

DisksInfo::~DisksInfo() {}

void DisksInfo::updateInfo()
{
    std::vector<std::future<void>> futures;
    for (auto& disk : m_allDisks)
    {
        futures.emplace_back(std::async(std::launch::async, [&disk]() {
			disk->updateInfo();
		}));
    }

	for (auto& future : futures)
		future.get();
}

const std::vector<std::unique_ptr<Disk>>& DisksInfo::allDisks() const
{
	return m_allDisks;
}
