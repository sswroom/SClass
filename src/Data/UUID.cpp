#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/UUID.h"

Data::UUID::UUID(const UInt8 *buff)
{
	MemCopyNO(this->data, buff, 16);
}

Data::UUID::~UUID()
{
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
