#pragma once

#include <vector>
#include <string>
#include <memory>
#include <list>

#include <Windows.h>

#pragma pack(1)

typedef struct {
    UINT32 Attributes;
    UINT16 FilePathListLength;
    //WCHAR  Description[1];
} EFI_LOAD_OPTION;

typedef struct {
    UINT8 Type;       ///< 0x01 Hardware Device Path
    ///< 0x02 ACPI Device Path
    ///< 0x03 Messaging Device Path
    ///< 0x04 Media Device Path
    ///< 0x05 BIOS Boot Specification Device Path
    ///< 0x7F End of Hardware Device Path

    UINT8 SubType;    ///< Varies by Type
    ///< 0xFF End Entire Device Path, or
    ///< 0x01 End This Instance of a Device Path and start a new
    ///< Device Path

    UINT16 Length;  ///< Specific Device Path data. Type and Sub-Type define
    ///< type of data. Size of data is included in Length.

} EFI_DEVICE_PATH_PROTOCOL;

typedef struct {
    EFI_DEVICE_PATH_PROTOCOL        Header;
    UINT32                          PartitionNumber;
    UINT64                          PartitionStart;
    UINT64                          PartitionSize;
    UINT8                           Signature[16];
    UINT8                           MBRType;
    UINT8                           SignatureType;
} HARDDRIVE_DEVICE_PATH;


typedef struct {
    EFI_DEVICE_PATH_PROTOCOL        Header;
    //WCHAR                           PathName[1];
} FILEPATH_DEVICE_PATH;


namespace UEFI
{

class FilePath
{
public:
    FilePath(const EFI_DEVICE_PATH_PROTOCOL &header, const UINT8 *data);
    FilePath(const FilePath &);

    UINT8 Type()const {return m_Header.Type;}
    UINT8 SubType() const {return m_Header.SubType;}
    inline UINT16 Length() const {return m_Header.Length;}
    inline UINT8 *Data() const {return m_Data.get();}

    static FilePath FromMediaDevice(const std::wstring filePath);
    std::wstring ToMediaDevice() const;

    static FilePath FromHardwareDevice(const std::wstring filePath);
    HARDDRIVE_DEVICE_PATH ToHardwareDevice() const;

private:

    EFI_DEVICE_PATH_PROTOCOL    m_Header;
    std::unique_ptr<UINT8[]>    m_Data;
};

class BootOption
{
public:
    BootOption(UINT8 *buffer, const DWORD orderLen);
    BootOption(const std::wstring descrption);

    void AddFilePath(const FilePath &fp);

    size_t CalcSize();

    bool Pack(unsigned char *buffer, size_t bufferLen);

    UINT32 attributes() {return m_Header.Attributes;}
    std::wstring description() {return m_Description;}
    std::wstring guid();
    std::wstring imagePath();

private:
    EFI_LOAD_OPTION             m_Header;
    std::wstring                m_Description;
    std::list<FilePath>         m_FilePathList;
    size_t                      m_OptionalSzie = 0;
};

}
