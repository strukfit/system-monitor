#include "Disk.h"

Disk::Disk(const char diskLetter):
	m_diskLetter(diskLetter)
{
	updateInfo();
}

Disk::~Disk() {}

void Disk::updateInfo()
{
	WCHAR drive[3] = { m_diskLetter, ':', '\0' };
	ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
	GetDiskFreeSpaceEx(drive, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

	m_totalBytes = totalNumberOfBytes.QuadPart;
	m_totalFreeBytes = totalNumberOfFreeBytes.QuadPart;
	m_totalUsedBytes = m_totalBytes - m_totalFreeBytes;
}

char Disk::diskLetter() const
{
	return m_diskLetter;
}

byte Disk::activeTime() const
{
	return m_activeTime;
}

float Disk::readSpeed() const
{
	return m_readSpeed;
}

float Disk::writeSpeed() const
{
	return m_writeSpeed;
}

float Disk::avgResponseTime() const
{
	return m_avgResponseTime;
}

ULONGLONG Disk::totalUsedBytes() const
{
	return m_totalUsedBytes;
}

ULONGLONG Disk::totalBytes() const
{
	return m_totalBytes;
}

ULONGLONG Disk::totalFreeBytes() const
{
	return m_totalFreeBytes;
}
