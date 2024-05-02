#ifndef _SM_TEXT_SMSUSERDATA
#define _SM_TEXT_SMSUSERDATA
#include "Data/ArrayListNN.h"
#include "Text/SMSUtil.h"

namespace Text
{
	class SMSUserData
	{
	private:
		const UTF16Char *msg;
		Text::SMSUtil::DCS dcs;
		UInt8 *udh;

	private:
		SMSUserData(const UTF16Char *msg, Text::SMSUtil::DCS dcs, const UInt8 *udh);

	public:
		~SMSUserData();
		Bool HasUDH();
		Text::SMSUtil::DCS GetDCS();
		UInt32 GetByteSize(); //Including UDL
		UInt32 GetBytes(UInt8 *bytes);
		const UInt8 *GetUDH();
		const UTF16Char *GetMessage();

		static UOSInt CreateSMSs(NN<Data::ArrayListNN<Text::SMSUserData>> smsList, const UTF8Char *smsMessage);
		static Optional<Text::SMSUserData> CreateSMSTrim(const UTF16Char *smsMessage, UInt8 *udh);
		static Optional<Text::SMSUserData> CreateSMSFromBytes(const UInt8 *bytes, Bool hasUDH, Text::SMSUtil::DCS dcs);
	};
};
#endif
