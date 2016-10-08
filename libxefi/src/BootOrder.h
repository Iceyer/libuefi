#pragma once

#include <vector>

#include "Type.h"

namespace UEFI {

class BootOrder {
public:
    const std::vector<UINT16>& GetOrders();
    bool Insert(UINT16 bootIndex);
    bool Remove(UINT16 bootIndex);
    UINT16 NewIndex();

private:
    bool SetOrders();

    typedef UINT16 IndexValue;
    typedef std::vector<UINT16>::iterator IndexIter;

    std::vector<UINT16> m_Orders;
};

}
