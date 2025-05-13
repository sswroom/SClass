#ifndef _SM_NET_SAMLSTATUSCODE
#define _SM_NET_SAMLSTATUSCODE
#include "Text/CString.h"

namespace Net
{
	enum class SAMLStatusCode
	{
		Unknown,
		Success,
		Requester,
		Responder,
		VersionMismatch,
		AuthnFailed,
		InvalidAttrNameOrValue,
		InvalidNameIDPolicy,
		NoAuthnContext,
		NoAvailableIDP,
		NoPassive,
		NoSupportedIDP,
		PartialLogout,
		ProxyCountExceeded,
		RequestDenied,
		RequestUnsupported,
		RequestVersionDeprecated,
		RequestVersionTooHigh,
		RequestVersionTooLow,
		ResourceNotRecognized,
		TooManyResponses,
		UnknownAttrProfile,
		UnknownPrincipal,
		UnsupportedBinding
	};

	Text::CStringNN SAMLStatusCodeGetName(SAMLStatusCode statusCode);
	Text::CStringNN SAMLStatusCodeGetString(SAMLStatusCode statusCode);
	SAMLStatusCode SAMLStatusCodeFromString(Text::CStringNN statusCode);
}
#endif
