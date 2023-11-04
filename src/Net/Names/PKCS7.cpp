#include "Stdafx.h"
#include "Net/Names/PKCS7.h"
#include "Net/Names/PKCS10.h"
#include "Net/Names/PKIX1Implicit88.h"
#include "Net/Names/PKIX1Explicit88.h"

void Net::Names::PKCS7::AddContentInfo(NotNullPtr<Net::ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ContentInfoCont);
}

void Net::Names::PKCS7::ContentInfo(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("ContentInfo"), ContentInfoCont);
}

void Net::Names::PKCS7::ContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("content-type"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.1"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), Data);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), SignedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.3"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), EnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.4"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), SignedAndEnvelopedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.5"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), DigestedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.7.6"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), EncryptedData);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.1.2"), Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("pkcs7-content"), AuthenticatedData);
	names->NextValue(CSTR("pkcs7-content")); ////////////////////////
}

void Net::Names::PKCS7::Data(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("data"));
}

void Net::Names::PKCS7::SignedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), SignedDataCont);
}

void Net::Names::PKCS7::SignedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("digestAlgorithms"), DigestAlgorithmIdentifiers);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("signerInfos"), SignerInfos);
}

void Net::Names::PKCS7::DigestAlgorithmIdentifiers(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("DigestAlgorithmIdentifier"), PKIX1Explicit88::AlgorithmIdentifierCont);
}

void Net::Names::PKCS7::CertificateSet(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("certificate"), PKIX1Explicit88::CertificateCont);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("extendedCertificate"));//, ExtendedCertificate);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("attributeCertificate"));//, AttributeCertificate);
}

void Net::Names::PKCS7::CertificateRevocationLists(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->NextValue(CSTR("CertificateRevocationLists"));//, CertificateListCont);
}

void Net::Names::PKCS7::SignerInfos(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SignerInfo"), SignerInfoCont);
}

void Net::Names::PKCS7::SignerInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signerIdentifier"), IssuerAndSerialNumberCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->Container(CSTR("signerIdentifier"), PKIX1Implicit88::SubjectKeyIdentifier);
  	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("digestAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("authenticatedAttributes"), PKCS10::AttributesCont);
  	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("digestEncryptionAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedDigest"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unauthenticatedAttributes"), PKCS10::AttributesCont);
}

void Net::Names::PKCS7::IssuerAndSerialNumberCont(NotNullPtr<ASN1Names> names)
{
	PKIX1Explicit88::AddName(names, CSTR("issuer"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("serialNumber"));
}

void Net::Names::PKCS7::AddDigestInfo(NotNullPtr<Net::ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, DigestInfoCont);
}

void Net::Names::PKCS7::DigestInfoCont(NotNullPtr<ASN1Names> names)
{
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("digestAlgorithm"));
	names->NextValue(CSTR("digest")); ////////////////////////
}

void Net::Names::PKCS7::EnvelopedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("enveloped-data"), EnvelopedDataCont);
}

void Net::Names::PKCS7::EnvelopedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("originatorInfo"), OriginatorInfoCont);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("recipientInfos"), RecipientInfos);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptedContentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unprotectedAttributes"), PKCS10::AttributesCont);
}

void Net::Names::PKCS7::OriginatorInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
}

void Net::Names::PKCS7::RecipientInfos(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("keyTransportRecipientInfo"), KeyTransportRecipientInfoCont);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("keyAgreementRecipientInfo"));//, KeyAgreementRecipientInfo);
	names->RepeatIfTypeIs(Net::ASN1Util::IT_CHOICE_1)->NextValue(CSTR("keyEncryptionKeyRecipientInfo"));//, KeyEncryptionKeyRecipientInfo);
}

void Net::Names::PKCS7::KeyTransportRecipientInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->//TypeIs(Net::ASN1Util::IT_INTEGER)->
		NextValue(CSTR("recipientIdentifier")); //RecipientIdentifier
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("keyEncryptionAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedKey"));
}

void Net::Names::PKCS7::SignedAndEnvelopedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), SignedAndEnvelopedDataCont);
}

void Net::Names::PKCS7::SignedAndEnvelopedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SET)->NextValue(CSTR("recipientInfos"));
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("digestAlgorithms"), DigestAlgorithmIdentifiers);
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentInfo"), ContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("certificates"), CertificateSet);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("crls"), CertificateRevocationLists);
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("signerInfos"), SignerInfos);
}

void Net::Names::PKCS7::DigestedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), DigestedDataCont);
}

void Net::Names::PKCS7::DigestedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), DigestedDataCont);
}

void Net::Names::PKCS7::EncryptedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encrypted-data"), EncryptedDataCont);
}

void Net::Names::PKCS7::EncryptedDataCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptedContentInfo"), EncryptedContentInfoCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->Container(CSTR("unprotectedAttributes"), PKCS10::AttributesCont);
}

void Net::Names::PKCS7::EncryptedContentInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("contentType"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("contentEncryptionAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CHOICE_0)->NextValue(CSTR("encryptedContent"));
}

void Net::Names::PKCS7::AuthenticatedData(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("signed-data"), AuthenticatedData);
}
