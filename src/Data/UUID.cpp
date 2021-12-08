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

Data::UUID::~UUID()
{
}

void Data::UUID::SetValue(const UInt8 *buff)
{
	MemCopyNO(this->data, buff, 16);
}

void Data::UUID::SetValue(UUID *uuid)
{
	MemCopyNO(this->data, uuid->data, 16);
}

UOSInt Data::UUID::GetValue(UInt8 *buff)
{
	MemCopyNO(buff, this->data, 16);
	return 16;
}

OSInt Data::UUID::CompareTo(UUID *uuid)
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
	v1 = ReadUInt16(&this->data[8]);
	v2 = ReadUInt16(&uuid->data[8]);
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

void Data::UUID::ToString(Text::StringBuilderUTF *sb)
{
	sb->AppendHex32(ReadUInt32(&this->data[0]));
	sb->AppendChar('-', 1);
	sb->AppendHex16(ReadUInt16(&this->data[4]));
	sb->AppendChar('-', 1);
	sb->AppendHex16(ReadUInt16(&this->data[6]));
	sb->AppendChar('-', 1);
	sb->AppendHex16(ReadUInt16(&this->data[8]));
	sb->AppendChar('-', 1);
	sb->AppendHexBuff(&this->data[10], 6, 0, Text::LineBreakType::None);
}

UTF8Char *Data::UUID::ToString(UTF8Char *sbuff)
{
	sbuff = Text::StrHexVal32(sbuff, ReadUInt32(&this->data[0]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16(sbuff, ReadUInt16(&this->data[4]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16(sbuff, ReadUInt16(&this->data[6]));
	*sbuff++ = '-';
	sbuff = Text::StrHexVal16(sbuff, ReadUInt16(&this->data[8]));
	*sbuff++ = '-';
	sbuff = Text::StrHexBytes(sbuff, &this->data[10], 6, 0);
	return sbuff;
}

Data::UUID *Data::UUID::Clone()
{
	return NEW_CLASS_D(Data::UUID(this->data));	
}

Bool Data::UUID::Equals(UUID *uuid)
{
	if (uuid == 0)
		return false;
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