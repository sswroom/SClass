#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPOIDDB.h"
#include "Net/SNMPUtil.h"

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
	OSInt len;
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
	OSInt len;
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
	sb->AppendC(pdu, (UOSInt)len);
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

Bool Net::ASN1Util::PDUToString(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, UOSInt level)
{
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
		case 0x2:
			if (len <= 4)
			{
				pdu = PDUParseUInt32(pdu, pduEnd, &iVal);
				if (pdu == 0)
				{
					return false;
				}
				sb->AppendChar('\t', level);
				sb->Append((const UTF8Char*)"INTEGER ");
				sb->AppendU32(iVal);
				sb->Append((const UTF8Char*)"\r\n");
			}
			else if (len <= 32)
			{
				sb->AppendChar('\t', level);
				sb->Append((const UTF8Char*)"BINARY ");
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LBT_NONE);
				sb->Append((const UTF8Char*)"\r\n");
				pdu += ofst + len;
			}
			else
			{
				sb->AppendChar('\t', level);
				sb->Append((const UTF8Char*)"BINARY\r\n");
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LBT_CRLF);
				sb->Append((const UTF8Char*)"\r\n");
				pdu += ofst + len;
			}
			break;
		case 0x3:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"BIT STRING ");
			sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x5:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"NULL\r\n");
			pdu += ofst + len;
			break;
		case 0x6:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"OID ");
			Net::SNMPUtil::OIDToString(&pdu[ofst], len, sb);
			sb->Append((const UTF8Char*)" (");
			Net::SNMPOIDDB::OIDToNameString(&pdu[ofst], len, sb);
			sb->Append((const UTF8Char*)")\r\n");
			pdu += ofst + len;
			break;
		case 0x0C:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"UTF8String ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x12:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"NumericString ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x13:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"PrintableString ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x14:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"T61String ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x15:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"VideotexString ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x16:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"IA5String ");
			sb->AppendC(&pdu[ofst], len);
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		case 0x17:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"UTCTIME ");
			if (len == 13 && pdu[ofst + 12] == 'Z')
			{
				Data::DateTime dt;
				dt.SetCurrTimeUTC();
				dt.SetValue((UInt16)((UInt32)(dt.GetYear() / 100) * 100 + Str2Digit(&pdu[ofst])), Str2Digit(&pdu[ofst + 2]), Str2Digit(&pdu[ofst + 4]), Str2Digit(&pdu[ofst + 6]), Str2Digit(&pdu[ofst + 8]), Str2Digit(&pdu[ofst + 10]), 0);
				sb->AppendDate(&dt);
			}
			else
			{
				sb->AppendC(&pdu[ofst], len);
			}
			sb->Append((const UTF8Char*)"\r\n");
			pdu += ofst + len;
			break;
		default:
			if (type < 0x30)
			{
				sb->AppendChar('\t', level);
				sb->Append((const UTF8Char*)"UNKNOWN 0x");
				sb->AppendHex8(type);
				sb->Append((const UTF8Char*)" (");
				sb->AppendHexBuff(&pdu[ofst], len, ' ', Text::LBT_NONE);
				sb->Append((const UTF8Char*)")\r\n");
				pdu += ofst + len;
				break;
			}
			return false;
		case 0x30:
		case 0x31:
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"SEQUENCE {\r\n");
			pdu += ofst;
			if (!PDUToString(pdu, pdu + len, sb, level + 1))
			{
				return false;
			}
			sb->AppendChar('\t', level);
			sb->Append((const UTF8Char*)"}\r\n");
			pdu += len;
			break;
		}
	}
	return true;
}

UInt32 Net::ASN1Util::Str2Digit(const UTF8Char *s)
{
	return (UInt32)(s[0] - 0x30) * 10 + (UInt32)(s[1] - 0x30);
}