#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"

void Crypto::Cert::CertNames::FreeNames(CertNames *names)
{
	SDEL_STRING(names->countryName);
	SDEL_STRING(names->stateOrProvinceName);
	SDEL_STRING(names->localityName);
	SDEL_STRING(names->organizationName);
	SDEL_STRING(names->organizationUnitName);
	SDEL_STRING(names->commonName);
	SDEL_STRING(names->emailAddress);
}

void Crypto::Cert::CertExtensions::FreeExtensions(CertExtensions *ext)
{
	if (ext->subjectAltName)
	{
		LIST_FREE_STRING(ext->subjectAltName);
		DEL_CLASS(ext->subjectAltName);
		ext->subjectAltName = 0;
	}
	if (ext->issuerAltName)
	{
		LIST_FREE_STRING(ext->issuerAltName);
		DEL_CLASS(ext->issuerAltName);
		ext->issuerAltName = 0;
	}
}

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

void Crypto::Cert::X509File::AppendSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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
			sb->AppendC(UTF8STRC("signature = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
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

void Crypto::Cert::X509File::AppendTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
			sb->AppendC(UTF8STRC("\r\n"));
			Text::StrUOSInt(sptr, i++);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->AppendC(UTF8STRC("serialNumber = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
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
	UOSInt itemOfst;
	if ((itemPDU = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, sbuff, &itemLen, &itemOfst)) != 0)
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, (const UTF8Char*)"subjectPublicKeyInfo");
			Crypto::Cert::X509PubKey *pubKey;
			Crypto::Cert::X509Key *key;
			NEW_CLASS(pubKey, Crypto::Cert::X509PubKey((const UTF8Char*)"PubKey", itemPDU, itemOfst + itemLen));
			key = pubKey->CreateKey();
			if (key)
			{
				key->ToString(sb);
				sb->AppendLB(Text::LineBreakType::CRLF);
				DEL_CLASS(key);
			}
			DEL_CLASS(pubKey);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertificate(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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

void Crypto::Cert::X509File::AppendPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
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
			sb->AppendC(UTF8STRC("privateKey = "));
			sb->AppendC(UTF8STRC("\r\n"));
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

void Crypto::Cert::X509File::AppendCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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
			sb->AppendC(UTF8STRC("serialNumber = "));
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
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
	UOSInt itemOfst;
	if ((itemPDU = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, sbuff, &itemLen, &itemOfst)) != 0)
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, (const UTF8Char*)"subjectPublicKeyInfo");
			Crypto::Cert::X509PubKey *pubKey;
			Crypto::Cert::X509Key *key;
			NEW_CLASS(pubKey, Crypto::Cert::X509PubKey((const UTF8Char*)"PubKey", itemPDU, itemOfst + itemLen));
			key = pubKey->CreateKey();
			if (key)
			{
				key->ToString(sb);
				sb->AppendLB(Text::LineBreakType::CRLF);
				DEL_CLASS(key);
			}
			DEL_CLASS(pubKey);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsCertificateRequestInfo(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendCertificateRequestInfo(pdu, pduEnd, sbuff, sb);
	AppendSigned(pdu, pduEnd, path, sb);
}

Bool Crypto::Cert::X509File::IsPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 2)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrConcat(sptr, ".2");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
{
	UOSInt itemOfst;
	UOSInt buffSize;
	const UInt8 *buff = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, path, &buffSize, &itemOfst);
	if (buff[0] == Net::ASN1Util::IT_SEQUENCE)
	{
		AppendSubjectPublicKeyInfo(buff + itemOfst, buff + itemOfst + buffSize, sb, (const UTF8Char*)"PubKey");
		Crypto::Cert::X509PubKey *pubKey;
		Crypto::Cert::X509Key *key;
		NEW_CLASS(pubKey, Crypto::Cert::X509PubKey((const UTF8Char*)"PubKey", buff, itemOfst + buffSize));
		key = pubKey->CreateKey();
		if (key)
		{
			key->ToString(sb);
			sb->AppendLB(Text::LineBreakType::CRLF);
			DEL_CLASS(key);
		}
		DEL_CLASS(pubKey);
	}
}

void Crypto::Cert::X509File::AppendVersion(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb)
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
				sb->AppendC(UTF8STRC("v1"));
				break;
			case 1:
				sb->AppendC(UTF8STRC("v2"));
				break;
			case 2:
				sb->AppendC(UTF8STRC("v3"));
				break;
			}
		}
	}
}

void Crypto::Cert::X509File::AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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
		sb->AppendC(UTF8STRC("algorithm = "));
		Net::ASN1Util::OIDToString(algorithm, algorithmLen, sb);
		const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(algorithm, algorithmLen);
		if (oid)
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->Append((const UTF8Char*)oid->name);
			sb->AppendChar(')', 1);
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if (parameters)
	{
		sb->Append(varName);
		sb->AppendChar('.', 1);
		sb->AppendC(UTF8STRC("parameters = "));
		if (parametersType == Net::ASN1Util::IT_NULL)
		{
			sb->AppendC(UTF8STRC("NULL"));
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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
			sb->AppendC(UTF8STRC("notBefore = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			sb->Append(varName);
			sb->AppendChar('.', 1);
			sb->AppendC(UTF8STRC("notBefore = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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
			sb->AppendC(UTF8STRC(".subjectPublicKey = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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

void Crypto::Cert::X509File::AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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

void Crypto::Cert::X509File::AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, const UTF8Char *varName)
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
		if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.3")))
		{
			sb->AppendC(UTF8STRC("commonName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.6")))
		{
			sb->AppendC(UTF8STRC("countryName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.7")))
		{
			sb->AppendC(UTF8STRC("localityName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.8")))
		{
			sb->AppendC(UTF8STRC("stateOrProvinceName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.10")))
		{
			sb->AppendC(UTF8STRC("organizationName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("2.5.4.11")))
		{
			sb->AppendC(UTF8STRC("organizationalUnitName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(typePDU, typeLen, UTF8STRC("1.2.840.113549.1.9.1")))
		{
			sb->AppendC(UTF8STRC("emailAddress"));
		}
		else
		{
			Net::ASN1Util::OIDToString(typePDU, typeLen, sb);
		}
		sb->AppendC(UTF8STRC(" = "));
		sb->AppendC(valuePDU, valueLen);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

Bool Crypto::Cert::X509File::NameGetByOID(const UInt8 *pdu, const UInt8 *pduEnd, const UTF8Char *oidText, UOSInt oidTextLen, Text::StringBuilderUTF8 *sb)
{
	Char sbuff[12];
	const UInt8 *itemPDU;
	const UInt8 *oidPDU;
	const UInt8 *strPDU;
	UOSInt itemLen;
	UOSInt oidLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	UOSInt i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrConcat(Text::StrUOSInt(sbuff, i), ".1");
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				oidPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &oidLen, &itemType);
				if (oidPDU != 0 && itemType == Net::ASN1Util::IT_OID && Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, oidText, oidTextLen))
				{
					strPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &oidLen, &itemType);
					if (strPDU)
					{
						sb->AppendC(strPDU, oidLen);
						return true;
					}
				}
			}
		}
	}
	return false;
}

Bool Crypto::Cert::X509File::NameGetCN(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb)
{
	return NameGetByOID(pdu, pduEnd, UTF8STRC("2.5.4.3"), sb);
}

Bool Crypto::Cert::X509File::NamesGet(const UInt8 *pdu, const UInt8 *pduEnd, CertNames *names)
{
	Char sbuff[12];
	const UInt8 *itemPDU;
	const UInt8 *oidPDU;
	const UInt8 *strPDU;
	UOSInt itemLen;
	UOSInt oidLen;
	UOSInt strLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	UOSInt i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrConcat(Text::StrUOSInt(sbuff, i), ".1");
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0)
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				oidPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &oidLen, &itemType);
				if (oidPDU != 0 && itemType == Net::ASN1Util::IT_OID)
				{
					strPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &strLen, &itemType);
					if (strPDU)
					{
						if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.6")))
						{
							SDEL_STRING(names->countryName);
							names->countryName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.8")))
						{
							SDEL_STRING(names->stateOrProvinceName);
							names->stateOrProvinceName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.7")))
						{
							SDEL_STRING(names->localityName);
							names->localityName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.10")))
						{
							SDEL_STRING(names->organizationName);
							names->organizationName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.11")))
						{
							SDEL_STRING(names->organizationUnitName);
							names->organizationUnitName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.4.3")))
						{
							SDEL_STRING(names->commonName);
							names->commonName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.1")))
						{
							SDEL_STRING(names->emailAddress);
							names->emailAddress = Text::String::New(strPDU, strLen);
						}
					}
				}
			}
		}
	}
	return true;
}

Bool Crypto::Cert::X509File::ExtensionsGet(const UInt8 *pdu, const UInt8 *pduEnd, CertExtensions *ext)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	const UInt8 *oidPDU;
	const UInt8 *strPDU;
	const UInt8 *subItemPDU;
	UOSInt itemLen;
	UOSInt oidLen;
	UOSInt strLen;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	UOSInt i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUOSInt(sbuff, i);
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, (const Char*)sbuff, &itemLen, &itemType)) != 0)
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				oidPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &oidLen, &itemType);
				if (oidPDU != 0 && itemType == Net::ASN1Util::IT_OID)
				{
					strPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &strLen, &itemType);
					if (strPDU && itemType == Net::ASN1Util::IT_OCTET_STRING)
					{
						if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.29.17"))) //id-ce-subjectAltName
						{
							if (ext->subjectAltName)
							{
								LIST_FREE_STRING(ext->subjectAltName);
								SDEL_CLASS(ext->subjectAltName)
							}
							NEW_CLASS(ext->subjectAltName, Data::ArrayList<Text::String*>());
							UOSInt j = 0;
							UOSInt k = Net::ASN1Util::PDUCountItem(strPDU, strPDU + strLen, "1");
							while (j < k)
							{
								j++;
								Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.")), j);
								subItemPDU = Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, (const Char*)sbuff, &subItemLen, &itemType);
								if (subItemPDU)
								{
									if (itemType == 0x87)
									{
										sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(subItemPDU));
										ext->subjectAltName->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
									}
									else
									{
										ext->subjectAltName->Add(Text::String::New(subItemPDU, subItemLen));
									}
								}
							}
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.29.14"))) //id-ce-subjectKeyIdentifier
						{
							subItemPDU = Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1", &subItemLen, &itemType);
							if (subItemPDU && subItemLen == 20)
							{
								ext->useSubjKeyId = true;
								MemCopyNO(ext->subjKeyId, subItemPDU, subItemLen);
							}
						}
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.29.35"))) //id-ce-authorityKeyIdentifier
						{
							subItemPDU = Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1.1", &subItemLen, &itemType);
							if (subItemPDU && subItemLen == 20)
							{
								ext->useAuthKeyId = true;
								MemCopyNO(ext->authKeyId, subItemPDU, subItemLen);
							}
						}
					}
				}
			}
		}
	}
	return true;
}

Crypto::Cert::X509Key *Crypto::Cert::X509File::PublicKeyGet(const UInt8 *pdu, const UInt8 *pduEnd)
{
	Net::ASN1Util::ItemType oidType;
	UOSInt oidLen;
	const UInt8 *oidPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.1", &oidLen, &oidType);
	Net::ASN1Util::ItemType bstrType;
	UOSInt bstrLen;
	const UInt8 *bstrPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &bstrLen, &bstrType);
	if (oidPDU != 0 && oidType == Net::ASN1Util::IT_OID && bstrPDU != 0 && bstrType == Net::ASN1Util::IT_BIT_STRING)
	{
		KeyType keyType = KeyTypeFromOID(oidPDU, oidLen, true);
		if (keyType != KeyType::Unknown)
		{
			if (bstrPDU[0] == 0)
			{
				bstrPDU++;
				bstrLen--;
			}
			Crypto::Cert::X509Key *key;
			NEW_CLASS(key, Crypto::Cert::X509Key((const UTF8Char*)"public.key", bstrPDU, bstrLen, keyType));
			return key;
		}
	}
	return 0;
}

UOSInt Crypto::Cert::X509File::KeyGetLeng(const UInt8 *pdu, const UInt8 *pduEnd, KeyType keyType)
{
	const UTF8Char *keyPDU;
	UOSInt keyLen;
	Net::ASN1Util::ItemType itemType;
	switch (keyType)
	{
	case KeyType::RSA:
		keyPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &keyLen, &itemType);
		if (keyPDU && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UOSInt cnt = Net::ASN1Util::PDUCountItem(keyPDU, keyPDU + keyLen, 0);
			if (cnt > 4)
			{
				UOSInt modulusLen;
				const UInt8 *modulus = Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "2", &modulusLen, &itemType);
				UOSInt privateExponentLen;
				const UInt8 *privateExponent = Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "4", &privateExponentLen, &itemType);
				if (modulus && privateExponent)
				{
					return (modulusLen - 1) << 3;
				}
			}
		}
		return 0;
	case KeyType::RSAPublic:
		if (pdu[0] == 0)
		{
			pdu++;
		}
		keyPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &keyLen, &itemType);
		if (keyPDU && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UOSInt modulusLen;
			const UInt8 *modulus = Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "1", &modulusLen, &itemType);
			if (modulus)
			{
				return (modulusLen - 1) << 3;
			}
		}
		return 0;
	case KeyType::DSA:
	case KeyType::ECDSA:
	case KeyType::ED25519:
	case KeyType::Unknown:
	default:
		return 0;
	}
}

Crypto::Cert::X509File::KeyType Crypto::Cert::X509File::KeyTypeFromOID(const UInt8 *oid, UOSInt oidLen, Bool pubKey)
{
	if (Net::ASN1Util::OIDEqualsText(oid, oidLen, UTF8STRC("1.2.840.113549.1.1.1")))
	{
		if (pubKey)
		{
			return KeyType::RSAPublic;
		}
		else
		{
			return KeyType::RSA;
		}
	}
	return KeyType::Unknown;
}

Crypto::Cert::X509File::X509File(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Net::ASN1Data(sourceName, buff, buffSize)
{
}

Crypto::Cert::X509File::X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : Net::ASN1Data(sourceName, buff, buffSize)
{
}

Crypto::Cert::X509File::~X509File()
{
}

Net::ASN1Data::ASN1Type Crypto::Cert::X509File::GetASN1Type()
{
	return ASN1Type::X509;
}

UOSInt Crypto::Cert::X509File::GetCertCount()
{
	return 0;
}

Bool Crypto::Cert::X509File::GetCertName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	return false;
}

Crypto::Cert::X509Cert *Crypto::Cert::X509File::NewCert(UOSInt index)
{
	return 0;
}

void Crypto::Cert::X509File::ToShortString(Text::StringBuilderUTF8 *sb)
{
	sb->Append(FileTypeGetName(this->GetFileType()));
	sb->AppendC(UTF8STRC(": "));
	this->ToShortName(sb);
}

Bool Crypto::Cert::X509File::IsSignatureKey(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key)
{
	UOSInt itemOfst;
	UOSInt dataSize;
	const UInt8 *data = Net::ASN1Util::PDUGetItemRAW(this->buff, this->buff + this->buffSize, "1.1", &dataSize, &itemOfst);
	Net::ASN1Util::ItemType itemType;
	UOSInt signSize;
	const UInt8 *signature = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", &signSize, &itemType);
	if (data == 0 || signature == 0 || itemType != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	if (signature[0] == 0)
	{
		signature++;
		signSize--;
	}
	UOSInt mySignSize;
	UInt8 mySignature[256];
	if (!ssl->Signature(key, Crypto::Hash::HT_SHA256, data, dataSize + itemOfst, mySignature, &mySignSize))
	{
		return false;
	}
	if (signSize != mySignSize)
	{
		return false;
	}
	while (signSize-- > 0)
	{
		if (signature[signSize] != mySignature[signSize])
		{
			return false;
		}
	}
	return true;
}

const UTF8Char *Crypto::Cert::X509File::FileTypeGetName(FileType fileType)
{
	switch (fileType)
	{
	case FileType::Cert:
		return (const UTF8Char*)"Cert";
	case FileType::CertRequest:
		return (const UTF8Char*)"CertReq";
	case FileType::Key:
		return (const UTF8Char*)"Key";
	case FileType::PrivateKey:
		return (const UTF8Char*)"PrivateKey";
	case FileType::Jks:
		return (const UTF8Char*)"JavaKeyStore";
	case FileType::PublicKey:
		return (const UTF8Char*)"PublicKey";
	case FileType::PKCS7:
		return (const UTF8Char*)"PKCS7";
	case FileType::PKCS12:
		return (const UTF8Char*)"PKCS12";
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Crypto::Cert::X509File::KeyTypeGetName(KeyType keyType)
{
	switch (keyType)
	{
	case KeyType::RSA:
		return (const UTF8Char*)"RSA";
	case KeyType::DSA:
		return (const UTF8Char*)"DSA";
	case KeyType::ECDSA:
		return (const UTF8Char*)"ECDSA";
	case KeyType::ED25519:
		return (const UTF8Char*)"ED25519";
	case KeyType::RSAPublic:
		return (const UTF8Char*)"RSAPublic";
	case KeyType::Unknown:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

Text::CString Crypto::Cert::X509File::KeyTypeGetOID(KeyType keyType)
{
	switch (keyType)
	{
	case KeyType::RSA:
		return {UTF8STRC("1.2.840.113549.1.1.1")};
	case KeyType::DSA:
		return {UTF8STRC("1.2.840.10040.4.1")};
	case KeyType::ECDSA:
		return {UTF8STRC("1.2.840.10045.2.1")};
	case KeyType::ED25519:
		return {UTF8STRC("1.3.101.112")};
	case KeyType::RSAPublic:
	case KeyType::Unknown:
	default:
		return {UTF8STRC("1.2.840.113549.1.1.1")};
	}
}
