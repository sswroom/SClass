#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509File.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"

Bool Crypto::Cert::X509File::IsSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 3)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".2");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrConcat(sptr, ".3");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrConcat(sptr, ".2");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"algorithmIdentifier");
		}
	}
	Text::StrConcat(sptr, ".3");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			sb->Append((const UTF8Char*)"signature = ");
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
}

Bool Crypto::Cert::X509File::IsTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 6)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UOSInt i = 1;
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		Text::StrUOSInt(sptr, i++);
	}
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UOSInt i = 1;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			sb->Append((const UTF8Char*)"version = ");
			AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
			sb->Append((const UTF8Char*)"\r\n");
			Text::StrUOSInt(sptr, i++);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->Append((const UTF8Char*)"serialNumber = ");
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"signature");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendName(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"issuer");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendValidity(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"validity");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendName(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"subject");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"subjectPublicKeyInfo");
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertificate(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendTBSCertificate(pdu, pduEnd, sbuff, sb);
	AppendSigned(pdu, pduEnd, path, sb);
}

Bool Crypto::Cert::X509File::IsPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 3 && cnt != 4)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr;
	sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrConcat(sptr, ".2");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrConcat(sptr, ".3");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_OCTET_STRING)
	{
		return false;
	}
	if (cnt == 4)
	{
		Text::StrConcat(sptr, ".4");
		if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SET)
		{
			return false;
		}
	}
	return true;
}

void Crypto::Cert::X509File::AppendPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Char *sptr;
	const UInt8 *itemPDU;
	UOSInt len;
	Net::ASN1Util::ItemType itemType;
	sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &len, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->Append((const UTF8Char*)"version = ");
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
	Text::StrConcat(sptr, ".2");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &len, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + len, sb, (const UTF8Char*)"privateKeyAlgorithm");
		}
	}
	Text::StrConcat(sptr, ".3");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &len, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			sb->Append((const UTF8Char*)"privateKey = ");
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 4)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UOSInt i = 1;
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UOSInt i = 1;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->Append((const UTF8Char*)"serialNumber = ");
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendName(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"subject");
		}
	}
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU, itemPDU + itemLen, sb, (const UTF8Char*)"subjectPublicKeyInfo");
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsCertificateRequestInfo(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendCertificateRequestInfo(pdu, pduEnd, sbuff, sb);
	AppendSigned(pdu, pduEnd, path, sb);
}

void Crypto::Cert::X509File::AppendVersion(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb)
{
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &itemLen, &itemType);
	if (itemPDU != 0 && itemType == Net::ASN1Util::IT_INTEGER)
	{
		if (itemLen == 1)
		{
			switch (itemPDU[0])
			{
			case 0:
				sb->Append((const UTF8Char*)"v1");
				break;
			case 1:
				sb->Append((const UTF8Char*)"v2");
				break;
			case 2:
				sb->Append((const UTF8Char*)"v3");
				break;
			}
		}
	}
}

void Crypto::Cert::X509File::AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	UOSInt algorithmLen;
	Net::ASN1Util::ItemType algorithmType;
	const UInt8 *algorithm = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &algorithmLen, &algorithmType);
	UOSInt parametersLen;
	Net::ASN1Util::ItemType parametersType;
	const UInt8 *parameters = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &parametersLen, &parametersType);
	if (algorithm && algorithmType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendChar('.', 1);
		sb->Append((const UTF8Char*)"algorithm = ");
		Net::ASN1Util::OIDToString(algorithm, algorithmLen, sb);
		const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(algorithm, algorithmLen);
		if (oid)
		{
			sb->Append((const UTF8Char*)" (");
			sb->Append((const UTF8Char*)oid->name);
			sb->AppendChar(')', 1);
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
	if (parameters)
	{
		sb->Append(varName);
		sb->AppendChar('.', 1);
		sb->Append((const UTF8Char*)"parameters = ");
		if (parametersType == Net::ASN1Util::IT_NULL)
		{
			sb->Append((const UTF8Char*)"NULL");
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
}

void Crypto::Cert::X509File::AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	Data::DateTime dt;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			sb->Append(varName);
			sb->AppendChar('.', 1);
			sb->Append((const UTF8Char*)"notBefore = ");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			sb->Append(varName);
			sb->AppendChar('.', 1);
			sb->Append((const UTF8Char*)"notBefore = ");
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
}

void Crypto::Cert::X509File::AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	UTF8Char sbuff[256];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			Text::StrConcat(Text::StrConcat(sbuff, varName), (const UTF8Char*)".algorithm");
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, sbuff);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			sb->Append(varName);
			sb->Append((const UTF8Char*)".subjectPublicKey = ");
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
		}
	}
}

void Crypto::Cert::X509File::AppendName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	Char sbuff[12];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	UOSInt i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUOSInt(sbuff, i);
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
		{
			if (itemType == Net::ASN1Util::IT_SET)
			{
				AppendRelativeDistinguishedName(itemPDU, itemPDU + itemLen, sb, varName);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	Char sbuff[12];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	UOSInt i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUOSInt(sbuff, i);
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				AppendAttributeTypeAndDistinguishedValue(itemPDU, itemPDU + itemLen, sb, varName);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName)
{
	const UInt8 *typePDU;
	UOSInt typeLen;
	Net::ASN1Util::ItemType typeType;
	const UInt8 *valuePDU;
	UOSInt valueLen;
	Net::ASN1Util::ItemType valueType;
	typePDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &typeLen, &typeType);
	valuePDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &valueLen, &valueType);
	if (typePDU && valuePDU && typeType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendChar('.', 1);
		if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.3"))
		{
			sb->Append((const UTF8Char*)"commonName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.6"))
		{
			sb->Append((const UTF8Char*)"countryName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.7"))
		{
			sb->Append((const UTF8Char*)"localityName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.8"))
		{
			sb->Append((const UTF8Char*)"stateOrProvinceName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.10"))
		{
			sb->Append((const UTF8Char*)"organizationName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "2.5.4.11"))
		{
			sb->Append((const UTF8Char*)"organizationalUnitName");
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, "1.2.840.113549.1.9.1"))
		{
			sb->Append((const UTF8Char*)"emailAddress");
		}
		else
		{
			Net::ASN1Util::OIDToString(typePDU, typeLen, sb);
		}
		sb->Append((const UTF8Char*)" = ");
		sb->AppendC(valuePDU, valueLen);
		sb->Append((const UTF8Char*)"\r\n");
	}
}

Crypto::Cert::X509File::X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Net::ASN1Data(sourceName, buff, buffSize)
{
}

Crypto::Cert::X509File::~X509File()
{
}

Net::ASN1Data::ASN1Type Crypto::Cert::X509File::GetASN1Type()
{
	return AT_X509;
}
