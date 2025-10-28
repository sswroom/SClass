#include "Stdafx.h"
#include "SIMD.h"
#include "Data/ByteTool.h"
#include "Math/Math_C.h"
#include "Media/LPCMConverter.h"

UOSInt Media::LPCMConverter::ChannelReduce(UInt16 bitPerSample, UInt16 srcNChannels, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize, UInt16 destNChannels, UnsafeArray<UInt8> destBuff)
{
	if (srcNChannels < destNChannels)
	{
		return 0;
	}
	else if (srcNChannels == destNChannels)
	{
		if (srcBuff != destBuff)
		{
			MemCopyNO(&destBuff[0], &srcBuff[0], srcSize);			
		}
		return srcSize;
	}
	UOSInt copySize = (UOSInt)(bitPerSample >> 3) * (UOSInt)destNChannels;
	UOSInt blockSize = (UOSInt)(bitPerSample >> 3) * (UOSInt)srcNChannels;
	UnsafeArray<const UInt8> srcEnd = srcBuff + srcSize;
	switch (blockSize)
	{
	case 4:
		while (srcBuff < srcEnd)
		{
			WriteNInt32(&destBuff[0], ReadNInt32(&srcBuff[0]));
			destBuff += copySize;
			srcBuff += blockSize;
		}
		break;
	case 8:
		while (srcBuff < srcEnd)
		{
			WriteNInt64(&destBuff[0], ReadNInt64(&srcBuff[0]));
			destBuff += copySize;
			srcBuff += blockSize;
		}
		break;
	default:
		while (srcBuff < srcEnd)
		{
			MemCopyNO(&destBuff[0], &srcBuff[0], copySize);
			destBuff += copySize;
			srcBuff += blockSize;
		}
		break;
	}
	return srcSize / blockSize * copySize;
}

UOSInt Media::LPCMConverter::Convert(UInt32 srcFormat, UInt16 srcBitPerSample, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize, UInt32 destFormat, UInt16 destBitPerSample, UnsafeArray<UInt8> destBuff)
{
    if (destFormat == 1 && destBitPerSample == 16)
    {
        if (srcFormat == 3)
        {
            if (srcBitPerSample == 32)
            {
                return ConvertF32_I16(destBuff, srcBuff, srcSize);
            }
        }
        else if (srcFormat == 1)
        {
            if (srcBitPerSample == 32)
            {
                return ConvertI32_I16(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 24)
            {
                return ConvertI24_I16(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 8)
            {
                return ConvertU8_I16(destBuff, srcBuff, srcSize);
            }
        }
    }
	else if (destFormat == 1 && destBitPerSample == 24)
	{
        if (srcFormat == 3)
        {
            if (srcBitPerSample == 32)
            {
                return ConvertF32_I24(destBuff, srcBuff, srcSize);
            }
        }
        else if (srcFormat == 1)
        {
            if (srcBitPerSample == 32)
            {
                return ConvertI32_I24(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 16)
            {
                return ConvertI16_I24(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 8)
            {
                return ConvertU8_I24(destBuff, srcBuff, srcSize);
            }
        }
	}
	else if (destFormat == 1 && destBitPerSample == 32)
	{
        if (srcFormat == 3)
        {
            if (srcBitPerSample == 32)
            {
                return ConvertF32_I32(destBuff, srcBuff, srcSize);
            }
        }
        else if (srcFormat == 1)
        {
            if (srcBitPerSample == 24)
            {
                return ConvertI24_I32(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 16)
            {
                return ConvertI16_I32(destBuff, srcBuff, srcSize);
            }
            else if (srcBitPerSample == 8)
            {
                return ConvertU8_I32(destBuff, srcBuff, srcSize);
            }
        }
	}
    MemCopyNO(&destBuff[0], &srcBuff[0], srcSize);
    return srcSize;
}

UOSInt Media::LPCMConverter::ConvertF32_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
    while (i-- > 0)
    {
        Int32 v = Double2Int32(ReadFloat(&srcBuff[0]) * 32767.0);
        Int16 v16 = SI32ToI16(v);
        WriteInt16(&destBuff[0], v16);

        srcBuff += 4;
        destBuff += 2;
    }
    return srcSize / 2;
}

UOSInt Media::LPCMConverter::ConvertI32_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[2];
        destBuff[1] = srcBuff[3];

        srcBuff += 4;
        destBuff += 2;
    }
    return srcSize / 2;
}

UOSInt Media::LPCMConverter::ConvertI24_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 3;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[1];
        destBuff[1] = srcBuff[2];

        srcBuff += 3;
        destBuff += 2;
    }
    return srcSize / 3 * 2;
}

UOSInt Media::LPCMConverter::ConvertU8_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UInt8 v;
    UOSInt i = srcSize;
    while (i-- > 0)
    {
        v = (UInt8)(srcBuff[0] - 0x80);
        destBuff[0] = v;
        destBuff[1] = v;

        srcBuff++;
        destBuff += 2;
    }
    return srcSize * 2;
}

UOSInt Media::LPCMConverter::ConvertF32_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
    while (i-- > 0)
    {
		Double v = ReadFloat(&srcBuff[0]) * 8388607.0;
		Int32 v32 = Math::SDouble2Int24(v);
        WriteInt32(&destBuff[0], v32);

        srcBuff += 4;
        destBuff += 4;
    }
    return srcSize / 4 * 3;
}

UOSInt Media::LPCMConverter::ConvertI32_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[1];
        destBuff[1] = srcBuff[2];
        destBuff[2] = srcBuff[3];

        srcBuff += 4;
        destBuff += 3;
    }
    return srcSize / 4 * 3;
}

UOSInt Media::LPCMConverter::ConvertI16_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 2;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[1];
        destBuff[1] = srcBuff[0];
        destBuff[2] = srcBuff[1];

        srcBuff += 2;
        destBuff += 3;
    }
    return srcSize / 2 * 3;
}

UOSInt Media::LPCMConverter::ConvertU8_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UInt8 v;
    UOSInt i = srcSize;
    while (i-- > 0)
    {
        v = (UInt8)(srcBuff[0] - 0x80);
        destBuff[0] = v;
        destBuff[1] = v;
        destBuff[2] = v;

        srcBuff++;
        destBuff += 3;
    }
    return srcSize * 3;
}

UOSInt Media::LPCMConverter::ConvertF32_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
	while (i-- > 0)
    {
		Double v = ReadFloat(&srcBuff[0]) * 2147483647.0;
		Int32 v32 = Math::SDouble2Int32(v);
        WriteInt32(&destBuff[0], v32);

        srcBuff += 4;
        destBuff += 4;
    }
    return srcSize;
}

UOSInt Media::LPCMConverter::ConvertI24_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 3;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[2];
        destBuff[1] = srcBuff[0];
        destBuff[2] = srcBuff[1];
        destBuff[3] = srcBuff[2];

        srcBuff += 3;
        destBuff += 4;
    }
    return srcSize / 3 * 4;
}

UOSInt Media::LPCMConverter::ConvertI16_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 2;
    while (i-- > 0)
    {
        destBuff[0] = srcBuff[0];
        destBuff[1] = srcBuff[1];
        destBuff[2] = srcBuff[0];
        destBuff[3] = srcBuff[1];

        srcBuff += 2;
        destBuff += 4;
    }
    return srcSize * 2;
}

UOSInt Media::LPCMConverter::ConvertU8_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UOSInt srcSize)
{
    UInt8 v;
    UOSInt i = srcSize;
    while (i-- > 0)
    {
        v = (UInt8)(srcBuff[0] - 0x80);
        destBuff[0] = v;
        destBuff[1] = v;
        destBuff[2] = v;
        destBuff[3] = v;

        srcBuff++;
        destBuff += 4;
    }
    return srcSize * 4;
}
