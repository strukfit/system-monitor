#include "Converter.h"

std::string Converter::convertBytes(ulonglong bytes)
{
    const float KILOBYTE = 1024.f;
    const float MEGABYTE = 1024.f * KILOBYTE;
    const float GIGABYTE = 1024.f * MEGABYTE;
    const float TERABYTE = 1024.f * GIGABYTE;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);

    if (bytes < KILOBYTE)
    {
        oss << bytes << " B";
    }
    else if (bytes < MEGABYTE)
    {
        oss << bytes / KILOBYTE << " Kb";
    }
    else if (bytes < GIGABYTE)
    {
        oss << bytes / MEGABYTE << " Mb";
    }
    else if (bytes < TERABYTE)
    {
        oss << bytes / GIGABYTE << " Gb";
    }
    else
    {
        oss << bytes / TERABYTE << " Tb";
    }

    return oss.str();
}