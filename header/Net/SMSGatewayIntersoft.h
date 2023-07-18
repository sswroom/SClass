#ifndef _SM_NET_SMSGATEWAYINTERSOFT
#define _SM_NET_SMSGATEWAYINTERSOFT
#include "IO/LogTool.h"
#include "IO/SMSGateway.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"

namespace Net
{
	class SMSGatewayIntersoft : public IO::SMSGateway
	{
	private:
		IO::LogTool *log;
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		Text::EncodingFactory *encFact;
		Text::String *userName;
		Text::String *password;
	public:
		SMSGatewayIntersoft(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, IO::LogTool *log);
		virtual ~SMSGatewayIntersoft();

		virtual Bool IsTargetValid(Text::CString targetNum);
		virtual Bool SendSMS(Text::CString targetNum, Text::CString msg);

		void SetAccount(Text::CString userName, Text::CString password);
		Bool SendSMS(Text::CString userName, Text::CString password, Text::CString targetNum, Text::CString msg);
	};
}
#endif
