#include "Stdafx.h"
#include "Net/SAMLLogoutResponse.h"

Net::SAMLLogoutResponse::SAMLLogoutResponse(ProcessError error, Text::CStringNN errorMessage)
{
	this->error = error;
	this->errorMessage = Text::String::New(errorMessage);
	this->rawResponse = 0;
	this->status = SAMLStatusCode::Unknown;
}

Net::SAMLLogoutResponse::~SAMLLogoutResponse()
{
	this->errorMessage->Release();
	OPTSTR_DEL(this->rawResponse);
}

Text::CStringNN Net::SAMLLogoutResponse::ProcessErrorGetName(ProcessError val)
{
	switch (val)
	{
	case ProcessError::Success:
		return CSTR("Success");
	case ProcessError::IDPMissing:
		return CSTR("IDPMissing");
	case ProcessError::SSLMissing:
		return CSTR("SSLMissing");
	case ProcessError::ParamMissing:
		return CSTR("ParamMissing");
	case ProcessError::SigAlgNotSupported:
		return CSTR("SigAlgNotSupported");
	case ProcessError::QueryStringError:
		return CSTR("QueryStringError");
	case ProcessError::KeyError:
		return CSTR("KeyError");
	case ProcessError::SignatureInvalid:
		return CSTR("SignatureInvalid");
	case ProcessError::MessageInvalid:
		return CSTR("MessageInvalid");
	case ProcessError::StatusError:
		return CSTR("StatusError");
	default:
		return CSTR("Unknown");
	}
}
