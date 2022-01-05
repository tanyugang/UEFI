#include "Motion.h"
#include "Elf.h"

typedef struct
{
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINT64                     FrameBufferSize;
} BOOT_CONFIG;

typedef struct
{
    UINT16 LimitLow;
    UINT16 BaseLow;
    UINT8  BaseMiddle;
    UINT8  SysFlag;
    UINT8  LimitHigh;
    UINT8  BaseHigh;
} GDT_SELECTOR;

typedef struct
{
    UINTN BufferSize;
    VOID *Buffer;
    UINTN MapSize;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
} MEMORY_MAP;

EFI_STATUS ByeBootServices(
    IN EFI_HANDLE ImageHandle
);
