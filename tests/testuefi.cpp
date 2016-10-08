#include "testuefi.h"

#include <QDebug>
#include <QtTest/QTest>

#include <Firmware>
#include <iostream>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "advapi32.lib")

TestUEFI::TestUEFI(QObject *parent) : QObject(parent)
{
}

void TestUEFI::testAll()
{
    Utils::RasiePrivileges();
    auto options = UEFI::ListBootOption();


    auto bootxxxx = UEFI::InsertBootOption(L"C:", L"testUEFI", L"/EFI/test");
    QVERIFY(!bootxxxx.empty());
    QVERIFY(UEFI::ListBootOption().size() == (options.size() + 1));

    for (auto opt : UEFI::ListBootOption()) {
//        qDebug() << QString::fromStdWString(opt) ;
        UEFI::GetBootOption(L"Boot" + opt);
    }

    UEFI::RemoveBootOption(bootxxxx);
    QVERIFY(UEFI::ListBootOption().size() == options.size());

}
