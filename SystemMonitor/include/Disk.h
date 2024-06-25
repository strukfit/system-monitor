#pragma once

#ifdef _WIN32
#include <windows.h>
//#else
//#include <sys/sysinfo.h>
//#include <fstream>
#endif

#include <string>

class Disk
{
public:
	Disk(const char diskLetter);
	~Disk();

	void updateInfo();

	char diskLetter() const;
	byte activeTime() const;
	float readSpeed() const;
	float writeSpeed() const;
	float avgResponseTime() const;

	ULONGLONG totalUsedBytes() const;
	ULONGLONG totalBytes() const;
	ULONGLONG totalFreeBytes() const;

private:
	const char m_diskLetter;

	byte m_activeTime;

	float m_readSpeed;
	float m_writeSpeed;
	float m_avgResponseTime;

	ULONGLONG m_totalUsedBytes;
	ULONGLONG m_totalBytes;
	ULONGLONG m_totalFreeBytes;
};