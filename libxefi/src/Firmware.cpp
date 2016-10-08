#include "Firmware.h"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <memory>

#include <Windows.h>

#include "BootOption.h"
#include "BootOrder.h"
#include "GUID.h"

using namespace std;

namespace Utils
{

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX &partInfo);


void RasiePrivileges(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                          &hToken)) {
        cout << "Failed OpenProcessToken" << endl;
        return;
    }

    LookupPrivilegeValue(NULL,
                         SE_SYSTEM_ENVIRONMENT_NAME,
                         &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    DWORD len;
    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, &len);

    if (GetLastError() != ERROR_SUCCESS) {
        cout << "Failed RasiePrivileges()" << endl;
        return;
    }
}

}

namespace UEFI
{

std::wstring InsertBootOption(
    const std::wstring &target,
    const std::wstring &descrption,
    const std::wstring &imagepath
)
{
    Utils::RasiePrivileges();
    BootOrder btorder;

    UINT16 bootIndex = btorder.NewIndex();

    wchar_t bootxxxx[32] = {0};
    swprintf_s(bootxxxx, 32, L"Boot%04x", bootIndex);

    BootOption btop(descrption);
    btop.AddFilePath(FilePath::FromHardwareDevice(target));
    btop.AddFilePath(FilePath::FromMediaDevice(imagepath));
    DWORD bufferSize = DWORD(btop.CalcSize());

//    std::wcout << "[InsertBootOption] BootOption Size: " << bufferSize  << std::endl;

    std::unique_ptr<UINT8[]> buffer(new UINT8[bufferSize]);
    if (!btop.Pack(buffer.get(), bufferSize)) {
        return L"";
    }
//    std::wcout << "[InsertBootOption] Set New Boot Option: " << bootxxxx << std::endl;
    if (SetFirmwareEnvironmentVariable(bootxxxx,
                                       UEFI_GUID,
                                       buffer.get(),
                                       bufferSize) == TRUE) {
//        std::wcout << "[InsertBootOption] Success!! SetFirmwareEnvironmentVariable" << std::endl;
    } else {
        std::wcout << "[InsertBootOption] Failed!! SetFirmwareEnvironmentVariable" << std::endl;
    }

//    std::wcout << "[InsertBootOption] Insert New Boot Option " << bootxxxx << " Success!!!" << std::endl;

    btorder.Insert(bootIndex);
    return bootxxxx;
}

void RemoveBootOption(const wstring &bootxxxx)
{
    Utils::RasiePrivileges();

    if (SetFirmwareEnvironmentVariable(bootxxxx.c_str(),
                                       UEFI_GUID,
                                       NULL,
                                       NULL) == TRUE) {
//        std::wcout << "Success!! SetFirmwareEnvironmentVariable" << endl;
    } else {
        std::wcout << "Failed!! SetFirmwareEnvironmentVariable" << GetLastError() << endl;
        return;
    }
    BootOrder btorder;
    UINT16 index = 0;
    for (size_t i = 4; i < 8; ++i) {
        index = index * 16 + (bootxxxx[i] - L'0');
    }
    btorder.Remove(index);
}

std::vector<wstring> ListBootOption()
{
    vector<wstring> strOrders;
    BootOrder btorder;
    for (UINT16 order : btorder.GetOrders()) {
        std::wstringstream stream;
        stream << std::setw(4) << std::setfill(L'0')
               << std::setiosflags(std::ios::right) << std::hex
               << order;
        wstring strOrder = stream.str();
        std::transform(strOrder.begin(), strOrder.end(), strOrder.begin(), ::toupper);
        strOrders.push_back(strOrder);
    }
    return strOrders;
}

BootOption GetBootOption(const wstring &bootxxxx)
{
    const size_t bufSize = 4096;
    UINT8 buffer[bufSize] = {4};
    DWORD orderLen = GetFirmwareEnvironmentVariable(
                         bootxxxx.c_str(), UEFI_GUID, buffer, bufSize);


    if (0 == orderLen) {
        // TODO:
        std::wcout << "Failed, GetFirmwareEnvironmentVariable(), GetLastError return " << GetLastError();
    }

    return BootOption(buffer, orderLen);
}

}
