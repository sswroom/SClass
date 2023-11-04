#include "Stdafx.h"
#include "Net/Names/General.h"
#include "Net/Names/PKCS7.h"

void Net::Names::General::PBEParam(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("salt"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("iterations"));
}

void Net::Names::General::ExtendedValidationCertificates(NotNullPtr<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("signedCertTimestamp"));
}

void Net::Names::General::AttributeOutlookExpress(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("issuerAndSerialNumber"), PKCS7::IssuerAndSerialNumberCont);
}
