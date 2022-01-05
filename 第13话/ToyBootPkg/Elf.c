#include "Elf.h"

EFI_STATUS Relocate(
    IN EFI_HANDLE ImageHandle,
    IN CHAR16 *FileName,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_PROTOCOL *Kernel;
    Status = GetFileHandle(ImageHandle, FileName, &Kernel);

    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }
    LogTip("Kernel file handle is getted.\n");
    #endif

    EFI_PHYSICAL_ADDRESS KernelBuffer;
    Status = ReadFile(Kernel, &KernelBuffer);

    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("Kernel is readed.\n");
    }  
    #endif

    Status = CheckELF(KernelBuffer);
    if(EFI_ERROR(Status))
    {
        return Status;
    }
 
    Status = LoadSegments(KernelBuffer, KernelEntry);
    return Status;
}

EFI_STATUS CheckELF(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINT32 Magic = GetValue(KernelBuffer, 0x00, 4);
    if(Magic != 0x464c457F)
    {
        #ifdef LOG
        LogError(NOT_ELF);
        #endif
        Status = NOT_ELF;
    }
    UINT8 Format = GetValue(KernelBuffer, 0x04, 1);
    if (Format == ELF_64)
    {
        #ifdef LOG
        LogTip("SUCCESS:Elf file is 64-bit.\n");
        #endif
    }
    else
    {
        #ifdef LOG
        LogError(NOT_64_BIT);
        #endif
        Status = NOT_64_BIT;
    }
    
    return Status;
}


EFI_STATUS LoadSegments(
    IN EFI_PHYSICAL_ADDRESS KernelBufferBase,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    ELF_HEADER_64 *ElfHeader = (ELF_HEADER_64 *)KernelBufferBase;
    PROGRAM_HEADER_64 *PHeader = (PROGRAM_HEADER_64 *)(KernelBufferBase + ElfHeader->Phoff);
    
    EFI_PHYSICAL_ADDRESS LowAddr = 0xFFFFFFFFFFFFFFFF;
    EFI_PHYSICAL_ADDRESS HighAddr = 0;

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {
        if (PHeader[i].Type == PT_LOAD)
        {
            if (LowAddr > PHeader[i].PAddress)
            {
                LowAddr = PHeader[i].PAddress;
            }
            if (HighAddr < (PHeader[i].PAddress + PHeader[i].SizeInMemory))
            {
                HighAddr = PHeader[i].PAddress + PHeader[i].SizeInMemory;
            }
        }
    }

    UINTN PageCount = ((HighAddr - LowAddr) >> 12) + 1;
    EFI_PHYSICAL_ADDRESS KernelRelocateBase;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderCode,
        PageCount,
        &KernelRelocateBase);
   
    if(EFI_ERROR(Status))
    {
        Print(L"Allocate pages for kernelrelocatebuffer error.\n");
        return Status;
    }
    UINT64 RelocateOffset = KernelRelocateBase - LowAddr;
    UINT64 *ZeroStart = (UINT64 *)KernelRelocateBase;
    for(UINTN i = 0; i < (PageCount << 9); i++)
    {
        *ZeroStart = 0x000000000000;
        ZeroStart++;
    }

    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {
        
        if (PHeader[i].Type == PT_LOAD)
        {    
            UINT8 *SourceStart = (UINT8 *)KernelBufferBase + PHeader[i].Offset;
            UINT8 *DestStart = (UINT8 *)PHeader[i].VAddress + RelocateOffset;
            
            for(UINTN j = 0; j < PHeader[i].SizeInFile; j++)
            {
                *DestStart = *SourceStart;                            
                SourceStart++;
                DestStart++;               
            }
        }        
    }
    *KernelEntry = ElfHeader->Entry + RelocateOffset;

    #ifdef LOG
    if(EFI_ERROR(Status))
    {
        LogError(Status);
    }else
    {
        LogTip("SUCCESS:Segs are loaded.\n");
    }      
    #endif
    return Status;
}



EFI_STATUS LoadSegs(
    IN EFI_FILE_PROTOCOL *Kernel,
    OUT EFI_PHYSICAL_ADDRESS *KernelEntry
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN EHeaderSize = sizeof(ELF_HEADER_64);
    ELF_HEADER_64 *EHeader;
    Status = gBS->AllocatePool(EfiLoaderData, EHeaderSize, (VOID **)&EHeader);
    Kernel->SetPosition(Kernel, 0);
    Status = Kernel->Read(Kernel, &EHeaderSize, EHeader);
    if(EFI_ERROR(Status))
    {
        Print(L"Error!Code=%d.\n", Status);
        return Status;
    }
    Print(L"EHeader@0x%llx, EHeader.PHoff=0x%llx.\n", EHeader, EHeader->Phoff);

    UINTN PHeaderTableSize = EHeader->PHeadCount * EHeader->PHeadSize;
    PROGRAM_HEADER_64 *PHeaderTable;
    Status = gBS->AllocatePool(EfiLoaderData, PHeaderTableSize, (VOID **)&PHeaderTable);
    Kernel->SetPosition(Kernel, EHeader->Phoff);
    Status = Kernel->Read(Kernel, &PHeaderTableSize, PHeaderTable);
    if(EFI_ERROR(Status))
    {
        Print(L"Error!Code=%d.\n", Status);
        return Status;
    }
    Print(L"PHeaderTable@0x%llx, PHeader ccount %d, PHeader size %d.\n",
        PHeaderTable,
        EHeader->PHeadCount,
        EHeader->PHeadSize);
    EFI_PHYSICAL_ADDRESS LowAddr = 0xFFFFFFFFFFFFFFFF;
    EFI_PHYSICAL_ADDRESS HighAddr = 0;

    for (UINTN i = 0; i < EHeader->PHeadCount; i++)
    {
        if (PHeaderTable[i].Type == PT_LOAD)
        {
            if (LowAddr > PHeaderTable[i].PAddress)
            {
                LowAddr = PHeaderTable[i].PAddress;
            }
            if (HighAddr < (PHeaderTable[i].PAddress + PHeaderTable[i].SizeInMemory))
            {
                HighAddr = PHeaderTable[i].PAddress + PHeaderTable[i].SizeInMemory;
            }
        }
    }

    UINTN PtLoadPageSize = ((HighAddr - LowAddr) >> 12) +1;
    Print(L"LowAddr=0x%llx, HighAddr=0x%llx, Size=%d, PageSize=%d.\n", LowAddr, HighAddr, HighAddr-LowAddr, PtLoadPageSize);
    EFI_PHYSICAL_ADDRESS RelocatedBuffer;
    Status = gBS->AllocatePages(AllocateAnyPages , EfiLoaderCode, PtLoadPageSize, &RelocatedBuffer);
    if(EFI_ERROR(Status))
    {
        Print(L"Error!Code=%d.\n", Status);
        return Status;
    }

    UINT64 *ZeroStart = (UINT64 *)RelocatedBuffer;
    for(UINTN i = 0; i < (PtLoadPageSize << 9); i++)
    {
        *ZeroStart = 0x000000000000;
        ZeroStart++;
    }

    Print(L"New memory start@0x%llx.\n", RelocatedBuffer);
    for (UINTN i = 0; i < EHeader->PHeadCount; i++)
    {
        if (PHeaderTable[i].Type == PT_LOAD)
        {
            UINTN SegmentSize = PHeaderTable[i].SizeInFile;
            Kernel->SetPosition(Kernel, PHeaderTable[i].Offset);
            Kernel->Read(Kernel, &SegmentSize, (VOID *)(RelocatedBuffer + PHeaderTable[i].PAddress - LowAddr));
            //Print(L"Read 0x%x bytes to 0x%llx.\n", SegmentSize, RelocatedBuffer + PHeaderTable[i].PAddress - LowAddr);
            /*UINT8 *BinByte = (UINT8 *)(RelocatedBuffer + PHeaderTable[i].PAddress - LowAddr);
            for(UINTN i = 0; i < 200; i++)
            {
                Print(L"%x ", BinByte[i]);
            }*/
        }
    }

    *KernelEntry = EHeader->Entry + RelocatedBuffer - LowAddr;
    return Status;
}

/*
EFI_STATUS LoadSegment(
    IN EFI_PHYSICAL_ADDRESS KernelBuffer,
    IN PROGRAM_HEADER_64 PHeader
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    UINTN SegPageSize = (PHeader.SizeInMemory + 4095) / 4096;
    if ((PHeader.SizeInMemory & PHeader.Align) != 0)
    {
        SegPageSize++;
        Print(L"Page + 1.\n");
    }
    Print(L"SegSize=%x, SegPageSize=%d.\n", PHeader.SizeInMemory, SegPageSize);
    EFI_PHYSICAL_ADDRESS SegmentAddress;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderCode,
        SegPageSize,
        &SegmentAddress
    );

    if(EFI_ERROR(Status))
    {
        #ifdef LOG
        LogError(Status);
        #endif
        Print(L"Allocate memory failed.\n");
        return Status;
    }else
    {
        #ifdef LOG
        LogTip("SUCCESS:Memory for Segment is ready.\n");
        #endif
    }
    Print(L"Dest=%llx, Source=%llx, Size=%d.\n",
            PHeader.VAddress,
            KernelBuffer + PHeader.Offset,
            PHeader.SizeInFile);
    CopyMem((VOID *)PHeader.VAddress, 
            (VOID *)(KernelBuffer + PHeader.Offset),
            PHeader.SizeInFile
    );

    if(PHeader.SizeInMemory > PHeader.SizeInFile)
    {
        SetMem(
            (VOID *)(PHeader.VAddress + PHeader.SizeInFile),
            PHeader.SizeInMemory - PHeader.SizeInFile,
            0
        );
        Print(L"SizeInMemory bigger than SizeInFile.\n");
    }

    return Status;
    
}*/

EFI_STATUS GetElfInfo(
    IN EFI_PHYSICAL_ADDRESS KernelAddress,
    IN OUT EFI_PHYSICAL_ADDRESS *KernelEntry)
{
    EFI_STATUS Status = EFI_SUCCESS;
    ELF_HEADER_64 *ElfHeader = (ELF_HEADER_64 *)KernelAddress;
    PROGRAM_HEADER_64 *PHeader = (PROGRAM_HEADER_64 *)(KernelAddress + ElfHeader->Phoff);
    
    EFI_PHYSICAL_ADDRESS LowAddr = 0xFFFFFFFFFFFFFFFF;
    EFI_PHYSICAL_ADDRESS HighAddr = 0;

    Print(L"KernelBase = %x.\n", KernelAddress);
    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {
        if (PHeader[i].Type == PT_LOAD)
        {
            if (LowAddr > PHeader[i].PAddress)
            {
                LowAddr = PHeader[i].PAddress;
            }
            if (HighAddr < PHeader[i].PAddress + PHeader[i].SizeInMemory)
            {
                HighAddr = PHeader[i].PAddress + PHeader[i].Offset;
            }
        }
        //PHeader++;
    }

    Print(L"LowAddr:%016llx, HighAddr:%016llx.\n", LowAddr, HighAddr);

    UINTN PageCount = (HighAddr - LowAddr + 4095) / 4096;
    EFI_PHYSICAL_ADDRESS KernelRelocateBase;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderCode,
        PageCount,
        &KernelRelocateBase);
   
    if(EFI_ERROR(Status))
    {
        Print(L"Allocate pages for kernelrelocate error.\n");
        return Status;
    }
    Print(L"KernelRelocateBase = %016llx.\n", KernelRelocateBase);
    UINTN RelocateOffset = KernelRelocateBase - LowAddr;

    Print(L"RelocateOffset:%016llx.\n", RelocateOffset);
    for (UINTN i = 0; i < ElfHeader->PHeadCount; i++)
    {
        if (PHeader[i].Type == PT_LOAD)
        {
            CopyMem(
                (VOID *)(PHeader[i].PAddress + RelocateOffset), (VOID *)(KernelAddress + PHeader[i].Offset), PHeader[i].SizeInFile
            );
            Print(L"Seg %d New Address = %016llx.\n", i, PHeader[i].PAddress + RelocateOffset);
            if(PHeader[i].SizeInMemory > PHeader[i].SizeInFile)
            {
                SetMem(
                    (VOID *)(KernelRelocateBase + PHeader[i].SizeInFile),
                    PHeader[i].SizeInMemory - PHeader[i].SizeInFile,
                    0
                );
            }
            
            //Print(L"Program Header Index:%d\n, relocated at %016llx.\n", );
        }
        
    }
    *KernelEntry = ElfHeader->Entry + RelocateOffset;
    Print(L"Relocated KernleEntry = %016llx.\n", *KernelEntry);

    return Status;
}