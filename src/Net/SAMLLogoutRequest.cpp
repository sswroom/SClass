#include "Stdafx.h"
#include "Net/SAMLLogoutRequest.h"

Net::SAMLLogoutRequest::SAMLLogoutRequest(ProcessError error, Text::CStringNN errorMessage)
{
	this->error = error;
	this->errorMessage = Text::String::New(errorMessage);
	this->rawResponse = 0;
	this->id = 0;
	this->nameId = 0;
	this->sessionIndex = 0;
}

Net::SAMLLogoutRequest::~SAMLLogoutRequest()
{
	this->errorMessage->Release();
	OPTSTR_DEL(this->rawResponse);
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->nameId);
	this->sessionIndex.FreeAll();
}

Text::CStringNN Net::SAMLLogoutRequest::ProcessErrorGetName(ProcessError val)
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
	case ProcessError::DestinationInvalid:
		return CSTR("DestinationInvalid");
	case ProcessError::IssuerInvalid:
		return CSTR("IssuerInvalid");
	case ProcessError::TimeInvalid:
		return CSTR("TimeInvalid");
	default:
		return CSTR("Unknown");
	}
}
