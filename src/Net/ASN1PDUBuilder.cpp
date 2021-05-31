#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1PDUBuilder.h"
#include "Text/MyString.h"

//SNMP / LDAP

Net::ASN1PDUBuilder::ASN1PDUBuilder()
{
	this->currLev = 0;
	this->buffSize = 128;
	this->buff = MemAlloc(UInt8, buffSize);
	this->currOffset = 0;
}

Net::ASN1PDUBuilder::~ASN1PDUBuilder()
{
	MemFree(this->buff);
}

void Net::ASN1PDUBuilder::AllocateSize(UOSInt size)
{
	if (this->currOffset + size > this->buffSize)
	{
		while (this->currOffset + size > this->buffSize)
		{
			this->buffSize = this->buffSize << 1;
		}
		UInt8 *newBuff = MemAlloc(UInt8, this->buffSize);
		MemCopyNO(newBuff, this->buff, this->currOffset);
		MemFree(this->buff);
		this->buff = newBuff;
	}
}

void Net::ASN1PDUBuilder::SequenceBegin(UInt8 type)
{
	this->AllocateSize(2);
	this->buff[this->currOffset] = type;
	this->currOffset += 2;
	this->seqOffset[this->currLev] = this->currOffset;
	this->currLev++;
}

void Net::ASN1PDUBuilder::SequenceEnd()
{
	if (this->currLev > 0)
	{
		UOSInt seqOffset;
		UOSInt seqLen;
		this->currLev--;
		seqOffset = this->seqOffset[this->currLev];
		seqLen = this->currOffset - seqOffset;
		if (seqLen < 128)
		{
			this->buff[seqOffset - 1] = (UInt8)seqLen;
		}
		else if (seqLen < 256)
		{
			this->AllocateSize(1);
			MemCopyO(&this->buff[seqOffset + 1], &this->buff[seqOffset], seqLen);
			this->buff[seqOffset - 1] = 0x81;
			this->buff[seqOffset] = (UInt8)seqLen;
			this->currOffset += 1;
		}
		else if (seqLen < 65536)
		{
			this->AllocateSize(2);
			MemCopyO(&this->buff[seqOffset + 2], &this->buff[seqOffset], seqLen);
			this->buff[seqOffset - 1] = 0x82;
			WriteMInt16(&this->buff[seqOffset], seqLen);
			this->currOffset += 2;
		}
		else
		{
			this->AllocateSize(3);
			MemCopyO(&this->buff[seqOffset + 3], &this->buff[seqOffset], seqLen);
			this->buff[seqOffset - 1] = 0x83;
			WriteMInt24(&this->buff[seqOffset], seqLen);
			this->currOffset += 3;
		}
	}
}

void Net::ASN1PDUBuilder::AppendBool(Bool v)
{
	this->AllocateSize(3);
	this->buff[this->currOffset] = 1;
	this->buff[this->currOffset + 1] = 1;
	this->buff[this->currOffset + 2] = v?1:0;
	this->currOffset += 3;
}

void Net::ASN1PDUBuilder::AppendInt32(Int32 v)
{
	if (v < 128 && v >= -128)
	{
		this->AllocateSize(3);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 1;
		this->buff[this->currOffset + 2] = (UInt8)(Int8)v;
		this->currOffset += 3;
	}
	else if (v < 32768 && v >= -32768)
	{
		this->AllocateSize(4);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 2;
		WriteMInt16(&this->buff[this->currOffset + 2], v);
		this->currOffset += 4;
	}
	else if (v < 8388608 && v >= -8388608)
	{
		this->AllocateSize(5);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 3;
		WriteMInt24(&this->buff[this->currOffset + 2], v);
		this->currOffset += 5;
	}
	else
	{
		this->AllocateSize(6);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 4;
		WriteMInt32(&this->buff[this->currOffset + 2], v);
		this->currOffset += 6;
	}
}

void Net::ASN1PDUBuilder::AppendUInt32(UInt32 v)
{
	if (v < 256)
	{
		this->AllocateSize(3);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 1;
		this->buff[this->currOffset + 2] = (UInt8)v;
		this->currOffset += 3;
	}
	else if (v < 65536)
	{
		this->AllocateSize(4);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 2;
		WriteMInt16(&this->buff[this->currOffset + 2], v);
		this->currOffset += 4;
	}
	else if (v < 0x1000000)
	{
		this->AllocateSize(5);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 3;
		WriteMInt24(&this->buff[this->currOffset + 2], v);
		this->currOffset += 5;
	}
	else
	{
		this->AllocateSize(6);
		this->buff[this->currOffset] = 2;
		this->buff[this->currOffset + 1] = 4;
		WriteMUInt32(&this->buff[this->currOffset + 2], v);
		this->currOffset += 6;
	}
}

void Net::ASN1PDUBuilder::AppendString(const UTF8Char *s)
{
	if (s == 0)
	{
		this->AllocateSize(2);
		this->buff[this->currOffset] = 4;
		this->buff[this->currOffset + 1] = 0;
		this->currOffset += 2;
		return;
	}
	UOSInt len = Text::StrCharCnt(s);
	if (len < 128)
	{
		this->AllocateSize(len + 2);
		this->buff[this->currOffset] = 4;
		this->buff[this->currOffset + 1] = (UInt8)len;
		MemCopyNO(&this->buff[this->currOffset + 2], s, len);
		this->currOffset += len + 2;
	}
	else if (len < 256)
	{
		this->AllocateSize(len + 3);
		this->buff[this->currOffset] = 4;
		this->buff[this->currOffset + 1] = 0x81;
		this->buff[this->currOffset + 2] = (UInt8)len;
		MemCopyNO(&this->buff[this->currOffset + 3], s, len);
		this->currOffset += len + 3;
	}
	else if (len < 65536)
	{
		this->AllocateSize(len + 4);
		this->buff[this->currOffset] = 4;
		this->buff[this->currOffset + 1] = 0x82;
		WriteMInt16(&this->buff[this->currOffset + 2], len);
		MemCopyNO(&this->buff[this->currOffset + 4], s, len);
		this->currOffset += len + 4;
	}
	else
	{
		this->AllocateSize(len + 5);
		this->buff[this->currOffset] = 4;
		this->buff[this->currOffset + 1] = 0x83;
		WriteMInt24(&this->buff[this->currOffset + 2], len);
		MemCopyNO(&this->buff[this->currOffset + 5], s, len);
		this->currOffset += len + 5;
	}
}

void Net::ASN1PDUBuilder::AppendNull()
{
	this->AllocateSize(2);
	this->buff[this->currOffset] = 5;
	this->buff[this->currOffset + 1] = 0;
	this->currOffset += 2;
}

void Net::ASN1PDUBuilder::AppendOID(const UInt8 *oid, UOSInt len)
{
	this->AllocateSize(len + 2);
	this->buff[this->currOffset] = 6;
	this->buff[this->currOffset + 1] = (UInt8)len;
	MemCopyNO(&this->buff[this->currOffset + 2], oid, len);
	this->currOffset += len + 2;
}

void Net::ASN1PDUBuilder::AppendChoice(UInt32 v)
{
	if (v < 256)
	{
		this->AllocateSize(3);
		this->buff[this->currOffset] = 10;
		this->buff[this->currOffset + 1] = 1;
		this->buff[this->currOffset + 2] = (UInt8)v;
		this->currOffset += 3;
	}
	else if (v < 65536)
	{
		this->AllocateSize(4);
		this->buff[this->currOffset] = 10;
		this->buff[this->currOffset + 1] = 2;
		WriteMInt16(&this->buff[this->currOffset + 2], v);
		this->currOffset += 4;
	}
	else if (v < 0x1000000)
	{
		this->AllocateSize(5);
		this->buff[this->currOffset] = 10;
		this->buff[this->currOffset + 1] = 3;
		WriteMInt24(&this->buff[this->currOffset + 2], v);
		this->currOffset += 5;
	}
	else
	{
		this->AllocateSize(6);
		this->buff[this->currOffset] = 10;
		this->buff[this->currOffset + 1] = 4;
		WriteMUInt32(&this->buff[this->currOffset + 2], v);
		this->currOffset += 6;
	}
}

void Net::ASN1PDUBuilder::AppendBuff(UInt8 type, const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize == 0)
	{
		this->AllocateSize(2);
		this->buff[this->currOffset] = type;
		this->buff[this->currOffset + 1] = 0;
		this->currOffset += 2;
	}
	else if (buffSize < 128)
	{
		this->AllocateSize(buffSize + 2);
		this->buff[this->currOffset] = type;
		this->buff[this->currOffset + 1] = (UInt8)buffSize;
		MemCopyNO(&this->buff[this->currOffset + 2], buff, buffSize);
		this->currOffset += buffSize + 2;
	}
	else if (buffSize < 256)
	{
		this->AllocateSize(buffSize + 3);
		this->buff[this->currOffset] = type;
		this->buff[this->currOffset + 1] = 0x81;
		this->buff[this->currOffset + 2] = (UInt8)buffSize;
		MemCopyNO(&this->buff[this->currOffset + 3], buff, buffSize);
		this->currOffset += buffSize + 3;
	}
	else if (buffSize < 65536)
	{
		this->AllocateSize(buffSize + 4);
		this->buff[this->currOffset] = type;
		this->buff[this->currOffset + 1] = 0x82;
		WriteMInt16(&this->buff[this->currOffset + 2], buffSize);
		MemCopyNO(&this->buff[this->currOffset + 4], buff, buffSize);
		this->currOffset += buffSize + 4;
	}
	else
	{
		this->AllocateSize(buffSize + 5);
		this->buff[this->currOffset] = type;
		this->buff[this->currOffset + 1] = 0x83;
		WriteMInt24(&this->buff[this->currOffset + 2], buffSize);
		MemCopyNO(&this->buff[this->currOffset + 5], buff, buffSize);
		this->currOffset += buffSize + 5;
	}

}

const UInt8 *Net::ASN1PDUBuilder::GetBuff(UOSInt *buffSize)
{
	*buffSize = this->currOffset;
	return this->buff;
}
