#include "Stdafx.h"
#include "Data/BinaryBuilder.h"
#include "Core/ByteTool_C.h"
#include "Text/MyStringW.h"

Data::BinaryBuilder::BinaryBuilder() : mstm()
{
}

Data::BinaryBuilder::BinaryBuilder(UIntOS initSize) : mstm(initSize)
{
}

Data::BinaryBuilder::~BinaryBuilder()
{
}

void Data::BinaryBuilder::AppendI32(Int32 val)
{
	UInt8 buff[4];
	WriteInt32(buff, val);
	this->mstm.Write(Data::ByteArrayR(buff, 4));
}

void Data::BinaryBuilder::AppendU32(UInt32 val)
{
	UInt8 buff[4];
	WriteUInt32(buff, val);
	this->mstm.Write(Data::ByteArrayR(buff, 4));
}

void Data::BinaryBuilder::AppendNI32(NInt32 val)
{
	UInt8 buff[4];
	WriteInt32(buff, val.IntVal());
	this->mstm.Write(Data::ByteArrayR(buff, 4));
}

void Data::BinaryBuilder::AppendI64(Int64 val)
{
	UInt8 buff[8];
	WriteInt64(buff, val);
	this->mstm.Write(Data::ByteArrayR(buff, 8));
}

void Data::BinaryBuilder::AppendF64(Double val)
{
	UInt8 buff[8];
	WriteDouble(buff, val);
	this->mstm.Write(Data::ByteArrayR(buff, 8));
}

void Data::BinaryBuilder::AppendStr(Text::CString s)
{
	UInt8 buff[6];
	Text::CStringNN nns;
	if (!s.SetTo(nns))
	{
		WriteInt32(buff, -1);
		this->mstm.Write(Data::ByteArrayR(buff, 2));
		return;
	}
	if (nns.leng < 65534)
	{
		WriteInt16(buff, nns.leng);
		this->mstm.Write(Data::ByteArrayR(buff, 2));
		if (nns.leng > 0)
		{
			this->mstm.Write(nns.ToByteArray());
		}
	}
	else
	{
		WriteInt16(buff, -2);
		WriteInt32(&buff[2], (Int32)(nns.leng - 65534));
		this->mstm.Write(Data::ByteArrayR(buff, 6));
		this->mstm.Write(nns.ToByteArray());
	}
}

void Data::BinaryBuilder::AppendStrOpt(Optional<Text::String> s)
{
	this->AppendStr(OPTSTR_CSTR(s));
}

void Data::BinaryBuilder::AppendChar(UTF8Char c)
{
	this->mstm.Write(Data::ByteArrayR(&c, 1));
}

void Data::BinaryBuilder::AppendBool(Bool b)
{
	UInt8 buff;
	buff = b?0xff:0;
	this->mstm.Write(Data::ByteArrayR(&buff, 1));
}

void Data::BinaryBuilder::AppendTS(Data::Timestamp ts)
{
	UInt8 buff[13];
	WriteInt64(buff, ts.inst.sec);
	WriteUInt32(&buff[8], ts.inst.nanosec);
	buff[12] = (UInt8)ts.tzQhr;
	this->mstm.Write(Data::ByteArrayR(buff, 13));
}

void Data::BinaryBuilder::AppendDate(Data::Date dat)
{
	UInt8 buff[8];
	WriteInt64(buff, dat.GetTotalDays());
	this->mstm.Write(Data::ByteArrayR(buff, 8));
}

void Data::BinaryBuilder::AppendIPAddr(NN<Net::SocketUtil::AddressInfo> addr)
{
	UInt8 ipType;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		ipType = (UInt8)Net::AddrType::IPv4;
		this->mstm.Write(Data::ByteArrayR(&ipType, 1));
		this->mstm.Write(Data::ByteArrayR(addr->addr, 4));
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		ipType = (UInt8)Net::AddrType::IPv6;
		this->mstm.Write(Data::ByteArrayR(&ipType, 1));
		this->mstm.Write(Data::ByteArrayR(addr->addr, 20));
	}
	else
	{
		ipType = (UInt8)Net::AddrType::Unknown;
		this->mstm.Write(Data::ByteArrayR(&ipType, 1));
	}
}

void Data::BinaryBuilder::AppendBArr(Data::ByteArrayR barr)
{
	UInt8 buff[6];
	UnsafeArray<UTF8Char> sptr = Text::StrWriteChar(buff, (UTF32Char)barr.GetSize());
	this->mstm.Write(Data::ByteArrayR(buff, (UIntOS)(sptr - buff)));
	this->mstm.Write(barr);
}

Data::ByteArrayR Data::BinaryBuilder::Build()
{
	return this->mstm.GetArray();
}
