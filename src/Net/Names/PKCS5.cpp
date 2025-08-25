#include "Stdafx.h"
#include "Net/Names/PKCS5.h"
#include "Net/Names/PKIX1Explicit88.h"

void __stdcall Net::Names::PKCS5::PBKDF2Params(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("salt-specified"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("salt-otherSource"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("iterationCount"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("keyLength"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("prf"), PKIX1Explicit88::AlgorithmIdentifierCont);
}

void __stdcall Net::Names::PKCS5::PBEParameter(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("salt"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("iterationCount"));
}

void __stdcall Net::Names::PKCS5::PBES2Params(NN<Net::ASN1Names> names)
{
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("keyDerivationFunc"));
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("encryptionScheme"));
}

void __stdcall Net::Names::PKCS5::PBMAC1Params(NN<ASN1Names> names)
{
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("keyDerivationFunc"));
	PKIX1Explicit88::AddAlgorithmIdentifier(names, CSTR("messageAuthScheme"));
}

void __stdcall Net::Names::PKCS5::RC2CBCParam(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("rc2ParameterVersion"));
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("iv"));
}
void __stdcall Net::Names::PKCS5::RC5CBCParams(NN<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("rounds"));
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("blockSizeInBits"));
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("iv"));
}
