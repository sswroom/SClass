#ifndef _SM_MEDIA_LPCMCONVERTER
#define _SM_MEDIA_LPCMCONVERTER

namespace Media
{
	class LPCMConverter
	{
	public:
        static UOSInt Convert(UInt32 srcFormat, UInt16 srcBitPerSample, const UInt8 *srcBuff, UOSInt srcSize, UInt32 destFormat, UInt16 destBitPerSample, UInt8 *destBuff);
        static UOSInt ConvertF32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI24_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertU8_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertF32_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI24_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI16_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertU8_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
	};
}
#endif
