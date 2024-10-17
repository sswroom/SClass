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
		Optional<IO::LogTool> log;
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<Text::EncodingFactory> encFact;
		Optional<Text::String> userName;
		Optional<Text::String> password;
	public:
		SMSGatewayIntersoft(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<Text::EncodingFactory> encFact, Optional<IO::LogTool> log);
		virtual ~SMSGatewayIntersoft();

		virtual Bool IsTargetValid(Text::CStringNN targetNum);
		virtual Bool SendSMS(Text::CStringNN targetNum, Text::CStringNN msg);

		void SetAccount(Text::CStringNN userName, Text::CStringNN password);
		Bool SendSMS(Text::CStringNN userName, Text::CStringNN password, Text::CStringNN targetNum, Text::CStringNN msg);
	};
}
#endif
