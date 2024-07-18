#ifndef _SM_TEXT_SMSUTIL
#define _SM_TEXT_SMSUTIL

namespace Text
{
	class SMSUtil
	{
	public:
		static UTF16Char gsm2text[];
		static UTF16Char gsm1b2text[];
		static UInt16 text2gsm[];

		typedef enum
		{
			DCS_GSM7BIT = 0,
			DCS_UCS2 = 8
		} DCS;

	public:
		static OSInt ToGSMBytes(UnsafeArray<const UTF16Char> msg, UnsafeArray<UInt8> buff, OutParam<DCS> dcs, OutParam<UInt32> dataLen);
		static OSInt ToUCS2Bytes(UnsafeArray<const UTF16Char> msg, UnsafeArray<UInt8> buff);
		static void GetTextInfo(UnsafeArray<const UTF16Char> msg, OutParam<DCS> dcs, OutParam<UInt32> dataLen);
		static UInt32 GSMTextSize2DataSize(UInt32 textSize);
		static Bool TrimGSMText(UnsafeArray<UTF16Char> msg);
		static UnsafeArrayOpt<UTF16Char> TrimGSMText(UnsafeArray<UTF16Char> destBuff, UnsafeArray<const UTF16Char> msg, UInt32 dataLen);
		static Bool IsPhone(UnsafeArray<const UTF16Char> txt);
		static Bool IsIntlPhone(UnsafeArray<const UTF8Char> txt);
		static Bool IsIntlPhone(UnsafeArray<const UTF16Char> txt);
	};
}
#endif
