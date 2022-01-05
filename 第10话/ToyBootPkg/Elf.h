#include <Library/BaseMemoryLib.h>
#include  <Library/MemoryAllocationLib.h>

#include "File.h"
#include "Log.h"
#include "Tools.h"

#define ELF_32 1
#define ELF_64 2
#define NOT_ELF -1
#define NOT_64_BIT -2
#define PT_LOAD 1


typedef struct 
{
    UINT32 Magic; //0x00
    UINT8  Format;//0x04,32 or 64 bits format 1=32, 2=64
    UINT8  Endianness;//0x05
    UINT8  Version;//0x06
    UINT8  OSAbi;//0x07
    UINT8  AbiVersion;//0x08
    UINT8  Rserved[7];//0x09
    UINT16 Type;//0x10
    UINT16 Machine;//0x12
    UINT32 ElfVersion;//0x14
    UINT32 Entry;//0x18
    UINT32 Phoff;//0x1C
    UINT32 Shoff;//0x20
    UINT32 Flags;//0x24
    UINT16 HeadSize;//0x28,size of elf head
    UINT16 PHeadSize;//0x2A, size of a program header table entry
    UINT16 PHeadCount;//0x2C, count of entries in the program header table
    UINT16 SHeadSize;//0x2E, size of a section header table entry
    UINT16 SHeadCount;//0x30, count of entries in the section header table
    UINT16 SNameIndex;//0x32, index of entry that contains the section names

} ELF_HEADER_32;

typedef struct 
{
    UINT32 Magic; //0x00
    UINT8  Format;//0x04,32 or 64 bits format
    UINT8  Endianness;//0x05
    UINT8  Version;//0x06
    UINT8  OSAbi;//0x07
    UINT8  AbiVersion;//0x08
    UINT8  Rserved[7];//0x09
    UINT16 Type;//0x10
    UINT16 Machine;//0x12
    UINT32 ElfVersion;//0x14
    UINT64 Entry;//0x18
    UINT64 Phoff;//0x20
    UINT64 Shoff;//0x28
    UINT32 Flags;//0x30
    UINT16 HeadSize;//0x34,size of elf head
    UINT16 PHeadSize;//0x36, size of a program header table entry
    UINT16 PHeadCount;//0x38, count of entries in the program header table
    UINT16 SHeadSize;//0x3A, size of a section header table entry
    UINT16 SHeadCount;//0x3C, count of entries in the section header table
    UINT16 SNameIndex;//0x3E, index of entry that contains the section names
} ELF_HEADER_64;

typedef struct
{
    UINT32 Type;//0x00, type of segment
    UINT32 Offset;//0x04, Offset of the segment in the file image
    UINT32 VAddress;//0x08, Virtual address of the segment in memory
    UINT32 PAddress;//0x0C, reserved for segment`s physical address
    UINT32 SizeInFile;//0x10, size in bytes of the segment in the file image
    UINT32 SizeInMemory;//0x14, size in bytes of the segment in memory
    UINT32 Flags;//0x18
    UINT32 Align;//0x1C, 0 and 1
} PROGRAM_HEADER_32;

typedef struct
{
    UINT32 Type;//0x00, type of segment
    UINT32 Flags;//0x04 Segment-dependent flags
    UINT64 Offset;//0x08
    UINT64 VAddress;//0x10
    UINT64 PAddress;//0x18
    UINT64 SizeInFile;//0x20
    UINT64 SizeInMemory;//0x28
    UINT64 Align;//0x30

} PROGRAM_HEADER_64;

EFI_STATUS CheckELF(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer
);

EFI_STATUS LoadSegments(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry
);

EFI_STATUS LoadSegs(
    IN EFI_FILE_PROTOCOL *Kernel,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry
);
/*EFI_STATUS LoadSegment(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer,
    IN PROGRAM_HEADER_64 PHeader
);*/

EFI_STATUS Relocate(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *FileName,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry
);

EFI_STATUS GetElfInfo(
    IN EFI_PHYSICAL_ADDRESS KernelAddress,
    IN OUT EFI_PHYSICAL_ADDRESS *KernelEntry
);