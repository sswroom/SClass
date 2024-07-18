#ifndef _SM_TEXT_SMSUSERDATA
#define _SM_TEXT_SMSUSERDATA
#include "Data/ArrayListNN.h"
#include "Text/SMSUtil.h"

namespace Text
{
	class SMSUserData
	{
	private:
		UnsafeArray<const UTF16Char> msg;
		Text::SMSUtil::DCS dcs;
		UnsafeArrayOpt<UInt8> udh;

	private:
		SMSUserData(UnsafeArray<const UTF16Char> msg, Text::SMSUtil::DCS dcs, UnsafeArrayOpt<const UInt8> udh);

	public:
		~SMSUserData();
		Bool HasUDH();
		Text::SMSUtil::DCS GetDCS();
		UInt32 GetByteSize(); //Including UDL
		UInt32 GetBytes(UnsafeArray<UInt8> bytes);
		UnsafeArrayOpt<const UInt8> GetUDH();
		UnsafeArray<const UTF16Char> GetMessage();

		static UOSInt CreateSMSs(NN<Data::ArrayListNN<Text::SMSUserData>> smsList, UnsafeArray<const UTF8Char> smsMessage);
		static Optional<Text::SMSUserData> CreateSMSTrim(UnsafeArray<const UTF16Char> smsMessage, UInt8 *udh);
		static Optional<Text::SMSUserData> CreateSMSFromBytes(UnsafeArray<const UInt8> bytes, Bool hasUDH, Text::SMSUtil::DCS dcs);
	};
};
#endif
