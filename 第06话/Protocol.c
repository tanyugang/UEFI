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

    return Status;
}