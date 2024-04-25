#include "Stdafx.h"
#include "Net/Names/PKCS9.h"

void Net::Names::PKCS9::AttributeContentType(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("contentType"));
}

void Net::Names::PKCS9::AttributeMessageDigest(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("messageDigest"));
}

void Net::Names::PKCS9::AttributeSigningTime(NN<ASN1Names> names)
{
	names->TypeIsTime()->NextValue(CSTR("signingTime"));
}

void Net::Names::PKCS9::AttributeFriendlyName(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_BMPSTRING)->NextValue(CSTR("friendlyName"));
}

void Net::Names::PKCS9::AttributeSMIMECapabilities(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("smimeCapabilities"), SMIMECapabilitiesCont);
}

void Net::Names::PKCS9::AttributeLocalKeyId(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("localKeyId"));
}

void Net::Names::PKCS9::SMIMECapabilitiesCont(NN<ASN1Names> names)
{
	names->RepeatIfTypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("SMIMECapability"), SMIMECapabilityCont);
}

void Net::Names::PKCS9::SMIMECapabilityCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("algorithm"));
	names->NextValue(CSTR("parameters"));
}

