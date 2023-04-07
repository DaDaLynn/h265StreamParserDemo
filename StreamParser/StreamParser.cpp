#define _CRT_SECURE_NO_WARNINGS 1 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

#include "h265StreamParser.h"

int main(int argc, char* argv[])
{
 
    const char* _file = "1920x1080_yuv420p_test0.265";
    int streamLen = initStream(_file);
    if (streamLen == -1)
    {
        printf("initStream file[%s] error!\n", _file);
        return -1;
    }

    FILE* pOutFile = fopen("out.265", "wb");
    int Pos = 0;
    int onePicLen = 0;
    while (onePicLen = nextNalLength(Pos))
    {
        std::vector<unsigned char> pTmpPic;
        pTmpPic.resize(onePicLen);
        readOneNal(&pTmpPic[0], Pos, onePicLen);
        fwrite(&pTmpPic[0], 1, onePicLen, pOutFile);
        Pos += onePicLen;
    }
    fclose(pOutFile);
    return 0;
}