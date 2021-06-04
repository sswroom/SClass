#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/ImageFilter/BWImgFilter.h"

void BWImgFilter_ProcYUY2(UInt8 *imgData, UOSInt pxCnt)
{
	while (pxCnt-- > 0)
	{
		imgData[1] = 0x80;
		imgData += 2;
	}
}

void BWImgFilter_ProcUYVY(UInt8 *imgData, UOSInt pxCnt)
{
	while (pxCnt-- > 0)
	{
		imgData[0] = 0x80;
		imgData += 2;
	}
}

void BWImgFilter_ProcAYUV(UInt8 *imgData, UOSInt pxCnt)
{
	while (pxCnt-- > 0)
	{
		WriteInt16(&imgData[0], 0x8080);
		imgData += 4;
	}
}

void BWImgFilter_ProcY416(UInt8 *imgData, UOSInt pxCnt)
{
	while (pxCnt-- > 0)
	{
		WriteUInt32(&imgData[0], 0x80008000);
		imgData += 8;
	}
}
