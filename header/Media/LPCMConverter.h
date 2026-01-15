#ifndef _SM_MEDIA_LPCMCONVERTER
#define _SM_MEDIA_LPCMCONVERTER

namespace Media
{
	class LPCMConverter
	{
	public:
		static UIntOS ChannelReduce(UInt16 bitPerSample, UInt16 srcNChannels, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize, UInt16 destNChannels, UnsafeArray<UInt8> destBuff);
        static UIntOS Convert(UInt32 srcFormat, UInt16 srcBitPerSample, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize, UInt32 destFormat, UInt16 destBitPerSample, UnsafeArray<UInt8> destBuff);
        static UIntOS ConvertF32_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI32_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI24_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertU8_I16(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertF32_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI32_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI16_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertU8_I24(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertF32_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI24_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertI16_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
        static UIntOS ConvertU8_I32(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> srcBuff, UIntOS srcSize);
	};
}
#endif
