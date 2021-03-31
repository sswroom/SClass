#include "Stdafx.h"
#include "SIMD.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/LPCMConverter.h"

UOSInt Media::LPCMConverter::Convert(UInt32 srcFormat, UInt16 srcBitPerSample, const UInt8 *srcBuff, UOSInt srcSize, UInt32 destFormat, UInt16 destBitPerSample, UInt8 *destBuff)
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
    MemCopyNO(destBuff, srcBuff, srcSize);
    return srcSize;
}

UOSInt Media::LPCMConverter::ConvertF32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize)
{
    UOSInt i = srcSize / 4;
    while (i-- > 0)
    {
        Int32 v = Math::Double2Int32(ReadFloat(srcBuff) * 32767.0);
        Int16 v16 = SI32ToI16(v);
        WriteInt16(destBuff, v16);

        srcBuff += 4;
        destBuff += 2;
    }
    return srcSize / 2;
}

UOSInt Media::LPCMConverter::ConvertI32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize)
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

UOSInt Media::LPCMConverter::ConvertI24_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize)
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

UOSInt Media::LPCMConverter::ConvertU8_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize)
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
