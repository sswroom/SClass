#include "Stdafx.h"
#include "Data/BinaryParser.h"

Data::BinaryParser::BinaryParser(Data::ByteArrayR buff) : buff(buff)
{
	this->currOfst = 0;
	this->error = false;
}

Data::BinaryParser::~BinaryParser()
{
}

Int32 Data::BinaryParser::NextI32()
{
	if (this->error)
		return 0;
	if (this->currOfst + 4 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	Int32 ret = ReadInt32(&this->buff[this->currOfst]);
	this->currOfst += 4;
	return ret;
}

UInt32 Data::BinaryParser::NextU32()
{
	if (this->error)
		return 0;
	if (this->currOfst + 4 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	UInt32 ret = ReadUInt32(&this->buff[this->currOfst]);
	this->currOfst += 4;
	return ret;
}

NInt32 Data::BinaryParser::NextNI32()
{
	if (this->error)
		return NINT32_NULL;
	if (this->currOfst + 4 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	NInt32 ret = ReadInt32(&this->buff[this->currOfst]);
	this->currOfst += 4;
	return ret;
}

Double Data::BinaryParser::NextF64()
{
	if (this->error)
		return 0;
	if (this->currOfst + 8 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	Double ret = ReadDouble(&this->buff[this->currOfst]);
	this->currOfst += 8;
	return ret;
}

Text::CStringNN Data::BinaryParser::NextStrNN()
{
	Text::CStringNN nns;
	if (this->NextStr().SetTo(nns))
	{
		return nns;
	}
	return CSTR("");
}

Text::CString Data::BinaryParser::NextStr()
{
	if (this->error)
		return 0;
	if (this->currOfst + 2 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	UInt16 b1 = ReadUInt16(&buff[this->currOfst]);
	if (b1 == 65535)
	{
		this->currOfst += 2;
		return 0;
	}
	if (b1 != 65534)
	{
		if (this->currOfst + 2 + b1 > this->buff.GetSize())
		{
			this->error = true;
			return 0;
		}
		Text::CStringNN s = Text::CStringNN(&this->buff[this->currOfst + 2], b1);
		this->currOfst = b1 + 2;
		return s;
	}
	if (this->currOfst + 6 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	UOSInt len = ReadInt32(&this->buff[this->currOfst + 2]) + 65534;
	if (this->currOfst + 6 + len > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	Text::CStringNN s = Text::CStringNN(&this->buff[this->currOfst + 6], len);
	this->currOfst = len + 6;
	return s;
}

Optional<Text::String> Data::BinaryParser::NextStrOpt()
{
	return Text::String::NewOrNull(this->NextStr());
}

UTF8Char Data::BinaryParser::NextChar()
{
	if (this->error)
		return 0;
	if (this->currOfst + 1 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	UTF8Char c = this->buff[this->currOfst];
	this->currOfst += 1;
	return c;
}

Bool Data::BinaryParser::NextBool()
{
	if (this->error)
		return false;
	if (this->currOfst + 1 > this->buff.GetSize())
	{
		this->error = true;
		return false;
	}
	Bool b;
	if (this->buff[this->currOfst] == 0)
		b = false;
	else if (this->buff[this->currOfst] == 0xff)
		b = true;
	else
	{
		this->error = true;
		return false;
	}
	this->currOfst += 1;
	return b;
}

Data::Timestamp Data::BinaryParser::NextTS()
{
	if (this->error)
		return 0;
	if (this->currOfst + 13 > this->buff.GetSize())
	{
		this->error = true;
		return 0;
	}
	Data::Timestamp ret = Data::Timestamp(Data::TimeInstant(ReadInt64(&this->buff[this->currOfst]), ReadUInt32(&this->buff[this->currOfst + 8])), this->buff[this->currOfst + 12]);
	this->currOfst += 13;
	return ret;
}

Data::Date Data::BinaryParser::NextDate()
{
	if (this->error)
		return nullptr;
	if (this->currOfst + 8 > this->buff.GetSize())
	{
		this->error = true;
		return nullptr;
	}
	Data::Date ret = Data::Date(ReadInt64(&this->buff[this->currOfst]));
	this->currOfst += 8;
	return ret;
}

Bool Data::BinaryParser::NextIPAddr(NN<Net::SocketUtil::AddressInfo> addr)
{
	if (this->error)
		return false;
	if (this->currOfst + 1 > this->buff.GetSize())
	{
		this->error = true;
		return false;
	}
	if ((Net::AddrType)this->buff[this->currOfst] == Net::AddrType::IPv4)
	{
		if (this->currOfst + 5 > this->buff.GetSize())
		{
			this->error = true;
			return false;
		}
		addr->addrType = Net::AddrType::IPv4;
		MemCopyNO(addr->addr, &this->buff[this->currOfst + 1], 4);
		this->currOfst += 5;
		return true;
	}
	else if ((Net::AddrType)this->buff[this->currOfst] == Net::AddrType::IPv6)
	{
		if (this->currOfst + 21 > this->buff.GetSize())
		{
			this->error = true;
			return false;
		}
		addr->addrType = Net::AddrType::IPv6;
		MemCopyNO(addr->addr, &this->buff[this->currOfst + 1], 20);
		this->currOfst += 21;
		return true;
	}
	else if ((Net::AddrType)this->buff[this->currOfst] == Net::AddrType::Unknown)
	{
		addr->addrType = Net::AddrType::Unknown;
		this->currOfst += 1;
		return true;
	}
	this->error = true;
	return false;
}

Bool Data::BinaryParser::HasError()
{
	return this->error;
}
