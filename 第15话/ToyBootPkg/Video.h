#include <Library/BaseMemoryLib.h>
#include <Protocol/GraphicsOutput.h>

#include "File.h"
#include "Tools.h"

#pragma pack(1)
typedef struct
{
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINTN                      FrameBufferSize;
    UINT32                     HorizontalResolution;
    UINT32                     VerticalResolution;
    UINT32                     PixelsPerScanLine;
} VIDEO_CONFIG; // @Video.h

typedef struct
{
    UINTN Size;
    UINTN PageSize;
    UINTN Width;
    UINTN Height;
    UINTN Offset;
    UINT64 PixelStart;
    UINT64 BitsPerPx;
} BMP_CONFIG;
#pragma pack(1)
EFI_STATUS GetGopHandle(
    IN EFI_HANDLE ImageHandle,
    EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop
);

EFI_STATUS SetVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
);

EFI_STATUS BmpTransform(
    IN EFI_PHYSICAL_ADDRESS BmpBase,
    OUT BMP_CONFIG *BmpConfig
);

EFI_STATUS DrawBmp(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
    IN BMP_CONFIG BmpConfig,
    IN UINTN X,
    IN UINTN Y
);