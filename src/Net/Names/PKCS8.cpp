#include "Stdafx.h"
#include "Net/Names/PKCS8.h"
#include "Net/Names/PKCS10.h"
#include "Net/Names/PKIX1Explicit88.h"

void Net::Names::PKCS8::PrivateKeyInfo(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("PrivateKeyInfo"), PrivateKeyInfoCont);
}

void Net::Names::PKCS8::PrivateKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_INTEGER)->NextValue(CSTR("version"));
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("privateKeyAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("privateKey"));
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("attributes"), PKCS10::AttributesCont);
}

void Net::Names::PKCS8::EncryptedPrivateKeyInfo(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("EncryptedPrivateKeyInfo"), EncryptedPrivateKeyInfoCont);
}

void Net::Names::PKCS8::EncryptedPrivateKeyInfoCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_SEQUENCE)->Container(CSTR("encryptionAlgorithm"), PKIX1Explicit88::AlgorithmIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_OCTET_STRING)->NextValue(CSTR("encryptedData"));
}
