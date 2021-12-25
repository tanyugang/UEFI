#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN NoHandles = 0;
    EFI_HANDLE *Buffer = NULL;

    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL, 
        &NoHandles,
        &Buffer
    );

    Print(L"Status = %d.\n", Status);

    if(EFI_ERROR(Status))
    {
        Print(L"Failed to LoacateHandleBuffer.\n");
        return Status;
    }
    Print(L"Hello, Protocol!\n");
    Print(L"NoHandles = %d.\n", NoHandles);
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
    Status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&Gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    Print(L"Status = %d.\n", Status);

    if(EFI_ERROR(Status))
    {
        Print(L"Failed to OpenProtocol.\n");
        return Status;
    }

    UINTN SizeOfInfo = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
    for(UINTN i = 0; i < Gop->Mode->MaxMode; i++)
    {
        Status = Gop->QueryMode(
            Gop,
            i,
            &SizeOfInfo,
            &Info
        );
        if(EFI_ERROR(Status))
        {
            Print(L"Failed to Querymode.\n");
            return Status;
        }
        Print(L"Mode %d, H = %d, V = %d.\n",
        i,
        Info->HorizontalResolution,
        Info->VerticalResolution);
    }

    Status = Gop->SetMode(Gop, 22);
    if(EFI_ERROR(Status))
    {
        Print(L"Failed to Setmode.\n");
        return Status;
    }

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Red = {0, 0, 255, 0};

    Gop->Blt(
        Gop,
        &Red,
        EfiBltVideoFill,
        0,0,
        0,0,
        100,100,
        0
    );

    if(EFI_ERROR(Status))
    {
        Print(L"Failed to Blt.\n");
        return Status;
    }

    Gop->Blt(
        Gop,
        NULL,
        EfiBltVideoToVideo,
        0,0,
        200,200,
        100,100,
        0
    );
    if(EFI_ERROR(Status))
    {
        Print(L"Failed to Blt.\n");
        return Status;
    }

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL Blue = {255, 0, 0, 0};

    EFI_PHYSICAL_ADDRESS BlueStart = Gop->Mode->FrameBufferBase
                                   + Gop->Mode->Info->PixelsPerScanLine * 100 * 4;

    EFI_GRAPHICS_OUTPUT_BLT_PIXEL *VideoHandle = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL*)BlueStart;

    UINTN NoPixels = Gop->Mode->Info->PixelsPerScanLine * 100;

    for(UINTN i = 0; i < NoPixels; i++)
    {
        *VideoHandle = Blue;
        VideoHandle++;
    }
    return Status;
}