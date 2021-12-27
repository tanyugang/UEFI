#include "Log.h"

EFI_FILE_PROTOCOL *LogFile;

EFI_STATUS LogError(EFI_STATUS Code)
{
    EFI_STATUS Status = EFI_SUCCESS;
    switch (Code)
    {
    case EFI_INVALID_PARAMETER:
        Status = LogTip("ERROR:EFI_INVALID_PARAMETER.\n");
        break;
    case EFI_NOT_FOUND:
        Status = LogTip("ERROR:EFI_NOT_FOUND.\n");
        break;
    case EFI_OUT_OF_RESOURCES:
        Status = LogTip("ERROR:EFI_OUT_OF_RESOURCES.\n");
        break;
    default:
        Status = LogTip("ERROR:Something must be wrong.\n");
        break;
    }

    return Status;
}

EFI_STATUS LogInitial(EFI_HANDLE ImageHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    UINTN HandleCount = 0;
    EFI_HANDLE *Buffer = NULL;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &Buffer
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to LocateHanleBuffer of SimpleFileSystemProtocol.\n");
        return Status;
    }
    Print(L"SUCCESS:Get %d handles that supported SimpleFileSystemProtocol.\n", HandleCount);
    #endif
    
    Status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open first handle that supported SimpleFileSystemProtocol.\n");
        return Status;
    }
    Print(L"SUCCESS:SimpleFileSystemProtocol is opened with first handle.\n");
    #endif
    EFI_FILE_PROTOCOL *File = NULL;
    Status = FileSystem->OpenVolume(
        FileSystem,
        &File
    );
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open volume.\n");
        return Status;
    }

    Print(L"SUCCESS:Volume is opened.\n");
    #endif

    File->Open(
        File,
        &LogFile,
        L"\\Log.txt",
        EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE,
        EFI_FILE_ARCHIVE
    );

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to open file");
        return Status;
    }

    Print(L"SUCCESS:File is opened.\n");
    #endif

    return Status;
}

EFI_STATUS LogTip(CHAR8 *Message)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN MsgLen = AsciiStrLen(Message);
    Status = LogFile->Write(LogFile, &MsgLen, Message);

    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to write LogFile.\n");
        return Status;
    }
    #endif

    return Status;
}

EFI_STATUS Close()
{
    EFI_STATUS Status = EFI_SUCCESS;
    Status = LogFile->Close(LogFile);
    #ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"ERROR:Failed to Close LogFile.\n");
        return Status;
    }
    Print(L"SUCCESS:LogFile is closed.\n");
    #endif
    return Status;
}