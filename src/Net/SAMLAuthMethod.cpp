#include "Stdafx.h"
#include "Net/SAMLAuthMethod.h"

Text::CStringNN Net::SAMLAuthMethodGetName(SAMLAuthMethod authMethod)
{
	switch (authMethod)
	{
		default:
		case SAMLAuthMethod::Unknown:
			return CSTR("Unknown");
		case SAMLAuthMethod::Password:
			return CSTR("Password");
		case SAMLAuthMethod::PasswordProtectedTransport:
			return CSTR("PasswordProtectedTransport");
		case SAMLAuthMethod::TLSClient:
			return CSTR("TLSClient");
		case SAMLAuthMethod::X509:
			return CSTR("X509");
		case SAMLAuthMethod::WindowsAuth:
			return CSTR("WindowsAuth");
		case SAMLAuthMethod::Kerberos:
			return CSTR("Kerberos");
		case SAMLAuthMethod::PGP:
			return CSTR("PGP");
		case SAMLAuthMethod::SecureRemotePassword:
			return CSTR("SecureRemotePassword");
		case SAMLAuthMethod::XMLDSig:
			return CSTR("XMLDSig");
		case SAMLAuthMethod::SPKI:
			return CSTR("SPKI");
		case SAMLAuthMethod::Smartcard:
			return CSTR("Smartcard");
		case SAMLAuthMethod::SmartcardPKI:
			return CSTR("SmartcardPKI");
	}
}

Text::CStringNN Net::SAMLAuthMethodGetString(SAMLAuthMethod authMethod)
{
	switch (authMethod)
	{
	default:
	case SAMLAuthMethod::Unknown:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:unspecified");
	case SAMLAuthMethod::Password:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Password");
	case SAMLAuthMethod::PasswordProtectedTransport:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:PasswordProtectedTransport");
	case SAMLAuthMethod::TLSClient:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:TLSClient");
	case SAMLAuthMethod::X509:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:X509");
	case SAMLAuthMethod::WindowsAuth:
		return CSTR("urn:federation:authentication:windows");
	case SAMLAuthMethod::Kerberos:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Kerberos");
	case SAMLAuthMethod::PGP:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:PGP");
	case SAMLAuthMethod::SecureRemotePassword:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SecureRemotePassword");
	case SAMLAuthMethod::XMLDSig:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:XMLDSig");
	case SAMLAuthMethod::SPKI:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SPKI");
	case SAMLAuthMethod::Smartcard:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Smartcard");
	case SAMLAuthMethod::SmartcardPKI:
		return CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SmartcardPKI");
	}
}

Net::SAMLAuthMethod Net::SAMLAuthMethodFromString(Text::CStringNN authMethod)
{
	if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Password")))
		return SAMLAuthMethod::Password;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:PasswordProtectedTransport")))
		return SAMLAuthMethod::PasswordProtectedTransport;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:TLSClient")))
		return SAMLAuthMethod::TLSClient;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:X509")))
		return SAMLAuthMethod::X509;
	else if (authMethod.Equals(CSTR("urn:federation:authentication:windows")))
		return SAMLAuthMethod::WindowsAuth;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Kerberos")))
		return SAMLAuthMethod::Kerberos;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:PGP")))
		return SAMLAuthMethod::PGP;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SecureRemotePassword")))
		return SAMLAuthMethod::SecureRemotePassword;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:XMLDSig")))
		return SAMLAuthMethod::XMLDSig;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SPKI")))
		return SAMLAuthMethod::SPKI;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:Smartcard")))
		return SAMLAuthMethod::Smartcard;
	else if (authMethod.Equals(CSTR("urn:oasis:names:tc:SAML:2.0:ac:classes:SmartcardPKI")))
		return SAMLAuthMethod::SmartcardPKI;
	else
		return SAMLAuthMethod::Unknown;
}
