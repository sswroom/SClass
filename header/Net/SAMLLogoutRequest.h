#ifndef _SM_NET_SAMLLOGOUTREQUEST
#define _SM_NET_SAMLLOGOUTREQUEST
#include "Data/ArrayListStringNN.h"
#include "Data/Timestamp.h"
#include "Net/SAMLStatusCode.h"
#include "Text/String.h"

namespace Net
{
	class SAMLLogoutRequest
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
			DestinationInvalid,
			IssuerInvalid,
			TimeInvalid
		};
	private:
		ProcessError error;
		NN<Text::String> errorMessage;
		Optional<Text::String> rawResponse;
		Optional<Text::String> id;
		Optional<Text::String> nameId;
		Data::ArrayListStringNN sessionIndex;

	public:
		SAMLLogoutRequest(ProcessError error, Text::CStringNN errorMessage);
		~SAMLLogoutRequest();

		ProcessError GetError() const { return this->error; }
		void SetError(ProcessError error) { this->error = error; }
		NN<Text::String> GetErrorMessage() const { return this->errorMessage; }
		void SetErrorMessage(Text::CStringNN errorMessage) { this->errorMessage->Release(); this->errorMessage = Text::String::New(errorMessage); }
		Optional<Text::String> GetRawResponse() const { return this->rawResponse; }
		void SetRawResponse(Text::CString rawResponse) { OPTSTR_DEL(this->rawResponse); this->rawResponse = Text::String::NewOrNull(rawResponse); }
		Optional<Text::String> GetID() const { return this->id; }
		void SetID(Text::CString id) { OPTSTR_DEL(this->id); this->id = Text::String::NewOrNull(id); }
		Optional<Text::String> GetNameID() const { return this->nameId; }
		void SetNameID(Text::CString nameId) { OPTSTR_DEL(this->nameId); this->nameId = Text::String::NewOrNull(nameId); }
		NN<const Data::ArrayListStringNN> GetSessionIndex() const { return this->sessionIndex; }
		void AddSessionIndex(Text::CStringNN sessionIndex) { this->sessionIndex.Add(Text::String::New(sessionIndex)); }

		static Text::CStringNN ProcessErrorGetName(ProcessError val);
	};
}
#endif
