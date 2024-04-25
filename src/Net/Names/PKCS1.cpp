#include "Stdafx.h"
#include "Net/Names/PKCS1.h"
#include "Net/Names/PKIX1Explicit88.h"

void Net::Names::PKCS1::RSAPublicKey(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("RSAPublicKey"), RSAPublicKeyCont);
}

void Net::Names::PKCS1::RSAPublicKeyCont(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("modulus"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("publicExponent"));
}

void Net::Names::PKCS1::RSAPrivateKey(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("RSAPrivateKey"), RSAPrivateKeyCont);
}

void Net::Names::PKCS1::RSAPrivateKeyCont(NN<ASN1Names> names)
{
	static Text::CStringNN Version[] = {CSTR("two-prime"), CSTR("multi")};
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->Enum(CSTR("Version"), Version, 2);
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("modulus"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("publicExponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("privateExponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime1"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime2"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent1"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent2"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("coefficient"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("otherPrimeInfos"), OtherPrimeInfos);
}

void Net::Names::PKCS1::OtherPrimeInfos(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("prime"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("exponent"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("coefficient"));
}

void Net::Names::PKCS1::AddDigestInfo(NN<Net::ASN1Names> names, Text::CStringNN name)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(name, DigestInfoCont);
}

void Net::Names::PKCS1::DigestInfoCont(NN<ASN1Names> names)
{
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("digestAlgorithm"));
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("digest"));
}
