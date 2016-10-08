#include <Windows.h>

#include <iostream>
#include <algorithm>
#include <functional>

static bool GetPartitionByHandle(HANDLE handle, PARTITION_INFORMATION_EX& partInfo){
   DWORD count;
   if (!DeviceIoControl(
     (HANDLE) handle,                   // handle to a partition
     IOCTL_DISK_GET_PARTITION_INFO_EX,  // dwIoControlCode
     NULL,                              // lpInBuffer
     0,                                 // nInBufferSize
     (LPVOID) &partInfo,                // output buffer
     (DWORD) sizeof(partInfo),          // size of output buffer
     (LPDWORD) &count,                  // number of bytes returned
     (LPOVERLAPPED) NULL                // OVERLAPPED structure
   )){
       return false;
   }
   return true;
}


namespace Utils {

using namespace std;

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX& partInfo){
    wstring::iterator new_end = remove_if(targetDev.begin(),
                                        targetDev.end(),
                                        bind2nd(equal_to<wchar_t>(),
                                        '\\'));
    targetDev = L"\\\\.\\" + targetDev;
    HANDLE handle = CreateFile(targetDev.c_str(), GENERIC_READ | GENERIC_WRITE,
              FILE_SHARE_READ|FILE_SHARE_WRITE,
              NULL, OPEN_EXISTING, 0, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        std::wcout <<"Open Dev Failed: "<<GetLastError()<<handle<<endl;
        return false;
    }
    return GetPartitionByHandle(handle, partInfo);
}



}
