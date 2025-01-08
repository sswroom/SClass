#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPUtil.h"

Net::SNMPUtil::ErrorStatus Net::SNMPUtil::PDUParseMessage(Data::ByteArrayR pdu, OutParam<Int32> reqId, NN<Data::ArrayListNN<BindingItem>> itemList)
{
	UOSInt i;
	if (pdu[0] != 0x30)
	{
		reqId.Set(0);
		return Net::SNMPUtil::ES_UNKRESP;
	}
	NN<BindingItem> item;
	UInt32 bindingLen;
	UOSInt bindingEnd;
	UInt32 pduLen;
	UInt32 err;
	i = Net::ASN1Util::PDUParseLen(pdu.Arr(), 1, pdu.GetSize(), pduLen);
	if (i > pdu.GetSize())
	{
		reqId.Set(0);
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (i + pduLen != pdu.GetSize())
	{
		reqId.Set(0);
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i] != 2 || pdu[i + 1] != 1)
	{
		reqId.Set(0);
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (pdu[i + 2] == 0 || pdu[i + 2] == 1) //v1 message / v2c message
	{
		i += 3;
		if (pdu[i] != 4)
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen >= pdu.GetSize())
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i += pduLen;
		if ((pdu[i] & 0xf0) != 0xa0)
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pdu.GetSize())
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 2)
		{
			reqId.Set(0);
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i + 1] == 1)
		{
			reqId.Set((Int8)pdu[i + 2]);
			i += 3;
		}
		else if (pdu[i + 1] == 2)
		{
			reqId.Set(ReadMInt16(&pdu[i + 2]));
			i += 4;
		}
		else if (pdu[i + 1] == 3)
		{
			reqId.Set(ReadMInt24(&pdu[i + 2]));
			i += 5;
		}
		else if (pdu[i + 1] == 4)
		{
			reqId.Set(ReadMInt32(&pdu[i + 2]));
			i += 6;
		}
		else if (pdu[i + 1] == 5)
		{
			reqId.Set(ReadMInt32(&pdu[i + 3]));
			i += 7;
		}
		else
		{
			reqId.Set(0);
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
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i += pduLen;
		if (pdu[i] != 0x30)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		while (i < pdu.GetSize())
		{
			if (pdu[i] != 0x30)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), bindingLen);
			if (i > pdu.GetSize())
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			bindingEnd = i + bindingLen;
			if (pdu[i] != 6)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
			if (i > pdu.GetSize() || pduLen > 64)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			if (i + pduLen > bindingEnd)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item = MemAllocNN(BindingItem);
			MemCopyNO(item->oid, &pdu[i], pduLen);
			item->oidLen = pduLen;
			i += pduLen;
			if (i + 2 > bindingEnd)
			{
				MemFreeNN(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valType = pdu[i];
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
			if (i + pduLen != bindingEnd)
			{
				MemFreeNN(item);
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
		reqId.Set(0);
		return Net::SNMPUtil::ES_UNKRESP;
	}
}

Net::SNMPUtil::ErrorStatus Net::SNMPUtil::PDUParseTrapMessage(Data::ByteArrayR pdu, NN<TrapInfo> trap, NN<Data::ArrayListNN<BindingItem>> itemList)
{
	UOSInt i;
	if (pdu[0] != 0x30)
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	NN<BindingItem> item;
	UInt32 bindingLen;
	UOSInt bindingEnd;
	UInt32 pduLen;
	i = Net::ASN1Util::PDUParseLen(pdu.Arr(), 1, pdu.GetSize(), pduLen);
	if (i > pdu.GetSize())
	{
		return Net::SNMPUtil::ES_UNKRESP;
	}
	if (i + pduLen != pdu.GetSize())
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
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen >= pdu.GetSize())
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
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (pdu[i] != 6)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize() || pduLen > 64)
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
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (pduLen != 4)
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		trap->agentIPv4 = ReadNUInt32(&pdu[i]);
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
		i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
		if (i > pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		if (i + pduLen != pdu.GetSize())
		{
			return Net::SNMPUtil::ES_UNKRESP;
		}
		while (i < pdu.GetSize())
		{
			if (pdu[i] != 0x30)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), bindingLen);
			if (i > pdu.GetSize())
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			bindingEnd = i + bindingLen;
			if (pdu[i] != 6)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
			if (i > pdu.GetSize() || pduLen > 64)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			if (i + pduLen > bindingEnd)
			{
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item = MemAllocNN(BindingItem);
			MemCopyNO(item->oid, &pdu[i], pduLen);
			item->oidLen = pduLen;
			i += pduLen;
			if (i + 2 > bindingEnd)
			{
				MemFreeNN(item);
				return Net::SNMPUtil::ES_UNKRESP;
			}
			item->valType = pdu[i];
			i = Net::ASN1Util::PDUParseLen(pdu.Arr(), i + 1, pdu.GetSize(), pduLen);
			if (i + pduLen != bindingEnd)
			{
				MemFreeNN(item);
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

Text::CStringNN Net::SNMPUtil::TypeGetName(UInt8 type)
{
	switch (type)
	{
	case 2:
		return CSTR("INTEGER");
	case 4:
		return CSTR("OCTET STRING");
	case 5:
		return CSTR("NULL");
	case 6:
		return CSTR("OBJECT IDENTIFIER");
	case 0x30:
		return CSTR("SEQUENCE");
	case 0x40:
		return CSTR("IpAddress");
	case 0x41:
		return CSTR("Counter32");
	case 0x42:
		return CSTR("Gauge32");
	case 0x43:
		return CSTR("Timeticks");
	case 0x44:
		return CSTR("Opaque");
	case 0x46:
		return CSTR("Counter64");
	case 0xA0:
		return CSTR("GetRequest-PDU");
	case 0xA1:
		return CSTR("GetNextRequest-PDU");
	case 0xA2:
		return CSTR("GetResponse-PDU");
	case 0xA3:
		return CSTR("SetRequest-PDU");
	case 0xA4:
		return CSTR("Trap-PDU");
	default:
		return CSTR("UNKNOWN");
	}
}

Bool Net::SNMPUtil::ValueToInt32(UInt8 type, const UInt8 *pduBuff, UOSInt valLen, OutParam<Int32> outVal)
{
	switch (type)
	{
	case 2:
		if (valLen == 1)
		{
			outVal.Set((Int8)pduBuff[0]);
			return true;
		}
		else if (valLen == 2)
		{
			outVal.Set(ReadMInt16(pduBuff));
			return true;
		}
		else if (valLen == 3)
		{
			outVal.Set(ReadMInt24(pduBuff));
			return true;
		}
		else if (valLen == 4)
		{
			outVal.Set(ReadMInt32(pduBuff));
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
			outVal.Set(pduBuff[0]);
			return true;
		}
		else if (valLen == 2)
		{
			outVal.Set(ReadMUInt16(pduBuff));
			return true;
		}
		else if (valLen == 3)
		{
			outVal.Set((Int32)ReadMUInt24(pduBuff));
			return true;
		}
		else if (valLen == 4)
		{
			outVal.Set(ReadMInt32(pduBuff));
			return true;
		}
		else
		{
			return false;
		}
	case 0x46:
		if (valLen == 1)
		{
			outVal.Set(pduBuff[0]);
			return true;
		}
		else if (valLen == 2)
		{
			outVal.Set((Int32)ReadMUInt16(pduBuff));
			return true;
		}
		else if (valLen == 3)
		{
			outVal.Set((Int32)ReadMUInt24(pduBuff));
			return true;
		}
		else if (valLen == 4)
		{
			outVal.Set(ReadMInt32(pduBuff));
			return true;
		}
		else if (valLen == 8)
		{
			outVal.Set(ReadMInt32(pduBuff));
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

void __stdcall Net::SNMPUtil::FreeBindingItem(NN<BindingItem> item)
{
	if (item->valBuff)
	{
		MemFree(item->valBuff);
	}
	MemFreeNN(item);
}

Text::CStringNN Net::SNMPUtil::ErrorStatusToString(ErrorStatus err)
{
	switch (err)
	{
	case ES_NOERROR:
		return CSTR("No Error");
	case ES_TOOBIG:
		return CSTR("Too Big");
	case ES_NOSUCHNAME:
		return CSTR("No Such Name");
	case ES_BADVALUE:
		return CSTR("Bad Value");
	case ES_READONLY:
		return CSTR("Read Only");
	case ES_GENERROR:
		return CSTR("General Error");
	case ES_NORESP:
		return CSTR("No Response");
	case ES_UNKRESP:
		return CSTR("Unknown Response");
	default:
		return CSTR("Unknown");
	}
}
