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

void Crypto::Cert::X509File::AppendSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::CString name;
	Char cbuff[256];
	Char *cptr = Text::StrConcat(cbuff, path);
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrConcat(cptr, ".2");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("algorithmIdentifier");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrConcat(cptr, ".3");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("signature = "));
			sb->AppendHexBuff(itemPDU + 1, itemLen - 1, ':', Text::LineBreakType::None);
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

void Crypto::Cert::X509File::AppendTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::CString name;
	Char cbuff[256];
	Char *cptr = Text::StrConcat(cbuff, path);
	*cptr++ = '.';
	UOSInt i = 1;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
			sb->AppendC(UTF8STRC("\r\n"));
			Text::StrUOSInt(cptr, i++);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("serialNumber = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("signature");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("issuer");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("validity");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendValidity(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("subject");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUOSInt(cptr, i++);
	UOSInt itemOfst;
	if ((itemPDU = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, cbuff, &itemLen, &itemOfst)) != 0)
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("subjectPublicKeyInfo");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, name);
			Crypto::Cert::X509Key *key;
			Crypto::Cert::X509PubKey pubKey(name, itemPDU, itemOfst + itemLen);
			key = pubKey.CreateKey();
			if (key)
			{
				key->ToString(sb);
				sb->AppendLB(Text::LineBreakType::CRLF);
				DEL_CLASS(key);
			}
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_3)
		{
			name = CSTR("extensions");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendCRLExtensions(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertificate(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendTBSCertificate(pdu, pduEnd, sbuff, sb, varName);
	AppendSigned(pdu, pduEnd, path, sb, varName);
}

Bool Crypto::Cert::X509File::IsTBSCertList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
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
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_INTEGER)
	{
		Text::StrUOSInt(sptr, i++);
	}
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
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_UTCTIME)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_UTCTIME)
	{
		Text::StrUOSInt(sptr, i++);
	}
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUOSInt(sptr, i++);
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff);
	if (itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0 && itemType != Net::ASN1Util::IT_UNKNOWN)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendTBSCertList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	Data::DateTime dt;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::CString name;
	Char cbuff[256];
	Char cbuff2[20];
	Char *cptr = Text::StrConcat(cbuff, path);
	*cptr++ = '.';
	UOSInt i = 1;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subitemPDU;
	UOSInt subitemLen;
	const UInt8 *subsubitemPDU;
	UOSInt subsubitemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(pdu, pduEnd, cbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			Text::StrUOSInt(cptr, i++);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("signature");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("issuer");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("thisUpdate = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_UTCTIME)
	{
		if (Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("nextUpdate = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		Text::StrUOSInt(cptr, i++);
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UOSInt j = 0;
		while (true)
		{
			Text::StrUOSInt(cbuff2, ++j);
			if ((subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff2, &subitemLen, &itemType)) == 0 || itemType != Net::ASN1Util::IT_SEQUENCE)
			{
				break;
			}

			if ((subsubitemPDU = Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "1", &subsubitemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
			{
				if (varName.v)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
				}
				sb->AppendC(UTF8STRC("revokedCertificates["));
				sb->AppendUOSInt(j);
				sb->AppendC(UTF8STRC("].userCertificate = "));
				sb->AppendHexBuff(subsubitemPDU, subsubitemLen, ':', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if ((subsubitemPDU = Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "2", &subsubitemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(subsubitemPDU, subsubitemLen, &dt))
			{
				if (varName.v)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
				}
				sb->AppendC(UTF8STRC("revokedCertificates["));
				sb->AppendUOSInt(j);
				sb->AppendC(UTF8STRC("].revocationDate = "));
				sb->AppendDate(&dt);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			UOSInt itemOfst;
			if ((subsubitemPDU = Net::ASN1Util::PDUGetItemRAW(subitemPDU, subitemPDU + subitemLen, "3", &subsubitemLen, &itemOfst)) != 0 && subsubitemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
			{
				sptr = sbuff;
				if (varName.v)
				{
					sptr = varName.ConcatTo(sptr);
					*sptr++ = '.';
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("revokedCertificates["));
				sptr = Text::StrUOSInt(sptr, j);
				sptr = Text::StrConcatC(sptr, UTF8STRC("].crlEntryExtensions"));
				AppendCRLExtensions(subsubitemPDU, subsubitemPDU + itemOfst + subsubitemLen, sb, CSTRP(sbuff, sptr));
			}
		}
	}
	Text::StrUOSInt(cptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			name = CSTR("crlExtensions");
			if (varName.v)
			{
				sptr = varName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendCRLExtensions(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertList(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificateList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendTBSCertList(pdu, pduEnd, sbuff, sb, varName);
	AppendSigned(pdu, pduEnd, path, sb, varName);
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
	KeyType keyType = KeyType::Unknown;
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
			AppendAlgorithmIdentifier(itemPDU, itemPDU + len, sb, CSTR("privateKeyAlgorithm"), false, &keyType);
		}
	}
	Text::StrConcat(sptr, ".3");
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &len, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			sb->AppendC(UTF8STRC("privateKey = "));
			sb->AppendC(UTF8STRC("\r\n"));
			if (keyType != KeyType::Unknown)
			{
				Crypto::Cert::X509Key privkey(CSTR("PrivKey"), itemPDU, len, keyType);
				privkey.ToString(sb);
			}
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
			AppendName(itemPDU, itemPDU + itemLen, sb, CSTR("subject"));
		}
	}
	Text::StrUOSInt(sptr, i++);
	UOSInt itemOfst;
	if ((itemPDU = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, sbuff, &itemLen, &itemOfst)) != 0)
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, CSTR("subjectPublicKeyInfo"));
			Crypto::Cert::X509PubKey *pubKey;
			Crypto::Cert::X509Key *key;
			NEW_CLASS(pubKey, Crypto::Cert::X509PubKey(CSTR("PubKey"), itemPDU, itemOfst + itemLen));
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
	Text::StrUOSInt(sptr, i++);
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			const UInt8 *extOID;
			UOSInt extOIDLen;
			const UInt8 *ext;
			UOSInt extLen;
			if ((extOID = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &extOIDLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OID &&
				(ext = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &extLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SET)
			{
				if (Net::ASN1Util::OIDEqualsText(extOID, extOIDLen, UTF8STRC("1.2.840.113549.1.9.14")))
				{
					AppendCRLExtensions(ext, ext + extLen, sb, CSTR("extensionRequest"));
				}
			}
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
	AppendSigned(pdu, pduEnd, path, sb, CSTR_NULL);
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
		AppendSubjectPublicKeyInfo(buff + itemOfst, buff + itemOfst + buffSize, sb, CSTR("PubKey"));
		Crypto::Cert::X509PubKey *pubKey;
		Crypto::Cert::X509Key *key;
		NEW_CLASS(pubKey, Crypto::Cert::X509PubKey(CSTR("PubKey"), buff, itemOfst + buffSize));
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

Bool Crypto::Cert::X509File::IsContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, path) != Net::ASN1Util::IT_SEQUENCE)
		return false;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 2)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	Text::StrConcat(sptr, ".2");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UOSInt buffSize;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *buff = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &buffSize, &itemType);
	if (itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		Net::ASN1Util::ItemType itemType1;
		UOSInt contentTypeLen;
		const UInt8 *contentType = Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "1", &contentTypeLen, &itemType1);
		Net::ASN1Util::ItemType itemType2;
		UOSInt contentLen;
		const UInt8 *content = Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "2", &contentLen, &itemType2);

		if (contentType)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("content-type = "));
			Net::ASN1Util::OIDToString(contentType, contentTypeLen, sb);
			const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(contentType, contentTypeLen);
			if (oid)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendSlow((const UTF8Char*)oid->name);
				sb->AppendUTF8Char(')');
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (contentType && content)
		{
			if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.1"))) //data
			{
				UOSInt itemLen;
				const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(content, content + contentLen, "1", &itemLen, &itemType);
				if (itemPDU != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
				{
					sptr = varName.ConcatTo(sbuff);
					sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
					AppendData(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), dataType);
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.2"))) //signedData
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
				AppendPKCS7SignedData(content, content + contentLen, sb, CSTRP(sbuff, sptr));
			}
			else if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.3"))) //envelopedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.4"))) //signedAndEnvelopedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.5"))) //digestedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(contentType, contentTypeLen, UTF8STRC("1.2.840.113549.1.7.6"))) //encryptedData
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
				AppendEncryptedData(content, content + contentLen, sb, CSTRP(sbuff, sptr), dataType);
			}
		}
	}
}

Bool Crypto::Cert::X509File::IsPFX(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, path) != Net::ASN1Util::IT_SEQUENCE)
		return false;
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 2 && cnt != 3)
	{
		return false;
	}
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrConcat(sptr, ".2");
	if (!IsContentInfo(pdu, pduEnd, sbuff))
	{
		return false;
	}
	if (cnt == 3)
	{
		Text::StrConcat(sptr, ".3");
		if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
		{
			return false;
		}
	}
	return true;
}

void Crypto::Cert::X509File::AppendPFX(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt buffSize;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *buff = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &buffSize, &itemType);
	if (itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UOSInt cnt = Net::ASN1Util::PDUCountItem(buff, buff + buffSize, 0);

		Net::ASN1Util::ItemType itemType;
		UOSInt versionLen;
		const UInt8 *version = Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "1", &versionLen, &itemType);
		if (version && itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.v)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			Net::ASN1Util::IntegerToString(version, versionLen, sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		sptr = sbuff;
		if (varName.v)
		{
			sptr = varName.ConcatTo(sptr);
			*sptr++ = '.';
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("authSafe"));
		AppendContentInfo(buff, buff + buffSize, "2", sb, CSTRP(sbuff, sptr), ContentDataType::AuthenticatedSafe);
		if (cnt == 3)
		{
			sptr = sbuff;
			if (varName.v)
			{
				sptr = varName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("macData"));
			AppendMacData(buff, buff + buffSize, "3", sb, CSTRP(sbuff, sptr));
		}
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

void Crypto::Cert::X509File::AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName, Bool pubKey, KeyType *keyTypeOut)
{
	KeyType keyType = KeyType::Unknown;
	UOSInt algorithmLen;
	Net::ASN1Util::ItemType algorithmType;
	const UInt8 *algorithm = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &algorithmLen, &algorithmType);
	UOSInt parametersLen;
	Net::ASN1Util::ItemType parametersType;
	const UInt8 *parameters = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &parametersLen, &parametersType);
	if (algorithm && algorithmType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("algorithm = "));
		Net::ASN1Util::OIDToString(algorithm, algorithmLen, sb);
		keyType = KeyTypeFromOID(algorithm, algorithmLen, pubKey);
		const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(algorithm, algorithmLen);
		if (oid)
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->AppendSlow((const UTF8Char*)oid->name);
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if (parameters)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("parameters = "));
		if (parametersType == Net::ASN1Util::IT_NULL)
		{
			sb->AppendC(UTF8STRC("NULL"));
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if (keyTypeOut)
	{
		*keyTypeOut = keyType;
	}
}

void Crypto::Cert::X509File::AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	Data::DateTime dt;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if ((itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME) && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("notBefore = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if ((itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME) && Net::ASN1Util::PDUParseUTCTimeCont(itemPDU, itemLen, &dt))
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("notAfter = "));
			sb->AppendDate(&dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	KeyType keyType = KeyType::Unknown;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".algorithm"));
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), true, &keyType);
		}
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".subjectPublicKey = "));
			sb->AppendHexBuff(itemPDU + 1, itemLen - 1, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
			if (keyType != KeyType::Unknown)
			{
				sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".subjectPublicKey"));
				Crypto::Cert::X509Key pkey(CSTRP(sbuff, sptr), itemPDU + 1, itemLen - 1, keyType);
				pkey.ToString(sb);
				sb->AppendC(UTF8STRC("\r\n"));
			}
		}
	}
}

void Crypto::Cert::X509File::AppendName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
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

void Crypto::Cert::X509File::AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
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

void Crypto::Cert::X509File::AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
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
		sb->AppendUTF8Char('.');
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

void Crypto::Cert::X509File::AppendCRLExtensions(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[12];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UOSInt i = 1;
			Text::StrUOSInt(sbuff, i);
			while ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType)) != 0)
			{
				AppendCRLExtension(subItemPDU, subItemPDU + subItemLen, sb, varName);
				Text::StrUOSInt(sbuff, ++i);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendCRLExtension(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	const UInt8 *extension = 0;
	UOSInt extensionLen = 0;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	if ((extension = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &extensionLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_OID)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("extensionType = "));
			Net::ASN1Util::OIDToString(extension, extensionLen, sb);
			const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(extension, extensionLen);
			if (oid)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendSlow((const UTF8Char*)oid->name);
				sb->AppendUTF8Char(')');
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	else
	{
		return;
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_BOOLEAN)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("critical = "));
			Net::ASN1Util::BooleanToString(itemPDU, itemLen, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "3", &itemLen, &itemType)) == 0)
			{
				return;
			}
		}
		if (itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("1.3.6.1.5.5.7.1.1"))) //id-pe-authorityInfoAccess
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UOSInt i = 1;
					Text::StrUOSInt(sbuff, i);
					while ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						const UInt8 *descPDU;
						UOSInt descLen;
						if ((descPDU = Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, "1", &descLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OID)
						{
							sb->Append(varName);
							sb->AppendC(UTF8STRC(".authorityInfoAccess["));
							sb->AppendUOSInt(i);
							sb->AppendC(UTF8STRC("].accessMethod = "));
							Net::ASN1Util::OIDToString(descPDU, descLen, sb);
							sb->AppendC(UTF8STRC(" ("));
							Net::ASN1OIDDB::OIDToNameString(descPDU, descLen, sb);
							sb->AppendC(UTF8STRC(")\r\n"));
						}
						sptr = varName.ConcatTo(sbuff);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".authorityInfoAccess["));
						sptr = Text::StrUOSInt(sptr, i);
						sptr = Text::StrConcatC(sptr, UTF8STRC("].accessLocation"));
						AppendGeneralName(subItemPDU, subItemPDU + subItemLen, "2", sb, CSTRP(sbuff, sptr));
						
						Text::StrUOSInt(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.14"))) //id-ce-subjectKeyIdentifier
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
				sb->AppendC(UTF8STRC("subjectKeyId = "));
				if (itemLen == 22 && itemPDU[1] == 20)
				{
					sb->AppendHexBuff(itemPDU + 2, itemLen - 2, ':', Text::LineBreakType::None);
				}
				else
				{
					sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.15"))) //id-ce-keyUsage
			{
				if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_BIT_STRING)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("keyUsage ="));
					if (subItemLen >= 2)
					{
						if (subItemPDU[1] & 0x80) sb->AppendC(UTF8STRC(" digitalSignature"));
						if (subItemPDU[1] & 0x40) sb->AppendC(UTF8STRC(" nonRepudiation"));
						if (subItemPDU[1] & 0x20) sb->AppendC(UTF8STRC(" keyEncipherment"));
						if (subItemPDU[1] & 0x10) sb->AppendC(UTF8STRC(" dataEncipherment"));
						if (subItemPDU[1] & 0x8) sb->AppendC(UTF8STRC(" keyAgreement"));
						if (subItemPDU[1] & 0x4) sb->AppendC(UTF8STRC(" keyCertSign"));
						if (subItemPDU[1] & 0x2) sb->AppendC(UTF8STRC(" cRLSign"));
						if (subItemPDU[1] & 0x1) sb->AppendC(UTF8STRC(" encipherOnly"));
					}
					if (subItemLen >= 3)
					{
						if (subItemPDU[2] & 0x80) sb->AppendC(UTF8STRC(" decipherOnly"));
					}
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.17"))) //id-ce-subjectAltName
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".subjectAltName"));
				AppendGeneralNames(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.19"))) //id-ce-basicConstraints
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_BOOLEAN)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("basicConstraints.cA = "));
						Net::ASN1Util::BooleanToString(subItemPDU, subItemLen, sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
					if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("basicConstraints.pathLenConstraint = "));
						Net::ASN1Util::IntegerToString(subItemPDU, subItemLen, sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.20"))) //id-ce-cRLNumber
			{
				if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("cRLNumber = "));
					Net::ASN1Util::IntegerToString(subItemPDU, subItemLen, sb);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.21"))) //id-ce-cRLReasons
			{
				if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_ENUMERATED && subItemLen == 1)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("cRLReasons = "));
					switch (subItemPDU[0])
					{
					case 0:
						sb->AppendC(UTF8STRC("unspecified"));
						break;
					case 1:
						sb->AppendC(UTF8STRC("keyCompromise"));
						break;
					case 2:
						sb->AppendC(UTF8STRC("cACompromise"));
						break;
					case 3:
						sb->AppendC(UTF8STRC("affiliationChanged"));
						break;
					case 4:
						sb->AppendC(UTF8STRC("superseded"));
						break;
					case 5:
						sb->AppendC(UTF8STRC("cessationOfOperation"));
						break;
					case 6:
						sb->AppendC(UTF8STRC("certificateHold"));
						break;
					case 8:
						sb->AppendC(UTF8STRC("removeFromCRL"));
						break;
					case 9:
						sb->AppendC(UTF8STRC("privilegeWithdrawn"));
						break;
					case 10:
						sb->AppendC(UTF8STRC("aACompromise"));
						break;
					default:
						sb->AppendC(UTF8STRC("unknown"));
						break;
					}
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.31"))) //id-ce-cRLDistributionPoints
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UOSInt i = 1;
					Text::StrUOSInt(sbuff, i);
					while (AppendDistributionPoint(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
					{
						Text::StrUOSInt(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.32"))) //id-ce-certificatePolicies
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UOSInt i = 1;
					Text::StrUOSInt(sbuff, i);
					while (AppendPolicyInformation(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
					{
						Text::StrUOSInt(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.35"))) //id-ce-authorityKeyIdentifier
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UOSInt i = 1;
					Text::StrUOSInt(sbuff, i);
					while ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType)) != 0)
					{
						if (itemType == 0x80)
						{
							sb->Append(varName);
							sb->AppendUTF8Char('.');
							sb->AppendC(UTF8STRC("authorityKey.keyId = "));
							sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text::LineBreakType::None);
							sb->AppendC(UTF8STRC("\r\n"));
						}
						else if (itemType == 0x81 || itemType == 0xa1)
						{
							sptr = varName.ConcatTo(sbuff);
							sptr = Text::StrConcatC(sptr, UTF8STRC(".authorityKey.authorityCertIssuer"));
							AppendGeneralName(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTRP(sbuff, sptr));
						}
						else if (itemType == 0x82)
						{
							sb->Append(varName);
							sb->AppendUTF8Char('.');
							sb->AppendC(UTF8STRC("authorityKey.authorityCertSerialNumber = "));
							sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text::LineBreakType::None);
							sb->AppendC(UTF8STRC("\r\n"));
						}
						Text::StrUOSInt(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(extension, extensionLen, UTF8STRC("2.5.29.37"))) //id-ce-extKeyUsage
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UOSInt i = 1;
					const UInt8 *subItemPDU;
					UOSInt subItemLen;
					Text::StrUOSInt(sbuff, i);

					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("extKeyUsage ="));

					while ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType)) != 0)
					{
						if (itemType == Net::ASN1Util::IT_OID)
						{
							if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.1")))
							{
								sb->AppendC(UTF8STRC(" serverAuth"));
							}
							else if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.2")))
							{
								sb->AppendC(UTF8STRC(" clientAuth"));
							}
							else if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.3")))
							{
								sb->AppendC(UTF8STRC(" codeSigning"));
							}
							else if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.4")))
							{
								sb->AppendC(UTF8STRC(" emailProtection"));
							}
							else if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.8")))
							{
								sb->AppendC(UTF8STRC(" timeStamping"));
							}
							else if (Net::ASN1Util::OIDEqualsText(subItemPDU, subItemLen, UTF8STRC("1.3.6.1.5.5.7.3.9")))
							{
								sb->AppendC(UTF8STRC(" OCSPSigning"));
							}
						}
						Text::StrUOSInt(sbuff, ++i);
					}
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
		}
	}
}

void Crypto::Cert::X509File::AppendGeneralNames(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[11];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UOSInt i = 1;
			Text::StrUOSInt(sbuff, i);
			while (AppendGeneralName(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
			{
				Text::StrUOSInt(sbuff, ++i);
			}
		}
	}
}

Bool Crypto::Cert::X509File::AppendGeneralName(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &subItemLen, &itemType)) != 0)
	{
		switch (0x8F & (UOSInt)itemType)
		{
		case 0x80:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".otherName = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x81:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".rfc822Name = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x82:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".dNSName = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x83:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".x400Address = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x84:
			if ((subItemPDU = Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, path, &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".directoryName"));
				AppendName(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
			}
			return true;
		case 0x85:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".ediPartyName = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x86:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".uniformResourceIdentifier = "));
			sb->AppendC(subItemPDU, subItemLen);
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x87:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".iPAddress = "));
			if (subItemLen == 4)
			{
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(subItemPDU));
				sb->AppendP(sbuff, sptr);
			}
			else if (subItemLen == 16)
			{
				Net::SocketUtil::AddressInfo addr;
				Net::SocketUtil::SetAddrInfoV6(&addr, subItemPDU, 0);
				sptr = Net::SocketUtil::GetAddrName(sbuff, &addr);
				sb->AppendP(sbuff, sptr);
			}
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x88:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".registeredID = "));
			Net::ASN1Util::OIDToString(subItemPDU, subItemLen, sb);
			{
				const Net::ASN1OIDDB::OIDInfo *ent = Net::ASN1OIDDB::OIDGetEntry(subItemPDU, subItemLen);
				if (ent)
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendSlow((const UTF8Char*)ent->name);
					sb->AppendUTF8Char(')');
				}
			}
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		}
	}
	return false;
}

Bool Crypto::Cert::X509File::AppendDistributionPoint(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UOSInt i = 1;
			Text::StrUOSInt(sbuff, i);
			while ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType)) != 0)
			{
				switch ((UOSInt)itemType)
				{
				case Net::ASN1Util::IT_CONTEXT_SPECIFIC_0:
					sptr = varName.ConcatTo(sbuff);
					*sptr++ = '.';
					sptr = Text::StrConcatC(sptr, UTF8STRC("distributionPoint"));
					AppendDistributionPointName(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
					break;
				case Net::ASN1Util::IT_CONTEXT_SPECIFIC_1:
					if ((subItemPDU = Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_BIT_STRING)
					{
						sb->Append(varName);
						sb->AppendC(UTF8STRC(".reasons ="));
						if (subItemLen >= 2)
						{
							if (subItemPDU[1] & 0x80) sb->AppendC(UTF8STRC("unused"));
							if (subItemPDU[1] & 0x40) sb->AppendC(UTF8STRC("keyCompromise"));
							if (subItemPDU[1] & 0x20) sb->AppendC(UTF8STRC("cACompromise"));
							if (subItemPDU[1] & 0x10) sb->AppendC(UTF8STRC("affiliationChanged"));
							if (subItemPDU[1] & 0x8) sb->AppendC(UTF8STRC("superseded"));
							if (subItemPDU[1] & 0x4) sb->AppendC(UTF8STRC("cessationOfOperation"));
							if (subItemPDU[1] & 0x2) sb->AppendC(UTF8STRC("certificateHold"));
							if (subItemPDU[1] & 0x1) sb->AppendC(UTF8STRC("privilegeWithdrawn"));
						}
						if (subItemLen >= 3)
						{
							if (subItemPDU[2] & 0x80) sb->AppendC(UTF8STRC("aACompromise"));
						}
						sb->AppendC(UTF8STRC("\r\n"));
					}
					break;
				case Net::ASN1Util::IT_CONTEXT_SPECIFIC_2:
					sptr = varName.ConcatTo(sbuff);
					*sptr++ = '.';
					sptr = Text::StrConcatC(sptr, UTF8STRC("cRLIssuer"));
					AppendGeneralNames(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
					break;
				}
				Text::StrUOSInt(sbuff, ++i);
			}
			return true;
		}
	}
	return false;
}

void Crypto::Cert::X509File::AppendDistributionPointName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i;
	Char pathBuff[16];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0)
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text::StrConcatC(sptr, UTF8STRC("fullName"));
			i = 0;
			Text::StrUOSInt(pathBuff, ++i);
			while (AppendGeneralName(itemPDU, itemPDU + itemLen, pathBuff, sb, CSTRP(sbuff, sptr)))
			{
				Text::StrUOSInt(pathBuff, ++i);
			}
		}
		else if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text::StrConcatC(sptr, UTF8STRC("nameRelativeToCRLIssuer"));
			AppendRelativeDistinguishedName(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
		}
	}
}

Bool Crypto::Cert::X509File::AppendPolicyInformation(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[64];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType);
		if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_OID)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("policyIdentifier = "));
			Net::ASN1Util::OIDToString(subItemPDU, subItemLen, sb);
			sb->AppendC(UTF8STRC(" ("));
			Net::ASN1OIDDB::OIDToNameString(subItemPDU, subItemLen, sb);
			sb->AppendUTF8Char(')');
			sb->AppendC(UTF8STRC("\r\n"));
		}
		subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subItemLen, &itemType);
		if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			const UInt8 *policyQualifierInfoPDU;
			UOSInt policyQualifierInfoLen;
			UOSInt i = 0;
			Text::StrUOSInt(sbuff, ++i);
			while ((policyQualifierInfoPDU = Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, (const Char*)sbuff, &policyQualifierInfoLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if ((itemPDU = Net::ASN1Util::PDUGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OID)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("policyQualifiers["));
					sb->AppendUOSInt(i);
					sb->AppendC(UTF8STRC("].policyQualifierId = "));
					Net::ASN1Util::OIDToString(itemPDU, itemLen, sb);
					sb->AppendC(UTF8STRC(" ("));
					Net::ASN1OIDDB::OIDToNameString(itemPDU, itemLen, sb);
					sb->AppendUTF8Char(')');
					sb->AppendC(UTF8STRC("\r\n"));
				}
				if ((itemPDU = Net::ASN1Util::PDUGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "2", &itemLen, &itemType)) != 0)
				{
					if (itemType == Net::ASN1Util::IT_IA5STRING)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("policyQualifiers["));
						sb->AppendUOSInt(i);
						sb->AppendC(UTF8STRC("].qualifier = "));
						sb->AppendC(itemPDU, itemLen);
						sb->AppendC(UTF8STRC("\r\n"));
					}
					else if (itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						/////////////////////////////////// UserNotice
					}
				}
				Text::StrUOSInt(sbuff, ++i);
			}
		}
		return true;
	}
	return false;
}

void Crypto::Cert::X509File::AppendPKCS7SignedData(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[16];
	Net::ASN1Util::ItemType itemType;
	UOSInt itemOfst;
	UOSInt itemLen;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType);
	if (itemPDU != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UOSInt i;
		UOSInt subItemLen;
		const UInt8 *subItemPDU;
		if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->AppendC(UTF8STRC("signedData.version = "));
			Net::ASN1Util::IntegerToString(subItemPDU, subItemLen, sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SET)
		{
			AppendPKCS7DigestAlgorithmIdentifiers(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.digestAlgorithms"));
		}
		if ((subItemPDU = Net::ASN1Util::PDUGetItemRAW(itemPDU, itemPDU + itemLen, "3", &subItemLen, &itemOfst)) != 0 && subItemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendContentInfo(subItemPDU, subItemPDU + itemOfst + subItemLen, "1", sb, CSTR("signedData.contentInfo"), ContentDataType::Unknown);
		}
		i = 4;
		subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "4", &subItemLen, &itemType);
		if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.certificates"));
			Text::StrUOSInt(sbuff, ++i);
			subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType);
		}
		if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			//AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.crls"));
			Text::StrUOSInt(sbuff, ++i);
			subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, &subItemLen, &itemType);
		}
		if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_SET)
		{
			AppendPKCS7SignerInfos(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.signerInfos"));
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7DigestAlgorithmIdentifiers(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[16];
	UOSInt i;
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	i = 0;
	while (true)
	{
		Text::StrUOSInt(sbuff, ++i);
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, (const Char*)sbuff, &itemLen, &itemType)) == 0)
		{
			return;
		}
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, varName, false, 0);
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7SignerInfos(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	Char cbuff[32];
	UOSInt i;
	UOSInt itemOfst;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	i = 0;
	while (true)
	{
		Text::StrUOSInt(cbuff, ++i);
		itemPDU = Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, cbuff, &itemLen, &itemOfst);
		if (itemPDU == 0)
		{
			break;
		}
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendPKCS7SignerInfo(itemPDU, itemPDU + itemOfst + itemLen, sb, varName);
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7SignerInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Char cbuff[32];
	UOSInt i;
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType);
	if (itemPDU == 0 || itemType != Net::ASN1Util::IT_SEQUENCE)
	{
		return;
	}
	UOSInt subItemLen;
	const UInt8 *subItemPDU;
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("version = "));
		Net::ASN1Util::IntegerToString(subItemPDU, subItemLen, sb);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("issuerAndSerialNumber"));
		AppendIssuerAndSerialNumber(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
	}
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("digestAlgorithm"));
		AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	i = 4;
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "4", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("authenticatedAttributes"));
		AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		Text::StrUOSInt(cbuff, ++i);
		subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, &subItemLen, &itemType);
	}
	if (subItemPDU != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("digestEncryptionAlgorithm"));
		AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	Text::StrUOSInt(cbuff, ++i);
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("encryptedDigest = "));
		sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	Text::StrUOSInt(cbuff, ++i);
	if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("unauthenticatedAttributes"));
		AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
	}
}

void Crypto::Cert::X509File::AppendIssuerAndSerialNumber(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("issuer"));
		AppendName(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("serialNumber = "));
		sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendPKCS7Attributes(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Char cbuff[16];
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	UOSInt oidLen;
	const UInt8 *oidPDU;
	Net::ASN1Util::ItemType oidType;
	UOSInt valueLen;
	const UInt8 *valuePDU;
	UOSInt i = 0;
	while (true)
	{
		Text::StrUOSInt(cbuff, ++i);
		if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, &itemLen, &itemType)) == 0)
		{
			return;
		}
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			oidPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &oidLen, &oidType);
			valuePDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &valueLen, &itemType);
			if (oidPDU != 0 && oidType == Net::ASN1Util::IT_OID)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
				sb->AppendC(UTF8STRC("attributeType = "));
				Net::ASN1Util::OIDToString(oidPDU, oidLen, sb);
				const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(oidPDU, oidLen);
				if (oid)
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendSlow((const UTF8Char*)oid->name);
					sb->AppendUTF8Char(')');
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (valuePDU && itemType == Net::ASN1Util::IT_SET)
			{
				if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.3"))) //contentType
				{
					if ((itemPDU = Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OID)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("contentType = "));
						Net::ASN1Util::OIDToString(itemPDU, itemLen, sb);
						const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(itemPDU, itemLen);
						if (oid)
						{
							sb->AppendC(UTF8STRC(" ("));
							sb->AppendSlow((const UTF8Char*)oid->name);
							sb->AppendUTF8Char(')');
						}
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
				else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.4"))) //messageDigest
				{
					if ((itemPDU = Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("messageDigest = "));
						sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
				else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.5"))) //signing-time
				{
					if ((itemPDU = Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_UTCTIME)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("signing-time = "));
						Net::ASN1Util::UTCTimeToString(itemPDU, itemLen, sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
				else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.15"))) //smimeCapabilities
				{
					/////////////////////////////////////
				}
				else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.2.840.113549.1.9.16.2.11"))) //id-aa-encrypKeyPref
				{
					if ((itemPDU = Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
					{
						sptr = varName.ConcatTo(sbuff);
						*sptr++ = '.';
						sptr = Text::StrConcatC(sptr, UTF8STRC("encrypKeyPref"));
						AppendIssuerAndSerialNumber(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
					}
				}
				else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("1.3.6.1.4.1.311.16.4"))) //outlookExpress
				{
					if ((itemPDU = Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						sptr = varName.ConcatTo(sbuff);
						*sptr++ = '.';
						sptr = Text::StrConcatC(sptr, UTF8STRC("outlookExpress"));
						AppendIssuerAndSerialNumber(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
					}
				}
				
			}
		}
	}
}

Bool Crypto::Cert::X509File::AppendMacData(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subItemPDU;
	UOSInt subItemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".mac"));
			AppendDigestInfo(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		}
		if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("macSalt = "));
			sb->AppendHexBuff(subItemPDU, subItemLen, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if ((subItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", &subItemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("iterations = "));
			Net::ASN1Util::IntegerToString(subItemPDU, subItemLen, sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		return true;
	}
	return false;
}

void Crypto::Cert::X509File::AppendDigestInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".digestAlgorithm"));
		AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".digest = "));
		sb->AppendHexBuff(itemPDU, itemLen, ' ', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendData(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName, ContentDataType dataType)
{
	switch (dataType)
	{
	case ContentDataType::AuthenticatedSafe:
		AppendAuthenticatedSafe(pdu, pduEnd, sb, varName);
		break;
	case ContentDataType::Unknown:
	default:
		break;
	}
}

void Crypto::Cert::X509File::AppendEncryptedData(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	const UInt8 *itemPDU;
	UOSInt itemLen;
	const UInt8 *subitemPDU;
	UOSInt subitemLen;
	Net::ASN1Util::ItemType itemType;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		if (varName.v)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
		}
		sb->AppendC(UTF8STRC("version = "));
		AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
		sb->AppendC(UTF8STRC("\r\n"));

		if ((subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", &subitemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = sbuff;
			if (varName.v)
			{
				sptr = varName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("encryptedContentInfo"));
			AppendEncryptedContentInfo(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr), dataType);
		}
		if ((subitemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", &subitemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			sptr = sbuff;
			if (varName.v)
			{
				sptr = varName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("unprotectedAttributes"));
			AppendPKCS7Attributes(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr));
		}
	}
}

void Crypto::Cert::X509File::AppendAuthenticatedSafe(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Char cbuff[16];
	const UInt8 *itemPDU;
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	UOSInt i;
	UOSInt j;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		i = 0;
		j = Net::ASN1Util::PDUCountItem(itemPDU, itemPDU + itemLen, 0);
		while (i < j)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '[';
			sptr = Text::StrUOSInt(sptr, i);
			*sptr++ = ']';
			*sptr = 0;
			Text::StrUOSInt(cbuff, ++i);
			AppendContentInfo(itemPDU, itemPDU + itemLen, cbuff, sb, CSTRP(sbuff, sptr), ContentDataType::Unknown);
		}
	}
}

void Crypto::Cert::X509File::AppendEncryptedContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF8 *sb, Text::CString varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Net::ASN1Util::ItemType itemType;
	UOSInt itemLen;
	const UInt8 *itemPDU;
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".contentType = "));
		Net::ASN1Util::OIDToString(itemPDU, itemLen, sb);
		const Net::ASN1OIDDB::OIDInfo *oid = Net::ASN1OIDDB::OIDGetEntry(itemPDU, itemLen);
		if (oid)
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->AppendSlow((const UTF8Char*)oid->name);
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", &itemLen, &itemType)) != 0 && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = sbuff;
		if (varName.v)
		{
			sptr = varName.ConcatTo(sptr);
			*sptr++ = '.';
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("contentEncryptionAlgorithm"));
		AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "3", &itemLen, &itemType)) != 0 && (itemType & 0x8F) == 0x80)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".encryptedContent = "));
		sb->AppendHexBuff(itemPDU, itemLen, ' ', Text::LineBreakType::None);
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
					if (strPDU && itemType == Net::ASN1Util::IT_BOOLEAN)
					{
						strPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", &strLen, &itemType);
					}
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
						else if (Net::ASN1Util::OIDEqualsText(oidPDU, oidLen, UTF8STRC("2.5.29.15"))) //id-ce-keyUsage
						{
							subItemPDU = Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1", &subItemLen, &itemType);
							if (subItemPDU && itemType == Net::ASN1Util::IT_BIT_STRING && subItemLen >= 2)
							{
								if (subItemPDU[1] & 6)
								{
									ext->caCert = true;
								}
								if (subItemPDU[1] & 0x80)
								{
									ext->digitalSign = true;
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}

Crypto::Cert::X509Key *Crypto::Cert::X509File::PublicKeyGetNew(const UInt8 *pdu, const UInt8 *pduEnd)
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
		if (keyType == KeyType::ECPublic)
		{
			Net::ASN1Util::ItemType paramType;
			UOSInt paramLen;
			const UInt8 *paramPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.2", &paramLen, &paramType);
			if (paramPDU != 0 && paramType == Net::ASN1Util::IT_OID)
			{
				///////////////////////////////////////
//				Crypto::Cert::X509Key *key;
//				NEW_CLASS(key, Crypto::Cert::X509Key(CSTR("public.key"), bstrPDU + 1, bstrLen - 1, keyType));
//				return key;
			}
		}
		else if (keyType != KeyType::Unknown)
		{
			Crypto::Cert::X509Key *key;
			NEW_CLASS(key, Crypto::Cert::X509Key(CSTR("public.key"), bstrPDU + 1, bstrLen - 1, keyType));
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
	case KeyType::ECPublic:
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
	else if (Net::ASN1Util::OIDEqualsText(oid, oidLen, UTF8STRC("1.2.840.10045.2.1")))
	{
		return KeyType::ECPublic;
	}
	return KeyType::Unknown;
}

Crypto::Hash::HashType Crypto::Cert::X509File::HashTypeFromOID(const UInt8 *oid, UOSInt oidLen)
{
	if (Net::ASN1Util::OIDEqualsText(oid, oidLen, UTF8STRC("2.16.840.1.101.3.4.2.1")))
	{
		return Crypto::Hash::HT_SHA256;
	}
	return Crypto::Hash::HT_UNKNOWN;
}

Bool Crypto::Cert::X509File::AlgorithmIdentifierGet(const UInt8 *pdu, const UInt8 *pduEnd, AlgType *algType)
{
	UOSInt cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, 0);
	if (cnt != 2 && cnt != 1)
	{
		return false;
	}
	UOSInt itemLen;
	Net::ASN1Util::ItemType itemType;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", &itemLen, &itemType);
	if (itemPDU == 0 || itemType != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.2"))) //md2WithRSAEncryption
	{
		*algType = AlgType::MD2WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.4"))) //md5WithRSAEncryption
	{
		*algType = AlgType::MD5WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.5"))) //sha1WithRSAEncryption
	{
		*algType = AlgType::SHA1WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.11"))) //sha256WithRSAEncryption
	{
		*algType = AlgType::SHA256WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.12"))) //sha384WithRSAEncryption
	{
		*algType = AlgType::SHA384WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.13"))) //sha512WithRSAEncryption
	{
		*algType = AlgType::SHA512WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.113549.1.1.14"))) //sha224WithRSAEncryption
	{
		*algType = AlgType::SHA224WithRSAEncryption;
	}
	else if (Net::ASN1Util::OIDEqualsText(itemPDU, itemLen, UTF8STRC("1.2.840.10045.4.3.2"))) //ecdsa-with-SHA256
	{
		*algType = AlgType::ECDSAWithSHA256;
	}
	else
	{
		*algType = AlgType::Unknown;
	}

	return true;
}

Crypto::Cert::X509File::X509File(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : Net::ASN1Data(sourceName, buff, buffSize)
{
}

Crypto::Cert::X509File::X509File(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize) : Net::ASN1Data(sourceName, buff, buffSize)
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

Crypto::Cert::X509Cert *Crypto::Cert::X509File::GetNewCert(UOSInt index)
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

Bool Crypto::Cert::X509File::GetSignedInfo(SignedInfo *signedInfo)
{
	UOSInt itemLen;
	UOSInt itemOfst;
	Net::ASN1Util::ItemType itemType;
	signedInfo->payload = Net::ASN1Util::PDUGetItemRAW(this->buff, this->buff + this->buffSize, "1.1", &itemLen, &itemOfst);
	if (signedInfo->payload == 0)
	{
		return false;
	}
	signedInfo->payloadSize = itemLen + itemOfst;
	const UInt8 *itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.2", &itemLen, &itemType);
	if (itemPDU == 0 || itemType != Net::ASN1Util::IT_SEQUENCE || !AlgorithmIdentifierGet(itemPDU, itemPDU + itemLen, &signedInfo->algType))
	{
		return false;
	}
	if ((itemPDU = Net::ASN1Util::PDUGetItem(this->buff, this->buff + this->buffSize, "1.3", &itemLen, &itemType)) == 0 || itemType != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	signedInfo->signature = itemPDU + 1;
	signedInfo->signSize = itemLen - 1;
	return true;
}

Crypto::Hash::HashType Crypto::Cert::X509File::GetAlgHash(AlgType algType)
{
	switch (algType)
	{
	case AlgType::SHA1WithRSAEncryption:
		return Crypto::Hash::HT_SHA1;
	case AlgType::SHA256WithRSAEncryption:
		return Crypto::Hash::HT_SHA256;
	case AlgType::SHA512WithRSAEncryption:
		return Crypto::Hash::HT_SHA512;
	case AlgType::SHA384WithRSAEncryption:
		return Crypto::Hash::HT_SHA384;
	case AlgType::SHA224WithRSAEncryption:
		return Crypto::Hash::HT_SHA224;
	case AlgType::MD2WithRSAEncryption:
		return Crypto::Hash::HT_UNKNOWN;
	case AlgType::MD5WithRSAEncryption:
		return Crypto::Hash::HT_MD5;
	case AlgType::ECDSAWithSHA256:
		return Crypto::Hash::HT_SHA256;
	case AlgType::Unknown:
	default:
		return Crypto::Hash::HT_UNKNOWN;
	}
}

Text::CString Crypto::Cert::X509File::FileTypeGetName(FileType fileType)
{
	switch (fileType)
	{
	case FileType::Cert:
		return CSTR("Cert");
	case FileType::CertRequest:
		return CSTR("CertReq");
	case FileType::Key:
		return CSTR("Key");
	case FileType::PrivateKey:
		return CSTR("PrivateKey");
	case FileType::PublicKey:
		return CSTR("PublicKey");
	case FileType::PKCS7:
		return CSTR("PKCS7");
	case FileType::PKCS12:
		return CSTR("PKCS12");
	case FileType::CRL:
		return CSTR("CRL");
	default:
		return CSTR("Unknown");
	}
}

Text::CString Crypto::Cert::X509File::KeyTypeGetName(KeyType keyType)
{
	switch (keyType)
	{
	case KeyType::RSA:
		return CSTR("RSA");
	case KeyType::DSA:
		return CSTR("DSA");
	case KeyType::ECDSA:
		return CSTR("ECDSA");
	case KeyType::ED25519:
		return CSTR("ED25519");
	case KeyType::RSAPublic:
		return CSTR("RSAPublic");
	case KeyType::ECPublic:
		return CSTR("ECPublic");
	case KeyType::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Text::CString Crypto::Cert::X509File::KeyTypeGetOID(KeyType keyType)
{
	switch (keyType)
	{
	case KeyType::RSA:
		return CSTR("1.2.840.113549.1.1.1");
	case KeyType::DSA:
		return CSTR("1.2.840.10040.4.1");
	case KeyType::ECDSA:
		return CSTR("1.2.840.10045.2.1");
	case KeyType::ED25519:
		return CSTR("1.3.101.112");
	case KeyType::ECPublic:
		return CSTR("1.2.840.10045.2.1");
	case KeyType::RSAPublic:
	case KeyType::Unknown:
	default:
		return CSTR("1.2.840.113549.1.1.1");
	}
}

Text::CString Crypto::Cert::X509File::ValidStatusGetName(ValidStatus validStatus)
{
	switch (validStatus)
	{
	case ValidStatus::Valid:
		return CSTR("Valid");
	case ValidStatus::SelfSigned:
		return CSTR("SelfSigned");
	case ValidStatus::SignatureInvalid:
		return CSTR("SignatureInvalid");
	case ValidStatus::Revoked:
		return CSTR("Revoked");
	case ValidStatus::FileFormatInvalid:
		return CSTR("FileFormatInvalid");
	case ValidStatus::UnknownIssuer:
		return CSTR("UnknownIssuer");
	case ValidStatus::Expired:
		return CSTR("Expired");
	case ValidStatus::UnsupportedAlgorithm:
		return CSTR("UnsupportedAlgorithm");
	default:
		return CSTR("Unknown");
	}
}

Text::CString Crypto::Cert::X509File::ValidStatusGetDesc(ValidStatus validStatus)
{
	switch (validStatus)
	{
	case ValidStatus::Valid:
		return CSTR("Valid");
	case ValidStatus::SelfSigned:
		return CSTR("Self-Signed Certificate");
	case ValidStatus::SignatureInvalid:
		return CSTR("Signature Invalid");
	case ValidStatus::Revoked:
		return CSTR("Certificate Revoked");
	case ValidStatus::FileFormatInvalid:
		return CSTR("File Format Invalid");
	case ValidStatus::UnknownIssuer:
		return CSTR("Unknown Issuer");
	case ValidStatus::Expired:
		return CSTR("Certificate Expired");
	case ValidStatus::UnsupportedAlgorithm:
		return CSTR("Unsupported Algorithm");
	default:
		return CSTR("Unknown");
	}

}
