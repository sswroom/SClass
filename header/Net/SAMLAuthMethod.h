#ifndef _SM_NET_SAMLAUTHMETHOD
#define _SM_NET_SAMLAUTHMETHOD
#include "Text/CString.h"

namespace Net
{
	enum class SAMLAuthMethod
	{
		Unknown,
		Password,
		PasswordProtectedTransport,
		TLSClient,
		X509,
		WindowsAuth,
		Kerberos,
		PGP,
		SecureRemotePassword,
		XMLDSig,
		SPKI,
		Smartcard,
		SmartcardPKI
	};

	Text::CStringNN SAMLAuthMethodGetName(SAMLAuthMethod authMethod);
	Text::CStringNN SAMLAuthMethodGetString(SAMLAuthMethod authMethod);
	SAMLAuthMethod SAMLAuthMethodFromString(Text::CStringNN authMethod);
}
#endif
