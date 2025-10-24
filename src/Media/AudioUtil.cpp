#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math_C.h"

extern "C" void AudioUtil_Amplify8(UInt8 *audioBuff, UOSInt buffSize, Int32 vol)
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

extern "C" void AudioUtil_Amplify16(UInt8 *audioBuff, UOSInt buffSize, Int32 vol)
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

extern "C" void AudioUtil_ConvI16_F32(const UInt8 *srcBuff, UInt8 *destBuff, UOSInt nSample)
{
	Double mul = 1 / 32768.0;
	while (nSample-- > 0)
	{
		WriteFloat(destBuff, (Single)(ReadInt16(srcBuff) * mul));
		srcBuff += 2;
		destBuff += 4;
	}
}

extern "C" void AudioUtil_ConvF32_I16(const UInt8 *srcBuff, UInt8 *destBuff, UOSInt nSample)
{
	while (nSample-- > 0)
	{
		WriteInt16(destBuff, Math::SDouble2Int16(ReadFloat(srcBuff) * 32767.0));
		srcBuff += 4;
		destBuff += 2;
	}
}
