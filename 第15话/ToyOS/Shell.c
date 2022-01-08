#include "Shell.h"
BASE Base;
char *Hello;

int ShellInitial()
{
    Base.Setup.Height = 860;
    Base.Setup.Width = 1400;
    Base.Setup.X = 20;
    Base.Setup.Y = 20;
    Base.Setup.OutLines = 1;

    // Draw  Background
    BLOCK BackGround;
    BackGround.Color = 0xFF000000;
    BackGround.Start.X =  20;
    BackGround.Start.Y = 20;
    BackGround.End.X = 1420;
    BackGround.End.Y = 880;
    DrawBlock(BackGround);


    if(Base.Setup.OutLines)
    {    
        // Draw OutLine Up
        BLOCK OutLine;
        OutLine.Color = 0xFFFFFFFF;
        OutLine.Start.X =  20;
        OutLine.Start.Y = 20;
        OutLine.End.X = 1420;
        OutLine.End.Y = 22;
        DrawBlock(OutLine);
        
        // Draw OutLine Middle
        OutLine.Start.X =  22;
        OutLine.Start.Y = 68;
        OutLine.End.X = 1418;
        OutLine.End.Y = 70;
        DrawBlock(OutLine);

        // Draw OutLine Down
        OutLine.Start.X =  20;
        OutLine.Start.Y = 878;
        OutLine.End.X = 1420;
        OutLine.End.Y = 880;
        DrawBlock(OutLine);
    
        // Draw OutLine Left
        OutLine.Start.X =  20;
        OutLine.Start.Y = 22;
        OutLine.End.X = 22;
        OutLine.End.Y = 878;
        DrawBlock(OutLine);

        // Draw OutLine right
        OutLine.Start.X =  1418;
        OutLine.Start.Y = 22;
        OutLine.End.X = 1420;
        OutLine.End.Y = 878;
        DrawBlock(OutLine);
    }
    
    return 0;
}
