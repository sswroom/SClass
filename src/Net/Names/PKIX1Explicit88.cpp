#include "Stdafx.h"
#include "Net/Names/General.h"
#include "Net/Names/PKCS1.h"
#include "Net/Names/PKIX1Explicit88.h"
#include "Net/Names/PKIX1Implicit88.h"
#include "Net/Names/RFC2459.h"

void Net::Names::PKIX1Explicit88::AddAttributeTypeAndValue(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, AttributeTypeAndValueCont);
}

void Net::Names::PKIX1Explicit88::AttributeTypeAndValueCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("type"));
	names->NextValue(CSTR("value"));
}

void Net::Names::PKIX1Explicit88::AddName(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, RDNSequenceCont);
}

void Net::Names::PKIX1Explicit88::Name(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Name"), RDNSequenceCont);
}

void Net::Names::PKIX1Explicit88::RDNSequenceCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("rdnSequence"), RelativeDistinguishedNameCont);
}

void Net::Names::PKIX1Explicit88::RelativeDistinguishedName(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SET)->Container(CSTR("RelativeDistinguishedName"), RelativeDistinguishedNameCont);
}

void Net::Names::PKIX1Explicit88::RelativeDistinguishedNameCont(NotNullPtr<ASN1Names> names)
{
	AddAttributeTypeAndValue(names, CSTR("AttributeTypeAndValue"));
}

void Net::Names::PKIX1Explicit88::Certificate(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Certificate"), CertificateCont);
}

void Net::Names::PKIX1Explicit88::CertificateCont(NotNullPtr<ASN1Names> names)
{
	AddTBSCertificate(names, CSTR("tbsCertificate"));
	AddAlgorithmIdentifier(names, CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}

void Net::Names::PKIX1Explicit88::AddTBSCertificate(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, TBSCertificateCont);
}

void Net::Names::PKIX1Explicit88::TBSCertificateCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("version"), Version);
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("serialNumber"));
	AddAlgorithmIdentifier(names, CSTR("signature"));
	AddName(names, CSTR("issuer"));
	AddValidity(names, CSTR("validity"));
	AddName(names, CSTR("subject"));
	AddSubjectPublicKeyInfo(names, CSTR("subjectPublicKeyInfo"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->NextValue(CSTR("issuerUniqueID"));/////////////////////
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->NextValue(CSTR("subjectUniqueID"));//////////////////////
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_3)->Container(CSTR("extensions"), Extensions);
}

void Net::Names::PKIX1Explicit88::Version(NotNullPtr<ASN1Names> names)
{
	static Text::CStringNN Version[] = {CSTR("v1"), CSTR("v2"), CSTR("v3")};
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->Enum(CSTR("Version"), Version, 3);
}

void Net::Names::PKIX1Explicit88::AddValidity(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ValidityCont);
}

void Net::Names::PKIX1Explicit88::ValidityCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIsTime()->NextValue(CSTR("notBefore"));
	names->TypeIsTime()->NextValue(CSTR("notAfter"));
}

void Net::Names::PKIX1Explicit88::AddSubjectPublicKeyInfo(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, SubjectPublicKeyInfoCont);
}

void Net::Names::PKIX1Explicit88::SubjectPublicKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	AddAlgorithmIdentifier(names, CSTR("algorithm"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.1.1"), Net::ASN1Util::IT_BIT_STRING)->Container(CSTR("subjectPublicKey"), PKCS1::RSAPublicKey);
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("subjectPublicKey"));
}

void Net::Names::PKIX1Explicit88::AddExtensions(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, ExtensionsCont);
}

void Net::Names::PKIX1Explicit88::Extensions(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Extensions"), ExtensionsCont);
}

void Net::Names::PKIX1Explicit88::ExtensionsCont(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Extension"), ExtensionCont);
}

void Net::Names::PKIX1Explicit88::ExtensionCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("extnID"));
	names->TypeIs(Net::ASN1Util::IT_BOOLEAN)->NextValue(CSTR("critical"));
	names->LastOIDAndTypeIs(CSTR("1.3.6.1.4.1.11129.2.4.2"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("extendedValidationCertificates"), General::ExtendedValidationCertificates);
	names->LastOIDAndTypeIs(CSTR("1.3.6.1.5.5.7.1.1"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("authorityInfoAccess"), RFC2459::AuthorityInfoAccessSyntax);
	names->LastOIDAndTypeIs(CSTR("2.5.29.14"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("subjectKeyIdentifier"), PKIX1Implicit88::SubjectKeyIdentifier);
	names->LastOIDAndTypeIs(CSTR("2.5.29.15"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("keyUsage"), PKIX1Implicit88::KeyUsage);
	names->LastOIDAndTypeIs(CSTR("2.5.29.17"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("subjectAltName"), PKIX1Implicit88::GeneralNames);
	names->LastOIDAndTypeIs(CSTR("2.5.29.19"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("basicConstraints"), PKIX1Implicit88::BasicConstraints);
	names->LastOIDAndTypeIs(CSTR("2.5.29.31"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("cRLDistributionPoints"), PKIX1Implicit88::CRLDistributionPoints);
	names->LastOIDAndTypeIs(CSTR("2.5.29.32"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("certificatePolicies"), PKIX1Implicit88::CertificatePolicies);
	names->LastOIDAndTypeIs(CSTR("2.5.29.35"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("authorityKeyIdentifier"), PKIX1Implicit88::AuthorityKeyIdentifier);
	names->LastOIDAndTypeIs(CSTR("2.5.29.37"), Net::ASN1Util::IT_OCTET_STRING)->Container(CSTR("extKeyUsage"), PKIX1Implicit88::ExtKeyUsageSyntax);
	names->NextValue(CSTR("extnValue"));//////////////////////////////
}

void Net::Names::PKIX1Explicit88::CertificateList(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertificateList"), CertificateListCont);
}

void Net::Names::PKIX1Explicit88::CertificateListCont(NotNullPtr<ASN1Names> names)
{
	AddTBSCertList(names, CSTR("tbsCertList"));
	AddAlgorithmIdentifier(names, CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}

void Net::Names::PKIX1Explicit88::AddTBSCertList(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, TBSCertListCont);
}

void Net::Names::PKIX1Explicit88::TBSCertListCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("Version"));
	AddAlgorithmIdentifier(names, CSTR("signature"));
	AddName(names, CSTR("issuer"));
	names->TypeIsTime()->NextValue(CSTR("thisUpdate"));
	names->TypeIsTime()->NextValue(CSTR("nextUpdate"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("revokedCertificates"), RevokedCertificates);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("crlExtensions"), Extensions);
}

void Net::Names::PKIX1Explicit88::RevokedCertificates(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("revokedCertificate"), RevokedCertificateCont);
}

void Net::Names::PKIX1Explicit88::RevokedCertificateCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("userCertificate"));
	names->TypeIsTime()->NextValue(CSTR("revocationDate"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("crlEntryExtensions"), ExtensionsCont);
}

void Net::Names::PKIX1Explicit88::AddAlgorithmIdentifier(NotNullPtr<ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, AlgorithmIdentifierCont);
}

void Net::Names::PKIX1Explicit88::AlgorithmIdentifierCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("algorithm"));
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.1.3"), Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("parameters"), General::PBEParam); //pbeWithSHAAnd3-KeyTripleDES-CBC
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.12.1.6"), Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("parameters"), General::PBEParam); //pbeWithSHAAnd40BitRC2-CBC
	names->NextValue(CSTR("parameters"));
}
