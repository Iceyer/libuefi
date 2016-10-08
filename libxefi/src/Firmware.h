#pragma once

#include <vector>
#include <string>

#include "BootOption.h"

namespace UEFI
{

std::vector<std::wstring> ListBootOption();

BootOption GetBootOption(const std::wstring &bootxxxx);

std::wstring InsertBootOption(
    const std::wstring &targetLetter,
    const std::wstring &descrption,
    const std::wstring &imagepath);

void RemoveBootOption(const std::wstring &bootxxxx);

}

#include <Windows.h>

namespace Utils
{

void RasiePrivileges(void);

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX &partInfo);

}
