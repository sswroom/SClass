#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/SNMPInfo.h"
#include "Net/SNMPOIDDB.h"
#include "Net/SNMPUtil.h"
#include "Net/SocketUtil.h"

Net::SNMPInfo::SNMPInfo()
{

}

Net::SNMPInfo::~SNMPInfo()
{

}

void Net::SNMPInfo::PDUSeqGetDetail(const UInt8 *pdu, OSInt pduSize, OSInt level, Text::StringBuilderUTF *sb)
{
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	sb->Append((const UTF8Char*)"{\r\n");
	OSInt i = 0;
	while (i < pduSize)
	{
		i += this->PDUGetDetail(0, &pdu[i], pduSize - i, level + 1, sb);
		if (i < pduSize)
		{
			sb->Append((const UTF8Char*)",\r\n");
		}
	}
	sb->Append((const UTF8Char*)"\r\n");
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	sb->Append((const UTF8Char*)"}");
}

OSInt Net::SNMPInfo::PDUGetDetail(const UTF8Char *name, const UInt8 *pdu, OSInt pduSize, OSInt level, Text::StringBuilderUTF *sb)
{
	if (pduSize < 2)
	{
		return pduSize;
	}
	if (level > 0)
	{
		sb->AppendChar('\t', level);
	}
	if (name)
	{
		sb->Append(name);
		sb->AppendChar(' ', 1);
	}
	UInt8 t = pdu[0];
	OSInt len = pdu[1];
	OSInt hdrSize = 2;
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
		sb->AppendHexBuff(pdu, pduSize, ' ', Text::LBT_NONE);
		return pduSize;
	}
	switch (t)
	{
	case 2:
		sb->Append((const UTF8Char*)"INTEGER ");
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
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 4:
		sb->Append((const UTF8Char*)"OCTET STRING ");
		{
			Bool isBin = false;
			OSInt i = 0;
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
				sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
			}
			else
			{
				sb->Append((const UTF8Char*)"\"");
				sb->AppendC(&pdu[hdrSize], len);
				sb->Append((const UTF8Char*)"\"");
			}
		}
		return len + hdrSize;
	case 5:
		sb->Append((const UTF8Char*)"NULL");
		if (len > 0)
		{
			sb->AppendChar(' ', 1);
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 6:
		sb->Append((const UTF8Char*)"OBJECT IDENTIFIER ");
		Net::SNMPUtil::OIDToString(&pdu[hdrSize], len, sb);
		sb->Append((const UTF8Char*)" (");
		Net::SNMPOIDDB::OIDToNameString(&pdu[hdrSize], len, sb);
		sb->Append((const UTF8Char*)")");
		return len + hdrSize;
	case 0x30:
		sb->Append((const UTF8Char*)"SEQUENCE\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0x40:
		sb->Append((const UTF8Char*)"IpAddress ");
		if (len == 4)
		{
			UTF8Char sbuff[16];
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNInt32(&pdu[hdrSize]));
			sb->Append(sbuff);
		}
		else
		{
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 0x41:
		sb->Append((const UTF8Char*)"Counter32 ");
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
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 0x42:
		sb->Append((const UTF8Char*)"Gauge32 ");
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
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 0x43:
		sb->Append((const UTF8Char*)"Timeticks ");
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
			sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		}
		return len + hdrSize;
	case 0xA0:
		sb->Append((const UTF8Char*)"GetRequest-PDU\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA1:
		sb->Append((const UTF8Char*)"GetNextRequest-PDU\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA2:
		sb->Append((const UTF8Char*)"GetResponse-PDU\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA3:
		sb->Append((const UTF8Char*)"SetRequest-PDU\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	case 0xA4:
		sb->Append((const UTF8Char*)"Trap-PDU\r\n");
		this->PDUSeqGetDetail(&pdu[hdrSize], len, level, sb);
		return len + hdrSize;
	default:
		sb->Append((const UTF8Char*)"UNKNOWN(");
		sb->AppendU16(t);
		sb->Append((const UTF8Char*)") ");
		sb->AppendHexBuff(&pdu[hdrSize], len, ' ', Text::LBT_NONE);
		return len + hdrSize;
	}
}

void Net::SNMPInfo::ValueToString(UInt8 type, const UInt8 *pduBuff, OSInt valLen, Text::StringBuilderUTF *sb)
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
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
		}
		break;
	case 4:
		{
			Bool isBin = false;
			OSInt i = 0;
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
				sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
			}
			else
			{
				sb->Append((const UTF8Char*)"\"");
				sb->AppendC(pduBuff, valLen);
				sb->Append((const UTF8Char*)"\"");
			}
		}
		break;
	case 5:
		if (valLen > 0)
		{
			sb->AppendChar(' ', 1);
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
		}
		break;
	case 6:
		Net::SNMPUtil::OIDToString(pduBuff, valLen, sb);
		sb->Append((const UTF8Char*)" (");
		Net::SNMPOIDDB::OIDToNameString(pduBuff, valLen, sb);
		sb->Append((const UTF8Char*)")");
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
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNInt32(pduBuff));
			sb->Append(sbuff);
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
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
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
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
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
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
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
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
			sb->AppendU32(ReadMUInt64(pduBuff));
		}
		else
		{
			sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
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
		sb->AppendHexBuff(pduBuff, valLen, ' ', Text::LBT_NONE);
		break;
	}
}
