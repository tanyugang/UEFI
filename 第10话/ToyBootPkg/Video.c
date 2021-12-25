#include "Video.h"
struct bmp_header {
  UINT8 sig[2];
  UINT32 file_size;
  UINT16 reserved1;
  UINT16 reserved2;
  UINT32 header_size;
  UINT32 info_header_size;
  UINT32 width;
  UINT32 height;
  UINT16 plane_num;
  UINT16 color_bit;
  UINT32 compression_type;
  UINT32 compression_size;
  UINT32 horizontal_pixel;
  UINT32 vertical_pixel;
  UINT32 color_num;
  UINT32 essentail_num;
};

EFI_STATUS GetGopHandle(
    IN EFI_HANDLE ImageHandle,
    OUT EFI_GRAPHICS_OUTPUT_PROTOCOL **Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN HandleCount = 0;
    EFI_HANDLE *HandleBuffer;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to LocateHanleBuffer of GraphicsOutputProtocol.\n");
        return Status;
    }
    Print(L"SUCCESS:Get %d handles that supported GraphicsOutputProtocol.\n", HandleCount);
    
    #endif
    Status = gBS->OpenProtocol(
        HandleBuffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)Gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open first handle that supported GraphicsOutputProtocol.\n");
        return Status;
    }

    Print(L"SUCCESS:GraphicsOutputProtocol is opened with first handle.\n");
    
    #endif

    return Status;
}

EFI_STATUS SetVideoMode(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo;
    UINTN ModeInfoSize = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
    UINTN H = 0;
    UINTN V = 0;
    UINTN ModeIndex = 0;

    for(UINTN i = 0; i < Gop->Mode->MaxMode; i++)
    {
        Status = Gop->QueryMode(Gop, i, &ModeInfoSize, &ModeInfo);
        H = ModeInfo->HorizontalResolution;
        V = ModeInfo->VerticalResolution;
        if(((H == 1024) && (V == 768)) || ((H == 1440) && (V == 900)))
        {    
            ModeIndex = i;            
        }
    }

    Status = Gop->SetMode(Gop, ModeIndex);    

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to SetMode.\n");
        return Status;
    }
    Print(L"SUCCESS:SetMode to Index:%d.\n", ModeIndex);
    
    #endif
    
    return Status;
}


EFI_STATUS BmpTransform(
    IN EFI_PHYSICAL_ADDRESS BmpBase,
    OUT BMP_CONFIG *BmpConfig
)    
{  
    EFI_STATUS Status = EFI_SUCCESS;
    // Not used, just for example
    struct bmp_header *bheader = (struct bmp_header *)BmpBase;
    Print(L"File size = %08lx.\n", bheader->file_size);
    BmpConfig->Size = GetValue(BmpBase, 0x02, 4);
    BmpConfig->PageSize = (BmpConfig->Size >> 12) + 1;
    BmpConfig->Offset = GetValue(BmpBase, 0x0A, 4);
    
    BmpConfig->Width = GetValue(BmpBase, 0x12, 4);
    BmpConfig->Height = GetValue(BmpBase, 0x16, 4);
    
    EFI_PHYSICAL_ADDRESS PixelStart;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        BmpConfig->PageSize,
        &PixelStart
    );

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to AllocatePages for PixelArea.\n");
        return Status;
    }
    Print(L"SUCCESS:Memory for PixelArea is ready.\n");
    #endif

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelFromFile = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)(BmpBase 
                                                    + BmpConfig->Offset 
                                                    + BmpConfig->Width * BmpConfig->Height * 4);
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelToBuffer = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)PixelStart;
    for(UINTN i = 0; i < BmpConfig->Height; i++)
    {
        PixelFromFile -= BmpConfig->Width;
        for(UINTN j = 0; j < BmpConfig->Width; j++)
        {
            *PixelToBuffer = *PixelFromFile;
            PixelToBuffer++;
            PixelFromFile++;
        }
        PixelFromFile -= BmpConfig->Width;
    }

    BmpConfig->PixelStart = PixelStart;

    return Status;
}

EFI_STATUS DrawBmp(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
    IN BMP_CONFIG BmpConfig,
    IN UINTN X,
    IN UINTN Y
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    Status = Gop->Blt(
        Gop,
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)BmpConfig.PixelStart,
        EfiBltBufferToVideo,
        0,0,
        X,Y,
        BmpConfig.Width,BmpConfig.Height,0
    );

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to Blt Logo.bmp, code=%d.\n", Status);
        return Status;
    }
    Print(L"SUCCESS:You should see the Logo.\n");
    #endif
    return Status;
}
