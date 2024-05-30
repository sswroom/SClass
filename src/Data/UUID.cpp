#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"

Data::UUID::UUID()
{
	MemClear(this->data, 16);
}

Data::UUID::UUID(const UInt8 *buff)
{
	this->SetValue(buff);
}

Data::UUID::UUID(Text::CStringNN str)
{
	this->SetValue(str);
}

Data::UUID::~UUID()
{
}

void Data::UUID::SetValue(UnsafeArray<const UInt8> buff)
{
	MemCopyNO(this->data, buff.Ptr(), 16);
}

void Data::UUID::SetValue(NN<UUID> uuid)
{
	MemCopyNO(this->data, uuid->data, 16);
}

void Data::UUID::SetValue(Text::CStringNN str)
{
	if (str.leng == 38 && str.v[0] == '{' && str.v[37] == '}')
	{
		str.v = str.v + 1;
		str.leng = 36;
	}
	else if (str.leng != 36)
	{
		MemClear(this->data, 16);
		return;
	}
	if (str.v[8] != '-' || str.v[13] != '-' || str.v[18] != '-' || str.v[23] != '-')
	{
		MemClear(this->data, 16);
		return;
	}
	WriteUInt32(&this->data[0], Text::StrHex2UInt32C(&str.v[0]));
	WriteUInt16(&this->data[4], Text::StrHex2UInt16C(&str.v[9]));
	WriteUInt16(&this->data[6], Text::StrHex2UInt16C(&str.v[14]));
	WriteMUInt16(&this->data[8], Text::StrHex2UInt16C(&str.v[19]));
	Text::StrHex2Bytes(&str.v[24], &this->data[10]);
}

UOSInt Data::UUID::GetValue(UInt8 *buff) const
{
	MemCopyNO(buff, this->data, 16);
	return 16;
}

OSInt Data::UUID::CompareTo(NN<UUID> uuid) const
{
	UInt32 v1 = ReadUInt32(&this->data[0]);
	UInt32 v2 = ReadUInt32(&uuid->data[0]);
	if (v1 > v2)
	{
		return 1;
	}
	else if (v1 < v2)
	{
		return -1;
	}
	v1 = ReadUInt16(&this->data[4]);
	v2 = ReadUInt16(&uuid->data[4]);
	if (v1 > v2)
	{
		return 1;
	}
	else if (v1 < v2)
	{
		return -1;
	}
	v1 = ReadUInt16(&this->data[6]);
	v2 = ReadUInt16(&uuid->data[6]);
	if (v1 > v2)
	{
		return 1;
	}
	else if (v1 < v2)
	{
		return -1;
	}
	v1 = ReadMUInt16(&this->data[8]);
	v2 = ReadMUInt16(&uuid->data[8]);
	if (v1 > v2)
	{
		return 1;
	}
	else if (v1 < v2)
	{
		return -1;
	}
	UOSInt i = 10;
	while (i < 16)
	{
		if (this->data[i] > uuid->data[i])
		{
			return 1;
		}
		else if (this->data[i] < uuid->data[i])
		{
			return -1;
		}
	}
	return 0;
}

UInt32 Data::UUID::GetTimeLow() const
{
	return ReadUInt32(&this->data[0]);
}

UInt16 Data::UUID::GetTimeMid() const
{
	return ReadUInt16(&this->data[4]);
}

UInt16 Data::UUID::GetTimeHiAndVersion() const
{
	return ReadUInt16(&this->data[6]);
}

UInt8 Data::UUID::GetClkSeqHiRes() const
{
	return this->data[8];
}

UInt8 Data::UUID::GetClkSeqLow() const
{
	return this->data[9];
}

UInt64 Data::UUID::GetNode() const
{
	return ReadMUInt64(&this->data[8]) & 0xffffffffffffLL;
}

const UInt8 *Data::UUID::GetBytes() const
{
	return this->data;
}

void Data::UUID::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	sb->AppendHex32LC(ReadUInt32(&this->data[0]));
	sb->AppendUTF8Char('-');
	sb->AppendHex16LC(ReadUInt16(&this->data[4]));
	sb->AppendUTF8Char('-');
	sb->AppendHex16LC(ReadUInt16(&this->data[6]));
	sb->AppendUTF8Char('-');
	sb->AppendHex16LC(ReadMUInt16(&this->data[8]));
	sb->AppendUTF8Char('-');
	sb->AppendHexBuffLC(&this->data[10], 6, 0, Text::LineBreakType::None);
}

UnsafeArray<UTF8Char> Data::UUID::ToString(UnsafeArray<UTF8Char> sbuff) const
{
	sbuff = Text::StrHexVal32LC(sbuff, ReadUInt32(&this->data[0]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16LC(sbuff, ReadUInt16(&this->data[4]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16LC(sbuff, ReadUInt16(&this->data[6]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16LC(sbuff, ReadMUInt16(&this->data[8]));
	*sbuff++ = '-';
	sbuff = Text::StrHexBytesLC(sbuff, &this->data[10], 6, 0);
	return sbuff;
}

NN<Data::UUID> Data::UUID::Clone() const
{
	NN<Data::UUID> ret;
	NEW_CLASSNN(ret, Data::UUID(this->data));
	return ret;	
}

Bool Data::UUID::Equals(NN<UUID> uuid) const
{
	UOSInt i = 16;
	while (i-- > 0)
	{
		if (uuid->data[i] != this->data[i])
		{
			return false;
		}
	}
	return true;
}