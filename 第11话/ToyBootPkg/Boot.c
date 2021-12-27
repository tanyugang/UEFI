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




    EFI_PHYSICAL_ADDRESS KernelEntryPoint;
    Status = Relocate(ImageHandle, L"\\Kernel.elf", &KernelEntryPoint);
    Print(L"Entry@0x%llx.\n", KernelEntryPoint);
    
    /*BOOT_CONFIG BootConfig;
    //Status = gBS->AllocatePages(AllocateAnyPages, EfiLoaderData, 1, (EFI_PHYSICAL_ADDRESS *)&BootConfig);
    
    BootConfig.FrameBufferBase = VideoConfig.FrameBufferBase;
    BootConfig.FrameBufferSize = VideoConfig.FrameBufferSize;
    Print(L"\nFrameBufferBase=0x%llX.\n", (UINT64)BootConfig.FrameBufferBase);
    Status = ByeBootServices(ImageHandle);
    UINT64 (*KernelEntry)(BOOT_CONFIG *BootConfig);
    KernelEntry = (UINT64 (*)(BOOT_CONFIG *BootConfig))KernelEntryPoint;
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *VideoHandle = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)(0xC0000000);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL DarkBlue = {0x9A, 0x67, 0x18, 0};
    UINT64 NoPixels = 900 * 1440;
    for(int i = 0; i < NoPixels; i++)
    {
        *VideoHandle = DarkBlue;
        VideoHandle++;
    }
    UINT64 PassBack = 0;
    PassBack = KernelEntry(&BootConfig);
    Print(L"\nPassBack=0x%llX.\n", PassBack);*/
    //Status = ByeBootServices(ImageHandle);
    EFI_FILE_PROTOCOL *Bin;
    Status = GetFileHandle(ImageHandle, L"\\Kernel.bin", &Bin);
    EFI_PHYSICAL_ADDRESS BinAddress;
    Status = ReadFile(Bin, &BinAddress);
    Status = ByeBootServices(ImageHandle);
    asm("jmp %0": : "m"(BinAddress));
    

   

    //void (*kernel_entry)(Kernel_Boot_Info* boot_info);
    //kernel_entry = (void (*)(Kernel_Boot_Info*))*kernel_entry_point;
	// Jump to kernel entry.
	//kernel_entry(&boot_info);
    
 

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

