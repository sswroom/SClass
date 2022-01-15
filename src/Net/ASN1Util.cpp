#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "Net/ASN1OIDDB.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

UOSInt Net::ASN1Util::PDUParseLen(const UInt8 *pdu, UOSInt ofst, UOSInt pduSize, UInt32 *len)
{
	if (ofst >= pduSize)
	{
		*len = 0;
		return pduSize + 1;
	}
	if (pdu[ofst] & 0x80)
	{
		if (pdu[ofst] == 0x81)
		{
			if (ofst + 2 > pduSize)
			{
				*len = 0;
				return pduSize + 1;
			}
			*len = pdu[ofst + 1];
			return ofst + 2;
		}
		else if (pdu[ofst] == 0x82)
		{
			if (ofst + 3 > pduSize)
			{
				*len = 0;
				return pduSize + 1;
			}
			*len = ReadMUInt16(&pdu[ofst + 1]);
			return ofst + 3;
		}
		else if (pdu[ofst] == 0x83)
		{
			if (ofst + 4 > pduSize)
			{
				*len = 0;
				return pduSize + 1;
			}
			*len = ReadMUInt24(&pdu[ofst + 1]);
			return ofst + 4;
		}
		else if (pdu[ofst] == 0x84)
		{
			if (ofst + 5 > pduSize)
			{
				*len = 0;
				return pduSize + 1;
			}
			*len = ReadMUInt32(&pdu[ofst + 1]);
			return ofst + 5;
		}
		*len = 0;
		return pduSize + 1;
	}
	else
	{
		*len = pdu[ofst];
		return ofst + 1;
	}
}

const UInt8 *Net::ASN1Util::PDUParseSeq(const UInt8 *pdu, const UInt8 *pduEnd, UInt8 *type, const UInt8 **seqEnd)
{
	UOSInt len;
	if (pduEnd - pdu < 2)
		return 0;
	*type = pdu[0];
	if (pdu[1] < 0x80)
	{
		len = pdu[1];
		pdu += 2;
	}
	else if (pdu[1] == 0x81 && pduEnd - pdu >= 3)
	{
		len = pdu[2];
		pdu += 3;
	}
	else if (pdu[1] == 0x82 && pduEnd - pdu >= 4)
	{
		len = ReadMUInt16(&pdu[2]);
		pdu += 4;
	}
	else if (pdu[1] == 0x83 && pduEnd - pdu >= 5)
	{
		len = ReadMUInt24(&pdu[2]);
		pdu += 5;
	}
	else if (pdu[1] == 0x84 && pduEnd - pdu >= 6)
	{
		len = ReadMUInt32(&pdu[2]);
		pdu += 6;
	}
	else
	{
		return 0;
	}
	if (pdu + len > pduEnd)
	{
		return 0;
	}
	*seqEnd = pdu + len;
	return pdu;
}

const UInt8 *Net::ASN1Util::PDUParseUInt32(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val)
{
	if (pduEnd - pdu < 3)
		return 0;
	if (pdu[0] != 2)
		return 0;
	if (pdu[1] == 1)
	{
		*val = pdu[2];
		return pdu + 3;
	}
	else if (pdu[1] == 2 && pduEnd - pdu >= 4)
	{
		*val = ReadMUInt16(&pdu[2]);
		return pdu + 4;
	}
	else if (pdu[1] == 3 && pduEnd - pdu >= 5)
	{
		*val = ReadMUInt24(&pdu[2]);
		return pdu + 5;
	}
	else if (pdu[1] == 4 && pduEnd - pdu >= 6)
	{
		*val = ReadMUInt32(&pdu[2]);
		return pdu + 6;
	}
	else
	{
		return 0;
	}
}

const UInt8 *Net::ASN1Util::PDUParseString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb)
{
	UOSInt len;
	if (pduEnd - pdu < 2)
		return 0;
	if (pdu[0] != 4)
		return 0;
	if (pdu[1] < 0x80)
	{
		len = pdu[1];
		pdu += 2;
	}
	else if (pdu[1] == 0x81 && pduEnd - pdu >= 3)
	{
		len = pdu[2];
		pdu += 3;
	}
	else if (pdu[1] == 0x82 && pduEnd - pdu >= 4)
	{
		len = ReadMUInt16(&pdu[2]);
		pdu += 4;
	}
	else if (pdu[1] == 0x83 && pduEnd - pdu >= 5)
	{
		len = ReadMUInt24(&pdu[2]);
		pdu += 5;
	}
	else if (pdu[1] == 0x84 && pduEnd - pdu >= 6)
	{
		len = ReadMUInt32(&pdu[2]);
		pdu += 6;
	}
	else
	{
		return 0;
	}
	if (pdu + len > pduEnd)
	{
		return 0;
	}
	sb->AppendC(pdu, len);
	return pdu + len;
}

const UInt8 *Net::ASN1Util::PDUParseChoice(const UInt8 *pdu, const UInt8 *pduEnd, UInt32 *val)
{
	if (pduEnd - pdu < 3)
		return 0;
	if (pdu[0] != 10)
		return 0;
	if (pdu[1] == 1)
	{
		*val = pdu[2];
		return pdu + 3;
	}
	else if (pdu[1] == 2 && pduEnd - pdu >= 4)
	{
		*val = ReadMUInt16(&pdu[2]);
		return pdu + 4;
	}
	else if (pdu[1] == 3 && pduEnd - pdu >= 5)
	{
		*val = ReadMUInt24(&pdu[2]);
		return pdu + 5;
	}
	else if (pdu[1] == 4 && pduEnd - pdu >= 6)
	{
		*val = ReadMUInt32(&pdu[2]);
		return pdu + 6;
	}
	else
	{
		return 0;
	}
}

Bool Net::ASN1Util::PDUParseUTCTimeCont(const UInt8 *pdu, UOSInt len, Data::DateTime *dt)
{
	if (len == 13 && pdu[12] == 'Z')
	{
		dt->SetCurrTimeUTC();
		dt->SetValue((UInt16)((UInt32)(dt->GetYear() / 100) * 100 + Str2Digit(pdu)), (OSInt)Str2Digit(&pdu[2]), (OSInt)Str2Digit(&pdu[4]), (OSInt)Str2Digit(&pdu[6]), (OSInt)Str2Digit(&pdu[8]), (OSInt)Str2Digit(&pdu[10]), 0);
		return true;
	}
	return false;
}

Bool Net::ASN1Util::PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, UOSInt level)
{
	Text::StringBuilderUTF8 *innerSb;
	while (pdu < pduEnd)
	{
		UInt8 type = pdu[0];
		UInt32 len;
		UInt32 iVal;
		UOSInt ofst;
		UOSInt size;

		size = (UOSInt)(pduEnd - pdu);
		ofst = PDUParseLen(pdu, 1, size, &len);
		if (ofst > size)
		{
			return false;
		}
		else if (ofst + len > size)
		{
			return false;
		}

		switch (type)
		{
		case 0x1:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("BOOLEAN "));
			if (len == 1)
			{
				if (pdu[ofst] == 0xFF)
				{
					sb->AppendC(UTF8STRC("(0xFF TRUE)"));
				}
				else if (pdu[ofst] == 0)
				{
					sb->AppendC(UTF8STRC("(0x00 FALSE)"));
				}
				else
				{
					sb->AppendC(UTF8STRC("(0x"));
					sb->AppendHex8(pdu[ofst]);
					sb->AppendChar(')', 1);
				}
			}
			else
			{
				sb->AppendChar('(', 1);
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
				sb->AppendChar(')', 1);
			}
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x2:
			if (len <= 4)
			{
				pdu = PDUParseUInt32(pdu, pduEnd, &iVal);
				if (pdu == 0)
				{
					return false;
				}
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("INTEGER "));
				sb->AppendU32(iVal);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			else if (len <= 32)
			{
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("INTEGER "));
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC("\r\n"));
				pdu += ofst + len;
			}
			else
			{
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("INTEGER\r\n"));
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::CRLF);
				sb->AppendC(UTF8STRC("\r\n"));
				pdu += ofst + len;
			}
			break;
		case 0x3:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("BIT STRING "));
			sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x4:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("OCTET STRING "));
			NEW_CLASS(innerSb, Text::StringBuilderUTF8());
			if (PDUToString(&pdu[ofst], &pdu[ofst + len], innerSb, level + 1))
			{
				sb->AppendC(UTF8STRC("{\r\n"));
				sb->Append(innerSb->ToString());
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("}\r\n"));
			}
			else
			{
				sb->AppendC(UTF8STRC("("));
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC(")\r\n"));
			}
			DEL_CLASS(innerSb);
			pdu += ofst + len;
			break;
		case 0x5:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("NULL\r\n"));
			pdu += ofst + len;
			break;
		case 0x6:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("OID "));
			Net::ASN1Util::OIDToString(&pdu[ofst], len, sb);
			sb->AppendC(UTF8STRC(" ("));
			Net::ASN1OIDDB::OIDToNameString(&pdu[ofst], len, sb);
			sb->AppendC(UTF8STRC(")\r\n"));
			pdu += ofst + len;
			break;
		case 0x0C:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("UTF8String "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x12:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("NumericString "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x13:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("PrintableString "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x14:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("T61String "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x15:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("VideotexString "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x16:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("IA5String "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x17:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("UTCTIME "));
			if (len == 13 && pdu[ofst + 12] == 'Z')
			{
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				dt.SetValue((UInt16)((UInt32)(dt.GetYear() / 100) * 100 + Str2Digit(&pdu[ofst])), (OSInt)Str2Digit(&pdu[ofst + 2]), (OSInt)Str2Digit(&pdu[ofst + 4]), (OSInt)Str2Digit(&pdu[ofst + 6]), (OSInt)Str2Digit(&pdu[ofst + 8]), (OSInt)Str2Digit(&pdu[ofst + 10]), 0);
				sb->AppendDate(&dt);
			}
			else
			{
				sb->AppendC(&pdu[ofst], len);
			}
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x1C:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("UniversalString "));
			sb->AppendC(&pdu[ofst], len);
			sb->AppendC(UTF8STRC("\r\n"));
			pdu += ofst + len;
			break;
		case 0x1E:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("BMPString ("));
			sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC(")\r\n"));
			pdu += ofst + len;
			break;
		default:
			if (type < 0x30)
			{
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("UNKNOWN 0x"));
				sb->AppendHex8(type);
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC(")\r\n"));
				pdu += ofst + len;
				break;
			}
			else
			{
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("UNKNOWN 0x"));
				sb->AppendHex8(type);
				NEW_CLASS(innerSb, Text::StringBuilderUTF8());
				if (PDUToString(&pdu[ofst], &pdu[ofst + len], innerSb, level + 1))
				{
					sb->AppendC(UTF8STRC(" {\r\n"));
					sb->Append(innerSb->ToString());
					sb->AppendChar('\t', level);
					sb->AppendC(UTF8STRC("}\r\n"));
				}
				else
				{
					sb->AppendC(UTF8STRC(" ("));
					if (Text::StringTool::IsASCIIText(&pdu[ofst], len))
					{
						sb->AppendC(&pdu[ofst], len);
					}
					else
					{
						sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
					}
					sb->AppendC(UTF8STRC(")\r\n"));
				}
				DEL_CLASS(innerSb);
				pdu += ofst + len;
				break;
			}
		case 0x30:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("SEQUENCE {\r\n"));
			pdu += ofst;
			if (!PDUToString(pdu, pdu + len, sb, level + 1))
			{
				return false;
			}
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("}\r\n"));
			pdu += len;
			break;
		case 0x31:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("SET {\r\n"));
			pdu += ofst;
			if (!PDUToString(pdu, pdu + len, sb, level + 1))
			{
				return false;
			}
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("}\r\n"));
			pdu += len;
			break;
		case 0xA0:
		case 0xA1:
		case 0xA2:
		case 0xA3:
			sb->AppendChar('\t', level);
			sb->AppendC(UTF8STRC("CONTEXT SPECIFIC["));
			sb->AppendU16((UInt16)(type - 0xA0));
			sb->AppendC(UTF8STRC("] "));
			NEW_CLASS(innerSb, Text::StringBuilderUTF8());
			if (PDUToString(&pdu[ofst], &pdu[ofst + len], innerSb, level + 1))
			{
				sb->AppendC(UTF8STRC("{\r\n"));
				sb->Append(innerSb->ToString());
				sb->AppendChar('\t', level);
				sb->AppendC(UTF8STRC("}\r\n"));
			}
			else
			{
				sb->AppendC(UTF8STRC("("));
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LineBreakType::None);
				sb->AppendC(UTF8STRC(")\r\n"));
			}
			DEL_CLASS(innerSb);
			pdu += ofst + len;
			break;
		}
	}
	return true;
}

const UInt8 *Net::ASN1Util::PDUGetItemRAW(const UInt8 *pdu, const UInt8 *pduEnd, const Char *cpath, UOSInt *len, UOSInt *itemOfst)
{
	if (len)
	{
		*len = 0;
	}
	UTF8Char sbuff[11];
	const UTF8Char *path = (const UTF8Char*)cpath;
	UInt32 itemLen;
	UOSInt size;
	UOSInt cnt;
	UOSInt ofst;
	if (path == 0 || path[0] == 0)
	{
		return 0;
	}
	size = Text::StrIndexOf(path, '.');
	if (size == INVALID_INDEX)
	{
		cnt = Text::StrToUOSInt(path);
		path = 0;
	}
	else if (size > 0 && size < 11)
	{
		Text::StrConcatC(sbuff, path, size);
		cnt = Text::StrToUOSInt(sbuff);
		path += size + 1;
	}
	else
	{
		return 0;
	}

	if (cnt == 0)
	{
		return 0;
	}

	while (pdu < pduEnd)
	{
		size = (UOSInt)(pduEnd - pdu);
		ofst = PDUParseLen(pdu, 1, size, &itemLen);
		if (ofst > size)
		{
			return 0;
		}
		else if (ofst + itemLen > size)
		{
			return 0;
		}

		cnt--;
		if (cnt == 0)
		{
			if (path == 0)
			{
				if (itemOfst)
					*itemOfst = ofst;
				if (len)
					*len = itemLen;
				return pdu;
			}
			return PDUGetItemRAW(&pdu[ofst], &pdu[ofst + itemLen], (const Char*)path, len, itemOfst);
		}
		pdu += ofst + itemLen;
	}
	return 0;
}

const UInt8 *Net::ASN1Util::PDUGetItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, UOSInt *len, ItemType *itemType)
{
	if (itemType)
	{
		*itemType = IT_UNKNOWN;
	}
	UOSInt itemOfst;
	pdu = PDUGetItemRAW(pdu, pduEnd, path, len, &itemOfst);
	if (pdu == 0)
	{
		return 0;
	}
	if (itemType)
	{
		*itemType = (ItemType)pdu[0];
	}
	return &pdu[itemOfst];
}

Net::ASN1Util::ItemType Net::ASN1Util::PDUGetItemType(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path)
{
	Net::ASN1Util::ItemType itemType;
	PDUGetItem(pdu, pduEnd, path, 0, &itemType);
	return itemType;
}

UOSInt Net::ASN1Util::PDUCountItem(const UInt8 *pdu, const UInt8 *pduEnd, const Char *cpath)
{
	UTF8Char sbuff[11];
	const UTF8Char *path = (const UTF8Char*)cpath;
	UInt32 len;
	UOSInt size;
	UOSInt cnt;
	UOSInt ofst;
	if (path == 0 || path[0] == 0)
	{
		cnt = 0;
		while (pdu < pduEnd)
		{
			size = (UOSInt)(pduEnd - pdu);
			ofst = PDUParseLen(pdu, 1, size, &len);
			if (ofst > size)
			{
				return 0;
			}
			else if (ofst + len > size)
			{
				return 0;
			}
			cnt++;
			pdu += ofst + len;
		}
		return cnt;
	}
	size = Text::StrIndexOf(path, '.');
	if (size == INVALID_INDEX)
	{
		cnt = Text::StrToUOSInt(path);
		path = 0;
	}
	else if (size > 0 && size < 11)
	{
		Text::StrConcatC(sbuff, path, size);
		cnt = Text::StrToUOSInt(sbuff);
		path += size + 1;
	}
	else
	{
		return 0;
	}

	if (cnt == 0)
	{
		return 0;
	}

	while (pdu < pduEnd)
	{
		size = (UOSInt)(pduEnd - pdu);
		ofst = PDUParseLen(pdu, 1, size, &len);
		if (ofst > size)
		{
			return 0;
		}
		else if (ofst + len > size)
		{
			return 0;
		}

		cnt--;
		if (cnt == 0)
		{
			return PDUCountItem(pdu + ofst, pdu + ofst + len, (const Char*)path);
		}
		pdu += ofst + len;
	}
	return 0;
}

Bool Net::ASN1Util::PDUIsValid(const UInt8 *pdu, const UInt8 *pduEnd)
{
	UInt32 len;
	UOSInt size;
	UOSInt ofst;
	while (pdu < pduEnd)
	{
		size = (UOSInt)(pduEnd - pdu);
		ofst = PDUParseLen(pdu, 1, size, &len);
		if (ofst > size)
		{
			return false;
		}
		else if (ofst + len > size)
		{
			return false;
		}
		if (pdu[0] >= 0x30 && pdu[0] < 0x40)
		{
			if (!PDUIsValid(pdu + ofst, pdu + ofst + size))
			{
				return false;
			}
		}
		pdu += ofst + len;
	}
	return true;
}

OSInt Net::ASN1Util::OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len)
{
	UOSInt i = 0;
	while (true)
	{
		if (i == oid1Len && i == oid2Len)
		{
			return 0;
		}
		else if (i >= oid1Len)
		{
			return -1;
		}
		else if (i >= oid2Len)
		{
			return 1;
		}
		else if (oid1[i] > oid2[i])
		{
			return 1;
		}
		else if (oid1[i] < oid2[i])
		{
			return -1;
		}
		i++;
	}
}

Bool Net::ASN1Util::OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len)
{
	if (oid1Len < oid2Len)
		return false;
	UOSInt i = 0;
	while (i < oid2Len)
	{
		if (oid1[i] != oid2[i])
			return false;
		i++;
	}
	return true;
}

Bool Net::ASN1Util::OIDEqualsText(const UInt8 *oidPDU, UOSInt oidPDULen, const UTF8Char *oidText, UOSInt oidTextLen)
{
	UInt8 oid2[32];
	UOSInt oidLen2 = OIDText2PDU(oidText, oidTextLen, oid2);
	return OIDCompare(oidPDU, oidPDULen, oid2, oidLen2) == 0;
}

void Net::ASN1Util::OIDToString(const UInt8 *pdu, UOSInt pduSize, Text::StringBuilderUTF *sb)
{
	UInt32 v = 0;
	UOSInt i = 1;
	sb->AppendU16(pdu[0] / 40);
	sb->AppendChar('.', 1);
	sb->AppendU16(pdu[0] % 40);
	while (i < pduSize)
	{
		v = (v << 7) | (pdu[i] & 0x7f);
		if ((pdu[i] & 0x80) == 0)
		{
			sb->AppendChar('.', 1);
			sb->AppendU32(v);
			v = 0;
		}
		i++;
	}
}

UOSInt Net::ASN1Util::OIDCalcPDUSize(const UTF8Char *oidText, UOSInt oidTextLen)
{
	UInt32 v;
	UOSInt retSize = 1;
	UTF8Char *buff = MemAlloc(UTF8Char, oidTextLen + 1);
	Text::StrConcatC(buff, oidText, oidTextLen);
	UTF8Char *sarr[3];
	UOSInt i;
	i = Text::StrSplit(sarr, 3, buff, '.');
	if (i == 1 || i == 2)
	{
		MemFree(buff);
		return 1;
	}
	sarr[1] = sarr[2];
	while (true)
	{
		i = Text::StrSplit(sarr, 2, sarr[1], '.');
		if (!Text::StrToUInt32(sarr[0], &v))
		{
			MemFree(buff);
			return retSize;
		}
		while (v >= 128)
		{
			retSize++;
			v = v >> 7;
		}
		retSize++;
		if (i != 2)
			break;
	}
	MemFree(buff);
	return retSize;
}

UOSInt Net::ASN1Util::OIDText2PDU(const UTF8Char *oidText, UOSInt oidTextLen, UInt8 *pduBuff)
{
	UInt32 v;
	UOSInt retSize = 1;
	UTF8Char *buff = MemAlloc(UTF8Char, oidTextLen + 1);
	Text::StrConcatC(buff, oidText, oidTextLen);
	UTF8Char *sarr[3];
	UOSInt i;
	i = Text::StrSplit(sarr, 3, buff, '.');
	if (i == 1)
	{
		if (!Text::StrToUInt8(sarr[0], &pduBuff[0]))
		{
			MemFree(buff);
			return 0;
		}
		pduBuff[0] = (UInt8)(pduBuff[0] * 40);
		MemFree(buff);
		return 1;
	}
	if (!Text::StrToUInt8(sarr[0], &pduBuff[0]) || !Text::StrToUInt8(sarr[1], &pduBuff[1]))
	{
		MemFree(buff);
		return 0;
	}
	pduBuff[0] = (UInt8)(pduBuff[0] * 40 + pduBuff[1]);
	if (i == 2)
	{
		MemFree(buff);
		return 1;
	}
	sarr[1] = sarr[2];
	while (true)
	{
		i = Text::StrSplit(sarr, 2, sarr[1], '.');
		if (!Text::StrToUInt32(sarr[0], &v))
		{
			MemFree(buff);
			return 0;
		}
		if (v < 128)
		{
			pduBuff[retSize] = (UInt8)v;
			retSize++;
		}
		else if (v < 0x4000)
		{
			pduBuff[retSize] = 0x80 | (UInt8)(v >> 7);
			pduBuff[retSize + 1] = (UInt8)(v & 0x7f);
			retSize += 2;
		}
		else if (v < 0x200000)
		{
			pduBuff[retSize] = 0x80 | (UInt8)(v >> 14);
			pduBuff[retSize + 1] = 0x80 | (UInt8)((v >> 7) & 0x7f);
			pduBuff[retSize + 2] = (UInt8)(v & 0x7f);
			retSize += 3;
		}
		else if (v < 0x10000000)
		{
			pduBuff[retSize] = 0x80 | (UInt8)(v >> 21);
			pduBuff[retSize + 1] = 0x80 | (UInt8)((v >> 14) & 0x7f);
			pduBuff[retSize + 2] = 0x80 | (UInt8)((v >> 7) & 0x7f);
			pduBuff[retSize + 3] = (UInt8)(v & 0x7f);
			retSize += 4;
		}
		else
		{
			pduBuff[retSize] = 0x80 | (UInt8)(v >> 28);
			pduBuff[retSize + 1] = 0x80 | (UInt8)((v >> 21) & 0x7f);
			pduBuff[retSize + 2] = 0x80 | (UInt8)((v >> 14) & 0x7f);
			pduBuff[retSize + 3] = 0x80 | (UInt8)((v >> 7) & 0x7f);
			pduBuff[retSize + 4] = (UInt8)(v & 0x7f);
			retSize += 5;
		}
		
		if (i != 2)
			break;
	}
	MemFree(buff);
	return retSize;
}

void Net::ASN1Util::OIDToCPPCode(const UInt8 *oid, UOSInt oidLen, const UTF8Char *objectName, Text::StringBuilderUTF *sb)
{
	OSInt k;
	sb->AppendChar('\t', 1);
	sb->AppendC(UTF8STRC("{\""));
	sb->Append(objectName);
	sb->AppendC(UTF8STRC("\","));
	k = (OSInt)(60 - Text::StrCharCnt(objectName));
	if (k > 0)
	{
		sb->AppendChar('\t', (UOSInt)(k + 3) >> 2);
	}
	if (oidLen < 10)
	{
		sb->AppendUOSInt(oidLen);
		sb->AppendC(UTF8STRC(",  {"));
	}
	else
	{
		sb->AppendUOSInt(oidLen);
		sb->AppendC(UTF8STRC(", {"));
	}
	k = 0;
	while (k < (OSInt)oidLen)
	{
		if (k > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		sb->AppendC(UTF8STRC("0x"));
		sb->AppendHex8(oid[k]);
		k++;
	}
	sb->AppendC(UTF8STRC("}}, // "));
	OIDToString(oid, oidLen, sb);
	sb->AppendC(UTF8STRC("\r\n"));
}

UInt32 Net::ASN1Util::Str2Digit(const UTF8Char *s)
{
	return (UInt32)(s[0] - 0x30) * 10 + (UInt32)(s[1] - 0x30);
}