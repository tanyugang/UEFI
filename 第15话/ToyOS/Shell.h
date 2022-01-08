#include "Video.h"

#pragma pack(1)

typedef struct
{
    UINT32 Width;
    UINT32 Height;
    UINT32 X;
    UINT32 Y;
    UINT32 OutLines;
} SETUP;


typedef struct
{
    SETUP Setup;
} BASE;
#pragma pack()

int ShellInitial();

