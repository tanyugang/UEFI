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
        LogTip("Video is good now.\n");
    }
    #endif

    EFI_FILE_PROTOCOL *Bin;
    Status = GetFileHandle(ImageHandle, L"\\Kernel.bin", &Bin);
    EFI_PHYSICAL_ADDRESS BinAddress;
    Status = ReadFile(Bin, &BinAddress);
    Status = ByeBootServices(ImageHandle);
    asm("jmp %0": : "m"(BinAddress)); 

    return Status;
}

EFI_STATUS ByeBootServices(
    IN EFI_HANDLE ImageHandle
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    Print(L"\nBye BS.\n");
    MEMORY_MAP MemMap = {4096 * 4, NULL,4096,0,0,0};

    Status = gBS->AllocatePool(
        EfiLoaderData,
        MemMap.BufferSize,
        &MemMap.Buffer
    );
    if(EFI_ERROR(Status))
    {
        Print(L"Allocate pool for memory map failed.\n");
        return Status;
    }    
  
    Status = gBS->GetMemoryMap(
        &MemMap.BufferSize,
        (EFI_MEMORY_DESCRIPTOR *)MemMap.Buffer,
        &MemMap.MapKey,
        &MemMap.DescriptorSize,
        &MemMap.DescriptorVersion
    );

    if(EFI_ERROR(Status))
    {
        Print(L"Get memory map error.\n");
        return Status;
    }
 
    Status = gBS->ExitBootServices(
        ImageHandle, MemMap.MapKey
    );

    if(EFI_ERROR(Status))
    {
        Print(L"ExitBootServices error.\n");
        return Status;
    }

    return Status;
}

