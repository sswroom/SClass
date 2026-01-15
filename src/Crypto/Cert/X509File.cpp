#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Cert/X509File.h"
#include "Crypto/Cert/X509FileList.h"
#include "Crypto/Cert/X509PubKey.h"
#include "Core/ByteTool_C.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SSLEngine.h"
#include "Text/MyStringW.h"

void Crypto::Cert::CertNames::FreeNames(NN<CertNames> names)
{
	OPTSTR_DEL(names->countryName);
	OPTSTR_DEL(names->stateOrProvinceName);
	OPTSTR_DEL(names->localityName);
	OPTSTR_DEL(names->organizationName);
	OPTSTR_DEL(names->organizationUnitName);
	OPTSTR_DEL(names->commonName);
	OPTSTR_DEL(names->emailAddress);
}

void Crypto::Cert::CertExtensions::FreeExtensions(NN<CertExtensions> ext)
{
	NN<Data::ArrayListStringNN> strList;
	if (ext->subjectAltName.SetTo(strList))
	{
		strList->FreeAll();
		strList.Delete();
		ext->subjectAltName = nullptr;
	}
	if (ext->issuerAltName.SetTo(strList))
	{
		strList->FreeAll();
		strList.Delete();
		ext->issuerAltName = nullptr;
	}
}

Bool Crypto::Cert::X509File::IsSigned(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 3)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
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

void Crypto::Cert::X509File::AppendSigned(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN name;
	Text::CStringNN nnvarName;
	Char cbuff[256];
	UnsafeArray<Char> cptr = Text::StrConcat(cbuff, path);
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrConcat(cptr, ".2");
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("algorithmIdentifier");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrConcat(cptr, ".3");
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_BIT_STRING)
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("signature = "));
			sb->AppendHexBuff(itemPDU + 1, itemLen - 1, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

Bool Crypto::Cert::X509File::IsTBSCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 6)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UIntOS i = 1;
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		Text::StrUIntOS(sptr, i++);
	}
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendTBSCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN name;
	Text::CStringNN nnvarName;
	Char cbuff[256];
	UnsafeArray<Char> cptr = Text::StrConcat(cbuff, path);
	*cptr++ = '.';
	UIntOS i = 1;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
			sb->AppendC(UTF8STRC("\r\n"));
			Text::StrUIntOS(cptr, i++);
		}
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("serialNumber = "));
			sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("signature");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("issuer");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("validity");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendValidity(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("subject");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUIntOS(cptr, i++);
	UIntOS itemOfst;
	if (Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, cbuff, itemLen, itemOfst).SetTo(itemPDU))
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("subjectPublicKeyInfo");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, name);
			NN<Crypto::Cert::X509Key> key;
			Crypto::Cert::X509PubKey pubKey(name, Data::ByteArrayR(itemPDU, itemOfst + itemLen));
			if (pubKey.CreateKey().SetTo(key))
			{
				key->ToString(sb);
				sb->AppendLB(Text::LineBreakType::CRLF);
				key.Delete();
			}
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_3)
		{
			name = CSTR("extensions");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendCRLExtensions(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertificate(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendTBSCertificate(pdu, pduEnd, sbuff, sb, varName);
	AppendSigned(pdu, pduEnd, path, sb, varName);
}

Bool Crypto::Cert::X509File::IsTBSCertList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 4)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UIntOS i = 1;
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_INTEGER)
	{
		Text::StrUIntOS(sptr, i++);
	}
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_UTCTIME)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) == Net::ASN1Util::IT_UTCTIME)
	{
		Text::StrUIntOS(sptr, i++);
	}
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	Net::ASN1Util::ItemType itemType = Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff);
	if (itemType != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0 && itemType != Net::ASN1Util::IT_UNKNOWN)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendTBSCertList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	Data::DateTime dt;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN name;
	Text::CStringNN nnvarName;
	Char cbuff[256];
	Char cbuff2[20];
	UnsafeArray<Char> cptr = Text::StrConcat(cbuff, path);
	*cptr++ = '.';
	UIntOS i = 1;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subitemPDU;
	UIntOS subitemLen;
	UnsafeArray<const UInt8> subsubitemPDU;
	UIntOS subsubitemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(pdu, pduEnd, cbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			Text::StrUIntOS(cptr, i++);
		}
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("signature");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, name, false, 0);
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			name = CSTR("issuer");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendName(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(itemPDU, itemLen), dt))
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("thisUpdate = "));
			sb->AppendDateTime(dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_UTCTIME)
	{
		if (Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(itemPDU, itemLen), dt))
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("nextUpdate = "));
			sb->AppendDateTime(dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		Text::StrUIntOS(cptr, i++);
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UIntOS j = 0;
		while (true)
		{
			Text::StrUIntOS(cbuff2, ++j);
			if (!Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff2, subitemLen, itemType).SetTo(subitemPDU) || itemType != Net::ASN1Util::IT_SEQUENCE)
			{
				break;
			}

			if (Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "1", subsubitemLen, itemType).SetTo(subsubitemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
			{
				if (varName.SetTo(nnvarName))
				{
					sb->Append(nnvarName);
					sb->AppendUTF8Char('.');
				}
				sb->AppendC(UTF8STRC("revokedCertificates["));
				sb->AppendUIntOS(j);
				sb->AppendC(UTF8STRC("].userCertificate = "));
				sb->AppendHexBuff(subsubitemPDU, subsubitemLen, ':', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			if (Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "2", subsubitemLen, itemType).SetTo(subsubitemPDU) && itemType == Net::ASN1Util::IT_UTCTIME && Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(subsubitemPDU, subsubitemLen), dt))
			{
				if (varName.SetTo(nnvarName))
				{
					sb->Append(nnvarName);
					sb->AppendUTF8Char('.');
				}
				sb->AppendC(UTF8STRC("revokedCertificates["));
				sb->AppendUIntOS(j);
				sb->AppendC(UTF8STRC("].revocationDate = "));
				sb->AppendDateTime(dt);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			UIntOS itemOfst;
			if (Net::ASN1Util::PDUGetItemRAW(subitemPDU, subitemPDU + subitemLen, "3", subsubitemLen, itemOfst).SetTo(subsubitemPDU) && subsubitemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
			{
				sptr = sbuff;
				if (varName.SetTo(nnvarName))
				{
					sptr = nnvarName.ConcatTo(sptr);
					*sptr++ = '.';
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("revokedCertificates["));
				sptr = Text::StrUIntOS(sptr, j);
				sptr = Text::StrConcatC(sptr, UTF8STRC("].crlEntryExtensions"));
				AppendCRLExtensions(subsubitemPDU, subsubitemPDU + itemOfst + subsubitemLen, sb, CSTRP(sbuff, sptr));
			}
		}
	}
	Text::StrUIntOS(cptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			name = CSTR("crlExtensions");
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sbuff);
				*sptr++ = '.';
				sptr = name.ConcatTo(sptr);
				name = CSTRP(sbuff, sptr);
			}
			AppendCRLExtensions(itemPDU, itemPDU + itemLen, sb, name);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsTBSCertList(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificateList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendTBSCertList(pdu, pduEnd, sbuff, sb, varName);
	AppendSigned(pdu, pduEnd, path, sb, varName);
}

Bool Crypto::Cert::X509File::IsPrivateKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 3 && cnt != 4)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr;
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

void Crypto::Cert::X509File::AppendPrivateKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb)
{
	Char sbuff[256];
	UnsafeArray<Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS len;
	Net::ASN1Util::ItemType itemType;
	KeyType keyType = KeyType::Unknown;
	sptr = Text::StrConcat(sbuff, path);
	Text::StrConcat(sptr, ".1");
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, len, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->AppendC(UTF8STRC("version = "));
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrConcat(sptr, ".2");
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, len, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + len, sb, CSTR("privateKeyAlgorithm"), false, keyType);
		}
	}
	Text::StrConcat(sptr, ".3");
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, len, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			sb->AppendC(UTF8STRC("privateKey = "));
			sb->AppendC(UTF8STRC("\r\n"));
			if (keyType != KeyType::Unknown)
			{
				Crypto::Cert::X509Key privkey(CSTR("PrivKey"), Data::ByteArrayR(itemPDU, len), keyType);
				privkey.ToString(sb);
			}
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateRequestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt < 4)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UIntOS i = 1;
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_INTEGER)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_SEQUENCE)
	{
		return false;
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, sbuff) != Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		return false;
	}
	return true;
}

void Crypto::Cert::X509File::AppendCertificateRequestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb)
{
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
	*sptr++ = '.';
	UIntOS i = 1;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subitemPDU;
	UIntOS subitemLen;
	Net::ASN1Util::ItemType itemType;
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->AppendC(UTF8STRC("serialNumber = "));
			AppendVersion(pdu, pduEnd, sbuff, sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendName(itemPDU, itemPDU + itemLen, sb, CSTR("subject"));
		}
	}
	Text::StrUIntOS(sptr, i++);
	UIntOS itemOfst;
	if (Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, sbuff, itemLen, itemOfst).SetTo(itemPDU))
	{
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendSubjectPublicKeyInfo(itemPDU + itemOfst, itemPDU + itemOfst + itemLen, sb, CSTR("subjectPublicKeyInfo"));
			Crypto::Cert::X509PubKey pubKey(CSTR("PubKey"), Data::ByteArrayR(itemPDU, itemOfst + itemLen));
			NN<Crypto::Cert::X509Key> key;
			if (pubKey.CreateKey().SetTo(key))
			{
				key->ToString(sb);
				sb->AppendLB(Text::LineBreakType::CRLF);
				key.Delete();
			}
		}
	}
	Text::StrUIntOS(sptr, i++);
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		i = 1;
		Text::StrUIntOS(sbuff, i);
		while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, sbuff, subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UnsafeArray<const UInt8> extOID;
			UIntOS extOIDLen;
			UnsafeArray<const UInt8> ext;
			UIntOS extLen;
			if (Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "1", extOIDLen, itemType).SetTo(extOID) && itemType == Net::ASN1Util::IT_OID &&
				Net::ASN1Util::PDUGetItem(subitemPDU, subitemPDU + subitemLen, "2", extLen, itemType).SetTo(ext) && itemType == Net::ASN1Util::IT_SET)
			{
				if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extOID, extOIDLen), CSTR("1.2.840.113549.1.9.14")))
				{
					AppendCRLExtensions(ext, ext + extLen, sb, CSTR("extensionRequest"));
				}
				else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extOID, extOIDLen), CSTR("1.3.6.1.4.1.311.13.2.3"))) //szOID_OS_VERSION
				{
					AppendMSOSVersion(ext, ext + extLen, sb, CSTR("osVersion"));
				}
				else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extOID, extOIDLen), CSTR("1.3.6.1.4.1.311.21.20"))) //szOID_REQUEST_CLIENT_INFO
				{
					AppendMSRequestClientInfo(ext, ext + extLen, sb, CSTR("reqClientInfo"));
				}
				else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extOID, extOIDLen), CSTR("1.3.6.1.4.1.311.13.2.2"))) //szOID_ENROLLMENT_CSP_PROVIDER
				{
					AppendMSEnrollmentCSPProvider(ext, ext + extLen, sb, CSTR("enrollCSPProv"));
				}
			}
			Text::StrUIntOS(sbuff, ++i);
		}
	}
}

Bool Crypto::Cert::X509File::IsCertificateRequest(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	return IsSigned(pdu, pduEnd, path) && IsCertificateRequestInfo(pdu, pduEnd, sbuff);
}

void Crypto::Cert::X509File::AppendCertificateRequest(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb)
{
	Char sbuff[256];
	Text::StrConcat(Text::StrConcat(sbuff, path), ".1");
	AppendCertificateRequestInfo(pdu, pduEnd, sbuff, sb);
	AppendSigned(pdu, pduEnd, path, sb, nullptr);
}

Bool Crypto::Cert::X509File::IsPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 2)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
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

void Crypto::Cert::X509File::AppendPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb)
{
	UIntOS itemOfst;
	UIntOS buffSize;
	UnsafeArray<const UInt8> buff;
	if (Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, path, buffSize, itemOfst).SetTo(buff) && buff[0] == Net::ASN1Util::IT_SEQUENCE)
	{
		AppendSubjectPublicKeyInfo(buff + itemOfst, buff + itemOfst + buffSize, sb, CSTR("PubKey"));
		Crypto::Cert::X509PubKey pubKey(CSTR("PubKey"), Data::ByteArrayR(buff, itemOfst + buffSize));
		NN<Crypto::Cert::X509Key> key;
		if (pubKey.CreateKey().SetTo(key))
		{
			key->ToString(sb);
			sb->AppendLB(Text::LineBreakType::CRLF);
			key.Delete();
		}
	}
}

Bool Crypto::Cert::X509File::IsContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, path) != Net::ASN1Util::IT_SEQUENCE)
		return false;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 2)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
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

void Crypto::Cert::X509File::AppendContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	Text::CStringNN nnvarName;
	UnsafeArray<UTF8Char> sptr;
	UIntOS buffSize;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> buff;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, buffSize, itemType).SetTo(buff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		Net::ASN1Util::ItemType itemType1;
		UIntOS contentTypeLen;
		UnsafeArrayOpt<const UInt8> optcontentType = Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "1", contentTypeLen, itemType1);
		UnsafeArray<const UInt8> contentType;
		Net::ASN1Util::ItemType itemType2;
		UIntOS contentLen;
		UnsafeArray<const UInt8> content;

		if (optcontentType.SetTo(contentType))
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("content-type = "));
			Net::ASN1Util::OIDToString(Data::ByteArrayR(contentType, contentTypeLen), sb);
			NN<const Net::ASN1OIDDB::OIDInfo> oid;
			if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(contentType, contentTypeLen)).SetTo(oid))
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendSlow((const UTF8Char*)oid->name);
				sb->AppendUTF8Char(')');
			}
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (optcontentType.SetTo(contentType) && Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "2", contentLen, itemType2).SetTo(content))
		{
			if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.1"))) //data
			{
				UIntOS itemLen;
				UnsafeArray<const UInt8> itemPDU;
				if (Net::ASN1Util::PDUGetItem(content, content + contentLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
				{
					if (varName.SetTo(nnvarName))
					{
						sptr = nnvarName.ConcatTo(sbuff);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
					}
					else
					{
						sptr = Text::StrConcatC(sbuff, UTF8STRC("pkcs7-content"));
					}
					AppendData(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), dataType);
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.2"))) //signedData
			{
				if (varName.SetTo(nnvarName))
				{
					sptr = nnvarName.ConcatTo(sbuff);
					sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("pkcs7-content"));
				}
				AppendPKCS7SignedData(content, content + contentLen, sb, CSTRP(sbuff, sptr));
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.3"))) //envelopedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.4"))) //signedAndEnvelopedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.5"))) //digestedData
			{

			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(contentType, contentTypeLen), CSTR("1.2.840.113549.1.7.6"))) //encryptedData
			{
				if (varName.SetTo(nnvarName))
				{
					sptr = nnvarName.ConcatTo(sbuff);
					sptr = Text::StrConcatC(sptr, UTF8STRC(".pkcs7-content"));
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("pkcs7-content"));
				}
				AppendEncryptedData(content, content + contentLen, sb, CSTRP(sbuff, sptr), dataType);
			}
		}
	}
}

Bool Crypto::Cert::X509File::IsPFX(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path)
{
	if (Net::ASN1Util::PDUGetItemType(pdu, pduEnd, path) != Net::ASN1Util::IT_SEQUENCE)
		return false;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, path);
	if (cnt != 2 && cnt != 3)
	{
		return false;
	}
	Char sbuff[256];
	UnsafeArray<Char> sptr = Text::StrConcat(sbuff, path);
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

void Crypto::Cert::X509File::AppendPFX(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CString varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN nnvarName;
	UIntOS buffSize;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> buff;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, buffSize, itemType).SetTo(buff) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UIntOS cnt = Net::ASN1Util::PDUCountItem(buff, buff + buffSize, nullptr);

		Net::ASN1Util::ItemType itemType;
		UIntOS versionLen;
		UnsafeArray<const UInt8> version;
		if (Net::ASN1Util::PDUGetItem(buff, buff + buffSize, "1", versionLen, itemType).SetTo(version) && itemType == Net::ASN1Util::IT_INTEGER)
		{
			if (varName.SetTo(nnvarName))
			{
				sb->Append(nnvarName);
				sb->AppendUTF8Char('.');
			}
			sb->AppendC(UTF8STRC("version = "));
			Net::ASN1Util::IntegerToString(Data::ByteArrayR(version, versionLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		sptr = sbuff;
		if (varName.SetTo(nnvarName))
		{
			sptr = nnvarName.ConcatTo(sptr);
			*sptr++ = '.';
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("authSafe"));
		AppendContentInfo(buff, buff + buffSize, "2", sb, CSTRP(sbuff, sptr), ContentDataType::AuthenticatedSafe);
		if (cnt == 3)
		{
			sptr = sbuff;
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("macData"));
			AppendMacData(buff, buff + buffSize, "3", sb, CSTRP(sbuff, sptr));
		}
	}
}

void Crypto::Cert::X509File::AppendVersion(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, NN<Text::StringBuilderUTF8> sb)
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
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

void Crypto::Cert::X509File::AppendAlgorithmIdentifier(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName, Bool pubKey, OptOut<KeyType> keyTypeOut)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	KeyType keyType = KeyType::Unknown;
	KeyType innerKeyType = KeyType::Unknown;
	UIntOS algorithmLen;
	Net::ASN1Util::ItemType algorithmType;
	UnsafeArray<const UInt8> algorithm;
	UIntOS parametersLen;
	Net::ASN1Util::ItemType parametersType;
	UnsafeArray<const UInt8> parameters;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", algorithmLen, algorithmType).SetTo(algorithm) && algorithmType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("algorithm = "));
		Net::ASN1Util::OIDToString(Data::ByteArrayR(algorithm, algorithmLen), sb);
		keyType = KeyTypeFromOID(Data::ByteArrayR(algorithm, algorithmLen), pubKey);
		NN<const Net::ASN1OIDDB::OIDInfo> oid;
		if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(algorithm, algorithmLen)).SetTo(oid))
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->AppendSlow((const UTF8Char*)oid->name);
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC("\r\n"));
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", parametersLen, parametersType).SetTo(parameters))
		{
			if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(algorithm, algorithmLen), CSTR("1.2.840.113549.1.5.13")) && parametersType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(parameters, parameters + parametersLen, "1", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".pbes2.kdf"));
						AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), true, innerKeyType);
					}
				}
				if (Net::ASN1Util::PDUGetItem(parameters, parameters + parametersLen, "2", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".pbes2.encryptScheme"));
						AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), true, innerKeyType);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(algorithm, algorithmLen), CSTR("1.2.840.113549.1.5.12")) && parametersType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(parameters, parameters + parametersLen, "1", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_OCTET_STRING)
					{
						sb->Append(varName);
						sb->AppendC(UTF8STRC(".pbkdf2.salt = "));
						sb->AppendHexBuff(itemPDU, itemLen, ' ', Text::LineBreakType::None);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
				if (Net::ASN1Util::PDUGetItem(parameters, parameters + parametersLen, "2", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_INTEGER)
					{
						sb->Append(varName);
						sb->AppendC(UTF8STRC(".pbkdf2.iterationCount = "));
						Net::ASN1Util::IntegerToString(Data::ByteArrayR(itemPDU, itemLen), sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
				if (Net::ASN1Util::PDUGetItem(parameters, parameters + parametersLen, "3", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".pbkdf2.prf"));
						AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), true, innerKeyType);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(algorithm, algorithmLen), CSTR("2.16.840.1.101.3.4.1.42")) && parametersType == Net::ASN1Util::IT_OCTET_STRING)
			{
				sb->Append(varName);
				sb->AppendC(UTF8STRC(".aes256-cbc.iv = "));
				sb->AppendHexBuff(parameters, parametersLen, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(algorithm, algorithmLen), CSTR("1.2.840.10045.2.1")) && parametersType == Net::ASN1Util::IT_OID)
			{
				sb->Append(varName);
				sb->AppendC(UTF8STRC(".ecPublicKey.parameters = "));
				Net::ASN1Util::OIDToString(Data::ByteArrayR(parameters, parametersLen), sb);
				NN<const Net::ASN1OIDDB::OIDInfo> oid;
				if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(parameters, parametersLen)).SetTo(oid))
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendSlow((const UTF8Char*)oid->name);
					sb->AppendUTF8Char(')');
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else
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
		}
	}
	keyTypeOut.Set(keyType);
}

void Crypto::Cert::X509File::AppendValidity(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Data::DateTime dt;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU))
	{
		if ((itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME) && Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(itemPDU, itemLen), dt))
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("notBefore = "));
			sb->AppendDateTime(dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU))
	{
		if ((itemType == Net::ASN1Util::IT_UTCTIME || itemType == Net::ASN1Util::IT_GENERALIZEDTIME) && Net::ASN1Util::PDUParseUTCTimeCont(Data::ByteArrayR(itemPDU, itemLen), dt))
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("notAfter = "));
			sb->AppendDateTime(dt);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendSubjectPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	KeyType keyType = KeyType::Unknown;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = Text::StrConcatC(varName.ConcatTo(sbuff), UTF8STRC(".algorithm"));
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), true, keyType);
		}
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU))
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
				Crypto::Cert::X509Key pkey(CSTRP(sbuff, sptr), Data::ByteArrayR(itemPDU + 1, itemLen - 1), keyType);
				pkey.ToString(sb);
				sb->AppendC(UTF8STRC("\r\n"));
			}
		}
	}
}

void Crypto::Cert::X509File::AppendName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Char sbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUIntOS(sbuff, i);
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SET)
			{
				AppendRelativeDistinguishedName(itemPDU, itemPDU + itemLen, sb, varName);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendRelativeDistinguishedName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Char sbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUIntOS(sbuff, i);
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				AppendAttributeTypeAndDistinguishedValue(itemPDU, itemPDU + itemLen, sb, varName);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendAttributeTypeAndDistinguishedValue(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UnsafeArray<const UInt8> typePDU;
	UIntOS typeLen;
	Net::ASN1Util::ItemType typeType;
	UnsafeArray<const UInt8> valuePDU;
	UIntOS valueLen;
	Net::ASN1Util::ItemType valueType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", typeLen, typeType).SetTo(typePDU) && Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", valueLen, valueType).SetTo(valuePDU) && typeType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.3")))
		{
			sb->AppendC(UTF8STRC("commonName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.4")))
		{
			sb->AppendC(UTF8STRC("surname"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.6")))
		{
			sb->AppendC(UTF8STRC("countryName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.7")))
		{
			sb->AppendC(UTF8STRC("localityName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.8")))
		{
			sb->AppendC(UTF8STRC("stateOrProvinceName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.9")))
		{
			sb->AppendC(UTF8STRC("streetAddress"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.10")))
		{
			sb->AppendC(UTF8STRC("organizationName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.11")))
		{
			sb->AppendC(UTF8STRC("organizationalUnitName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.12")))
		{
			sb->AppendC(UTF8STRC("title"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.42")))
		{
			sb->AppendC(UTF8STRC("givenName"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("2.5.4.43")))
		{
			sb->AppendC(UTF8STRC("initials"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("0.9.2342.19200300.100.1.25")))
		{
			sb->AppendC(UTF8STRC("domainComponent"));
		}
		else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(typePDU, typeLen), CSTR("1.2.840.113549.1.9.1")))
		{
			sb->AppendC(UTF8STRC("emailAddress"));
		}
		else
		{
			Net::ASN1Util::OIDToString(Data::ByteArrayR(typePDU, typeLen), sb);
		}
		sb->AppendC(UTF8STRC(" = "));
		if (valueType == Net::ASN1Util::IT_BMPSTRING)
		{
			sb->AppendUTF16BE(valuePDU, valueLen >> 1);
		}
		else
		{
			sb->AppendC(valuePDU, valueLen);
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendCRLExtensions(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE || itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			UIntOS i = 1;
			Text::StrUIntOS(sbuff, i);
			while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU))
			{
				AppendCRLExtension(subItemPDU, subItemPDU + subItemLen, sb, varName);
				Text::StrUIntOS(sbuff, ++i);
			}
		}
	}
}

void Crypto::Cert::X509File::AppendCRLExtension(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> extension;
	UIntOS extensionLen;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", extensionLen, itemType).SetTo(extension))
	{
		if (itemType == Net::ASN1Util::IT_OID)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("extensionType = "));
			Net::ASN1Util::OIDToString(Data::ByteArrayR(extension, extensionLen), sb);
			NN<const Net::ASN1OIDDB::OIDInfo> oid;
			if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(extension, extensionLen)).SetTo(oid))
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
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_BOOLEAN)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("critical = "));
			Net::ASN1Util::BooleanToString(Data::ByteArrayR(itemPDU, itemLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
			if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, "3", itemLen, itemType).SetTo(itemPDU))
			{
				return;
			}
		}
		if (itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("1.3.6.1.5.5.7.1.1"))) //id-pe-authorityInfoAccess
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UIntOS i = 1;
					Text::StrUIntOS(sbuff, i);
					while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						UnsafeArray<const UInt8> descPDU;
						UIntOS descLen;
						if (Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, "1", descLen, itemType).SetTo(descPDU) && itemType == Net::ASN1Util::IT_OID)
						{
							sb->Append(varName);
							sb->AppendC(UTF8STRC(".authorityInfoAccess["));
							sb->AppendUIntOS(i);
							sb->AppendC(UTF8STRC("].accessMethod = "));
							Net::ASN1Util::OIDToString(Data::ByteArrayR(descPDU, descLen), sb);
							sb->AppendC(UTF8STRC(" ("));
							Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(descPDU, descLen), sb);
							sb->AppendC(UTF8STRC(")\r\n"));
						}
						sptr = varName.ConcatTo(sbuff);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".authorityInfoAccess["));
						sptr = Text::StrUIntOS(sptr, i);
						sptr = Text::StrConcatC(sptr, UTF8STRC("].accessLocation"));
						AppendGeneralName(subItemPDU, subItemPDU + subItemLen, "2", sb, CSTRP(sbuff, sptr));
						
						Text::StrUIntOS(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.14"))) //id-ce-subjectKeyIdentifier
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
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.15"))) //id-ce-keyUsage
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_BIT_STRING)
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
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.17"))) //id-ce-subjectAltName
			{
				sptr = varName.ConcatTo(sbuff);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".subjectAltName"));
				AppendGeneralNames(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.19"))) //id-ce-basicConstraints
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_BOOLEAN)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("basicConstraints.cA = "));
						Net::ASN1Util::BooleanToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
					if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("basicConstraints.pathLenConstraint = "));
						Net::ASN1Util::IntegerToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
						sb->AppendC(UTF8STRC("\r\n"));
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.20"))) //id-ce-cRLNumber
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("cRLNumber = "));
					Net::ASN1Util::IntegerToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.21"))) //id-ce-cRLReasons
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_ENUMERATED && subItemLen == 1)
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
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.31"))) //id-ce-cRLDistributionPoints
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UIntOS i = 1;
					Text::StrUIntOS(sbuff, i);
					while (AppendDistributionPoint(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
					{
						Text::StrUIntOS(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.32"))) //id-ce-certificatePolicies
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UIntOS i = 1;
					Text::StrUIntOS(sbuff, i);
					while (AppendPolicyInformation(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
					{
						Text::StrUIntOS(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.35"))) //id-ce-authorityKeyIdentifier
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UIntOS i = 1;
					Text::StrUIntOS(sbuff, i);
					while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU))
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
						Text::StrUIntOS(sbuff, ++i);
					}
				}
			}
			else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(extension, extensionLen), CSTR("2.5.29.37"))) //id-ce-extKeyUsage
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
				{
					UIntOS i = 1;
					UnsafeArray<const UInt8> subItemPDU;
					UIntOS subItemLen;
					Text::StrUIntOS(sbuff, i);

					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("extKeyUsage ="));

					while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU))
					{
						if (itemType == Net::ASN1Util::IT_OID)
						{
							if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.1")))
							{
								sb->AppendC(UTF8STRC(" serverAuth"));
							}
							else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.2")))
							{
								sb->AppendC(UTF8STRC(" clientAuth"));
							}
							else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.3")))
							{
								sb->AppendC(UTF8STRC(" codeSigning"));
							}
							else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.4")))
							{
								sb->AppendC(UTF8STRC(" emailProtection"));
							}
							else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.8")))
							{
								sb->AppendC(UTF8STRC(" timeStamping"));
							}
							else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(subItemPDU, subItemLen), CSTR("1.3.6.1.5.5.7.3.9")))
							{
								sb->AppendC(UTF8STRC(" OCSPSigning"));
							}
						}
						Text::StrUIntOS(sbuff, ++i);
					}
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
		}
	}
}

void Crypto::Cert::X509File::AppendMSOSVersion(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::ItemType::IT_IA5STRING)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".version = "));
		sb->AppendC(itemPDU, itemLen);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendMSRequestClientInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::ItemType::IT_SEQUENCE)
	{
		UIntOS subitemLen;
		UnsafeArray<const UInt8> subitemPDU;
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_INTEGER)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".unknown = "));
			Net::ASN1Util::IntegerToString(Data::ByteArrayR(subitemPDU, subitemLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_UTF8STRING)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".machine = "));
			sb->AppendC(subitemPDU, subitemLen);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_UTF8STRING)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".user = "));
			sb->AppendC(subitemPDU, subitemLen);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "4", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_UTF8STRING)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".software = "));
			sb->AppendC(subitemPDU, subitemLen);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendMSEnrollmentCSPProvider(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::ItemType::IT_SEQUENCE)
	{
		UIntOS subitemLen;
		UnsafeArray<const UInt8> subitemPDU;
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_INTEGER)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".unknown = "));
			Net::ASN1Util::IntegerToString(Data::ByteArrayR(subitemPDU, subitemLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::ItemType::IT_BMPSTRING)
		{
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".provider = "));
			sb->AppendUTF16BE(subitemPDU, subitemLen >> 1);
			sb->AppendC(UTF8STRC("\r\n"));
		}
	}
}

void Crypto::Cert::X509File::AppendGeneralNames(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[11];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UIntOS i = 1;
			Text::StrUIntOS(sbuff, i);
			while (AppendGeneralName(itemPDU, itemPDU + itemLen, (const Char*)sbuff, sb, varName))
			{
				Text::StrUIntOS(sbuff, ++i);
			}
		}
	}
}

Bool Crypto::Cert::X509File::AppendGeneralName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, subItemLen, itemType).SetTo(subItemPDU))
	{
		switch (0x8F & (UIntOS)itemType)
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
			if (Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, path, subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
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
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&subItemPDU[0]));
				sb->AppendP(sbuff, sptr);
			}
			else if (subItemLen == 16)
			{
				Net::SocketUtil::AddressInfo addr;
				Net::SocketUtil::SetAddrInfoV6(addr, &subItemPDU[0], 0);
				sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
				sb->AppendP(sbuff, sptr);
			}
			sb->AppendC(UTF8STRC("\r\n"));
			return true;
		case 0x88:
			sb->Append(varName);
			sb->AppendC(UTF8STRC(".registeredID = "));
			Net::ASN1Util::OIDToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
			{
				NN<const Net::ASN1OIDDB::OIDInfo> ent;
				if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(subItemPDU, subItemLen)).SetTo(ent))
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

Bool Crypto::Cert::X509File::AppendDistributionPoint(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, itemLen, itemType).SetTo(itemPDU) != 0)
	{
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UIntOS i = 1;
			Text::StrUIntOS(sbuff, i);
			while (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU))
			{
				switch ((UIntOS)itemType)
				{
				case Net::ASN1Util::IT_CONTEXT_SPECIFIC_0:
					sptr = varName.ConcatTo(sbuff);
					*sptr++ = '.';
					sptr = Text::StrConcatC(sptr, UTF8STRC("distributionPoint"));
					AppendDistributionPointName(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
					break;
				case Net::ASN1Util::IT_CONTEXT_SPECIFIC_1:
					if (Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_BIT_STRING)
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
				Text::StrUIntOS(sbuff, ++i);
			}
			return true;
		}
	}
	return false;
}

void Crypto::Cert::X509File::AppendDistributionPointName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	Char pathBuff[16];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU))
	{
		if (itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '.';
			sptr = Text::StrConcatC(sptr, UTF8STRC("fullName"));
			i = 0;
			Text::StrUIntOS(pathBuff, ++i);
			while (AppendGeneralName(itemPDU, itemPDU + itemLen, pathBuff, sb, CSTRP(sbuff, sptr)))
			{
				Text::StrUIntOS(pathBuff, ++i);
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

Bool Crypto::Cert::X509File::AppendPolicyInformation(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[64];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_OID)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("policyIdentifier = "));
			Net::ASN1Util::OIDToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
			sb->AppendC(UTF8STRC(" ("));
			Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
			sb->AppendUTF8Char(')');
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UnsafeArray<const UInt8> policyQualifierInfoPDU;
			UIntOS policyQualifierInfoLen;
			UIntOS i = 0;
			Text::StrUIntOS(sbuff, ++i);
			while (Net::ASN1Util::PDUGetItem(subItemPDU, subItemPDU + subItemLen, (const Char*)sbuff, policyQualifierInfoLen, itemType).SetTo(policyQualifierInfoPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
				{
					sb->Append(varName);
					sb->AppendUTF8Char('.');
					sb->AppendC(UTF8STRC("policyQualifiers["));
					sb->AppendUIntOS(i);
					sb->AppendC(UTF8STRC("].policyQualifierId = "));
					Net::ASN1Util::OIDToString(Data::ByteArrayR(itemPDU, itemLen), sb);
					sb->AppendC(UTF8STRC(" ("));
					Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(itemPDU, itemLen), sb);
					sb->AppendUTF8Char(')');
					sb->AppendC(UTF8STRC("\r\n"));
				}
				if (Net::ASN1Util::PDUGetItem(policyQualifierInfoPDU, policyQualifierInfoPDU + policyQualifierInfoLen, "2", itemLen, itemType).SetTo(itemPDU))
				{
					if (itemType == Net::ASN1Util::IT_IA5STRING)
					{
						sb->Append(varName);
						sb->AppendUTF8Char('.');
						sb->AppendC(UTF8STRC("policyQualifiers["));
						sb->AppendUIntOS(i);
						sb->AppendC(UTF8STRC("].qualifier = "));
						sb->AppendC(itemPDU, itemLen);
						sb->AppendC(UTF8STRC("\r\n"));
					}
					else if (itemType == Net::ASN1Util::IT_SEQUENCE)
					{
						/////////////////////////////////// UserNotice
					}
				}
				Text::StrUIntOS(sbuff, ++i);
			}
		}
		return true;
	}
	return false;
}

void Crypto::Cert::X509File::AppendPKCS7SignedData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[16];
	Net::ASN1Util::ItemType itemType;
	UIntOS itemOfst;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		UIntOS i;
		UIntOS subItemLen;
		UnsafeArray<const UInt8> subItemPDU;
		UnsafeArrayOpt<const UInt8> optsubItemPDU;
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->AppendC(UTF8STRC("signedData.version = "));
			Net::ASN1Util::IntegerToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SET)
		{
			AppendPKCS7DigestAlgorithmIdentifiers(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.digestAlgorithms"));
		}
		if (Net::ASN1Util::PDUGetItemRAW(itemPDU, itemPDU + itemLen, "3", subItemLen, itemOfst).SetTo(subItemPDU) && subItemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendContentInfo(subItemPDU, subItemPDU + itemOfst + subItemLen, "1", sb, CSTR("signedData.contentInfo"), ContentDataType::Unknown);
		}
		i = 4;
		optsubItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "4", subItemLen, itemType);
		if (optsubItemPDU.SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.certificates"));
			Text::StrUIntOS(sbuff, ++i);
			optsubItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType);
		}
		if (optsubItemPDU.SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
		{
			//AppendCertificate(subItemPDU, subItemPDU + subItemLen, "1", sb, CSTR("signedData.crls"));
			Text::StrUIntOS(sbuff, ++i);
			optsubItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, (const Char*)sbuff, subItemLen, itemType);
		}
		if (optsubItemPDU.SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SET)
		{
			AppendPKCS7SignerInfos(subItemPDU, subItemPDU + subItemLen, sb, CSTR("signedData.signerInfos"));
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7DigestAlgorithmIdentifiers(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[16];
	UIntOS i;
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	i = 0;
	while (true)
	{
		Text::StrUIntOS(sbuff, ++i);
		if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, (const Char*)sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			return;
		}
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, varName, false, 0);
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7SignerInfos(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	Char cbuff[32];
	UIntOS i;
	UIntOS itemOfst;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	i = 0;
	while (true)
	{
		Text::StrUIntOS(cbuff, ++i);
		if (!Net::ASN1Util::PDUGetItemRAW(pdu, pduEnd, cbuff, itemLen, itemOfst).SetTo(itemPDU))
		{
			break;
		}
		if (itemPDU[0] == Net::ASN1Util::IT_SEQUENCE)
		{
			AppendPKCS7SignerInfo(itemPDU, itemPDU + itemOfst + itemLen, sb, varName);
		}
	}
}

void Crypto::Cert::X509File::AppendPKCS7SignerInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Char cbuff[32];
	UIntOS i;
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_SEQUENCE)
	{
		return;
	}
	UIntOS subItemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UnsafeArrayOpt<const UInt8> optsubItemPDU;
	if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("version = "));
		Net::ASN1Util::IntegerToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("issuerAndSerialNumber"));
		AppendIssuerAndSerialNumber(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
	}
	if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("digestAlgorithm"));
		AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	i = 4;
	optsubItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "4", subItemLen, itemType);
	if (optsubItemPDU.SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("authenticatedAttributes"));
		AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		Text::StrUIntOS(cbuff, ++i);
		optsubItemPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType);
	}
	if (optsubItemPDU.SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("digestEncryptionAlgorithm"));
		AppendAlgorithmIdentifier(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	Text::StrUIntOS(cbuff, ++i);
	if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("encryptedDigest = "));
		sb->AppendHexBuff(subItemPDU, subItemLen, ':', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	Text::StrUIntOS(cbuff, ++i);
	if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, cbuff, subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_1)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("unauthenticatedAttributes"));
		AppendPKCS7Attributes(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
	}
}

void Crypto::Cert::X509File::AppendIssuerAndSerialNumber(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("issuer"));
		AppendName(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
	{
		sb->Append(varName);
		sb->AppendUTF8Char('.');
		sb->AppendC(UTF8STRC("serialNumber = "));
		sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendPKCS7Attributes(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Char cbuff[16];
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS oidLen;
	UnsafeArray<const UInt8> oidPDU;
	Net::ASN1Util::ItemType oidType;
	UIntOS valueLen;
	UnsafeArray<const UInt8> valuePDU;
	UIntOS i = 0;
	while (true)
	{
		Text::StrUIntOS(cbuff, ++i);
		if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
		{
			return;
		}
		if (itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, oidType).SetTo(oidPDU) && oidType == Net::ASN1Util::IT_OID)
			{
				sb->Append(varName);
				sb->AppendUTF8Char('.');
				sb->AppendC(UTF8STRC("attributeType = "));
				Net::ASN1Util::OIDToString(Data::ByteArrayR(oidPDU, oidLen), sb);
				NN<const Net::ASN1OIDDB::OIDInfo> oid;
				if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(oidPDU, oidLen)).SetTo(oid))
				{
					sb->AppendC(UTF8STRC(" ("));
					sb->AppendSlow((const UTF8Char*)oid->name);
					sb->AppendUTF8Char(')');
				}
				sb->AppendC(UTF8STRC("\r\n"));
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", valueLen, itemType).SetTo(valuePDU) && itemType == Net::ASN1Util::IT_SET)
				{
					if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.3"))) //contentType
					{
						if (Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
						{
							sb->Append(varName);
							sb->AppendUTF8Char('.');
							sb->AppendC(UTF8STRC("contentType = "));
							Net::ASN1Util::OIDToString(Data::ByteArrayR(itemPDU, itemLen), sb);
							NN<const Net::ASN1OIDDB::OIDInfo> oid;
							if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(itemPDU, itemLen)).SetTo(oid))
							{
								sb->AppendC(UTF8STRC(" ("));
								sb->AppendSlow((const UTF8Char*)oid->name);
								sb->AppendUTF8Char(')');
							}
							sb->AppendC(UTF8STRC("\r\n"));
						}
					}
					else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.4"))) //messageDigest
					{
						if (Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
						{
							sb->Append(varName);
							sb->AppendUTF8Char('.');
							sb->AppendC(UTF8STRC("messageDigest = "));
							sb->AppendHexBuff(itemPDU, itemLen, ':', Text::LineBreakType::None);
							sb->AppendC(UTF8STRC("\r\n"));
						}
					}
					else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.5"))) //signing-time
					{
						if (Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_UTCTIME)
						{
							sb->Append(varName);
							sb->AppendUTF8Char('.');
							sb->AppendC(UTF8STRC("signing-time = "));
							Net::ASN1Util::UTCTimeToString(Data::ByteArrayR(itemPDU, itemLen), sb);
							sb->AppendC(UTF8STRC("\r\n"));
						}
					}
					else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.15"))) //smimeCapabilities
					{
						/////////////////////////////////////
					}
					else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.16.2.11"))) //id-aa-encrypKeyPref
					{
						if (Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
						{
							sptr = varName.ConcatTo(sbuff);
							*sptr++ = '.';
							sptr = Text::StrConcatC(sptr, UTF8STRC("encrypKeyPref"));
							AppendIssuerAndSerialNumber(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr));
						}
					}
					else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.3.6.1.4.1.311.16.4"))) //outlookExpress
					{
						if (Net::ASN1Util::PDUGetItem(valuePDU, valuePDU + valueLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
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
}

Bool Crypto::Cert::X509File::AppendMacData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArrayOpt<const Char> path, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, path, itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = varName.ConcatTo(sbuff);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".mac"));
			AppendDigestInfo(subItemPDU, subItemPDU + subItemLen, sb, CSTRP(sbuff, sptr));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("macSalt = "));
			sb->AppendHexBuff(subItemPDU, subItemLen, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_INTEGER)
		{
			sb->Append(varName);
			sb->AppendUTF8Char('.');
			sb->AppendC(UTF8STRC("iterations = "));
			Net::ASN1Util::IntegerToString(Data::ByteArrayR(subItemPDU, subItemLen), sb);
			sb->AppendC(UTF8STRC("\r\n"));
		}
		return true;
	}
	return false;
}

void Crypto::Cert::X509File::AppendDigestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = varName.ConcatTo(sbuff);
		sptr = Text::StrConcatC(sptr, UTF8STRC(".digestAlgorithm"));
		AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".digest = "));
		sb->AppendHexBuff(itemPDU, itemLen, ' ', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void Crypto::Cert::X509File::AppendData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName, ContentDataType dataType)
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

void Crypto::Cert::X509File::AppendEncryptedData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	Text::CStringNN nnvarName;
	UIntOS itemLen;
	UnsafeArray<const UInt8> subitemPDU;
	UIntOS subitemLen;
	Net::ASN1Util::ItemType itemType;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		if (varName.SetTo(nnvarName))
		{
			sb->Append(nnvarName);
			sb->AppendUTF8Char('.');
		}
		sb->AppendC(UTF8STRC("version = "));
		AppendVersion(itemPDU, itemPDU + itemLen, "1", sb);
		sb->AppendC(UTF8STRC("\r\n"));

		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			sptr = sbuff;
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("encryptedContentInfo"));
			AppendEncryptedContentInfo(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr), dataType);
		}
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", subitemLen, itemType).SetTo(subitemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			sptr = sbuff;
			if (varName.SetTo(nnvarName))
			{
				sptr = nnvarName.ConcatTo(sptr);
				*sptr++ = '.';
			}
			sptr = Text::StrConcatC(sptr, UTF8STRC("unprotectedAttributes"));
			AppendPKCS7Attributes(subitemPDU, subitemPDU + subitemLen, sb, CSTRP(sbuff, sptr));
		}
	}
}

void Crypto::Cert::X509File::AppendAuthenticatedSafe(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Char cbuff[16];
	UnsafeArray<const UInt8> itemPDU;
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS i;
	UIntOS j;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		i = 0;
		j = Net::ASN1Util::PDUCountItem(itemPDU, itemPDU + itemLen, nullptr);
		while (i < j)
		{
			sptr = varName.ConcatTo(sbuff);
			*sptr++ = '[';
			sptr = Text::StrUIntOS(sptr, i);
			*sptr++ = ']';
			*sptr = 0;
			Text::StrUIntOS(cbuff, ++i);
			AppendContentInfo(itemPDU, itemPDU + itemLen, cbuff, sb, CSTRP(sbuff, sptr), ContentDataType::Unknown);
		}
	}
}

void Crypto::Cert::X509File::AppendEncryptedContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CStringNN varName, ContentDataType dataType)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_OID)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".contentType = "));
		Net::ASN1Util::OIDToString(Data::ByteArrayR(itemPDU, itemLen), sb);
		NN<const Net::ASN1OIDDB::OIDInfo> oid;
		if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(itemPDU, itemLen)).SetTo(oid))
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->AppendSlow((const UTF8Char*)oid->name);
			sb->AppendUTF8Char(')');
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
	{
		sptr = sbuff;
		sptr = varName.ConcatTo(sptr);
		*sptr++ = '.';
		sptr = Text::StrConcatC(sptr, UTF8STRC("contentEncryptionAlgorithm"));
		AppendAlgorithmIdentifier(itemPDU, itemPDU + itemLen, sb, CSTRP(sbuff, sptr), false, 0);
	}
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "3", itemLen, itemType).SetTo(itemPDU) && (itemType & 0x8F) == 0x80)
	{
		sb->Append(varName);
		sb->AppendC(UTF8STRC(".encryptedContent = "));
		sb->AppendHexBuff(itemPDU, itemLen, ' ', Text::LineBreakType::None);
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

Bool Crypto::Cert::X509File::NameGetByOID(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Text::CStringNN oidText, NN<Text::StringBuilderUTF8> sb)
{
	Char sbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UnsafeArray<const UInt8> oidPDU;
	UnsafeArray<const UInt8> strPDU;
	UIntOS itemLen;
	UIntOS oidLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrConcat(Text::StrUIntOS(sbuff, i), ".1");
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, itemType).SetTo(oidPDU) && itemType == Net::ASN1Util::IT_OID && Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), oidText))
				{
					if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", oidLen, itemType).SetTo(strPDU))
					{
						if (itemType == Net::ASN1Util::IT_BMPSTRING)
						{
							sb->AppendUTF16BE(strPDU, oidLen >> 1);
						}
						else
						{
							sb->AppendC(strPDU, oidLen);
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

UnsafeArrayOpt<UTF8Char> Crypto::Cert::X509File::NameGetByOID(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Text::CStringNN oidText, UnsafeArray<UTF8Char> sbuff)
{
	Char cbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UnsafeArray<const UInt8> oidPDU;
	UnsafeArray<const UInt8> strPDU;
	UIntOS itemLen;
	UIntOS oidLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrConcat(Text::StrUIntOS(cbuff, i), ".1");
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, cbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, itemType).SetTo(oidPDU) && itemType == Net::ASN1Util::IT_OID && Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), oidText))
				{
					if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", oidLen, itemType).SetTo(strPDU))
					{
						if (itemType == Net::ASN1Util::IT_BMPSTRING)
						{
							return Text::StrUTF16BE_UTF8C(sbuff, strPDU, oidLen >> 1);
						}
						else
						{
							return Text::StrConcatC(sbuff, strPDU, oidLen);
						}
					}
				}
			}
		}
	}
	return nullptr;
}

Bool Crypto::Cert::X509File::NameGetCN(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb)
{
	return NameGetByOID(pdu, pduEnd, CSTR("2.5.4.3"), sb);
}

UnsafeArrayOpt<UTF8Char> Crypto::Cert::X509File::NameGetCN(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UnsafeArray<UTF8Char> sbuff)
{
	return NameGetByOID(pdu, pduEnd, CSTR("2.5.4.3"), sbuff);
}

Bool Crypto::Cert::X509File::NamesGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<CertNames> names)
{
	Char sbuff[12];
	UnsafeArray<const UInt8> itemPDU;
	UnsafeArray<const UInt8> oidPDU;
	UnsafeArray<const UInt8> strPDU;
	UIntOS itemLen;
	UIntOS oidLen;
	UIntOS strLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrConcat(Text::StrUIntOS(sbuff, i), ".1");
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, itemType).SetTo(oidPDU) && itemType == Net::ASN1Util::IT_OID)
				{
					if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", strLen, itemType).SetTo(strPDU))
					{
						if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.6")))
						{
							OPTSTR_DEL(names->countryName);
							names->countryName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.8")))
						{
							OPTSTR_DEL(names->stateOrProvinceName);
							names->stateOrProvinceName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.7")))
						{
							OPTSTR_DEL(names->localityName);
							names->localityName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.10")))
						{
							OPTSTR_DEL(names->organizationName);
							names->organizationName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.11")))
						{
							OPTSTR_DEL(names->organizationUnitName);
							names->organizationUnitName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.4.3")))
						{
							OPTSTR_DEL(names->commonName);
							names->commonName = Text::String::New(strPDU, strLen);
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("1.2.840.113549.1.9.1")))
						{
							OPTSTR_DEL(names->emailAddress);
							names->emailAddress = Text::String::New(strPDU, strLen);
						}
					}
				}
			}
		}
	}
	return true;
}

Bool Crypto::Cert::X509File::ExtensionsGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<CertExtensions> ext)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UInt8> itemPDU;
	UnsafeArray<const UInt8> oidPDU;
	UnsafeArrayOpt<const UInt8> optstrPDU;
	UnsafeArray<const UInt8> strPDU;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS itemLen;
	UIntOS oidLen;
	UIntOS strLen;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUIntOS(sbuff, i);
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, (const Char*)sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, itemType).SetTo(oidPDU) && itemType == Net::ASN1Util::IT_OID)
				{
					optstrPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", strLen, itemType);
					if (optstrPDU.SetTo(strPDU) && itemType == Net::ASN1Util::IT_BOOLEAN)
					{
						optstrPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", strLen, itemType);
					}
					if (optstrPDU.SetTo(strPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
					{
						if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.29.17"))) //id-ce-subjectAltName
						{
							NN<Data::ArrayListStringNN> strList;
							if (ext->subjectAltName.SetTo(strList))
							{
								strList->FreeAll();
								strList.Delete();
							}
							NEW_CLASSNN(strList, Data::ArrayListStringNN());
							ext->subjectAltName = strList;
							UIntOS j = 0;
							UIntOS k = Net::ASN1Util::PDUCountItem(strPDU, strPDU + strLen, "1");
							while (j < k)
							{
								j++;
								Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("1.")), j);
								if (Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU))
								{
									if (itemType == 0x87)
									{
										sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&subItemPDU[0]));
										strList->Add(Text::String::New(sbuff, (UIntOS)(sptr - sbuff)));
									}
									else
									{
										strList->Add(Text::String::New(subItemPDU, subItemLen));
									}
								}
							}
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.29.14"))) //id-ce-subjectKeyIdentifier
						{
							if (Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1", subItemLen, itemType).SetTo(subItemPDU) && subItemLen == 20)
							{
								ext->useSubjKeyId = true;
								MemCopyNO(ext->subjKeyId, &subItemPDU[0], subItemLen);
							}
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.29.35"))) //id-ce-authorityKeyIdentifier
						{
							if (Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1.1", subItemLen, itemType).SetTo(subItemPDU) && subItemLen == 20)
							{
								ext->useAuthKeyId = true;
								MemCopyNO(ext->authKeyId, &subItemPDU[0], subItemLen);
							}
						}
						else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.29.15"))) //id-ce-keyUsage
						{
							if (Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, "1", subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_BIT_STRING && subItemLen >= 2)
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

UIntOS Crypto::Cert::X509File::ExtensionsGetCRLDistributionPoints(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Data::ArrayListObj<Text::CString>> crlDistributionPoints)
{
	UTF8Char sbuff[32];
	UIntOS ret = 0;
	UnsafeArray<const UInt8> itemPDU;
	UnsafeArray<const UInt8> oidPDU;
	UnsafeArrayOpt<const UInt8> optstrPDU;
	UnsafeArray<const UInt8> strPDU;
	UnsafeArray<const UInt8> subItemPDU;
	UIntOS itemLen;
	UIntOS oidLen;
	UIntOS strLen;
	UIntOS subItemLen;
	Net::ASN1Util::ItemType itemType;
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	UIntOS i = 0;
	while (i < cnt)
	{
		i++;

		Text::StrUIntOS(sbuff, i);
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, (const Char*)sbuff, itemLen, itemType).SetTo(itemPDU))
		{
			if (itemType == Net::ASN1Util::IT_SEQUENCE)
			{
				if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", oidLen, itemType).SetTo(oidPDU) && itemType == Net::ASN1Util::IT_OID)
				{
					optstrPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "2", strLen, itemType);
					if (optstrPDU.SetTo(strPDU) && itemType == Net::ASN1Util::IT_BOOLEAN)
					{
						optstrPDU = Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "3", strLen, itemType);
					}
					if (optstrPDU.SetTo(strPDU) && itemType == Net::ASN1Util::IT_OCTET_STRING)
					{
						if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(oidPDU, oidLen), CSTR("2.5.29.31"))) //id-ce-cRLDistributionPoints
						{
							UIntOS j = 0;
							UIntOS k = Net::ASN1Util::PDUCountItem(strPDU, strPDU + strLen, "1");
							while (j < k)
							{
								j++;
								Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("1.")), j);
								if (Net::ASN1Util::PDUGetItem(strPDU, strPDU + strLen, (const Char*)sbuff, subItemLen, itemType).SetTo(subItemPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
								{
									ret += DistributionPointAdd(subItemPDU, subItemPDU + subItemLen, crlDistributionPoints);
								}
							}
						}
					}
				}
			}
		}
	}
	return ret;
}

UIntOS Crypto::Cert::X509File::DistributionPointAdd(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Data::ArrayListObj<Text::CString>> crlDistributionPoints)
{
	Net::ASN1Util::ItemType itemType;
	UIntOS itemLen;
	UnsafeArray<const UInt8> itemPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
	{
		if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CONTEXT_SPECIFIC_0)
		{
			if (Net::ASN1Util::PDUGetItem(itemPDU, itemPDU + itemLen, "1", itemLen, itemType).SetTo(itemPDU) && itemType == Net::ASN1Util::IT_CHOICE_6)
			{
				crlDistributionPoints->Add(Text::CString(itemPDU, itemLen));
				return 1;
			}
		}
	}
	return 0;
}

Optional<Crypto::Cert::X509Key> Crypto::Cert::X509File::PublicKeyGetNew(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd)
{
	Net::ASN1Util::ItemType oidType;
	UIntOS oidLen;
	UnsafeArray<const UInt8> oidPDU;
	Net::ASN1Util::ItemType bstrType;
	UIntOS bstrLen;
	UnsafeArray<const UInt8> bstrPDU;
	if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.1", oidLen, oidType).SetTo(oidPDU) && oidType == Net::ASN1Util::IT_OID &&
		Net::ASN1Util::PDUGetItem(pdu, pduEnd, "2", bstrLen, bstrType).SetTo(bstrPDU) && bstrType == Net::ASN1Util::IT_BIT_STRING)
	{
		KeyType keyType = KeyTypeFromOID(Data::ByteArrayR(oidPDU, oidLen), true);
		if (keyType == KeyType::ECPublic)
		{
			Net::ASN1Util::ItemType paramType;
			UIntOS paramLen;
			UnsafeArray<const UInt8> paramPDU;
			if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.2", paramLen, paramType).SetTo(paramPDU) && paramType == Net::ASN1Util::IT_OID)
			{
				return Crypto::Cert::X509Key::FromECPublicKey(Data::ByteArrayR(bstrPDU + 1, bstrLen - 1), Data::ByteArrayR(paramPDU, paramLen));
				///////////////////////////////////////
//				Crypto::Cert::X509Key *key;
//				NEW_CLASS(key, Crypto::Cert::X509Key(CSTR("public.key"), bstrPDU + 1, bstrLen - 1, keyType));
//				return key;
			}
		}
		else if (keyType != KeyType::Unknown)
		{
			Crypto::Cert::X509Key *key;
			NEW_CLASS(key, Crypto::Cert::X509Key(CSTR("public.key"), Data::ByteArrayR(bstrPDU + 1, bstrLen - 1), keyType));
			return key;
		}
	}
	return nullptr;
}

UIntOS Crypto::Cert::X509File::KeyGetLeng(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, KeyType keyType)
{
	UnsafeArray<const UInt8> keyPDU;
	UIntOS keyLen;
	Net::ASN1Util::ItemType itemType;
	switch (keyType)
	{
	case KeyType::RSA:
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", keyLen, itemType).SetTo(keyPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UIntOS cnt = Net::ASN1Util::PDUCountItem(keyPDU, keyPDU + keyLen, nullptr);
			if (cnt > 4)
			{
				UIntOS modulusLen;
				UnsafeArray<const UInt8> modulus;
				UIntOS privateExponentLen;
				UnsafeArray<const UInt8> privateExponent;
				if (Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "2", modulusLen, itemType).SetTo(modulus) &&
					Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "4", privateExponentLen, itemType).SetTo(privateExponent))
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
		if (Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", keyLen, itemType).SetTo(keyPDU) && itemType == Net::ASN1Util::IT_SEQUENCE)
		{
			UIntOS modulusLen;
			UnsafeArray<const UInt8> modulus;
			if (Net::ASN1Util::PDUGetItem(keyPDU, keyPDU + keyLen, "1", modulusLen, itemType).SetTo(modulus))
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

Crypto::Cert::X509File::KeyType Crypto::Cert::X509File::KeyTypeFromOID(Data::ByteArrayR oid, Bool pubKey)
{
	if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.2.840.113549.1.1.1")))
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
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.2.840.10045.2.1")))
	{
		if (pubKey)
		{
			return KeyType::ECPublic;
		}
		else
		{
			return KeyType::ECDSA;
		}
	}
	return KeyType::Unknown;
}

Crypto::Cert::X509File::ECName Crypto::Cert::X509File::ECNameFromOID(Data::ByteArrayR oid)
{
	if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.2.840.10045.3.1.7")))
	{
		return ECName::secp256r1;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.3.132.0.34")))
	{
		return ECName::secp384r1;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.3.132.0.35")))
	{
		return ECName::secp521r1;
	}
	return ECName::Unknown;
}

Bool Crypto::Cert::X509File::AlgorithmIdentifierGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<AlgType> algType)
{
	UIntOS cnt = Net::ASN1Util::PDUCountItem(pdu, pduEnd, nullptr);
	if (cnt != 2 && cnt != 1)
	{
		return false;
	}
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> itemPDU;
	if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.2"))) //md2WithRSAEncryption
	{
		algType.Set(AlgType::MD2WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.4"))) //md5WithRSAEncryption
	{
		algType.Set(AlgType::MD5WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.5"))) //sha1WithRSAEncryption
	{
		algType.Set(AlgType::SHA1WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.11"))) //sha256WithRSAEncryption
	{
		algType.Set(AlgType::SHA256WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.12"))) //sha384WithRSAEncryption
	{
		algType.Set(AlgType::SHA384WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.13"))) //sha512WithRSAEncryption
	{
		algType.Set(AlgType::SHA512WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.113549.1.1.14"))) //sha224WithRSAEncryption
	{
		algType.Set(AlgType::SHA224WithRSAEncryption);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.10045.4.3.2"))) //ecdsa-with-SHA256
	{
		algType.Set(AlgType::ECDSAWithSHA256);
	}
	else if (Net::ASN1Util::OIDEqualsText(Data::ByteArrayR(itemPDU, itemLen), CSTR("1.2.840.10045.4.3.3"))) //ecdsa-with-SHA384
	{
		algType.Set(AlgType::ECDSAWithSHA384);
	}
	else
	{
		algType.Set(AlgType::Unknown);
		return false;
	}

	return true;
}

Crypto::Cert::X509File::X509File(NN<Text::String> sourceName, Data::ByteArrayR buff) : Net::ASN1Data(sourceName, buff)
{
}

Crypto::Cert::X509File::X509File(Text::CStringNN sourceName, Data::ByteArrayR buff) : Net::ASN1Data(sourceName, buff)
{
}

Crypto::Cert::X509File::~X509File()
{
}

Net::ASN1Data::ASN1Type Crypto::Cert::X509File::GetASN1Type() const
{
	return ASN1Type::X509;
}

UIntOS Crypto::Cert::X509File::GetCertCount()
{
	return 0;
}

Bool Crypto::Cert::X509File::GetCertName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

Optional<Crypto::Cert::X509Cert> Crypto::Cert::X509File::GetNewCert(UIntOS index)
{
	return nullptr;
}

void Crypto::Cert::X509File::ToShortString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->Append(FileTypeGetName(this->GetFileType()));
	sb->AppendC(UTF8STRC(": "));
	this->ToShortName(sb);
}

Bool Crypto::Cert::X509File::IsSignatureKey(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key) const
{
	UIntOS itemOfst;
	UIntOS dataSize;
	UnsafeArray<const UInt8> data;
	Net::ASN1Util::ItemType itemType;
	UIntOS signSize;
	UnsafeArray<const UInt8> signature;
	if (!Net::ASN1Util::PDUGetItemRAW(this->buff.Arr(), this->buff.ArrEnd(), "1.1", dataSize, itemOfst).SetTo(data) ||
		!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", signSize, itemType).SetTo(signature) ||
		itemType != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	if (signature[0] != 0)
		return false;
	signature++;
	signSize--;
	if (!ssl->SignatureVerify(key, Crypto::Hash::HashType::SHA256, Data::ByteArrayR(data, dataSize + itemOfst), Data::ByteArrayR(signature, signSize)))
	{
		return false;
	}
	return true;
}

Bool Crypto::Cert::X509File::GetSignedInfo(NN<SignedInfo> signedInfo) const
{
	UIntOS itemLen;
	UIntOS itemOfst;
	Net::ASN1Util::ItemType itemType;
	if (!Net::ASN1Util::PDUGetItemRAW(this->buff.Arr(), this->buff.ArrEnd(), "1.1", itemLen, itemOfst).SetTo(signedInfo->payload))
	{
		return false;
	}
	signedInfo->payloadSize = itemLen + itemOfst;
	UnsafeArray<const UInt8> itemPDU;
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.2", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_SEQUENCE || !AlgorithmIdentifierGet(itemPDU, itemPDU + itemLen, signedInfo->algType))
	{
		return false;
	}
	if (!Net::ASN1Util::PDUGetItem(this->buff.Arr(), this->buff.ArrEnd(), "1.3", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_BIT_STRING)
	{
		return false;
	}
	signedInfo->signature = itemPDU + 1;
	signedInfo->signSize = itemLen - 1;
	return true;
}

Bool Crypto::Cert::X509File::ParseDigestType(NN<DigestInfo> digestInfo, UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd)
{
	UIntOS itemLen;
	Net::ASN1Util::ItemType itemType;
	UnsafeArray<const UInt8> itemPDU;
	if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.1.1", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_OID)
	{
		return false;
	}
	digestInfo->hashType = HashTypeFromOID(Data::ByteArrayR(itemPDU, itemLen));
	if (!Net::ASN1Util::PDUGetItem(pdu, pduEnd, "1.2", itemLen, itemType).SetTo(itemPDU) || itemType != Net::ASN1Util::IT_OCTET_STRING)
	{
		return false;
	}
	digestInfo->hashVal = itemPDU;
	digestInfo->hashLen = itemLen;
	return true;
}

Crypto::Hash::HashType Crypto::Cert::X509File::GetAlgHash(AlgType algType)
{
	switch (algType)
	{
	case AlgType::SHA1WithRSAEncryption:
		return Crypto::Hash::HashType::SHA1;
	case AlgType::SHA256WithRSAEncryption:
		return Crypto::Hash::HashType::SHA256;
	case AlgType::SHA512WithRSAEncryption:
		return Crypto::Hash::HashType::SHA512;
	case AlgType::SHA384WithRSAEncryption:
		return Crypto::Hash::HashType::SHA384;
	case AlgType::SHA224WithRSAEncryption:
		return Crypto::Hash::HashType::SHA224;
	case AlgType::MD2WithRSAEncryption:
		return Crypto::Hash::HashType::Unknown;
	case AlgType::MD5WithRSAEncryption:
		return Crypto::Hash::HashType::MD5;
	case AlgType::ECDSAWithSHA256:
		return Crypto::Hash::HashType::SHA256;
	case AlgType::ECDSAWithSHA384:
		return Crypto::Hash::HashType::SHA384;
	case AlgType::Unknown:
	default:
		return Crypto::Hash::HashType::Unknown;
	}
}

Text::CStringNN Crypto::Cert::X509File::FileTypeGetName(FileType fileType)
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
	case FileType::EPrivateKey:
		return CSTR("EPrivateKey");
	case FileType::PKCS7:
		return CSTR("PKCS7");
	case FileType::PKCS12:
		return CSTR("PKCS12");
	case FileType::CRL:
		return CSTR("CRL");
	case FileType::FileList:
		return CSTR("FileList");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Crypto::Cert::X509File::KeyTypeGetName(KeyType keyType)
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

Text::CStringNN Crypto::Cert::X509File::KeyTypeGetOID(KeyType keyType)
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

Text::CStringNN Crypto::Cert::X509File::ECNameGetName(ECName ecName)
{
	switch (ecName)
	{
	case ECName::secp256r1:
		return CSTR("secp256r1");
	case ECName::secp384r1:
		return CSTR("secp384r1");
	case ECName::secp521r1:
		return CSTR("secp521r1");
	case ECName::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Crypto::Cert::X509File::ECNameGetOID(ECName ecName)
{
	switch (ecName)
	{
	case ECName::secp256r1:
		return CSTR("1.2.840.10045.3.1.7");
	case ECName::secp384r1:
		return CSTR("1.3.132.0.34");
	case ECName::secp521r1:
		return CSTR("1.3.132.0.35");
	case ECName::Unknown:
	default:
		return CSTR("1.3.132.0.34");
	}
}

Text::CStringNN Crypto::Cert::X509File::ValidStatusGetName(ValidStatus validStatus)
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

Text::CStringNN Crypto::Cert::X509File::ValidStatusGetDesc(ValidStatus validStatus)
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

Crypto::Hash::HashType Crypto::Cert::X509File::HashTypeFromOID(Data::ByteArrayR oid)
{
	if (Net::ASN1Util::OIDEqualsText(oid, CSTR("2.16.840.1.101.3.4.2.1")))
	{
		return Crypto::Hash::HashType::SHA256;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("2.16.840.1.101.3.4.2.2")))
	{
		return Crypto::Hash::HashType::SHA384;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("2.16.840.1.101.3.4.2.3")))
	{
		return Crypto::Hash::HashType::SHA512;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("2.16.840.1.101.3.4.2.4")))
	{
		return Crypto::Hash::HashType::SHA224;
	}
	else if (Net::ASN1Util::OIDEqualsText(oid, CSTR("1.3.14.3.2.26")))
	{
		return Crypto::Hash::HashType::SHA1;
	}
	return Crypto::Hash::HashType::Unknown;
}

Optional<Crypto::Cert::X509File> Crypto::Cert::X509File::CreateFromCerts(NN<const Data::ReadingListNN<Crypto::Cert::Certificate>> certs)
{
	if (certs->GetCount() == 0)
	{
		return nullptr;
	}
	else if (certs->GetCount() == 1)
	{
		return certs->GetItemNoCheck(0)->CreateX509Cert();
	}
	else
	{
		UIntOS i = 1;
		UIntOS j = certs->GetCount();
		Crypto::Cert::X509FileList *certList = 0;
		NN<Crypto::Cert::X509Cert> cert;
		if (certs->GetItemNoCheck(0)->CreateX509Cert().SetTo(cert))
		{
			NEW_CLASS(certList, Crypto::Cert::X509FileList(cert->GetSourceNameObj(), cert));
		}
		while (i < j)
		{
			if (certs->GetItemNoCheck(i)->CreateX509Cert().SetTo(cert))
			{
				if (certList)
					certList->AddFile(cert);
				else
				{
					NEW_CLASS(certList, Crypto::Cert::X509FileList(cert->GetSourceNameObj(), cert));
				}
			}
			i++;
		}
		return certList;
	}
}

Optional<Crypto::Cert::X509File> Crypto::Cert::X509File::CreateFromCertsAndClear(NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> certs)
{
	if (certs->GetCount() == 0)
	{
		return nullptr;
	}
	else if (certs->GetCount() == 1)
	{
		return certs->RemoveAt(0);
	}
	else
	{
		UIntOS i = 1;
		UIntOS j = certs->GetCount();
		NN<Crypto::Cert::X509FileList> certList;
		NN<Crypto::Cert::X509Cert> cert = certs->GetItemNoCheck(0);
		NEW_CLASSNN(certList, Crypto::Cert::X509FileList(cert->GetSourceNameObj(), cert));
		while (i < j)
		{
			certList->AddFile(certs->GetItemNoCheck(i));
			i++;
		}
		certs->Clear();
		return certList;
	}
}

UIntOS Crypto::Cert::X509File::ECSignData2RAW(UnsafeArray<UInt8> outBuff, Data::ByteArrayR signData)
{
	UnsafeArray<const UInt8> buff1;
	UIntOS len1;
	UnsafeArray<const UInt8> buff2;
	UIntOS len2;
	Net::ASN1Util::ItemType type1;
	Net::ASN1Util::ItemType type2;
	if (Net::ASN1Util::PDUGetItem(signData.Arr(), signData.ArrEnd(), "1.1", len1, type1).SetTo(buff1) &&
		Net::ASN1Util::PDUGetItem(signData.Arr(), signData.ArrEnd(), "1.2", len2, type2).SetTo(buff2) &&
		type1 == Net::ASN1Util::ItemType::IT_INTEGER && type2 == Net::ASN1Util::ItemType::IT_INTEGER)
	{
		if (buff1[0] == 0)
		{
			buff1++;
			len1--;
		}
		if (buff2[0] == 0)
		{
			buff2++;
			len2--;
		}
		MemCopyNO(outBuff.Ptr(), buff1.Ptr(), len1);
		MemCopyNO(&outBuff[len1], buff2.Ptr(), len2);
		return len1 + len2;
	}
	return 0;
}
