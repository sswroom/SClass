#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"

extern "C" void AudioUtil_Amplify8(UInt8 *audioBuff, OSInt buffSize, Int32 vol)
{
	Int32 v;
	while (buffSize-- > 0)
	{
		v = *audioBuff;
		v = 128 + (((v - 128) * vol) >> 16);
		if (v < 0)
			*audioBuff = 0;
		else if (v > 255)
			*audioBuff = 255;
		else
			*audioBuff = (UInt8)v;
		audioBuff++;
	}
}

extern "C" void AudioUtil_Amplify16(UInt8 *audioBuff, OSInt buffSize, Int32 vol)
{
	Int64 v;
	while (buffSize > 0)
	{
		v = (ReadInt16(audioBuff) * (Int64)vol) >> 16;
		if (v < -32768)
			WriteInt16(audioBuff, -32768);
		else if (v > 32767)
			WriteInt16(audioBuff, 32767);
		else
			WriteInt16(audioBuff, v);
		audioBuff += 2;
		buffSize -= 2;
	}
}
