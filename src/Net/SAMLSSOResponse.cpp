#include "Stdafx.h"
#include "Net/SAMLSSOResponse.h"

Net::SAMLSSOResponse::SAMLSSOResponse(ResponseError error, Text::CStringNN errorMessage)
{
	this->error = error;
	this->errorMessage = Text::String::New(errorMessage);
	this->rawResponse = 0;
	this->decResponse = 0;
	this->id = 0;
	this->issuer = 0;
	this->audience = 0;
	this->notBefore = 0;
	this->notOnOrAfter = 0;
	this->nameID = 0;
	this->name = 0;
	this->givenname = 0;
	this->surname = 0;
	this->emailAddress = 0;
}

Net::SAMLSSOResponse::~SAMLSSOResponse()
{
	this->errorMessage->Release();
	OPTSTR_DEL(this->rawResponse);
	OPTSTR_DEL(this->decResponse);
	OPTSTR_DEL(this->id);
	OPTSTR_DEL(this->issuer);
	OPTSTR_DEL(this->audience);
	OPTSTR_DEL(this->nameID);
	OPTSTR_DEL(this->name);
	OPTSTR_DEL(this->givenname);
	OPTSTR_DEL(this->surname);
	OPTSTR_DEL(this->emailAddress);
}

Text::CStringNN Net::SAMLSSOResponse::ResponseErrorGetName(Net::SAMLSSOResponse::ResponseError val)
{
	switch (val)
	{
	case Net::SAMLSSOResponse::ResponseError::Success:
		return CSTR("Success");
	case Net::SAMLSSOResponse::ResponseError::ResponseNotFound:
		return CSTR("ResponseNotFound");
	case Net::SAMLSSOResponse::ResponseError::IDPMissing:
		return CSTR("IDPMissing");
	case Net::SAMLSSOResponse::ResponseError::SignKeyMissing:
		return CSTR("SignKeyMissing");
	case Net::SAMLSSOResponse::ResponseError::SignKeyInvalid:
		return CSTR("SignKeyInvalid");
	case Net::SAMLSSOResponse::ResponseError::ResponseFormatError:
		return CSTR("ResponseFormatError");
	case Net::SAMLSSOResponse::ResponseError::DecryptFailed:
		return CSTR("DecryptFailed");
	case Net::SAMLSSOResponse::ResponseError::UnexpectedIssuer:
		return CSTR("UnexpectedIssuer");
	case Net::SAMLSSOResponse::ResponseError::TimeOutOfRange:
		return CSTR("TimeOutOfRange");
	case Net::SAMLSSOResponse::ResponseError::InvalidAudience:
		return CSTR("InvalidAudience");
	case Net::SAMLSSOResponse::ResponseError::UsernameMissing:
		return CSTR("UsernameMissing");
	default:
		return CSTR("Unknown");
	}
}
