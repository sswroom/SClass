#ifndef _SM_MEDIA_LPCMCONVERTER
#define _SM_MEDIA_LPCMCONVERTER

namespace Media
{
	class LPCMConverter
	{
	public:
		static UOSInt ChannelReduce(UInt16 bitPerSample, UInt16 srcNChannels, const UInt8 *srcBuff, UOSInt srcSize, UInt16 destNChannels, UInt8 *destBuff);
        static UOSInt Convert(UInt32 srcFormat, UInt16 srcBitPerSample, const UInt8 *srcBuff, UOSInt srcSize, UInt32 destFormat, UInt16 destBitPerSample, UInt8 *destBuff);
        static UOSInt ConvertF32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI32_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI24_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertU8_I16(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertF32_I24(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI32_I24(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI16_I24(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertU8_I24(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertF32_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI24_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertI16_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
        static UOSInt ConvertU8_I32(UInt8 *destBuff, const UInt8 *srcBuff, UOSInt srcSize);
	};
}
#endif
