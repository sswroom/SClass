#include "Stdafx.h"
#include "Net/Names/General.h"
#include "Net/Names/InformationFramework.h"
#include "Net/Names/PKCS9.h"
#include "Net/Names/RFC8551.h"

void Net::Names::InformationFramework::AttributeCont(NotNullPtr<ASN1Names> names)
{
	names->TypeIs(Net::ASN1Util::IT_OID)->NextValue(CSTR("attrId"));
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.1"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeEmailAddress);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.2"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeUnstructuredName);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.3"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeContentType);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.4"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeMessageDigest);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.5"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeSigningTime);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.6"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeCounterSignature);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.7"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeChallengePassword);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.8"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeUnstructuredAddress);
//	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.9"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), AttributeExtendedCertificateAttributes);

	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.15"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeSMIMECapabilities);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.16.2.11"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), RFC8551::SMIMEEncryptionKeyPreference);

	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.20"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeFriendlyName);
	names->LastOIDAndTypeIs(CSTR("1.2.840.113549.1.9.21"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), PKCS9::AttributeLocalKeyId);
	names->LastOIDAndTypeIs(CSTR("1.3.6.1.4.1.311.16.4"), Net::ASN1Util::IT_SET)->Container(CSTR("attrValues"), General::AttributeOutlookExpress);
	
	names->TypeIs(Net::ASN1Util::IT_SET)->NextValue(CSTR("attrValues"));
}
