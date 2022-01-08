#include "Boot.h"

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;   
    UINT8 Step = 1;
    VIDEO_CONFIG VideoConfig;

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
    DrawStep(Step++);
    DrawStep(Step++);
    Status = DrawLogo(ImageHandle);

    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Logo is on the screen.\n");
    }
    #endif
    DrawStep(Step++);
    EFI_PHYSICAL_ADDRESS KernelEntryPoint;
    Status = Relocate(ImageHandle, L"\\Kernel.elf", &KernelEntryPoint);
    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Kernel entry getted.\n");
    }
    #endif
    DrawStep(Step++);
    
    BootConfig.VideoConfig = VideoConfig;

    UINT64 (*KernelEntry)(BOOT_CONFIG *BootConfig);
    KernelEntry = (UINT64 (*)(BOOT_CONFIG *BootConfig))KernelEntryPoint;
    
    UINT64 PassBack = KernelEntry(&BootConfig);
    Print(L"PassBack=%d.\n", PassBack);
    //Never return here
    return Status;
}