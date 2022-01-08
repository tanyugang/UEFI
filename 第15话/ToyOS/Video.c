#include "Video.h"

VIDEO_CONFIG *VideoConfig;
UINT32 *VideoStart;

int VideoInitial(BOOT_CONFIG *BootConfig)
{

    VideoConfig = &BootConfig->VideoConfig;
    VideoStart = (UINT32 *)BootConfig->VideoConfig.FrameBufferBase;

    BLOCK BackGround;

    BackGround.Start.X = 0;
    BackGround.Start.Y = 0;
    BackGround.End.X = BootConfig->VideoConfig.HorizontalResolution - 1;
    BackGround.End.Y = BootConfig->VideoConfig.VerticalResolution - 1;
    BackGround.Color = 0xFF18679A;
    DrawBlock(BackGround);
    return 0;
}

int DrawPixel(PIXEL Pixel)
{
    UINT32 *Position = VideoStart
                       + (Pixel.Y - 1) * VideoConfig->PixelsPerScanLine
                       + Pixel.X;   
    *Position = Pixel.Color;
     return 0;
}

int DrawBlock(BLOCK Block)
{
    PIXEL Pixel;
    Pixel.Color = Block.Color;
    for(int i = Block.Start.Y; i < Block.End.Y; i++)
    {
        for(int j = Block.Start.X; j < Block.End.X; j++)
        {
            Pixel.X = j;
            Pixel.Y = i;
            DrawPixel(Pixel);
        }
    }
    return 0;
}
