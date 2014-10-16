#include "BootOrder.h"

#include "GUID.h"

#include <iostream>

#include <Windows.h>
#include <QDebug>
using namespace std;

namespace UEFI {

const wchar_t BootOrderStr[] = L"BootOrder";

const std::vector<UINT16>& BootOrder::GetOrders() {
    m_Orders = std::vector<UINT16>();

    const size_t bufSize = 4096;
    UINT8 buffer[bufSize];
    DWORD orderLen = GetFirmwareEnvironmentVariable(
        BootOrderStr, UEFI_GUID, buffer, bufSize);

    if (0 == orderLen)
    {
        DWORD dwErr = GetLastError();
        qDebug()<<"Failed, GetFirmwareEnvironmentVariable(), GetLastError return "<<dwErr<<endl;
        return m_Orders;
    }
    qDebug()<<"Dump BootOrder variable, Len: "<<orderLen<<endl;

    for(UINT32 i = 0; i < orderLen/2; i++) {
        qDebug()<<hex<<((UINT16*)buffer)[i]<<endl;
        m_Orders.push_back(((UINT16*)buffer)[i]);
    }
    return m_Orders;
}

bool BootOrder::Insert(UINT16 bootIndex){
    this->GetOrders();
    for (auto item: m_Orders){
        if (bootIndex == item){
            return true;
        }
    }
    m_Orders.push_back(bootIndex);
    return this->SetOrders();
}

bool BootOrder::Remove(UINT16 bootIndex){
    this->GetOrders();
    m_Orders.erase(find(m_Orders.begin(), m_Orders.end(), bootIndex));
    return this->SetOrders();
}

UINT16 BootOrder::NewIndex(){
    this->GetOrders();
    size_t searchRange = m_Orders.size();
    qDebug()<<"Order Length"<<searchRange;
    vector<bool> orders = {false};
    for (auto item: m_Orders){
        if (item < searchRange) {
            orders[item] = true;
        }
    }

    UINT16 i = 0;
    for(i = 0; i < searchRange; ++i) {
        if (!orders[i]) {
            return i;
        }
    }
    qDebug()<<"New Order "<<i;
    return i;
}

bool BootOrder::SetOrders() {
    const size_t bufSize = 4096;
    UINT8 buffer[bufSize];
    size_t index = 0;
    for (auto item: m_Orders){
        ((IndexValue*)buffer)[index] = item;
        index++;
    }

    if (!SetFirmwareEnvironmentVariable(BootOrderStr,
                                       UEFI_GUID,
                                       buffer,
                                       DWORD(m_Orders.size()*sizeof(IndexValue)/sizeof(UINT8))) == TRUE) {
            qDebug()<<"Failed!! SetFirmwareEnvironmentVariable";
            return false;
    }
    qDebug()<<"Success!! SetFirmwareEnvironmentVariable";
    return true;
}

}
