#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPInfo.h"
#include "Net/SNMPUtil.h"
#include "Net/SocketUtil.h"

Net::SNMPInfo::SNMPInfo()
{

}

Net::SNMPInfo::~SNMPInfo()
{

}

void Net::SNMPInfo::PDUSeqGetDetail(const UInt8 *pdu, UOSInt pduSize, UOSInt level, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	sb->AppendC(UTF8STRC("{\r\n"));
	UOSInt i = 0;
	while (i < pduSize)
	{
		i += this->PDUGetDetail(CSTR_NULL, &pdu[i], pduSize - i, level + 1, sb);
		if (i < pduSize)
		{
			sb->AppendC(UTF8STRC(",\r\n"));
		}
	}
	sb->AppendC(UTF8STRC("\r\n"));
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	sb->AppendC(UTF8STRC("}"));
}

UOSInt Net::SNMPInfo::PDUGetDetail(Text::CString name, const UInt8 *pdu, UOSInt pduSize, UOSInt level, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (pduSize < 2)
	{
		return pduSize;
	}
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	if (name.leng > 0)
	{
		sb->Append(name);
		sb->AppendUTF8Char(' ');
	}
	UInt8 t = pdu[0];
	UOSInt len = pdu[1];
	UOSInt hdrSize = 2;
	if (len & 0x80)
	{
		if (len == 0x81)
		{
			len = pdu[2];
			hdrSize = 3;
		}
		else if (len == 0x82)
		{
			len = ReadMUInt16(&pdu[2]);
			hdrSize = 4;
		}
		else if (len == 0x83)
		{
			len = ReadMUInt24(&pdu[2]);
			hdrSize = 5;
		}
		else if (len == 0x84)
		{
			len = ReadMUInt32(&pdu[2]);
			hdrSize = 6;
		}
	}
	if (hdrSize + len > pduSize)
	{
		sb->AppendHexBuff(pdu, pduSize, ' ', Text::LineBreakType::None);
		return pduSize;
	}
	switch (t)
	{
	case 2:
		sb->AppendC(UTF8STRC("INTEGER "));
		if (len == 1)
		{
			sb->AppendI16((Int8)pdu[hdrSize]);
		}
		else if (len == 2)
		{
			sb->AppendI16(ReadMInt16(&pdu[hdrSize]));
		}
		else if (len == 3)
		{
			sb->AppendI32(ReadMInt24(&pdu[hdrSize]));
		}
		else if (len == 4)
		{
			sb->AppendI32(ReadMInt32(&pdu[hdrSize]));
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 4:
		sb->AppendC(UTF8STRC("OCTET STRING "));
		{
			Bool isBin = false;
			UOSInt i = 0;
			while (i < len)
			{
				if (pdu[i + hdrSize] < 0x20 || pdu[i + hdrSize] >= 0x7f)
				{
					isBin = true;
					break;
				}
				i++;
			}
			if (isBin)
			{
				sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
			}
			else
			{
				sb->AppendC(UTF8STRC("\""));
				sb->AppendC(&pdu[hdrSize], len);
				sb->AppendC(UTF8STRC("\""));
			}
		}
		return len + hdrSize;
	case 5:
		sb->AppendC(UTF8STRC("NULL"));
		if (len > 0)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 6:
		sb->AppendC(UTF8STRC("OBJECT IDENTIFIER "));
		Net::ASN1Util::OIDToString(&pdu[hdrSize], len, sb);
		sb->AppendC(UTF8STRC(" ("));
		Net::ASN1OIDDB::OIDToNameString(&pdu[hdrSize], len, sb);
		sb->AppendC(UTF8STRC(")"));
		return len + hdrSize;
	case 0x30:
		sb->AppendC(UTF8STRC("SEQUENCE\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0x40:
		sb->AppendC(UTF8STRC("IpAddress "));
		if (len == 4)
		{
			UTF8Char sbuff[16];
			UTF8Char *sptr;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&pdu[hdrSize]));
			sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 0x41:
		sb->AppendC(UTF8STRC("Counter32 "));
		if (len == 1)
		{
			sb->AppendU16(pdu[hdrSize]);
		}
		else if (len == 2)
		{
			sb->AppendU16(ReadMUInt16(&pdu[hdrSize]));
		}
		else if (len == 3)
		{
			sb->AppendU32(ReadMUInt24(&pdu[hdrSize]));
		}
		else if (len == 4)
		{
			sb->AppendU32(ReadMUInt32(&pdu[hdrSize]));
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 0x42:
		sb->AppendC(UTF8STRC("Gauge32 "));
		if (len == 1)
		{
			sb->AppendU16(pdu[hdrSize]);
		}
		else if (len == 2)
		{
			sb->AppendU16(ReadMUInt16(&pdu[hdrSize]));
		}
		else if (len == 3)
		{
			sb->AppendU32(ReadMUInt24(&pdu[hdrSize]));
		}
		else if (len == 4)
		{
			sb->AppendU32(ReadMUInt32(&pdu[hdrSize]));
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 0x43:
		sb->AppendC(UTF8STRC("Timeticks "));
		if (len == 1)
		{
			sb->AppendU16(pdu[hdrSize]);
		}
		else if (len == 2)
		{
			sb->AppendU16(ReadMUInt16(&pdu[hdrSize]));
		}
		else if (len == 3)
		{
			sb->AppendU32(ReadMUInt24(&pdu[hdrSize]));
		}
		else if (len == 4)
		{
			sb->AppendU32(ReadMUInt32(&pdu[hdrSize]));
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		}
		return len + hdrSize;
	case 0xA0:
		sb->AppendC(UTF8STRC("GetRequest-PDU\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA1:
		sb->AppendC(UTF8STRC("GetNextRequest-PDU\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA2:
		sb->AppendC(UTF8STRC("GetResponse-PDU\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA3:
		sb->AppendC(UTF8STRC("SetRequest-PDU\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA4:
		sb->AppendC(UTF8STRC("Trap-PDU\r\n"));
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	default:
		sb->AppendC(UTF8STRC("UNKNOWN("));
		sb->AppendU16(t);
		sb->AppendC(UTF8STRC(") "));
		sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LineBreakType::None);
		return len + hdrSize;
	}
}

void Net::SNMPInfo::ValueToString(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	switch (type)
	{
	case 2:
		if (valLen == 1)
		{
			sb->AppendI16((Int8)pduBuff[0]);
		}
		else if (valLen == 2)
		{
			sb->AppendI16(ReadMInt16(pduBuff));
		}
		else if (valLen == 3)
		{
			sb->AppendI32(ReadMInt24(pduBuff));
		}
		else if (valLen == 4)
		{
			sb->AppendI32(ReadMInt32(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 4:
		{
			Bool isBin = false;
			UOSInt i = 0;
			while (i < valLen)
			{
				if (pduBuff[i] < 0x20 || pduBuff[i] >= 0x7f)
				{
					isBin = true;
					break;
				}
				i++;
			}
			if (isBin)
			{
				sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
			}
			else
			{
				sb->AppendC(UTF8STRC("\""));
				sb->AppendC(pduBuff, valLen);
				sb->AppendC(UTF8STRC("\""));
			}
		}
		break;
	case 5:
		if (valLen > 0)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 6:
		Net::ASN1Util::OIDToString(pduBuff, valLen, sb);
		sb->AppendC(UTF8STRC(" ("));
		Net::ASN1OIDDB::OIDToNameString(pduBuff, valLen, sb);
		sb->AppendC(UTF8STRC(")"));
		break;
	case 0x30:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	case 0x40:
		if (valLen == 4)
		{
			UTF8Char sbuff[16];
			UTF8Char *sptr;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(pduBuff));
			sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 0x41:
		if (valLen == 1)
		{
			sb->AppendU16(pduBuff[0]);
		}
		else if (valLen == 2)
		{
			sb->AppendU16(ReadMUInt16(pduBuff));
		}
		else if (valLen == 3)
		{
			sb->AppendU32(ReadMUInt24(pduBuff));
		}
		else if (valLen == 4)
		{
			sb->AppendU32(ReadMUInt32(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 0x42:
		if (valLen == 1)
		{
			sb->AppendU16(pduBuff[0]);
		}
		else if (valLen == 2)
		{
			sb->AppendU16(ReadMUInt16(pduBuff));
		}
		else if (valLen == 3)
		{
			sb->AppendU32(ReadMUInt24(pduBuff));
		}
		else if (valLen == 4)
		{
			sb->AppendU32(ReadMUInt32(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 0x43:
		if (valLen == 1)
		{
			sb->AppendU16(pduBuff[0]);
		}
		else if (valLen == 2)
		{
			sb->AppendU16(ReadMUInt16(pduBuff));
		}
		else if (valLen == 3)
		{
			sb->AppendU32(ReadMUInt24(pduBuff));
		}
		else if (valLen == 4)
		{
			sb->AppendU32(ReadMUInt32(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 0x46:
		if (valLen == 1)
		{
			sb->AppendU16(pduBuff[0]);
		}
		else if (valLen == 2)
		{
			sb->AppendU16(ReadMUInt16(pduBuff));
		}
		else if (valLen == 3)
		{
			sb->AppendU32(ReadMUInt24(pduBuff));
		}
		else if (valLen == 4)
		{
			sb->AppendU32(ReadMUInt32(pduBuff));
		}
		else if (valLen == 8)
		{
			sb->AppendU64(ReadMUInt64(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		}
		break;
	case 0xA0:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	case 0xA1:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	case 0xA2:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	case 0xA3:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	case 0xA4:
		{
			Net::SNMPInfo snmp;
			snmp.PDUSeqGetDetail(pduBuff, valLen, 0, sb);
		}
		break;
	default:
		sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LineBreakType::None);
		break;
	}
}
