#ifndef _SM_TEXT_SMSMESSAGE
#define _SM_TEXT_SMSMESSAGE
#include "Text/SMSUserData.h"
#include "Data/DateTime.h"

namespace Text
{
	class SMSMessage
	{
	private:
		const UTF16Char *smsc;
		const UTF16Char *address;
		Optional<SMSUserData> ud;
		Data::DateTime msgTime;
		Bool mms;
		Bool replyPath;
		Bool statusReport;
		Bool rejectDup;
		UInt8 msgRef;

	private:
		static UTF16Char *ParsePDUPhone(UTF16Char *buff, const UInt8 *pduPhone, UInt8 phoneByteLen);
		static UInt8 ParseIBCD(UInt8 byte);
		static void ParseTimestamp(const UInt8 *buff, NN<Data::DateTime> time);
		static UInt8 *ToPDUPhone(UInt8 *buff, const UTF16Char *phoneNum, UInt8 *byteSize, UInt8 *phoneSize);
	public:
		SMSMessage(const UTF16Char *address, const UTF16Char *smsc, Optional<SMSUserData> ud);
		~SMSMessage();

		void SetMessageTime(NN<Data::DateTime> msgTime);
		void SetMoreMsgToSend(Bool mms);
		void SetReplyPath(Bool replyPath);
		void SetStatusReport(Bool statusReport);
		void SetRejectDuplicates(Bool rejectDup);
		void SetMessageRef(UInt8 msgRef);
		void GetMessageTime(NN<Data::DateTime> msgTime);
		const UTF16Char *GetAddress();
		const UTF16Char *GetSMSC();
		const UTF16Char *GetContent();

		UOSInt ToSubmitPDU(UInt8 *buff);

		static Text::SMSMessage *CreateFromPDU(const UInt8 *pduBytes);
	};
}
#endif
