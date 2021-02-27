#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPUtil.h"

Net::SNMPUtil::ErrorStatus Net::SNMPUtil::PDUParseMessage(const UInt8 *pdu, UOSInt pduSize, Int32 *reqId, Data::ArrayList<BindingItem*> *itemList)
{
	UOSInt i;
	if (pdu[0] != 0x30)
	{
		*reqId = 0;
		return Net::SNMPUtil::ES_UNKRESP;
	}
	BindingItem *item;
	Int32 bindingLen;
	UOSInt bindingEnd;
	Int32 pduLen;
	UInt32 err;
	i = Net::ASN1Util::PDUParseLen(pdu, 1, pduSize, &pduLen);
	if (pduLen == -1)
	{
		*reqId = 0;
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (i + pduLen != pduSize)
	{
		*reqId = 0;
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i] != 2 || pdu[i + 1] != 1)
	{
		*reqId = 0;
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i + 2] == 0 || pdu[i + 2] == 1) //v1 message / v2c message
	{
		i += 3;
		if (pdu[i] != 4)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen >= pduSize)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i += pduLen;
		if ((pdu[i] & 0xf0) != 0xa0)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pduSize)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 2)
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] == 1)
		{
			*reqId = pdu[i + 2];
			i += 3;
		}
		else if (pdu[i + 1] == 2)
		{
			*reqId = ReadMUInt16(&pdu[i + 2]);
			i += 4;
		}
		else if (pdu[i + 1] == 3)
		{
			*reqId = ReadMUInt24(&pdu[i + 2]);
			i += 5;
		}
		else if (pdu[i + 1] == 4)
		{
			*reqId = ReadMUInt32(&pdu[i + 2]);
			i += 6;
		}
		else
		{
			*reqId = 0;
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 2) //error-status
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] != 1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		err = pdu[i + 2];
		i += 3;
		if (pdu[i] != 2) //error-status
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i += pduLen;
		if (pdu[i] != 0x30)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pduSize)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		while (i < pduSize)
		{
			if (pdu[i] != 0x30)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &bindingLen);
			if (bindingLen == -1)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			bindingEnd = i + bindingLen;
			if (pdu[i] != 6)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
			if (pduLen == -1 || pduLen > 64)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			if (i + pduLen > bindingEnd)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item = MemAlloc(BindingItem, 1);
			MemCopyNO(item->oid, &pdu[i], pduLen);
			item->oidLen = pduLen;
			i += pduLen;
			if (i + 2 > bindingEnd)
			{
				MemFree(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valType = pdu[i];
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
			if (i + pduLen != bindingEnd)
			{
				MemFree(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valLen = pduLen;
			if (pduLen == 0)
			{
				item->valBuff = 0;
			}
			else
			{
				item->valBuff = MemAlloc(UInt8, pduLen);
				MemCopyNO(item->valBuff, &pdu[i], pduLen);
			}
			itemList->Add(item);
			i += pduLen;
		}
		return (Net::SNMPUtil::ErrorStatus)err;
	}
	else
	{
		*reqId = 0;
		return Net::SNMPUtil::ES_UNKRESP;
	}
}

Net::SNMPUtil::ErrorStatus Net::SNMPUtil::PDUParseTrapMessage(const UInt8 *pdu, UOSInt pduSize, TrapInfo *trap, Data::ArrayList<BindingItem*> *itemList)
{
	UOSInt i;
	if (pdu[0] != 0x30)
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	BindingItem *item;
	Int32 bindingLen;
	UOSInt bindingEnd;
	Int32 pduLen;
	i = Net::ASN1Util::PDUParseLen(pdu, 1, pduSize, &pduLen);
	if (pduLen == -1)
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (i + pduLen != pduSize)
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i] != 2 || pdu[i + 1] != 1)
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i + 2] == 0 || pdu[i + 2] == 1) //v1 message / v2c message
	{
		i += 3;
		if (pdu[i] != 4)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen >= pduSize)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pduLen < 64)
		{
			Text::StrConcatC(trap->community, &pdu[i], (UInt32)pduLen);
		}
		else
		{
			Text::StrConcatC(trap->community, &pdu[i], 63);
		}
		i += pduLen;
		if (pdu[i] != 0xa4)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pduSize)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 6)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1 || pduLen > 64)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		MemCopyNO(trap->entOID, &pdu[i], (UInt32)pduLen);
		trap->entOIDLen = pduLen;
		i += pduLen;
		if (pdu[i] != 0x40)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen != 4)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		trap->agentIPv4 = ReadNInt32(&pdu[i]);
		i += 4;
		if (pdu[i] != 2)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] == 1)
		{
			trap->genericTrap = pdu[i + 2];
			i += 3;
		}
		else if (pdu[i + 1] == 2)
		{
			trap->genericTrap = ReadMUInt16(&pdu[i + 2]);
			i += 4;
		}
		else if (pdu[i + 1] == 3)
		{
			trap->genericTrap = ReadMUInt24(&pdu[i + 2]);
			i += 5;
		}
		else if (pdu[i + 1] == 4)
		{
			trap->genericTrap = ReadMUInt32(&pdu[i + 2]);
			i += 6;
		}
		else
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 2)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] == 1)
		{
			trap->specificTrap = pdu[i + 2];
			i += 3;
		}
		else if (pdu[i + 1] == 2)
		{
			trap->specificTrap = ReadMUInt16(&pdu[i + 2]);
			i += 4;
		}
		else if (pdu[i + 1] == 3)
		{
			trap->specificTrap = ReadMUInt24(&pdu[i + 2]);
			i += 5;
		}
		else if (pdu[i + 1] == 4)
		{
			trap->specificTrap = ReadMUInt32(&pdu[i + 2]);
			i += 6;
		}
		else
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 0x43)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] == 1)
		{
			trap->timeStamp = pdu[i + 2];
			i += 3;
		}
		else if (pdu[i + 1] == 2)
		{
			trap->timeStamp = ReadMUInt16(&pdu[i + 2]);
			i += 4;
		}
		else if (pdu[i + 1] == 3)
		{
			trap->timeStamp = ReadMUInt24(&pdu[i + 2]);
			i += 5;
		}
		else if (pdu[i + 1] == 4)
		{
			trap->timeStamp = ReadMUInt32(&pdu[i + 2]);
			i += 6;
		}
		else
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}

		if (pdu[i] != 0x30)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
		if (pduLen == -1)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pduSize)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		while (i < pduSize)
		{
			if (pdu[i] != 0x30)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &bindingLen);
			if (bindingLen == -1)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			bindingEnd = i + bindingLen;
			if (pdu[i] != 6)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
			if (pduLen == -1 || pduLen > 64)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			if (i + pduLen > bindingEnd)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item = MemAlloc(BindingItem, 1);
			MemCopyNO(item->oid, &pdu[i], pduLen);
			item->oidLen = pduLen;
			i += pduLen;
			if (i + 2 > bindingEnd)
			{
				MemFree(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valType = pdu[i];
			i = Net::ASN1Util::PDUParseLen(pdu, i + 1, pduSize, &pduLen);
			if (i + pduLen != bindingEnd)
			{
				MemFree(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valLen = pduLen;
			if (pduLen == 0)
			{
				item->valBuff = 0;
			}
			else
			{
				item->valBuff = MemAlloc(UInt8, pduLen);
				MemCopyNO(item->valBuff, &pdu[i], pduLen);
			}
			itemList->Add(item);
			i += pduLen;
		}
		return Net::SNMPUtil::ES_NOERROR;
	}
	else
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
}

OSInt Net::SNMPUtil::OIDCompare(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len)
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

Bool Net::SNMPUtil::OIDStartsWith(const UInt8 *oid1, UOSInt oid1Len, const UInt8 *oid2, UOSInt oid2Len)
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

void Net::SNMPUtil::OIDToString(const UInt8 *pdu, UOSInt pduSize, Text::StringBuilderUTF *sb)
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

UOSInt Net::SNMPUtil::OIDCalcPDUSize(const UTF8Char *oid)
{
	UInt32 v;
	UOSInt retSize = 1;
	UOSInt len = Text::StrCharCnt(oid);
	UTF8Char *buff = MemAlloc(UTF8Char, len + 1);
	Text::StrConcatC(buff, oid, len);
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

UOSInt Net::SNMPUtil::OIDText2PDU(const UTF8Char *oid, UInt8 *pduBuff)
{
	UInt32 v;
	UOSInt retSize = 1;
	UOSInt len = Text::StrCharCnt(oid);
	UTF8Char *buff = MemAlloc(UTF8Char, len + 1);
	Text::StrConcatC(buff, oid, len);
	UTF8Char *sarr[3];
	UOSInt i;
	i = Text::StrSplit(sarr, 3, buff, '.');
	if (i == 1)
	{
		pduBuff[0] = (UInt8)(Text::StrToUInt32(sarr[0]) * 40);
		MemFree(buff);
		return 1;
	}
	pduBuff[0] = (UInt8)(Text::StrToUInt32(sarr[0]) * 40 + Text::StrToUInt32(sarr[1]));
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
			return retSize;
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

const UTF8Char *Net::SNMPUtil::TypeGetName(UInt8 type)
{
	switch (type)
	{
	case 2:
		return (const UTF8Char*)"INTEGER";
	case 4:
		return (const UTF8Char*)"OCTET STRING";
	case 5:
		return (const UTF8Char*)"NULL";
	case 6:
		return (const UTF8Char*)"OBJECT IDENTIFIER";
	case 0x30:
		return (const UTF8Char*)"SEQUENCE";
	case 0x40:
		return (const UTF8Char*)"IpAddress";
	case 0x41:
		return (const UTF8Char*)"Counter32";
	case 0x42:
		return (const UTF8Char*)"Gauge32";
	case 0x43:
		return (const UTF8Char*)"Timeticks";
	case 0x44:
		return (const UTF8Char*)"Opaque";
	case 0x46:
		return (const UTF8Char*)"Counter64";
	case 0xA0:
		return (const UTF8Char*)"GetRequest-PDU";
	case 0xA1:
		return (const UTF8Char*)"GetNextRequest-PDU";
	case 0xA2:
		return (const UTF8Char*)"GetResponse-PDU";
	case 0xA3:
		return (const UTF8Char*)"SetRequest-PDU";
	case 0xA4:
		return (const UTF8Char*)"Trap-PDU";
	default:
		return (const UTF8Char*)"UNKNOWN";
	}
}

Bool Net::SNMPUtil::ValueToInt32(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, Int32 *outVal)
{
	switch (type)
	{
	case 2:
		if (valLen == 1)
		{
			*outVal = (Int8)pduBuff[0];
			return true;
		}
		else if (valLen == 2)
		{
			*outVal = ReadMInt16(pduBuff);
			return true;
		}
		else if (valLen == 3)
		{
			*outVal = ReadMInt24(pduBuff);
			return true;
		}
		else if (valLen == 4)
		{
			*outVal = ReadMInt32(pduBuff);
			return true;
		}
		else
		{
			return false;
		}
	case 0x41:
	case 0x42:
	case 0x43:
		if (valLen == 1)
		{
			*outVal = pduBuff[0];
			return true;
		}
		else if (valLen == 2)
		{
			*outVal = ReadMUInt16(pduBuff);
			return true;
		}
		else if (valLen == 3)
		{
			*outVal = ReadMUInt24(pduBuff);
			return true;
		}
		else if (valLen == 4)
		{
			*outVal = ReadMInt32(pduBuff);
			return true;
		}
		else
		{
			return false;
		}
	case 0x46:
		if (valLen == 1)
		{
			*outVal = pduBuff[0];
			return true;
		}
		else if (valLen == 2)
		{
			*outVal = ReadMUInt16(pduBuff);
			return true;
		}
		else if (valLen == 3)
		{
			*outVal = ReadMUInt24(pduBuff);
			return true;
		}
		else if (valLen == 4)
		{
			*outVal = ReadMInt32(pduBuff);
			return true;
		}
		else if (valLen == 8)
		{
			*outVal = ReadMInt32(pduBuff);
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void Net::SNMPUtil::FreeBindingItem(BindingItem *item)
{
	if (item->valBuff)
	{
		MemFree(item->valBuff);
	}
	MemFree(item);
}

const UTF8Char *Net::SNMPUtil::ErrorStatusToString(ErrorStatus err)
{
	switch (err)
	{
	case ES_NOERROR:
		return (const UTF8Char*)"No Error";
	case ES_TOOBIG:
		return (const UTF8Char*)"Too Big";
	case ES_NOSUCHNAME:
		return (const UTF8Char*)"No Such Name";
	case ES_BADVALUE:
		return (const UTF8Char*)"Bad Value";
	case ES_READONLY:
		return (const UTF8Char*)"Read Only";
	case ES_GENERROR:
		return (const UTF8Char*)"General Error";
	case ES_NORESP:
		return (const UTF8Char*)"No Response";
	case ES_UNKRESP:
		return (const UTF8Char*)"Unknown Response";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
