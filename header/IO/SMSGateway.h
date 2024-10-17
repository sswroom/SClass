#ifndef _SM_IO_SMSGATEWAY
#define _SM_IO_SMSGATEWAY
#include "Text/CString.h"

namespace IO
{
	class SMSGateway
	{
	public:
		virtual ~SMSGateway() {};

		virtual Bool IsTargetValid(Text::CStringNN targetNum) = 0;
		virtual Bool SendSMS(Text::CStringNN targetNum, Text::CStringNN msg) = 0;
	};
}
#endif
