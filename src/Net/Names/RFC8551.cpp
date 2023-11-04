#include "Stdafx.h"
#include "Net/Names/PKCS7.h"
#include "Net/Names/RFC8551.h"

void Net::Names::RFC8551::SMIMEEncryptionKeyPreference(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)->Container(CSTR("issuerAndSerialNumber"), PKCS7::IssuerAndSerialNumberCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)->NextValue(CSTR("receipentKeyId"));//, RecipientKeyIdentifierCont);
	names->TypeIs(Net::ASN1Util::IT_CONTEXT_SPECIFIC_2)->NextValue(CSTR("subjectAltKeyIdentifier"));//, SubjectKeyIdentifierCont);
}
