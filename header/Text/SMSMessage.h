#ifndef _SM_TEXT_SMSMESSAGE
#define _SM_TEXT_SMSMESSAGE
#include "Text/SMSUserData.h"
#include "Data/DateTime.h"

namespace Text
{
	class SMSMessage
	{
	private:
		UnsafeArrayOpt<const UTF16Char> smsc;
		UnsafeArrayOpt<const UTF16Char> address;
		Optional<SMSUserData> ud;
		Data::DateTime msgTime;
		Bool mms;
		Bool replyPath;
		Bool statusReport;
		Bool rejectDup;
		UInt8 msgRef;

	private:
		static UnsafeArray<UTF16Char> ParsePDUPhone(UnsafeArray<UTF16Char> buff, UnsafeArray<const UInt8> pduPhone, UInt8 phoneByteLen);
		static UInt8 ParseIBCD(UInt8 byte);
		static void ParseTimestamp(const UInt8 *buff, NN<Data::DateTime> time);
		static UnsafeArray<UInt8> ToPDUPhone(UnsafeArray<UInt8> buff, UnsafeArray<const UTF16Char> phoneNum, OptOut<UInt8> byteSize, OptOut<UInt8> phoneSize);
	public:
		SMSMessage(UnsafeArray<const UTF16Char> address, UnsafeArrayOpt<const UTF16Char> smsc, Optional<SMSUserData> ud);
		~SMSMessage();

		void SetMessageTime(NN<Data::DateTime> msgTime);
		void SetMoreMsgToSend(Bool mms);
		void SetReplyPath(Bool replyPath);
		void SetStatusReport(Bool statusReport);
		void SetRejectDuplicates(Bool rejectDup);
		void SetMessageRef(UInt8 msgRef);
		void GetMessageTime(NN<Data::DateTime> msgTime);
		UnsafeArrayOpt<const UTF16Char> GetAddress();
		UnsafeArrayOpt<const UTF16Char> GetSMSC();
		UnsafeArrayOpt<const UTF16Char> GetContent();

		UOSInt ToSubmitPDU(UnsafeArray<UInt8> buff);

		static Optional<Text::SMSMessage> CreateFromPDU(const UInt8 *pduBytes);
	};
}
#endif
