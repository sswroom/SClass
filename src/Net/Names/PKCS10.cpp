#include "Stdafx.h"
#include "Net/Names/InformationFramework.h"
#include "Net/Names/PKCS10.h"
#include "Net/Names/PKIX1Explicit88.h"

void Net::Names::PKCS10::AddCertificationRequestInfo(NN<Net::ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, CertificationRequestInfoCont);
}

void Net::Names::PKCS10::CertificationRequestInfoCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	PKIX1Explicit88::AddName(names, CSTR("subject"));
	PKIX1Explicit88::AddSubjectPublicKeyInfo(names, CSTR("subjectPKInfo"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("attributes"), AttributesCont);
}

void Net::Names::PKCS10::AttributesCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("Attribute"), InformationFramework::AttributeCont);
}

void Net::Names::PKCS10::CertificationRequest(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("CertificationRequest"), CertificationRequestCont);
}

void Net::Names::PKCS10::CertificationRequestCont(NN<ASN1Names> names)
{
	AddCertificationRequestInfo(names, CSTR("certificationRequestInfo"));
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("signatureAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_BIT_STRING)->NextValue(CSTR("signature"));
}
