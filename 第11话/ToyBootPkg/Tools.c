#include "Tools.h"

UINTN GetValue(
    IN EFI_PHYSICAL_ADDRESS StartAddress,
    IN UINTN Offset,
    IN UINTN Size
)
{
    UINT8 *ByteStart = (UINT8 *)StartAddress + Offset;
    UINTN Result = 0;
    for(UINTN i = 0; i < Size; i++)
    {
        Result += *(ByteStart + i) << i * 8;
    }
    return Result;
}