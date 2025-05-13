#include "Stdafx.h"
#include "Net/SAMLStatusCode.h"

Text::CStringNN Net::SAMLStatusCodeGetName(SAMLStatusCode statusCode)
{
	switch (statusCode)
	{
	case SAMLStatusCode::Unknown:
		return CSTR("Unknown");
	case SAMLStatusCode::Success:
		return CSTR("Success");
	case SAMLStatusCode::Requester:
		return CSTR("Requester");
	case SAMLStatusCode::Responder:
		return CSTR("Responder");
	case SAMLStatusCode::VersionMismatch:
		return CSTR("VersionMismatch");
	case SAMLStatusCode::AuthnFailed:
		return CSTR("AuthnFailed");
	case SAMLStatusCode::InvalidAttrNameOrValue:
		return CSTR("InvalidAttrNameOrValue");
	case SAMLStatusCode::InvalidNameIDPolicy:
		return CSTR("InvalidNameIDPolicy");
	case SAMLStatusCode::NoAuthnContext:
		return CSTR("NoAuthnContext");
	case SAMLStatusCode::NoAvailableIDP:
		return CSTR("NoAvailableIDP");
	case SAMLStatusCode::NoPassive:
		return CSTR("NoPassive");
	case SAMLStatusCode::NoSupportedIDP:
		return CSTR("NoSupportedIDP");
	case SAMLStatusCode::PartialLogout:
		return CSTR("PartialLogout");
	case SAMLStatusCode::ProxyCountExceeded:
		return CSTR("ProxyCountExceeded");
	case SAMLStatusCode::RequestDenied:
		return CSTR("RequestDenied");
	case SAMLStatusCode::RequestUnsupported:
		return CSTR("RequestUnsupported");
	case SAMLStatusCode::RequestVersionDeprecated:
		return CSTR("RequestVersionDeprecated");
	case SAMLStatusCode::RequestVersionTooHigh:
		return CSTR("RequestVersionTooHigh");
	case SAMLStatusCode::RequestVersionTooLow:
		return CSTR("RequestVersionTooLow");
	case SAMLStatusCode::ResourceNotRecognized:
		return CSTR("ResourceNotRecognized");
	case SAMLStatusCode::TooManyResponses:
		return CSTR("TooManyResponses");
	case SAMLStatusCode::UnknownAttrProfile:
		return CSTR("UnknownAttrProfile");
	case SAMLStatusCode::UnknownPrincipal:
		return CSTR("UnknownPrincipal");
	case SAMLStatusCode::UnsupportedBinding:
		return CSTR("UnsupportedBinding");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Net::SAMLStatusCodeGetString(SAMLStatusCode statusCode)
{
	switch (statusCode)
	{
	case SAMLStatusCode::Unknown:
		return CSTR("");
	case SAMLStatusCode::Success:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:Success");
	case SAMLStatusCode::Requester:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:Requester");
	case SAMLStatusCode::Responder:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:Responder");
	case SAMLStatusCode::VersionMismatch:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:VersionMismatch");
	case SAMLStatusCode::AuthnFailed:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:AuthnFailed");
	case SAMLStatusCode::InvalidAttrNameOrValue:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:InvalidAttrNameOrValue");
	case SAMLStatusCode::InvalidNameIDPolicy:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:InvalidNameIDPolicy");
	case SAMLStatusCode::NoAuthnContext:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:NoAuthnContext");
	case SAMLStatusCode::NoAvailableIDP:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:NoAvailableIDP");
	case SAMLStatusCode::NoPassive:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:NoPassive");
	case SAMLStatusCode::NoSupportedIDP:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:NoSupportedIDP");
	case SAMLStatusCode::PartialLogout:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:PartialLogout");
	case SAMLStatusCode::ProxyCountExceeded:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:ProxyCountExceeded");
	case SAMLStatusCode::RequestDenied:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestDenied");
	case SAMLStatusCode::RequestUnsupported:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestUnsupported");
	case SAMLStatusCode::RequestVersionDeprecated:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionDeprecated");
	case SAMLStatusCode::RequestVersionTooHigh:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionTooHigh");
	case SAMLStatusCode::RequestVersionTooLow:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionTooLow");
	case SAMLStatusCode::ResourceNotRecognized:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:ResourceNotRecognized");
	case SAMLStatusCode::TooManyResponses:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:TooManyResponses");
	case SAMLStatusCode::UnknownAttrProfile:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:UnknownAttrProfile");
	case SAMLStatusCode::UnknownPrincipal:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:UnknownPrincipal");
	case SAMLStatusCode::UnsupportedBinding:
		return CSTR("urn:oasis:names:tc:SAML:2.0:status:UnsupportedBinding");
	default:
		return CSTR("");
	}
}

Net::SAMLStatusCode Net::SAMLStatusCodeFromString(Text::CStringNN statusCode)
{
	if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:Success")))
	{
		return SAMLStatusCode::Success;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:Requester")))
	{
		return SAMLStatusCode::Requester;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:Responder")))
	{
		return SAMLStatusCode::Responder;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:VersionMismatch")))
	{
		return SAMLStatusCode::VersionMismatch;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:AuthnFailed")))
	{
		return SAMLStatusCode::AuthnFailed;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:InvalidAttrNameOrValue")))
	{
		return SAMLStatusCode::InvalidAttrNameOrValue;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:InvalidNameIDPolicy")))
	{
		return SAMLStatusCode::InvalidNameIDPolicy;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:NoAuthnContext")))
	{
		return SAMLStatusCode::NoAuthnContext;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:NoAvailableIDP")))
	{
		return SAMLStatusCode::NoAvailableIDP;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:NoPassive")))
	{
		return SAMLStatusCode::NoPassive;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:NoSupportedIDP")))
	{
		return SAMLStatusCode::NoSupportedIDP;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:PartialLogout")))
	{
		return SAMLStatusCode::PartialLogout;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:ProxyCountExceeded")))
	{
		return SAMLStatusCode::ProxyCountExceeded;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestDenied")))
	{
		return SAMLStatusCode::RequestDenied;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestUnsupported")))
	{
		return SAMLStatusCode::RequestUnsupported;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionDeprecated")))
	{
		return SAMLStatusCode::RequestVersionDeprecated;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionTooHigh")))
	{
		return SAMLStatusCode::RequestVersionTooHigh;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:RequestVersionTooLow")))
	{
		return SAMLStatusCode::RequestVersionTooLow;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:ResourceNotRecognized")))
	{
		return SAMLStatusCode::ResourceNotRecognized;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:TooManyResponses")))
	{
		return SAMLStatusCode::TooManyResponses;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:UnknownAttrProfile")))
	{
		return SAMLStatusCode::UnknownAttrProfile;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:UnknownPrincipal")))
	{
		return SAMLStatusCode::UnknownPrincipal;
	}
	else if (statusCode.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:status:UnsupportedBinding")))
	{
		return SAMLStatusCode::UnsupportedBinding;
	}
	else
	{
		return SAMLStatusCode::Unknown;
	}
}
