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
		static OSInt ToGSMBytes(const UTF16Char *msg, UInt8 *buff, DCS *dcs, Int32 *dataLen);
		static OSInt ToUCS2Bytes(const UTF16Char *msg, UInt8 *buff);
		static void GetTextInfo(const UTF16Char *msg, DCS *dcs, Int32 *dataLen);
		static Int32 GSMTextSize2DataSize(Int32 textSize);
		static Bool TrimGSMText(UTF16Char *msg);
		static UTF16Char *TrimGSMText(UTF16Char *destBuff, const UTF16Char *msg, Int32 dataLen);
		static Bool IsPhone(const UTF16Char *txt);
		static Bool IsIntlPhone(const UTF8Char *txt);
		static Bool IsIntlPhone(const UTF16Char *txt);
	};
};
#endif
