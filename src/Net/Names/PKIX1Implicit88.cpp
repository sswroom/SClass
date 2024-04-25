#include "Stdafx.h"
#include "Net/Names/PKIX1Explicit88.h"
#include "Net/Names/PKIX1Implicit88.h"

void Net::Names::PKIX1Implicit88::AuthorityKeyIdentifier(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("AuthorityKeyIdentifier"), AuthorityKeyIdentifierCont);
}

void Net::Names::PKIX1Implicit88::AuthorityKeyIdentifierCont(NN<ASN1Names> names)
{
	names->TypeIsOpt(0)->NextValue(CSTR("keyIdentifier"));
	names->TypeIsOpt(1)->Container(CSTR("authorityCertIssuer"), GeneralNameCont);
	names->TypeIsOpt(2)->NextValue(CSTR("authorityCertSerialNumber"));
}

void Net::Names::PKIX1Implicit88::SubjectKeyIdentifier(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("SubjectKeyIdentifier"));
}

void Net::Names::PKIX1Implicit88::KeyUsage(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("KeyUsage"));
}

void Net::Names::PKIX1Implicit88::CertificatePolicies(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertificatePolicies"), CertificatePoliciesCont);
}

void Net::Names::PKIX1Implicit88::CertificatePoliciesCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PolicyInformation"), PolicyInformationCont);
}

void Net::Names::PKIX1Implicit88::PolicyInformationCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("policyIdentifier"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("policyQualifiers"), PolicyQualifiers);
}

void Net::Names::PKIX1Implicit88::PolicyQualifiers(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PolicyQualifierInfo"), PolicyQualifierInfoCont);
}

void Net::Names::PKIX1Implicit88::PolicyQualifierInfoCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("policyQualifierId"));
	names->NextValue(CSTR("qualifier"));
}

void Net::Names::PKIX1Implicit88::GeneralNames(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("GeneralName"), GeneralNameCont);
}

void Net::Names::PKIX1Implicit88::GeneralNameCont(NN<ASN1Names> names)
{
	names->TypeIsOpt(0)->NextValue(CSTR("otherName"));
	names->TypeIsOpt(1)->NextValue(CSTR("rfc822Name"));
	names->TypeIsOpt(2)->NextValue(CSTR("dNSName"));
	names->TypeIsOpt(3)->NextValue(CSTR("x400Address"));
	names->TypeIsOpt(4)->Container(CSTR("directoryName"), PKIX1Explicit88::Name);
	names->TypeIsOpt(5)->NextValue(CSTR("ediPartyName"));
	names->TypeIsOpt(6)->NextValue(CSTR("uniformResourceIdentifier"));
	names->TypeIsOpt(7)->NextValue(CSTR("iPAddress"));
	names->TypeIsOpt(8)->NextValue(CSTR("registeredID"));
	names->AllNotMatch()->NextValue(CSTR("unknown"));
}

void Net::Names::PKIX1Implicit88::BasicConstraints(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("BasicConstraints"), BasicConstraintsCont);
}

void Net::Names::PKIX1Implicit88::BasicConstraintsCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_BOOLEAN)->NextValue(CSTR("cA"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("pathLenConstraint"));
}

void Net::Names::PKIX1Implicit88::CRLDistributionPoints(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CRLDistributionPoints"), CRLDistributionPointsCont);
}

void Net::Names::PKIX1Implicit88::CRLDistributionPointsCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("DistributionPoint"), DistributionPointCont);
}

void Net::Names::PKIX1Implicit88::DistributionPointCont(NN<ASN1Names> names)
{
	names->TypeIsOpt(0)->Container(CSTR("distributionPoint"), DistributionPointName);
	names->TypeIsOpt(1)->Container(CSTR("reasons"), ReasonFlags);
	names->TypeIsOpt(2)->Container(CSTR("cRLIssuer"), GeneralNames);
}

void Net::Names::PKIX1Implicit88::DistributionPointName(NN<ASN1Names> names)
{
	names->TypeIsOpt(0)->Container(CSTR("fullName"), GeneralNameCont);
	names->TypeIsOpt(1)->Container(CSTR("nameRelativeToCRLIssuer"), PKIX1Explicit88::RelativeDistinguishedNameCont);
}

void Net::Names::PKIX1Implicit88::ReasonFlags(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("ReasonFlags"));
}

void Net::Names::PKIX1Implicit88::ExtKeyUsageSyntax(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("ExtKeyUsageSyntax"), ExtKeyUsageSyntaxCont);
}

void Net::Names::PKIX1Implicit88::ExtKeyUsageSyntaxCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("KeyPurposeId"));
}
