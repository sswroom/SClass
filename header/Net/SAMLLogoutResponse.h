#ifndef _SM_NET_SAMLLOGOUTRESPONSE
#define _SM_NET_SAMLLOGOUTRESPONSE
#include "Data/Timestamp.h"
#include "Net/SAMLStatusCode.h"
#include "Text/String.h"

namespace Net
{
	class SAMLLogoutResponse
	{
	public:
		enum class ProcessError
		{
			Success,
			IDPMissing,
			SSLMissing,
			ParamMissing,
			SigAlgNotSupported,
			QueryStringError,
			KeyError,
			SignatureInvalid,
			MessageInvalid,
			StatusError
		};
	private:
		ProcessError error;
		NN<Text::String> errorMessage;
		SAMLStatusCode status;
		Optional<Text::String> rawResponse;

	public:
		SAMLLogoutResponse(ProcessError error, Text::CStringNN errorMessage);
		~SAMLLogoutResponse();

		ProcessError GetError() const { return this->error; }
		void SetError(ProcessError error) { this->error = error; }
		NN<Text::String> GetErrorMessage() const { return this->errorMessage; }
		void SetErrorMessage(Text::CStringNN errorMessage) { this->errorMessage->Release(); this->errorMessage = Text::String::New(errorMessage); }
		SAMLStatusCode GetStatus() const { return this->status; }
		void SetStatus(SAMLStatusCode status) { this->status = status; }
		Optional<Text::String> GetRawResponse() const { return this->rawResponse; }
		void SetRawResponse(Text::CString rawResponse) { OPTSTR_DEL(this->rawResponse); this->rawResponse = Text::String::NewOrNull(rawResponse); }

		static Text::CStringNN ProcessErrorGetName(ProcessError val);
	};
}
#endif
