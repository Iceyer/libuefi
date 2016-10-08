#include "BootOption.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Protocol.h"

namespace Utils
{

bool GetPartitionInfo(std::wstring targetDev, PARTITION_INFORMATION_EX &partInfo);

}

namespace UEFI
{

BootOption::BootOption(UINT8 *buffer, const DWORD orderLen)
{
//    std::wcout << L"----------- Begin Boot Option -----------" << std::endl;

//    std::wcout << L"Boot Option Length: " << orderLen << std::endl;

//    for (DWORD i = 0; i < orderLen; ++i) {
//        if (i % 8 == 0) {
//            std::wcout << std::endl;
//        }
//        std::wcout << std::setw(2) << std::setfill(L'0')
//                   << std::setiosflags(std::ios::right) << std::hex
//                   << buffer[i] << " ";

//    }
//    std::wcout << std::endl << std::dec;
//    std::wcout.clear();

    UINT8 *originBuffer = buffer;
    std::unique_ptr<wchar_t> wstrBuffer(new wchar_t[orderLen]);
    memset(wstrBuffer.get(), 0, orderLen);

    // Decode Header
    memcpy(&m_Header, buffer,  sizeof(m_Header));
    buffer += sizeof(m_Header);
//    std::wcout << L"Attributes: 0x" << std::setw(8) << std::setfill(L'0')
//               << std::setiosflags(std::ios::right) << std::hex
//               << m_Header.Attributes << std::endl
//               << std::dec << L"FilePathListLength: " << m_Header.FilePathListLength << std::endl;
//    std::wcout.clear();

    // Decode Description
    size_t descLen = wcslen((wchar_t *)buffer);
    wcsncpy(wstrBuffer.get(), (wchar_t *)buffer, descLen);
    buffer += (descLen + 1) * 2;
    m_Description = wstrBuffer.get();
//    std::wcout << L"Description: " << m_Description;
//    std::wcout.clear();
//    std::wcout << L" Len: " << m_Description.length() << std::endl;

    // Loop Read FilePathList
    UINT16 currentFilePathOffset = 0;
    while (true) {
        EFI_DEVICE_PATH_PROTOCOL header;
        memcpy(&header, buffer,  sizeof(header));
//        std::wcout << L"Device Type: " << header.Type
//                   << " SubType: " << header.SubType
//                   << " Length: " << header.Length
//                   << std::endl;

        currentFilePathOffset += header.Length;
        buffer += header.Length;


        if (currentFilePathOffset > m_Header.FilePathListLength) {
            break;
        }

        m_FilePathList.push_back(FilePath(header, buffer - header.Length));

//        FilePath fp(header, buffer - header.Length);
//        switch (header.SubType) {
//        case HARDWARE_DEVICE_PATH:
//            std::wcout << L"Find FileList: PartitionStart " << fp.ToHardwareDevice().PartitionStart;
//            std::wcout.clear();
//            break;
//        case MEDIA_DEVICE_PATH:
//            std::wcout << L"Find FileList: " << fp.ToMediaDevice();
//            std::wcout.clear();
//            break;
//        default:
//            std::wcout << L"TODO FileList";
//            break;
//        }
//        std::wcout << std::endl;

        if (END_OF_HARDWARE_DEVICE_PATH == header.Type) {
            break;
        }

    }
//    std::wcout << L"Attributes: " << m_Header.Attributes << std::endl
//               << L"Description: " << m_Description << std::endl
//               << L"Find FileList Length: " << m_FilePathList.size()
//               << std::endl;

    //Ext Data Size
//    std::wcout << L"Parse Buffer: " << buffer - originBuffer
//               << L"\t\r\nTatal Buffer: " << orderLen
//               << std::endl;

//    std::wcout << L"----------- End Boot Option -----------" << std::endl << std::endl;
}

BootOption::BootOption(const std::wstring descrption)
{
    m_Header.Attributes = LOAD_OPTION_ACTIVE;
    m_Header.FilePathListLength = 0;
    m_Description = descrption;

    EFI_DEVICE_PATH_PROTOCOL buttHeader;
    buttHeader.Type = 0x7F;
    buttHeader.SubType = 0xFF;
    buttHeader.Length = sizeof(buttHeader);
    m_FilePathList.push_back(FilePath(buttHeader, reinterpret_cast<const UINT8 *>(&buttHeader)));
}

void BootOption::AddFilePath(const FilePath &fp)
{
    m_FilePathList.push_front(fp);
}

size_t BootOption::CalcSize()
{
    m_Header.FilePathListLength = 0;
    for (FilePath &fp : m_FilePathList) {
        m_Header.FilePathListLength += fp.Length();
    }

    return sizeof(m_Header)
           + (m_Description.length() + 1) * 2
           + m_Header.FilePathListLength
           + m_OptionalSzie;

}

std::wstring BootOption::guid()
{
    for (const FilePath &fp : m_FilePathList) {
        if (MEDIA_DEVICE_PATH == fp.Type() && HARDWARE_DEVICE_PATH == fp.SubType()) {
            HARDDRIVE_DEVICE_PATH hdp = fp.ToHardwareDevice();
            std::wstringstream wss;
            for (int i = 0; i < sizeof(hdp.Signature); ++i) {
                wss << hdp.Signature[i];
            }
            return wss.str();
        }
    }
    return L"";
}

std::wstring BootOption::imagePath()
{
    for (const FilePath &fp : m_FilePathList) {
        if (MEDIA_DEVICE_PATH == fp.Type() && MEDIA_DEVICE_PATH == fp.SubType()) {
            return fp.ToMediaDevice();
        }
    }
    return L"";
}

bool BootOption::Pack(UINT8 *buffer, size_t bufferLen)
{
    if (bufferLen < CalcSize()) {
        return false;
    }

    memset(buffer, 0, bufferLen);
    memcpy(buffer,  &m_Header, sizeof(m_Header));
    buffer += sizeof(m_Header);

    size_t descriptionSize = (m_Description.length() + 1) * 2;
    wcsncpy((wchar_t *)buffer, m_Description.c_str(), m_Description.size());
    buffer += descriptionSize - 2;
    *(wchar_t *)buffer = 0;
    buffer += 2;

    for (FilePath &fp : m_FilePathList) {
        memcpy(buffer,  fp.Data(), fp.Length());
        buffer += fp.Length();
    }
    buffer += m_OptionalSzie;
    return true;
}

FilePath::FilePath(const EFI_DEVICE_PATH_PROTOCOL &header, const UINT8 *data)
    : m_Header(header), m_Data(new UINT8[header.Length])
{
    memcpy_s(m_Data.get(), header.Length, data, header.Length);
}

FilePath::FilePath(const FilePath &fp)
    : m_Header(fp.m_Header), m_Data(new UINT8[fp.m_Header.Length])
{
    memcpy_s(m_Data.get(), fp.m_Header.Length, fp.Data(), fp.m_Header.Length);
}

FilePath FilePath::FromMediaDevice(const std::wstring filePath)
{
    EFI_DEVICE_PATH_PROTOCOL header;
    header.Type = MEDIA_DEVICE_PATH;
    header.SubType = MEDIA_DEVICE_PATH;
    header.Length = static_cast<UINT16>(sizeof(header) + filePath.size() * 2 + 2);

    std::unique_ptr<UINT8[]> data(new UINT8[header.Length]);
    memset(data.get(), 0, header.Length);
    memcpy_s(data.get(), header.Length, &header, sizeof(header));
    memcpy_s(data.get() + sizeof(header), header.Length,
             filePath.c_str(), filePath.size() * 2);
    return FilePath(header, data.get());
}

std::wstring FilePath::ToMediaDevice() const
{
    if (m_Header.SubType != MEDIA_DEVICE_PATH) {
        return L"INVAILD_STRING";
    }
    std::unique_ptr<wchar_t> wstrBuffer(new wchar_t[m_Header.Length]);
    memset(wstrBuffer.get(), 0, m_Header.Length);
    wcsncpy(wstrBuffer.get(), (wchar_t *)(m_Data.get() + sizeof(m_Header)), m_Header.Length / 2);
    return std::wstring(wstrBuffer.get());
}

FilePath FilePath::FromHardwareDevice(const std::wstring driver)
{
    HARDDRIVE_DEVICE_PATH devicePath;
    devicePath.Header.Type = MEDIA_DEVICE_PATH;
    devicePath.Header.SubType = HARDWARE_DEVICE_PATH;
    devicePath.Header.Length = static_cast<UINT16>(sizeof(HARDDRIVE_DEVICE_PATH));

    PARTITION_INFORMATION_EX part;
    Utils::GetPartitionInfo(driver, part);
    devicePath.PartitionNumber = part.PartitionNumber;
    devicePath.PartitionStart = part.StartingOffset.QuadPart / 512;
    devicePath.PartitionSize = part.PartitionLength.QuadPart / 512;
    devicePath.MBRType = 2;
    devicePath.SignatureType = 2;
    memcpy_s(devicePath.Signature, 16, &(part.Gpt.PartitionId), 16);

    return FilePath(devicePath.Header, reinterpret_cast<const UINT8 *>(&devicePath));
}

HARDDRIVE_DEVICE_PATH FilePath::ToHardwareDevice() const
{
    HARDDRIVE_DEVICE_PATH hdp;
    memset(&hdp, 0, sizeof(hdp));
    if (m_Header.SubType != HARDWARE_DEVICE_PATH) {
        return hdp;
    }
    memcpy_s(&hdp, sizeof(hdp), m_Data.get(), m_Header.Length);
    return hdp;
}

}
