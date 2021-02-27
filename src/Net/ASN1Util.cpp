#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"

UOSInt Net::ASN1Util::PDUParseLen(const UInt8 *pdu, UOSInt ofst, UOSInt pduSize, Int32 *len)
{
	if (ofst >= pduSize)
	{
		*len = -1;
		return pduSize;
	}
	if (pdu[ofst] & 0x80)
	{
		if (pdu[ofst] == 0x81)
		{
			if (ofst + 2 > pduSize)
			{
				*len = -1;
				return pduSize;
			}
			*len = pdu[ofst + 1];
			return ofst + 2;
		}
		else if (pdu[ofst] == 0x82)
		{
			if (ofst + 3 > pduSize)
			{
				*len = -1;
				return pduSize;
			}
			*len = ReadMUInt16(&pdu[ofst + 1]);
			return ofst + 3;
		}
		else if (pdu[ofst] == 0x83)
		{
			if (ofst + 4 > pduSize)
			{
				*len = -1;
				return pduSize;
			}
			*len = ReadMUInt24(&pdu[ofst + 1]);
			return ofst + 4;
		}
		else if (pdu[ofst] == 0x84)
		{
			if (ofst + 5 > pduSize)
			{
				*len = -1;
				return pduSize;
			}
			*len = ReadMInt32(&pdu[ofst + 1]);
			return ofst + 5;
		}
		*len = -1;
		return pduSize;
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
