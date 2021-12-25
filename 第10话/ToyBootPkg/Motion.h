#include "Video.h"
#include "File.h"

#define GAP 1

EFI_STATUS VideoInit(
    IN EFI_HANDLE ImageHandle,
    OUT VIDEO_CONFIG *VideoConfig
);

EFI_STATUS DrawLogo(
    IN EFI_HANDLE Imagehandle
);

EFI_STATUS DrawStep(
    IN UINTN Step
);
