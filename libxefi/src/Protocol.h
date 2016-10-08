#pragma once

//*******************************************************
// Attributes
//*******************************************************

#define LOAD_OPTION_ACTIVE          0x00000001
#define LOAD_OPTION_FORCE_RECONNECT 0x00000002

#define	LOAD_OPTION_HIDDEN          0x00000008
#define	LOAD_OPTION_CATEGORY        0x00001F00

#define LOAD_OPTION_CATEGORY_BOOT   0x00000000
#define LOAD_OPTION_CATEGORY_APP    0x00000100

// All values 0x00000200-0x00001F00 are reserved

//*******************************************************
// EFI_DEVICE_PATH_PROTOCOL Type
//*******************************************************

///< 0x01 Hardware Device Path
///< 0x02 ACPI Device Path
///< 0x03 Messaging Device Path
///< 0x04 Media Device Path
///< 0x05 BIOS Boot Specification Device Path
///< 0x7F End of Hardware Device Path
#define HARDWARE_DEVICE_PATH                    0x01
#define ACPI_DEVICE_PATH                        0x02
#define MESSAGING_DEVICE_PATH                   0x03
#define MEDIA_DEVICE_PATH                       0x04
#define BIOS_BOOT_SPECIFICATION_DEVICE_PATH     0x05
#define END_OF_HARDWARE_DEVICE_PATH             0x7F
