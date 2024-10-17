#ifndef _SM_NET_SMSGATEWAYSMARTONE
#define _SM_NET_SMSGATEWAYSMARTONE
#include "IO/LogTool.h"
#include "IO/SMSGateway.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/String.h"

namespace Net
{

	class SMSGatewaySmartone : public IO::SMSGateway
	{
	public:
		enum class RequestStatus
		{
			Success,
			MissingParam,
			IPNotAllow,
			InvalidParam,
			SendError,
			UEMOOptOutNotAllowed,
			UniqueMessageDuplicated,
			ServerError,
			UserMessageIDError,
			MessageQueueLimitExceed,
			RequestsPerSecondExceed,
			NetworkError
		};
	private:
		Optional<IO::LogTool> log;
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> fqdn;
		NN<Text::String> accountId;
		NN<Text::String> password;
		RequestStatus lastReqError;
	public:
		SMSGatewaySmartone(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<IO::LogTool> log, Text::CStringNN fqdn, Text::CStringNN accountId, Text::CStringNN password);
		virtual ~SMSGatewaySmartone();

		virtual Bool IsTargetValid(Text::CStringNN targetNum);
		virtual Bool SendSMS(Text::CStringNN targetNum, Text::CStringNN msg);

		RequestStatus GetLastReqError() const;
		static Text::CStringNN RequestStatusGetName(RequestStatus status);
	};
}
#endif
