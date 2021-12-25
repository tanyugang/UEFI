#include "Boot.h"

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;   

    #ifdef LOG
    Status = LogInitial(ImageHandle);
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Log is good now.\n");
    }
    
    #endif
    VIDEO_CONFIG VideoConfig;
    Status = VideoInit(ImageHandle, &VideoConfig);
    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Video is good now.\n");
    }
    #endif    
    
    Status = DrawLogo(ImageHandle);

    for(UINTN i = 0; i < 10; i++)
    {
        Status = DrawStep(i);
    }
    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Step is good now.\n");
    }
    #endif

    return Status;
}

