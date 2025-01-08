#include "Stdafx.h"
#include "Net/Names/InformationFramework.h"
#include "Net/Names/PKCS1.h"
#include "Net/Names/PKCS7.h"
#include "Net/Names/PKCS8.h"
#include "Net/Names/PKCS12.h"
#include "Net/Names/PKIX1Explicit88.h"

void __stdcall Net::Names::PKCS12::PFX(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PFX"), PFXCont);
}

void __stdcall Net::Names::PKCS12::PFXCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("Version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("authSafe"), AuthenticatedSafeContentInfoCont);
	AddMacData(names, CSTR("macData"));
}

void __stdcall Net::Names::PKCS12::AddMacData(NN<Net::ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, MacDataCont);
}

void __stdcall Net::Names::PKCS12::MacDataCont(NN<ASN1Names> names)
{
	PKCS7::AddDigestInfo(names, CSTR("mac"));
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("macSalt"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("iterations"));
}

void __stdcall Net::Names::PKCS12::AuthenticatedSafeContentInfoCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedSafeEncryptedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void __stdcall Net::Names::PKCS12::AuthenticatedSafeData(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("data"), AuthenticatedSafe);
}

void __stdcall Net::Names::PKCS12::AuthenticatedSafeEnvelopedData(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), PKCS7::EnvelopedDataCont);
}

void __stdcall Net::Names::PKCS12::AuthenticatedSafeEncryptedData(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encrypted-data"), PKCS7::EncryptedDataCont);
}

void __stdcall Net::Names::PKCS12::AuthenticatedSafe(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthenticatedSafe"), AuthSafeContentInfo);
}

void __stdcall Net::Names::PKCS12::AuthSafeContentInfo(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("ContentInfo"), AuthSafeContentInfoCont);
}

void __stdcall Net::Names::PKCS12::AuthSafeContentInfoCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), SafeContentsData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::SignedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::EnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::SignedAndEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::DigestedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::EncryptedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), PKCS7::AuthenticatedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void __stdcall Net::Names::PKCS12::SafeContentsData(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("data"), SafeContents);
}

void __stdcall Net::Names::PKCS12::SafeContents(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SafeContents"), SafeContentsCont);
}

void __stdcall Net::Names::PKCS12::SafeContentsCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SafeBag"), SafeBagCont);
}

void __stdcall Net::Names::PKCS12::SafeBagCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("bagId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("keyBag"), PKCS8::PrivateKeyInfo);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs8ShroudedKeyBag"), PKCS8::EncryptedPrivateKeyInfo);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certBag"), CertBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlBag"), CRLBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("secretBag"), SecretBag);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.10.1.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("safeContentsBag"), SafeContents);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("bagAttributes"), PKCS12Attributes);
}

void __stdcall Net::Names::PKCS12::CertBag(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertBag"), CertBagCont);
}

void __stdcall Net::Names::PKCS12::CertBagCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("certId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.22.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certValue"), X509Certificate);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.22.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certValue"), SdsiCertificate);
}

void __stdcall Net::Names::PKCS12::X509Certificate(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("x509Certificate"), PKIX1Explicit88::Certificate);
}

void __stdcall Net::Names::PKCS12::SdsiCertificate(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_IA5STRING)->NextValue(CSTR("sdsiCertificate"));
}

void __stdcall Net::Names::PKCS12::CRLBag(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CRLBag"), CRLBagCont);
}

void __stdcall Net::Names::PKCS12::CRLBagCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("crlId"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.23.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlValue"), X509CRL);
}

void __stdcall Net::Names::PKCS12::X509CRL(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("x509CRL"), PKIX1Explicit88::CertificateList);
}

void __stdcall Net::Names::PKCS12::SecretBag(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SecretBag"), SecretBagCont);
}

void __stdcall Net::Names::PKCS12::SecretBagCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("secretTypeId"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->NextValue(CSTR("secretValue"));
}

void __stdcall Net::Names::PKCS12::PKCS12Attributes(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PKCS12Attribute"), InformationFramework::AttributeCont);
}
