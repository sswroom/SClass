#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/JavaClass.h"
#include "Text/JSText.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

const UInt8 *IO::JavaClass::Type2String(const UInt8 *typeStr, Text::StringBuilderUTF *sb)
{
	OSInt arrLev = 0;
	UInt8 c;
	c = *typeStr++;
	while (c == '[')
	{
		arrLev++;
		c = *typeStr++;
	}
	switch (c)
	{
	case 'L':
		while (true)
		{
			c = *typeStr++;
			if (c == ';')
			{
				break;
			}
			else if (c == '/')
			{
				sb->AppendChar('.', 1);
			}
			else if (c == '$')
			{
				sb->AppendChar('.', 1);
			}
			else
			{
				sb->AppendChar(c, 1);
			}
		}
		break;
	case 'B':
		sb->Append((const UTF8Char*)"byte");
		break;
	case 'C':
		sb->Append((const UTF8Char*)"char");
		break;
	case 'D':
		sb->Append((const UTF8Char*)"double");
		break;
	case 'F':
		sb->Append((const UTF8Char*)"float");
		break;
	case 'I':
		sb->Append((const UTF8Char*)"int");
		break;
	case 'J':
		sb->Append((const UTF8Char*)"long");
		break;
	case 'S':
		sb->Append((const UTF8Char*)"short");
		break;
	case 'V':
		sb->Append((const UTF8Char*)"void");
		break;
	case 'Z':
		sb->Append((const UTF8Char*)"boolean");
		break;
	default:
		sb->Append((const UTF8Char*)"?");
		break;
	}

	while (arrLev-- > 0)
	{
		sb->AppendChar('[', 1);
		sb->AppendChar(']', 1);
	}
	return typeStr;
}

const UInt8 *IO::JavaClass::CondType2String(CondType ct)
{
	switch (ct)
	{
	case CT_EQ:
		return (const UTF8Char*)"==";
	case CT_NE:
		return (const UTF8Char*)"!=";
	case CT_LE:
		return (const UTF8Char*)"<=";
	case CT_GE:
		return (const UTF8Char*)">=";
	case CT_LT:
		return (const UTF8Char*)"<";
	case CT_GT:
		return (const UTF8Char*)">";
	default:
		return (const UTF8Char*)"?";
	}
}

const UInt8 *IO::JavaClass::CondType2IString(CondType ct)
{
	switch (ct)
	{
	case CT_EQ:
		return (const UTF8Char*)"!=";
	case CT_NE:
		return (const UTF8Char*)"==";
	case CT_LE:
		return (const UTF8Char*)">";
	case CT_GE:
		return (const UTF8Char*)"<";
	case CT_LT:
		return (const UTF8Char*)">=";
	case CT_GT:
		return (const UTF8Char*)"<=";
	default:
		return (const UTF8Char*)"?";
	}
}

void IO::JavaClass::DetailAccessFlags(UInt16 accessFlags, Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"0x");
	sb->AppendHex16(accessFlags);
	if (accessFlags & 1) //ACC_PUBLIC
	{
		sb->Append((const UTF8Char*)" Public");
	}
	if (accessFlags & 2) //ACC_PRIVATE
	{
		sb->Append((const UTF8Char*)" Private");
	}
	if (accessFlags & 4) //ACC_PROTECTED
	{
		sb->Append((const UTF8Char*)" Protected");
	}
	if (accessFlags & 8) //ACC_STATIC
	{
		sb->Append((const UTF8Char*)" Static");
	}
	if (accessFlags & 0x10) //ACC_FINAL
	{
		sb->Append((const UTF8Char*)" Final");
	}
	if (accessFlags & 0x20) //ACC_SUPER
	{
		sb->Append((const UTF8Char*)" Super");
	}
	if (accessFlags & 0x40) //ACC_VOLATILE
	{
		sb->Append((const UTF8Char*)" Volatile");
	}
	if (accessFlags & 0x80) //ACC_TRANSIENT
	{
		sb->Append((const UTF8Char*)" Transient");
	}
	if (accessFlags & 0x0100) //ACC_NATIVE
	{
		sb->Append((const UTF8Char*)" Interface");
	}
	if (accessFlags & 0x0200) //ACC_INTERFACE
	{
		sb->Append((const UTF8Char*)" Interface");
	}
	if (accessFlags & 0x0400) //ACC_ABSTRACT
	{
		sb->Append((const UTF8Char*)" Abstract");
	}
	if (accessFlags & 0x0800) //ACC_STRICT
	{
		sb->Append((const UTF8Char*)" Strict");
	}
	if (accessFlags & 0x1000) //ACC_SYNTHETIC
	{
		sb->Append((const UTF8Char*)" Synthetic");
	}
	if (accessFlags & 0x2000) //ACC_ANNOTATION
	{
		sb->Append((const UTF8Char*)" Annotation");
	}
	if (accessFlags & 0x4000) //ACC_ENUM
	{
		sb->Append((const UTF8Char*)" Enum");
	}
}

const UInt8 *IO::JavaClass::DetailAttribute(const UInt8 *attr, OSInt lev, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[256];
	UInt32 len = ReadMUInt32(&attr[2]);
	UInt16 nameIndex = ReadMUInt16(&attr[0]);
	sb->AppendChar(' ', lev << 1);
	sb->Append((const UTF8Char*)"Attr Name Index = ");
	sb->AppendU16(nameIndex);
	sbuff[0] = 0;
	if (this->GetConstName(sbuff, nameIndex))
	{
		sb->Append((const UTF8Char*)" (");
		sb->Append(sbuff);
		sb->AppendChar(')', 1);
	}
	sb->Append((const UTF8Char*)"\r\n");
	sb->AppendChar(' ', lev << 1);
	sb->Append((const UTF8Char*)"Attr Length = ");
	sb->AppendU32(len);
	sb->Append((const UTF8Char*)"\r\n");
	if (Text::StrEquals(sbuff, (const UTF8Char*)"ConstantValue"))
	{
		UInt16 i = ReadMUInt16(&attr[6]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr ConstantValue index = ");
		sb->AppendU16(i);
		this->DetailConstVal(i, sb, true);
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
	{
		const UInt8 *ptr;
		UInt32 codeLen = ReadMUInt32(&attr[10]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr Max Stack = ");
		sb->AppendU16(ReadMUInt16(&attr[6]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr Max Locals = ");
		sb->AppendU16(ReadMUInt16(&attr[8]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr Code Length = ");
		sb->AppendU32(codeLen);
		sb->Append((const UTF8Char*)"\r\n");
		this->DetailCode(&attr[14], codeLen, lev + 1, sb);
		ptr = &attr[14 + codeLen];
		UInt16 exceptLen = ReadMUInt16(&ptr[0]);
		UInt16 i;
		ptr += 2;
		i = 0;
		while (i < exceptLen)
		{
			UInt16 type;
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" start_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" end_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" handler_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" catch_type = ");
			type = ReadMUInt16(&ptr[6]);
			sb->AppendU16(type);
			if (type != 0)
			{
				this->DetailClassName(type, sb);
			}
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 8;
		}
		UInt16 attrCount = ReadMUInt16(&ptr[0]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr attributes_count = ");
		sb->AppendU16(attrCount);
		sb->Append((const UTF8Char*)"\r\n");
		ptr += 2;
		i = 0;
		while (i < attrCount)
		{
			ptr = this->DetailAttribute(ptr, lev + 1, sb);
			i++;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"Exceptions"))
	{
		UInt16 number_of_exceptions = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr number_of_exceptions = ");
		sb->AppendU16(number_of_exceptions);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < number_of_exceptions)
		{
			UInt16 clsIndex = ReadMUInt16(&ptr[0]);
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr exception index = ");
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 2;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"InnerClasses"))
	{
		UInt16 number_of_classes = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr number_of_classes = ");
		sb->AppendU16(number_of_classes);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < number_of_classes)
		{
			UInt16 clsIndex;
			clsIndex = ReadMUInt16(&ptr[0]);
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr inner class info index = ");
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");

			clsIndex = ReadMUInt16(&ptr[2]);
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr outer class info index = ");
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");

			clsIndex = ReadMUInt16(&ptr[4]);
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr inner name index = ");
			sb->AppendU16(clsIndex);
			if (this->GetConstName(sbuff, clsIndex))
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff);
				sb->AppendChar(')', 1);
			}
			sb->Append((const UTF8Char*)"\r\n");

			clsIndex = ReadMUInt16(&ptr[6]);
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr inner class access flags = ");
			this->DetailAccessFlags(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");

			i++;
			ptr += 8;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"EnclosingMethod"))
	{
		UInt16 clsIndex;
		const UInt8 *ptr = &attr[6];
		clsIndex = ReadMUInt16(&ptr[0]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr class index = ");
		sb->AppendU16(clsIndex);
		this->DetailClassName(clsIndex, sb);
		sb->Append((const UTF8Char*)"\r\n");

		clsIndex = ReadMUInt16(&ptr[2]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr method index = ");
		sb->AppendU16(clsIndex);
		if (clsIndex != 0)
		{
			this->DetailNameAndType(clsIndex, ReadMUInt16(&ptr[0]), sb);
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"LineNumberTable"))
	{
		UInt16 line_number_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr line_number_table_length = ");
		sb->AppendU16(line_number_table_length);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < line_number_table_length)
		{
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr start_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->Append((const UTF8Char*)", line_number = ");
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 4;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTable"))
	{
		UInt16 local_variable_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr local_variable_table_length = ");
		sb->AppendU16(local_variable_table_length);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < local_variable_table_length)
		{
			sb->AppendChar(' ', lev << 1);
			sb->Append((const UTF8Char*)"Attr start_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->Append((const UTF8Char*)", length = ");
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->Append((const UTF8Char*)", name_index = ");
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->Append((const UTF8Char*)", descriptor_index = ");
			sb->AppendU16(ReadMUInt16(&ptr[6]));
			sb->Append((const UTF8Char*)", index = ");
			sb->AppendU16(ReadMUInt16(&ptr[8]));
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 10;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"Signature"))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr Signature Index = ");
		sb->AppendU16(snameIndex);
		this->DetailName(snameIndex, sb, true);
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"SourceFile"))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr SourceFile Index = ");
		sb->AppendU16(snameIndex);
		this->DetailName(snameIndex, sb, true);
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"RuntimeVisibleAnnotations"))
	{
		UInt16 num_annotations = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr RuntimeVisibleAnnotations num_annotations = ");
		sb->AppendU16(num_annotations);
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(&attr[6], len, ' ', Text::LBT_CRLF);
		sb->Append((const UTF8Char*)"\r\n");

		while (i < num_annotations)
		{
			sb->AppendChar(' ', (lev + 1) << 1);
			ptr = this->DetailAnnotation(ptr, &attr[6 + len], sb);
			sb->Append((const UTF8Char*)"\r\n");

			i++;
		}
	}
	else
	{
		sb->AppendChar(' ', lev << 1);
		sb->Append((const UTF8Char*)"Attr ");
		sb->Append(sbuff);
		sb->Append((const UTF8Char*)" (not supported)\r\n");
	}

	attr += 6 + len;
	return attr;
}

void IO::JavaClass::DetailConstVal(UInt16 index, Text::StringBuilderUTF *sb, Bool brankets)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	UInt8 *ptr = this->constPool[index];
	if (ptr[0] == 3)
	{
		if (brankets)
		{
			sb->AppendChar(' ', 1);
			sb->AppendChar('(', 1);
		}
		sb->AppendI32(ReadMUInt32(&ptr[1]));
		if (brankets)
		{
			sb->AppendChar(')', 1);
		}
	}
	else if (ptr[0] == 4)
	{
		if (brankets)
		{
			sb->AppendChar(' ', 1);
			sb->AppendChar('(', 1);
		}
		Text::SBAppendF32(sb, ReadMFloat(&ptr[1]));
		if (brankets)
		{
			sb->AppendChar(')', 1);
		}
	}
	else if (ptr[0] == 5)
	{
		if (brankets)
		{
			sb->AppendChar(' ', 1);
			sb->AppendChar('(', 1);
		}
		sb->AppendI64(ReadMUInt64(&ptr[1]));
		if (brankets)
		{
			sb->AppendChar(')', 1);
		}
	}
	else if (ptr[0] == 6)
	{
		if (brankets)
		{
			sb->AppendChar(' ', 1);
			sb->AppendChar('(', 1);
		}
		Text::SBAppendF64(sb, ReadMDouble(&ptr[1]));
		if (brankets)
		{
			sb->AppendChar(')', 1);
		}
	}
	else if (ptr[0] == 8) //String
	{
		UInt16 sindex = ReadMUInt16(&ptr[1]);
		UInt16 strLen;
		ptr = this->constPool[sindex];
		if (ptr[0] == 1)
		{
			strLen = ReadMUInt16(&ptr[1]);
			if (brankets)
			{
				sb->AppendChar(' ', 1);
				sb->AppendChar('(', 1);
			}
			sb->AppendC(ptr + 3, strLen);
			if (brankets)
			{
				sb->AppendChar(')', 1);
			}
		}
	}
	else if (ptr[0] == 1) //String
	{
		UInt16 strLen;
		strLen = ReadMUInt16(&ptr[1]);
		if (brankets)
		{
			sb->AppendChar(' ', 1);
			sb->AppendChar('(', 1);
			sb->AppendC(ptr + 3, strLen);
			sb->AppendChar(')', 1);
		}
		else
		{
			const UTF8Char *s = Text::StrCopyNewC(ptr + 3, strLen);
			const UTF8Char *j = Text::JSText::ToNewJSTextDQuote(s);
			sb->Append(j);
			Text::JSText::FreeNewText(j);
			Text::StrDelNew(s);
		}
	}
}

void IO::JavaClass::DetailName(UInt16 index, Text::StringBuilderUTF *sb, Bool brankets)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	UInt8 *ptr = this->constPool[index];
	if (ptr[0] != 1)
	{
		return;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	if (brankets)
	{
		sb->AppendChar(' ', 1);
		sb->AppendChar('(', 1);
	}
	sb->AppendC(ptr + 3, strLen);
	if (brankets)
	{
		sb->AppendChar(')', 1);
	}
}

void IO::JavaClass::DetailClassName(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 7)
	{
		return;
	}
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][1]);
	this->DetailClassNameStr(nameIndex, sb);
}

void IO::JavaClass::DetailClassNameStr(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 1)
	{
		return;
	}
	UInt8 *ptr = this->constPool[index];
	if (ptr[0] != 1)
	{
		return;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	UInt8 *strStart;
	UInt8 *strEnd;
	UInt8 c;
	ptr += 3;
	sb->AppendChar(' ', 1);
	sb->AppendChar('(', 1);
	if (ptr[0] == '[')
	{
		Type2String(ptr, sb);
	}
	else
	{
		strEnd = ptr + strLen;
		strStart = ptr;
		while (ptr < strEnd)
		{
			c = *ptr++;
			if (c == '/')
			{
				sb->AppendC(strStart, ptr - strStart - 1);
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, strEnd - strStart);
	}
	sb->AppendChar(')', 1);
}

void IO::JavaClass::DetailFieldRef(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 9)
	{
		return;
	}
	UInt16 classIndex = ReadMUInt16(&this->constPool[index][1]);
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][3]);
	if (classIndex == 0 || classIndex >= this->constPoolCnt || nameIndex == 0 || nameIndex >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[classIndex][0] != 7 || this->constPool[nameIndex][0] != 12)
	{
		return;
	}
	UInt8 *ptr = this->constPool[classIndex];
	UInt16 i = ReadMUInt16(&ptr[1]);
	if (i == 0 || i >= this->constPoolCnt)
	{
		return;
	}
	ptr = this->constPool[i];
	if (ptr[0] != 1)
	{
		return;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	UInt8 *strStart;
	UInt8 *strEnd;
	UInt8 c;
	ptr += 3;
	strEnd = ptr + strLen;
	strStart = ptr;
	sb->AppendChar(' ', 1);
	while (ptr < strEnd)
	{
		c = *ptr++;
		if (c == '/')
		{
			sb->AppendC(strStart, ptr - strStart - 1);
			sb->AppendChar('.', 1);
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, strEnd - strStart);
	sb->AppendChar(',', 1);
	this->DetailNameAndType(nameIndex, this->thisClass, sb);
}

void IO::JavaClass::DetailMethodRef(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 10 && this->constPool[index][0] != 11)
	{
		return;
	}
	UInt16 classIndex = ReadMUInt16(&this->constPool[index][1]);
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][3]);
	if (classIndex == 0 || classIndex >= this->constPoolCnt || nameIndex == 0 || nameIndex >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[classIndex][0] != 7 || this->constPool[nameIndex][0] != 12)
	{
		return;
	}
	UInt8 *ptr = this->constPool[classIndex];
	UInt16 i = ReadMUInt16(&ptr[1]);
	if (i == 0 || i >= this->constPoolCnt)
	{
		return;
	}
	ptr = this->constPool[i];
	if (ptr[0] != 1)
	{
		return;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	UInt8 *strStart;
	UInt8 *strEnd;
	UInt8 c;
	ptr += 3;
	strEnd = ptr + strLen;
	strStart = ptr;
	sb->AppendChar(' ', 1);
	while (ptr < strEnd)
	{
		c = *ptr++;
		if (c == '/' || c == '$')
		{
			sb->AppendC(strStart, ptr - strStart - 1);
			sb->AppendChar('.', 1);
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, strEnd - strStart);
	sb->AppendChar(',', 1);
	this->DetailNameAndType(nameIndex, classIndex, sb);
}

void IO::JavaClass::DetailNameAndType(UInt16 index, UInt16 classIndex, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 12)
	{
		return;
	}
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][1]);
	UInt16 typeIndex = ReadMUInt16(&this->constPool[index][3]);
	this->DetailNameType(nameIndex, typeIndex, classIndex, (const UTF8Char*)" ", sb, 0, 0, 0, false);
}

void IO::JavaClass::DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, Text::StringBuilderUTF *sb, UTF8Char *typeBuff, const UInt8 *lvTable, UOSInt lvTableLen, Bool isStatic)
{
	if (nameIndex == 0 || nameIndex >= this->constPoolCnt || typeIndex == 0 || typeIndex >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[nameIndex][0] != 1 || this->constPool[typeIndex][0] != 1)
	{
		return;
	}

	UTF8Char sbuff[256];
	Data::ArrayList<const UTF8Char*> typeNames;
	Text::StringBuilderUTF8 sbParam;
	const UInt8 *ptr = this->constPool[typeIndex];
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	const UInt8 *strEnd;
	OSInt cnt = 0;
	Int32 paramId;
	UOSInt i;
	const UTF8Char *paramName;
	if (lvTable && lvTableLen > 0)
	{
		UOSInt local_variable_table_length = ReadMUInt16(lvTable);
		if (local_variable_table_length * 10 + 2 <= lvTableLen)
		{
			i = 0;
			while (i < local_variable_table_length)
			{
				if (this->GetConstName(sbuff, ReadMUInt16(&lvTable[2 + i * 10 + 4])))
				{
					typeNames.Add(Text::StrCopyNew(sbuff));
				}
				else
				{
					typeNames.Add(0);
				}
				
				i++;
			}
		}
	}

	if (isStatic)
	{
		paramId = 0;
	}
	else
	{
		paramId = 1;
	}
	ptr += 3;
	strEnd = ptr + strLen;
	if (typeBuff)
	{
		Text::StrConcatC(typeBuff, ptr, strLen);
	}
	if (ptr[0] == '(')
	{
		ptr++;
		sbParam.AppendChar('(', 1);
		while (ptr < strEnd && ptr[0] != ')')
		{
			if (sbParam.GetLength() > 1)
			{
				sbParam.AppendChar(',', 1);
				sbParam.AppendChar(' ', 1);
			}
			ptr = Type2String(ptr, &sbParam);
			sbParam.AppendChar(' ', 1);
			paramName = typeNames.GetItem(paramId);
			if (paramName)
			{
				sbParam.Append(paramName);
			}
			else
			{
				sbParam.AppendChar('v', 1);
				sbParam.AppendI32(paramId);
			}
			paramId++;
			cnt++;
		}
		sbParam.AppendChar(')', 1);
		ptr++;
	}
	i = typeNames.GetCount();
	while (i-- > 0)
	{
		paramName = typeNames.GetItem(i);
		SDEL_TEXT(paramName);
	}
	if (ptr >= strEnd)
	{
		return;
	}
	if (prefix)
	{
		sb->Append(prefix);
	}
	const UInt8 *ptr2 = this->constPool[nameIndex];
	strLen = ReadMUInt16(&ptr2[1]);
	if (strLen == 6 && Text::StrStartsWith(&ptr2[3], (const UTF8Char*)"<init>"))
	{
		ptr = this->constPool[classIndex];
		ptr = this->constPool[ReadMUInt16(&ptr[1])];
		strLen = ReadMUInt16(&ptr[1]);
		ptr += 3;
		const UInt8 *endPtr = ptr + strLen;
		const UInt8 *startPtr = ptr;
		while (ptr < endPtr)
		{
			if (*ptr++ == '/')
			{
				startPtr = ptr;
			}
		}
		sb->AppendC(startPtr, endPtr - startPtr);
	}
	else
	{
		Type2String(ptr, sb);
		sb->AppendChar(' ', 1);
		sb->AppendC(&ptr2[3], strLen);
	}
	sb->Append(sbParam.ToString());
}

void IO::JavaClass::DetailType(UInt16 typeIndex, Text::StringBuilderUTF *sb)
{
	if (typeIndex == 0 || typeIndex >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[typeIndex][0] != 1)
	{
		return;
	}	
	const UInt8 *ptr = this->constPool[typeIndex];
	ptr += 3;
	Type2String(ptr, sb);
}

void IO::JavaClass::DetailCode(const UInt8 *code, OSInt codeLen, OSInt lev, Text::StringBuilderUTF *sb)
{
	UInt16 val;
	while (codeLen > 0)
	{
		switch (code[0])
		{
		case 0x00:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"nop\r\n");
			code++;
			codeLen--;
			break;
		case 0x01:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aconst_null\r\n");
			code++;
			codeLen--;
			break;
		case 0x02:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_m1\r\n");
			code++;
			codeLen--;
			break;
		case 0x03:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x04:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x05:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x06:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x07:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_4\r\n");
			code++;
			codeLen--;
			break;
		case 0x08:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_5\r\n");
			code++;
			codeLen--;
			break;
		case 0x09:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lconst_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x0A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lconst_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x0B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x0C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x0D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x0E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dconst_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x0F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dconst_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x10:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"bipush ");
			val = code[1];
			sb->AppendU16(val);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x11:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"sipush ");
			val = ReadMUInt16(&code[1]);
			sb->AppendI16(val);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x12:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"ldc ");
			val = code[1];
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x13:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ldc_w ");
			val = ReadMUInt16(&code[1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x14:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ldc2_w ");
			val = ReadMUInt16(&code[1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x15:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"iload ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x16:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"lload ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x17:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"fload ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x18:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"dload ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x19:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"aload ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x1A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x1B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x1C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x1D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x1E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x1F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x20:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x21:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x22:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x23:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x24:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x25:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x26:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x27:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x28:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x29:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x2A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x2B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x2C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x2D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x2E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iaload\r\n");
			code++;
			codeLen--;
			break;
		case 0x2F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"laload\r\n");
			code++;
			codeLen--;
			break;
		case 0x30:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"faload\r\n");
			code++;
			codeLen--;
			break;
		case 0x31:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"daload\r\n");
			code++;
			codeLen--;
			break;
		case 0x32:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aaload\r\n");
			code++;
			codeLen--;
			break;
		case 0x33:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"baload\r\n");
			code++;
			codeLen--;
			break;
		case 0x34:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"caload\r\n");
			code++;
			codeLen--;
			break;
		case 0x35:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"saload\r\n");
			code++;
			codeLen--;
			break;
		case 0x36:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"istore ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x37:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"lstore ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x38:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"fstore ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x39:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"dstore ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x3A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"astore ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0x3B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x3C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x3D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x3E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x3F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x40:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x41:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x42:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x43:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x44:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x45:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x46:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x47:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x48:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x49:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x4A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x4B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_0\r\n");
			code++;
			codeLen--;
			break;
		case 0x4C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_1\r\n");
			code++;
			codeLen--;
			break;
		case 0x4D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_2\r\n");
			code++;
			codeLen--;
			break;
		case 0x4E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_3\r\n");
			code++;
			codeLen--;
			break;
		case 0x4F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x50:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x51:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x52:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x53:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x54:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"bastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x55:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"castore\r\n");
			code++;
			codeLen--;
			break;
		case 0x56:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"sastore\r\n");
			code++;
			codeLen--;
			break;
		case 0x57:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"pop\r\n");
			code++;
			codeLen--;
			break;
		case 0x58:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"pop2\r\n");
			code++;
			codeLen--;
			break;
		case 0x59:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup\r\n");
			code++;
			codeLen--;
			break;
		case 0x5A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup_x1\r\n");
			code++;
			codeLen--;
			break;
		case 0x5B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup_x2\r\n");
			code++;
			codeLen--;
			break;
		case 0x5C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2\r\n");
			code++;
			codeLen--;
			break;
		case 0x5D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2_x1\r\n");
			code++;
			codeLen--;
			break;
		case 0x5E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2_x2\r\n");
			code++;
			codeLen--;
			break;
		case 0x5F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"swap\r\n");
			code++;
			codeLen--;
			break;
		case 0x60:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iadd\r\n");
			code++;
			codeLen--;
			break;
		case 0x61:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ladd\r\n");
			code++;
			codeLen--;
			break;
		case 0x62:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fadd\r\n");
			code++;
			codeLen--;
			break;
		case 0x63:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dadd\r\n");
			code++;
			codeLen--;
			break;
		case 0x64:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"isub\r\n");
			code++;
			codeLen--;
			break;
		case 0x65:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lsub\r\n");
			code++;
			codeLen--;
			break;
		case 0x66:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fsub\r\n");
			code++;
			codeLen--;
			break;
		case 0x67:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dsub\r\n");
			code++;
			codeLen--;
			break;
		case 0x68:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"imul\r\n");
			code++;
			codeLen--;
			break;
		case 0x69:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lmul\r\n");
			code++;
			codeLen--;
			break;
		case 0x6A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fmul\r\n");
			code++;
			codeLen--;
			break;
		case 0x6B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dmul\r\n");
			code++;
			codeLen--;
			break;
		case 0x6C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"idiv\r\n");
			code++;
			codeLen--;
			break;
		case 0x6D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ldiv\r\n");
			code++;
			codeLen--;
			break;
		case 0x6E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fdiv\r\n");
			code++;
			codeLen--;
			break;
		case 0x6F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ddiv\r\n");
			code++;
			codeLen--;
			break;
		case 0x70:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"irem\r\n");
			code++;
			codeLen--;
			break;
		case 0x71:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lrem\r\n");
			code++;
			codeLen--;
			break;
		case 0x72:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"frem\r\n");
			code++;
			codeLen--;
			break;
		case 0x73:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"drem\r\n");
			code++;
			codeLen--;
			break;
		case 0x74:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ineg\r\n");
			code++;
			codeLen--;
			break;
		case 0x75:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lneg\r\n");
			code++;
			codeLen--;
			break;
		case 0x76:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fneg\r\n");
			code++;
			codeLen--;
			break;
		case 0x77:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dneg\r\n");
			code++;
			codeLen--;
			break;
		case 0x78:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ishl\r\n");
			code++;
			codeLen--;
			break;
		case 0x79:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lshl\r\n");
			code++;
			codeLen--;
			break;
		case 0x7A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ishr\r\n");
			code++;
			codeLen--;
			break;
		case 0x7B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lshr\r\n");
			code++;
			codeLen--;
			break;
		case 0x7C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iushr\r\n");
			code++;
			codeLen--;
			break;
		case 0x7D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lushr\r\n");
			code++;
			codeLen--;
			break;
		case 0x7E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iand\r\n");
			code++;
			codeLen--;
			break;
		case 0x7F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"land\r\n");
			code++;
			codeLen--;
			break;
		case 0x80:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ior\r\n");
			code++;
			codeLen--;
			break;
		case 0x81:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lor\r\n");
			code++;
			codeLen--;
			break;
		case 0x82:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ixor\r\n");
			code++;
			codeLen--;
			break;
		case 0x83:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lxor\r\n");
			code++;
			codeLen--;
			break;
		case 0x84:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"iinc ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)", ");
			sb->AppendI16((Int8)code[2]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x85:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2l\r\n");
			code++;
			codeLen--;
			break;
		case 0x86:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2f\r\n");
			code++;
			codeLen--;
			break;
		case 0x87:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2d\r\n");
			code++;
			codeLen--;
			break;
		case 0x88:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2i\r\n");
			code++;
			codeLen--;
			break;
		case 0x89:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2f\r\n");
			code++;
			codeLen--;
			break;
		case 0x8A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2d\r\n");
			code++;
			codeLen--;
			break;
		case 0x8B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2i\r\n");
			code++;
			codeLen--;
			break;
		case 0x8C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2l\r\n");
			code++;
			codeLen--;
			break;
		case 0x8D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2d\r\n");
			code++;
			codeLen--;
			break;
		case 0x8E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2i\r\n");
			code++;
			codeLen--;
			break;
		case 0x8F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2l\r\n");
			code++;
			codeLen--;
			break;
		case 0x90:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2f\r\n");
			code++;
			codeLen--;
			break;
		case 0x91:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2b\r\n");
			code++;
			codeLen--;
			break;
		case 0x92:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2c\r\n");
			code++;
			codeLen--;
			break;
		case 0x93:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2s\r\n");
			code++;
			codeLen--;
			break;
		case 0x94:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lcmp\r\n");
			code++;
			codeLen--;
			break;
		case 0x95:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fcmpl\r\n");
			code++;
			codeLen--;
			break;
		case 0x96:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fcmpg\r\n");
			code++;
			codeLen--;
			break;
		case 0x97:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dcmpl\r\n");
			code++;
			codeLen--;
			break;
		case 0x98:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dcmpg\r\n");
			code++;
			codeLen--;
			break;
		case 0x99:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifeq ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9A:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifne ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9B:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"iflt ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9C:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifge ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9D:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifgt ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9E:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifle ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0x9F:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpeq ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA0:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpne ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA1:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmplt ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA2:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpge ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA3:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpgt ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA4:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmple ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA5:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_acmpeq ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA6:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_acmpne ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA7:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"goto ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA8:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"jsr ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xA9:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"ret ");
			sb->AppendU16(code[1]);
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0xAA:
			sb->AppendChar(' ', lev << 1);
			////////////////////////////////////
			sb->AppendHexBuff(code, 17, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"tableswitch\r\n");
			code += 17;
			codeLen -= 17;
			break;
		case 0xAB:
			sb->AppendChar(' ', lev << 1);
			////////////////////////////////////
			sb->AppendHexBuff(code, 8, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"lookupswitch\r\n");
			code += 8;
			codeLen -= 8;
			break;
		case 0xAC:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ireturn\r\n");
			code++;
			codeLen--;
			break;
		case 0xAD:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lreturn\r\n");
			code++;
			codeLen--;
			break;
		case 0xAE:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"freturn\r\n");
			code++;
			codeLen--;
			break;
		case 0xB0:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"areturn\r\n");
			code++;
			codeLen--;
			break;
		case 0xB1:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"return\r\n");
			code++;
			codeLen--;
			break;
		case 0xB2:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"getstatic ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB3:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"putstatic ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB4:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"getfield ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB5:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"putfield ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB6:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"invokevirtual ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB7:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"invokespecial ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB8:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"invokestatic ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xB9:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 5, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"invokeinterface ");
			sb->AppendU16(val);
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
			sb->AppendU16(code[3]);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 5;
			codeLen -= 5;
			break;
		case 0xBA:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 5, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"invokedynamic ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 5;
			codeLen -= 5;
			break;
		case 0xBB:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"new ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xBC:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 2, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 10);
			val = code[1];
			sb->Append((const UTF8Char*)"newarray ");
			sb->AppendU16(val);
			switch (val)
			{
			case 4: //T_BOOLEAN
				sb->Append((const UTF8Char*)" (T_BOOLEAN)");
				break;
			case 5: //T_CHAR
				sb->Append((const UTF8Char*)" (T_CHAR)");
				break;
			case 6: //T_FLOAT
				sb->Append((const UTF8Char*)" (T_FLOAT)");
				break;
			case 7: //T_DOUBLE
				sb->Append((const UTF8Char*)" (T_DOUBLE)");
				break;
			case 8: //T_BYTE
				sb->Append((const UTF8Char*)" (T_BYTE)");
				break;
			case 9: //T_SHORT
				sb->Append((const UTF8Char*)" (T_SHORT)");
				break;
			case 10: //T_INT
				sb->Append((const UTF8Char*)" (T_INT)");
				break;
			case 11: //T_LONG
				sb->Append((const UTF8Char*)" (T_LONG)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\n");
			code += 2;
			codeLen -= 2;
			break;
		case 0xBD:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"anewarray ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xBE:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"arraylength\r\n");
			code++;
			codeLen--;
			break;
		case 0xBF:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"athrow\r\n");
			code++;
			codeLen--;
			break;
		case 0xC0:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"checkcast ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xC1:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"instanceof ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xC2:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"monitorenter\r\n");
			code++;
			codeLen--;
			break;
		case 0xC3:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"monitorexit\r\n");
			code++;
			codeLen--;
			break;
		case 0xC4:
			sb->AppendChar(' ', lev << 1);
			//////////////////////////////////////////
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"wide\r\n");
			code++;
			codeLen--;
			break;
		case 0xC5:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 4, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 4);
			val = ReadMUInt16(&code[1]);
			sb->Append((const UTF8Char*)"multianewarray ");
			sb->AppendU16(val);
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
			sb->AppendU16(code[3]);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			code += 4;
			codeLen -= 4;
			break;
		case 0xC6:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifnull ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xC7:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 3, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifnonnull ");
			sb->AppendI16(ReadMInt16(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 3;
			codeLen -= 3;
			break;
		case 0xC8:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 5, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"goto_w ");
			sb->AppendI32(ReadMInt32(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 5;
			codeLen -= 5;
			break;
		case 0xC9:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHexBuff(code, 5, ' ', Text::LBT_NONE);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"jsr_w ");
			sb->AppendI32(ReadMInt32(&code[1]));
			sb->Append((const UTF8Char*)"\r\n");
			code += 5;
			codeLen -= 5;
			break;
		case 0xCA:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"breakpoint\r\n");
			code++;
			codeLen--;
			break;
		case 0xFE:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"impdep1\r\n");
			code++;
			codeLen--;
			break;
		case 0xFF:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"impdep2\r\n");
			code++;
			codeLen--;
			break;
		default:
			sb->AppendChar(' ', lev << 1);
			sb->AppendHex8(code[0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"Unknown opcode\r\n");
			code++;
			codeLen--;
			break;
		}
	}
}

const UInt8 *IO::JavaClass::DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb)
{
	sb->AppendChar('@', 1);
	if (annoEnd - annoPtr < 4)
	{
		return annoPtr;
	}
	UInt16 type_index = ReadMUInt16(&annoPtr[0]);
	UInt16 num_element_value_pairs = ReadMUInt16(&annoPtr[2]);
	UOSInt i;
	annoPtr += 4;
	this->DetailType(type_index, sb);
	if (num_element_value_pairs > 0)
	{
		sb->AppendChar('(', 1);
		i = 0;
		while (i < num_element_value_pairs)
		{
			if (annoEnd - annoPtr < 4)
			{
				break;
			}
			if (i > 0)
			{
				sb->AppendChar(',', 1);
			}
			UInt16 element_name_index = ReadMUInt16(&annoPtr[0]);
			this->DetailName(element_name_index, sb, false);
			sb->AppendChar('=', 1);
			annoPtr = this->DetailElementValue(&annoPtr[2], annoEnd, sb);
			i++;
		}
		sb->AppendChar(')', 1);
	}
	return annoPtr;
}

/*
element_value {
    u1 tag; = '['
    union {
        u2 const_value_index;

        {   u2 type_name_index;
            u2 const_name_index;
        } enum_const_value;

        u2 class_info_index;

        annotation annotation_value;

        {   u2            num_values;
            element_value values[num_values];
        } array_value;
    } value;
}
*/
const UInt8 *IO::JavaClass::DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[128];
	if (annoEnd - annoPtr < 3)
	{
		return annoPtr;
	}

	if (annoPtr[0] == '[')
	{
		UInt16 num_values = ReadMUInt16(&annoPtr[1]);
		UOSInt i;
		sb->AppendChar('{', 1);
		annoPtr += 3;
		i = 0;
		while (i < num_values)
		{
			if (i > 0)
			{
				sb->AppendChar(',', 1);
			}
			annoPtr = this->DetailElementValue(annoPtr, annoEnd, sb);
			i++;
		}
		sb->AppendChar('}', 1);
		return annoPtr;
	}
	else if (annoPtr[0] == 's' || annoPtr[0] == 'B' || annoPtr[0] == 'C' || annoPtr[0] == 'D' || annoPtr[0] == 'F' || annoPtr[0] == 'I' || annoPtr[0] == 'J' || annoPtr[0] == 'S' || annoPtr[0] == 'Z')
	{
		this->DetailConstVal(ReadMUInt16(&annoPtr[1]), sb, false);
		annoPtr += 3;
		return annoPtr;
	}
	else if (annoPtr[0] == 'e')
	{
		UInt16 type_name_index = ReadMUInt16(&annoPtr[1]);
		UInt16 const_name_index = ReadMUInt16(&annoPtr[3]);
		this->DetailType(type_name_index, sb);
		sb->AppendChar('.', 1);
		this->GetConstName(sbuff, const_name_index);
		sb->Append(sbuff);
		annoPtr += 5;
		return annoPtr;
	}
	else
	{
		sb->AppendChar('?', 1);
		return annoPtr + 1;
	}
}

UTF8Char *IO::JavaClass::GetConstName(UTF8Char *sbuff, UInt16 index)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return 0;
	}
	UInt8 *ptr = this->constPool[index];
	if (ptr[0] != 1)
	{
		return 0;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	return Text::StrConcatC(sbuff, &ptr[3], strLen);
}

void IO::JavaClass::ClassNameString(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	if (this->constPool[index][0] != 7)
	{
		return;
	}
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][1]);
	UInt8 *ptr = this->constPool[nameIndex];
	if (ptr[0] != 1)
	{
		return;
	}
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	UInt8 *strStart;
	UInt8 *strEnd;
	UInt8 c;
	ptr += 3;
	if (ptr[0] == '[')
	{
		Type2String(ptr, sb);
	}
	else
	{
		strEnd = ptr + strLen;
		strStart = ptr;
		while (ptr < strEnd)
		{
			c = *ptr++;
			if (c == '/')
			{
				sb->AppendC(strStart, ptr - strStart - 1);
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
			else if (c == '$')
			{
				sb->AppendC(strStart, ptr - strStart - 1);
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, strEnd - strStart);
	}
}

UTF8Char *IO::JavaClass::GetLVName(UTF8Char *sbuff, UInt16 index, const UInt8 *lvTable, UOSInt lvTableLen, UOSInt codeOfst)
{
	if (lvTable && lvTableLen > 0)
	{
		UOSInt local_variable_table_length = ReadMUInt16(lvTable);
		if (local_variable_table_length * 10 + 2 >= lvTableLen)
		{
			UOSInt i = 0;
			while (i < local_variable_table_length)
			{
				UInt16 pcOfst = ReadMUInt16(&lvTable[2 + i * 10 + 0]);
				UInt16 length = ReadMUInt16(&lvTable[2 + i * 10 + 2]);
				UOSInt varI = ReadMUInt16(&lvTable[2 + i * 10 + 8]);

				if (varI == index && pcOfst <= codeOfst && pcOfst + length > codeOfst)
				{
					UTF8Char *ret = this->GetConstName(sbuff, ReadMUInt16(&lvTable[2 + i * 10 + 4]));
					if (ret)
					{
						return ret;
					}
				}
				i++;
			}
		}
	}
	*sbuff++ = 'v';
	return Text::StrUInt16(sbuff, index);
}


IO::JavaClass::JavaClass(const UTF8Char *sourceName, const UInt8 *buff, OSInt buffSize) : IO::ParsedObject(sourceName)
{
	this->fileBuff = 0;
	this->fileBuffSize = 0;
	this->constPoolCnt = 0;
	this->constPool = 0;
	this->accessFlags = 0;
	this->thisClass = 0;
	this->superClass = 0;
	this->interfaceCnt = 0;
	this->interfaces = 0;
	this->fieldsCnt = 0;
	this->fields = 0;
	this->methodCnt = 0;
	this->methods = 0;
	this->attrCnt = 0;
	this->attrs = 0;
	if (buffSize < 26)
	{
		return;
	}
	if (ReadMUInt32(&buff[0]) != 0xCAFEBABE)
	{
		return;
	}
	this->fileBuffSize = buffSize;
	this->fileBuff = MemAlloc(UInt8, buffSize);
	MemCopyNO(this->fileBuff, buff, buffSize);

	OSInt ofst;
	UInt16 constant_pool_count = ReadMUInt16(&this->fileBuff[8]);
	UInt16 i;
	UInt16 j;
	Bool valid = true;
	this->constPoolCnt = constant_pool_count;
	this->constPool = MemAlloc(UInt8*, this->constPoolCnt);
	this->constPool[0] = 0;
	i = 1;
	ofst = 10;
	while (i < constant_pool_count && ofst < this->fileBuffSize)
	{
		UInt16 strLen;
		this->constPool[i] = &this->fileBuff[ofst];
		switch (this->fileBuff[ofst])
		{
		case 1: //CONSTANT_Utf8
			if (ofst + 3 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			strLen = ReadMUInt16(&this->fileBuff[ofst + 1]);
			if (ofst + 3 + strLen > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 3 + strLen;
			break;
		case 3: //CONSTANT_Integer
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 4: //CONSTANT_Float
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 5: //CONSTANT_Long
			if (ofst + 9 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 9;
			i++;
			this->constPool[i] = 0;
			break;
		case 6: //CONSTANT_Double
			if (ofst + 9 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 9;
			i++;
			this->constPool[i] = 0;
			break;
		case 7: //CONSTANT_Class
			if (ofst + 3 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 3;
			break;
		case 8: //CONSTANT_String
			if (ofst + 3 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 3;
			break;
		case 9: //CONSTANT_Fieldref
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 10: //CONSTANT_Methodref
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 11: //CONSTANT_InterfaceMethodref
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 12: //CONSTANT_NameAndType
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		case 15: //CONSTANT_MethodHandle
			if (ofst + 4 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 4;
			break;
		case 16: //CONSTANT_MethodType
			if (ofst + 3 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 3;
			break;
		case 18: //CONSTANT_InvokeDynamic
			if (ofst + 5 > this->fileBuffSize)
			{
				this->constPoolCnt = i;
				valid = false;
				break;
			}
			ofst += 5;
			break;
		default:
			printf("Constant Pool %d/%d, unknown tag %d\r\n", i, constant_pool_count, this->fileBuff[ofst]);
			this->constPoolCnt = i;
			valid = false;
			break;
		}
		if (!valid)
		{
			break;
		}
		
		i++;
	}
	if (!valid || ofst + 12 > this->fileBuffSize)
	{
		return;
	}

	this->accessFlags = ReadMUInt16(&this->fileBuff[ofst]);
	this->thisClass = ReadMUInt16(&this->fileBuff[ofst + 2]);
	this->superClass = ReadMUInt16(&this->fileBuff[ofst + 4]);
	this->interfaceCnt = ReadMUInt16(&this->fileBuff[ofst + 6]);
	ofst += 8;
	if (ofst + this->interfaceCnt * 2 > this->fileBuffSize)
	{
		return ;
	}
	this->interfaces = &this->fileBuff[ofst];
	ofst += this->interfaceCnt * 2;
	if (ofst + 2 > this->fileBuffSize)
	{
		return;
	}
	UInt16 fields_count = ReadMUInt16(&this->fileBuff[ofst]);
	ofst += 2;
	this->fieldsCnt = fields_count;
	if (this->fieldsCnt > 0)
	{
		this->fields = MemAlloc(UInt8*, this->fieldsCnt);
		i = 0;
		while (i < fields_count)
		{
			this->fields[i] = &this->fileBuff[ofst];
			if (ofst + 8 > this->fileBuffSize)
			{
				this->fieldsCnt = i;
				return;
			}
			UInt16 attributes_count = ReadMUInt16(&this->fileBuff[ofst + 6]);
			ofst += 8;
			j = 0;
			while (j < attributes_count)
			{
				if (ofst + 6 > this->fileBuffSize)
				{
					this->fieldsCnt = i;
					return;
				}
				ofst += 6 + ReadMUInt32(&this->fileBuff[ofst + 2]);
				j++;
			}
			i++;
		}
	}

	if (ofst + 2 > this->fileBuffSize)
	{
		return ;
	}
	UInt16 methods_count = ReadMUInt16(&this->fileBuff[ofst]);
	ofst += 2;
	this->methodCnt = methods_count;
	if (this->methodCnt > 0)
	{
		this->methods = MemAlloc(UInt8*, this->methodCnt);
		i = 0;
		while (i < methods_count)
		{
			this->methods[i] = &this->fileBuff[ofst];
			if (ofst + 8 > this->fileBuffSize)
			{
				this->methodCnt = i;
				return;
			}
			UInt16 attributes_count = ReadMUInt16(&this->fileBuff[ofst + 6]);

			ofst += 8;
			j = 0;
			while (j < attributes_count)
			{
				if (ofst + 6 > this->fileBuffSize)
				{
					this->methodCnt = i;
					return;
				}
				ofst += 6 + ReadMUInt32(&this->fileBuff[ofst + 2]);
				j++;
			}
			i++;
		}
	}
	if (ofst + 2 > this->fileBuffSize)
	{
		return ;
	}
	UInt16 attributes_count = ReadMUInt16(&this->fileBuff[ofst]);
	ofst += 2;
	this->attrCnt = attributes_count;
	if (this->attrCnt > 0)
	{
		this->attrs = MemAlloc(UInt8*, this->attrCnt);
		j = 0;
		while (j < attributes_count)
		{
			this->attrs[j] = &this->fileBuff[ofst];
			if (ofst + 6 > this->fileBuffSize)
			{
				this->attrCnt = j;
				return;
			}
			ofst += 6 + ReadMUInt32(&this->fileBuff[ofst + 2]);
			j++;
		}
	}
}

IO::JavaClass::~JavaClass()
{
	if (this->fileBuff)
	{
		MemFree(this->fileBuff);
		this->fileBuff = 0;
	}
	if (this->constPool)
	{
		MemFree(this->constPool);
		this->constPool = 0;
	}
	if (this->fields)
	{
		MemFree(this->fields);
		this->fields = 0;
	}
	if (this->methods)
	{
		MemFree(this->methods);
		this->methods = 0;
	}
	if (this->attrs)
	{
		MemFree(this->attrs);
		this->attrs = 0;
	}
}

IO::ParsedObject::ParserType IO::JavaClass::GetParserType()
{
	return IO::ParsedObject::PT_JAVA_CLASS;
}

Bool IO::JavaClass::FileStructDetail(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Version = ");
	sb->AppendU16(ReadMUInt16(&this->fileBuff[6]));
	sb->AppendChar('.', 1);
	sb->AppendU16(ReadMUInt16(&this->fileBuff[4]));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"constant_pool_count = ");
	sb->AppendOSInt(this->constPoolCnt);
	sb->Append((const UTF8Char*)"\r\n");
	OSInt i;
	UInt16 j;
	i = 1;
	while (i < this->constPoolCnt)
	{
		UInt16 strLen;
		sb->Append((const UTF8Char*)"Const ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)": ");
		if (this->constPool[i] == 0)
		{
			sb->Append((const UTF8Char*)"unusable");
		}
		else
		{
			switch (this->constPool[i][0])
			{
			case 1: //CONSTANT_Utf8
				strLen = ReadMUInt16(&this->constPool[i][1]);
				sb->Append((const UTF8Char*)"UTF8 = ");
				sb->AppendC(&this->constPool[i][3], strLen);
				break;
			case 3: //CONSTANT_Integer
				sb->Append((const UTF8Char*)"Integer, value = ");
				sb->AppendI32(ReadMInt32(&this->constPool[i][1]));
				break;
			case 4: //CONSTANT_Float
				sb->Append((const UTF8Char*)"Float, value = ");
				Text::SBAppendF32(sb, ReadMFloat(&this->constPool[i][1]));
				break;
			case 5: //CONSTANT_Long
				sb->Append((const UTF8Char*)"Long, value = ");
				sb->AppendI64(ReadMInt64(&this->constPool[i][1]));
				break;
			case 6: //CONSTANT_Float
				sb->Append((const UTF8Char*)"Double, value = ");
				Text::SBAppendF64(sb, ReadMDouble(&this->constPool[i][1]));
				break;
			case 7: //CONSTANT_Class
				sb->Append((const UTF8Char*)"Class, Name index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 8: //CONSTANT_String
				sb->Append((const UTF8Char*)"String, String index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 9: //CONSTANT_Fieldref
				sb->Append((const UTF8Char*)"Fieldref, Class index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->Append((const UTF8Char*)", Name and type index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 10: //CONSTANT_Methodref
				sb->Append((const UTF8Char*)"Methodref, Class index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->Append((const UTF8Char*)", Name and type index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 11: //CONSTANT_InterfaceMethodref
				sb->Append((const UTF8Char*)"InterfaceMethodref, Class index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->Append((const UTF8Char*)", Name and type index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 12: //CONSTANT_NameAndType
				sb->Append((const UTF8Char*)"NameAndType, Name index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->Append((const UTF8Char*)", Descriptor index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 15: //CONSTANT_MethodHandle
				sb->Append((const UTF8Char*)"MethodHandle, Ref Kind = ");
				sb->AppendU16(this->constPool[i][1]);
				sb->Append((const UTF8Char*)", Ref index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][2]));
				break;
			case 16: //CONSTANT_MethodType
				sb->Append((const UTF8Char*)"MethodType, Descriptor index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 18: //CONSTANT_InvokeDynamic
				sb->Append((const UTF8Char*)"InvokeDynamic, bootstrap method attr index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->Append((const UTF8Char*)", Name and Type index = ");
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			}
		}
		sb->Append((const UTF8Char*)"\r\n");
		i++;
	}

	sb->Append((const UTF8Char*)"Access Flags = ");
	DetailAccessFlags(this->accessFlags, sb);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"this_class = ");
	sb->AppendU16(this->thisClass);
	this->DetailClassName(this->thisClass, sb);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"super_class = ");
	sb->AppendU16(this->superClass);
	this->DetailClassName(this->superClass, sb);
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"interfaces_count = ");
	sb->AppendOSInt(this->interfaceCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->interfaceCnt)
	{
		sb->Append((const UTF8Char*)"interfaces[");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)"] = ");
		sb->AppendU16(ReadMUInt16(&this->interfaces[i * 2]));
		sb->Append((const UTF8Char*)"\r\n");
		i++;
	}
	sb->Append((const UTF8Char*)"fields_count = ");
	sb->AppendOSInt(this->fieldsCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->fieldsCnt)
	{
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" Access Flags = ");
		DetailAccessFlags(ReadMUInt16(&this->fields[i][0]), sb);
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" name index = ");
		sb->AppendU16(ReadMUInt16(&this->fields[i][2]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" descriptor index = ");
		sb->AppendU16(ReadMUInt16(&this->fields[i][4]));
		sb->Append((const UTF8Char*)"\r\n");
		UInt16 attributes_count = ReadMUInt16(&this->fields[i][6]);
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" attributes count = ");
		sb->AppendU16(attributes_count);
		sb->Append((const UTF8Char*)"\r\n");
		const UInt8 *attr = &this->fields[i][8];
		j = 0;
		while (j < attributes_count)
		{
			attr = this->DetailAttribute(attr, 1, sb);
			j++;
		}
		i++;
	}
	sb->Append((const UTF8Char*)"methods_count = ");
	sb->AppendOSInt(this->methodCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->methodCnt)
	{
		UInt16 accessFlags = ReadMUInt16(&this->methods[i][0]);
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" Access Flags = ");
		DetailAccessFlags(accessFlags, sb);
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" name index = ");
		sb->AppendU16(ReadMUInt16(&this->methods[i][2]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" descriptor index = ");
		sb->AppendU16(ReadMUInt16(&this->methods[i][4]));
		this->DetailNameType(ReadMUInt16(&this->methods[i][2]), ReadMUInt16(&this->methods[i][4]), this->thisClass, (const UTF8Char*)" ", sb, 0, 0, 0, (accessFlags & 8) != 0);
		sb->Append((const UTF8Char*)"\r\n");
		UInt16 attributes_count = ReadMUInt16(&this->methods[i][6]);
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendOSInt(i);
		sb->Append((const UTF8Char*)" attributes count = ");
		sb->AppendU16(attributes_count);
		sb->Append((const UTF8Char*)"\r\n");
		const UInt8 *attr = &this->methods[i][8];
		j = 0;
		while (j < attributes_count)
		{
			attr = this->DetailAttribute(attr, 1, sb);
			j++;
		}
		i++;
	}

	sb->Append((const UTF8Char*)"attributes_count = ");
	sb->AppendOSInt(this->attrCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->attrCnt)
	{
		this->DetailAttribute(this->attrs[i], 1, sb);
		i++;
	}
	return true;
}

OSInt IO::JavaClass::FieldsGetCount()
{
	return this->fieldsCnt;
}

Bool IO::JavaClass::FieldsGetDecl(OSInt index, Text::StringBuilderUTF *sb)
{
	if (index < 0 || index >= this->fieldsCnt)
	{
		return false;
	}
	UTF8Char sbuff[256];
	UInt8 *ptr = this->fields[index];
	UInt16 accessFlags = ReadMUInt16(ptr);
	if (accessFlags & 1)
	{
		sb->Append((const UTF8Char*)"public ");
	}
	if (accessFlags & 2)
	{
		sb->Append((const UTF8Char*)"private ");
	}
	if (accessFlags & 4)
	{
		sb->Append((const UTF8Char*)"protected ");
	}
	if (accessFlags & 8)
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (accessFlags & 0x10)
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (accessFlags & 0x40)
	{
		sb->Append((const UTF8Char*)"volatile ");
	}
	this->GetConstName(sbuff, ReadMUInt16(&ptr[4]));
	Type2String(sbuff, sb);
	sb->AppendChar(' ', 1);
	this->GetConstName(sbuff, ReadMUInt16(&ptr[2]));
	sb->Append(sbuff);
	UInt16 attrCnt = ReadMUInt16(&ptr[6]);
	UInt16 i = 0;
	ptr += 8;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		sbuff[0] = 0;
		if (this->GetConstName(sbuff, nameIndex))
		{
			if (Text::StrEquals(sbuff, (const UTF8Char*)"ConstantValue"))
			{
				nameIndex = ReadMUInt16(&ptr[6]);
				if (nameIndex > 0 && nameIndex < this->constPoolCnt && this->constPool[nameIndex] != 0)
				{
					if (this->constPool[nameIndex][0] == 3)
					{
						sb->Append((const UTF8Char*)" = ");
						sb->AppendI32(ReadMInt32(&this->constPool[nameIndex][1]));
					}
					else if (this->constPool[nameIndex][0] == 4)
					{
						sb->Append((const UTF8Char*)" = ");
						Text::SBAppendF32(sb, ReadMFloat(&this->constPool[nameIndex][1]));
						sb->AppendChar('f', 1);
					}
					else if (this->constPool[nameIndex][0] == 5)
					{
						sb->Append((const UTF8Char*)" = ");
						sb->AppendI64(ReadMInt64(&this->constPool[nameIndex][1]));
						sb->AppendChar('L', 1);
					}
					else if (this->constPool[nameIndex][0] == 6)
					{
						sb->Append((const UTF8Char*)" = ");
						Text::SBAppendF64(sb, ReadMDouble(&this->constPool[nameIndex][1]));
					}
				}
			}
		}
		ptr += len + 6;

		i++;
	}
	return true;
}

OSInt IO::JavaClass::MethodsGetCount()
{
	return this->methodCnt;
}

Bool IO::JavaClass::MethodsGetDecl(OSInt index, Text::StringBuilderUTF *sb)
{
	if (index < 0 || index >= this->methodCnt)
	{
		return false;
	}

	UTF8Char sbuff[512];
	UInt8 *ptr = this->methods[index];
	UInt16 accessFlags = ReadMUInt16(ptr);
	UInt16 attrCnt = ReadMUInt16(&ptr[6]);
	UInt16 i;
	UInt8 *lvTable = 0;
	UOSInt lvTableLen = 0;

	ptr = &this->methods[index][8];
	i = 0;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		sbuff[0] = 0;
		if (this->GetConstName(sbuff, nameIndex))
		{
			if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
			{
				UInt32 codeLen = ReadMUInt32(&ptr[10]);
				UInt8 *ptr2;
				ptr2 = &ptr[14 + codeLen];
				UInt16 exceptLen = ReadMUInt16(&ptr2[0]);
				ptr2 += 2 + exceptLen * 8;
				UInt16 attrCount = ReadMUInt16(&ptr2[0]);
				ptr2 += 2;
				i = 0;
				while (i < attrCount)
				{
					UInt32 len = ReadMUInt32(&ptr2[2]);
					UInt16 nameIndex = ReadMUInt16(&ptr2[0]);
					sbuff[0] = 0;
					if (this->GetConstName(sbuff, nameIndex))
					{
						if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTable"))
						{
							lvTable = &ptr2[6];
							lvTableLen = len;
						}
					}
					ptr2 += 6 + len;
					i++;
				}

			}
		}
		ptr += len + 6;

		i++;
	}

	ptr = this->methods[index];
	if (accessFlags & 1)
	{
		sb->Append((const UTF8Char*)"public ");
	}
	if (accessFlags & 2)
	{
		sb->Append((const UTF8Char*)"private ");
	}
	if (accessFlags & 4)
	{
		sb->Append((const UTF8Char*)"protected ");
	}
	if (accessFlags & 8)
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (accessFlags & 0x10)
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (accessFlags & 0x20)
	{
		sb->Append((const UTF8Char*)"synchronized ");
	}
	if (accessFlags & 0x100)
	{
		sb->Append((const UTF8Char*)"native ");
	}
	if (accessFlags & 0x400)
	{
		sb->Append((const UTF8Char*)"abstract ");
	}
	this->DetailNameType(ReadMUInt16(&ptr[2]), ReadMUInt16(&ptr[4]), thisClass, 0, sb, 0, lvTable, lvTableLen, (accessFlags & 8) != 0);
	return true;
}

Bool IO::JavaClass::MethodsGetDetail(OSInt index, OSInt lev, Bool disasm, Text::StringBuilderUTF *sb)
{
	if (index < 0 || index >= this->methodCnt)
	{
		return false;
	}
	Text::StringBuilderUTF8 sbTmp;
	UTF8Char sbuff[256];
	UInt8 *ptr = this->methods[index];
	UInt16 accessFlags = ReadMUInt16(ptr);
	UInt16 attrCnt = ReadMUInt16(&ptr[6]);
	UInt16 i;
	UTF8Char typeBuff[256];
	UInt8 *lvTable = 0;
	UOSInt lvTableLen = 0;

	ptr = &this->methods[index][8];
	i = 0;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		UInt16 j;
		sbuff[0] = 0;
		if (this->GetConstName(sbuff, nameIndex))
		{
			if (Text::StrEquals(sbuff, (const UTF8Char*)"RuntimeVisibleAnnotations"))
			{
				UInt16 num_annotations = ReadMUInt16(&ptr[6]);
				const UInt8 *annptr = &ptr[8];
				j = 0;

				while (j < num_annotations)
				{
					sb->AppendChar(' ', lev  << 1);
					annptr = this->DetailAnnotation(annptr, &ptr[6 + len], sb);
					sb->Append((const UTF8Char*)"\r\n");

					j++;
				}
			}
			else if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
			{
				UInt32 codeLen = ReadMUInt32(&ptr[10]);
				UInt8 *ptr2;
				ptr2 = &ptr[14 + codeLen];
				UInt16 exceptLen = ReadMUInt16(&ptr2[0]);
				ptr2 += 2 + exceptLen * 8;
				UInt16 attrCount = ReadMUInt16(&ptr2[0]);
				ptr2 += 2;
				i = 0;
				while (i < attrCount)
				{
					UInt32 len = ReadMUInt32(&ptr2[2]);
					UInt16 nameIndex = ReadMUInt16(&ptr2[0]);
					sbuff[0] = 0;
					if (this->GetConstName(sbuff, nameIndex))
					{
						if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTable"))
						{
							lvTable = &ptr2[6];
							lvTableLen = len;
						}
					}
					ptr2 += 6 + len;
					i++;
				}

			}
		}
		ptr += len + 6;

		i++;
	}

	ptr = this->methods[index];
	sb->AppendChar(' ', lev << 2);
	if (accessFlags & 1)
	{
		sb->Append((const UTF8Char*)"public ");
	}
	if (accessFlags & 2)
	{
		sb->Append((const UTF8Char*)"private ");
	}
	if (accessFlags & 4)
	{
		sb->Append((const UTF8Char*)"protected ");
	}
	if (accessFlags & 8)
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (accessFlags & 0x10)
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (accessFlags & 0x20)
	{
		sb->Append((const UTF8Char*)"synchronized ");
	}
	if (accessFlags & 0x100)
	{
		sb->Append((const UTF8Char*)"native ");
	}
	if (accessFlags & 0x400)
	{
		sb->Append((const UTF8Char*)"abstract ");
	}
	this->DetailNameType(ReadMUInt16(&ptr[2]), ReadMUInt16(&ptr[4]), this->thisClass, 0, sb, typeBuff, lvTable, lvTableLen, (accessFlags & 8) != 0);
	ptr = &this->methods[index][8];
	i = 0;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		UInt16 j;
		sbuff[0] = 0;
		if (this->GetConstName(sbuff, nameIndex))
		{
			if (Text::StrEquals(sbuff, (const UTF8Char*)"Exceptions"))
			{
				UInt16 nExcept = ReadMUInt16(&ptr[6]);
				j = 0;
				while (j < nExcept)
				{
					if (j == 0)
					{
						sb->Append((const UTF8Char*)" throws ");
					}
					else
					{
						sb->Append((const UTF8Char*)", ");
					}
					sbTmp.ClearStr();
					this->ClassNameString(ReadMUInt16(&ptr[8 + j * 2]), &sbTmp);
					if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
					{
						sb->Append(sbTmp.ToString() + 10);
					}
					else
					{
						sb->Append(sbTmp.ToString());
					}
					
					j++;
				}
			}
		}
		ptr += len + 6;

		i++;
	}

	sb->Append((const UTF8Char*)"\r\n");
	sb->AppendChar(' ', lev << 2);
	sb->Append((const UTF8Char*)"{\r\n");
	if (disasm)
	{
		sb->AppendChar(' ', (lev + 1) << 2);
		sb->Append((const UTF8Char*)"/*\r\n");

		ptr = &this->methods[index][8];
		i = 0;
		while (i < attrCnt)
		{
			UInt32 len = ReadMUInt32(&ptr[2]);
			UInt16 nameIndex = ReadMUInt16(&ptr[0]);
			sbuff[0] = 0;
			if (this->GetConstName(sbuff, nameIndex))
			{
				if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
				{
					UInt32 codeLen = ReadMUInt32(&ptr[10]);
					this->DetailCode(&ptr[14], codeLen, (lev + 1) << 1, sb);
				}
			}
			ptr += len + 6;

			i++;
		}
		sb->AppendChar(' ', (lev + 1) << 2);
		sb->Append((const UTF8Char*)"*/\r\n");

		ptr = &this->methods[index][8];
		i = 0;
		while (i < attrCnt)
		{
			UInt32 len = ReadMUInt32(&ptr[2]);
			UInt16 nameIndex = ReadMUInt16(&ptr[0]);
			sbuff[0] = 0;
			if (this->GetConstName(sbuff, nameIndex))
			{
				if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
				{
					this->Decompile(ptr, (accessFlags & 8) != 0, typeBuff, lvTable, lvTableLen, lev + 1, sb);
				}
			}
			ptr += len + 6;

			i++;
		}
	}
	sb->AppendChar(' ', lev << 2);
	sb->Append((const UTF8Char*)"}\r\n");
	return true;
}

void IO::JavaClass::Decompile(const UInt8 *codeAttr, Bool staticFunc, const UTF8Char *typeBuff, const UInt8 *lvTable, UOSInt lvTableLen, OSInt lev, Text::StringBuilderUTF *sb)
{
	IO::JavaClass::DecompileEnv env;
	UInt16 maxLocal = ReadMUInt16(&codeAttr[8]);
	UInt32 codeLen = ReadMUInt32(&codeAttr[10]);
	Data::ArrayList<const UTF8Char *> stackVal;
	Data::ArrayList<const UTF8Char *> stackTypes;
	UTF8Char sbuff[256];
	OSInt i;
	Text::StringBuilderUTF8 sbTmp;
	env.localTypes = MemAlloc(const UTF8Char *, maxLocal);
	env.lvTable = lvTable;
	env.lvTableLen = lvTableLen;
	env.stacks = &stackVal;
	env.stackTypes = &stackTypes;
	env.staticFunc = staticFunc;

	if (maxLocal > 0)
	{
		i = maxLocal;
		while (i-- > 0)
		{
			env.localTypes[i] = 0;
			sbuff[0] = 'v';
			Text::StrOSInt(&sbuff[1], i);
		}
	}

	if (staticFunc)
	{
		i = 0;
	}
	else
	{
		sbTmp.ClearStr();
		this->ClassNameString(this->thisClass, &sbTmp);
		env.localTypes[0] = Text::StrCopyNew(sbTmp.ToString());
		i = 1;
	}
	UInt8 c;
	if (typeBuff[0] == '(')
	{
		typeBuff++;
	}
	while (true)
	{
		c = *typeBuff;
		if (c == ')' || c == 0)
		{
			break;
		}
		sbTmp.ClearStr();
		typeBuff = Type2String(typeBuff, &sbTmp);
		env.localTypes[i] = Text::StrCopyNew(sbTmp.ToString());
		i++;
	}

	const UInt8 *codePtr = &codeAttr[14];
	env.codeStart = codePtr;
	DecompileCode(codePtr, codePtr + codeLen, &env, lev, sb);
	if (stackVal.GetCount() != stackTypes.GetCount())
	{
		i = stackVal.GetCount();
		while (i-- > 0)
		{
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"// Unprocessed stack: ");
			sb->Append(stackVal.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			Text::StrDelNew(stackVal.GetItem(i));
		}
		i = stackTypes.GetCount();
		while (i-- > 0)
		{
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"// Unprocessed stack type: ");
			sb->Append(stackTypes.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			Text::StrDelNew(stackTypes.GetItem(i));
		}
	}
	else
	{
		i = stackVal.GetCount();
		while (i-- > 0)
		{
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"// Unprocessed stack: ");
			sb->Append(stackTypes.GetItem(i));
			sb->Append((const UTF8Char*)" ");
			sb->Append(stackVal.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			Text::StrDelNew(stackTypes.GetItem(i));
			Text::StrDelNew(stackVal.GetItem(i));
		}
	}

	i = maxLocal;
	while (i-- > 0)
	{
		if (env.localTypes[i])
		{
			Text::StrDelNew(env.localTypes[i]);
		}
	}
	MemFree(env.localTypes);
}

IO::JavaClass::EndType IO::JavaClass::DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, IO::JavaClass::DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb)
{
	Text::StringBuilderUTF8 sbTmp;
	Text::StringBuilderUTF8 sbTmp2;
	UInt16 classIndex;
	UInt16 val;
	UTF8Char sbuff[512];
	UTF8Char typeBuff[128];
	while (codePtr < codeEnd)
	{
		if (env->stacks->GetCount() != env->stackTypes->GetCount())
		{
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"// stack and type count mismatch: ");
			sb->AppendOSInt(env->stacks->GetCount());
			sb->Append((const UTF8Char*)" vs ");
			sb->AppendOSInt(env->stackTypes->GetCount());
			sb->Append((const UTF8Char*)", next code = ");
			sb->AppendHex8(codePtr[0]);
			sb->Append((const UTF8Char*)"\r\n");
			return ET_ERROR;
		}
		switch (codePtr[0])
		{
		case 0x00: //nop
			codePtr++;
			break;
		case 0x01: //aconst_null
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"null"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"Object"));
			codePtr++;
			break;
		case 0x02: //iconst_m1
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"-1"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x03: //iconst_0
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x04: //iconst_1
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"1"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x05: //iconst_2
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"2"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x06: //iconst_3
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"3"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x07: //iconst_4
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"4"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x08: //iconst_5
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"5"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x09: //lconst_0
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x0A: //lconst_1
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"1"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x0B: //fconst_0
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0.0f"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0C: //fconst_1
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"1.0f"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0D: //fconst_2
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"2.0f"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0E: //dconst_0
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0.0"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x0F: //dconst_1
			env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"1.0"));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x10: //bipush
			sbTmp.ClearStr();
			sbTmp.AppendU16(codePtr[1]);
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr += 2;
			break;
		case 0x11: //sipush
			sbTmp.ClearStr();
			sbTmp.AppendI16(ReadMInt16(&codePtr[1]));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr += 3;
			break;
		case 0x12: //ldc
			this->DecompileLDC(codePtr[1], env);
			codePtr += 2;
			break;
		case 0x13: //ldc_w
			this->DecompileLDC(ReadMUInt16(&codePtr[1]), env);
			codePtr += 3;
			break;
		case 0x14: //ldc2_w
			this->DecompileLDC(ReadMUInt16(&codePtr[1]), env);
			codePtr += 3;
			break;
		case 0x15: //iload
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[codePtr[1]]));
			codePtr += 2;
			break;
		case 0x16: //lload
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[codePtr[1]]));
			codePtr += 2;
			break;
		case 0x17: //fload
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[codePtr[1]]));
			codePtr += 2;
			break;
		case 0x18: //dload
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[codePtr[1]]));
			codePtr += 2;
			break;
		case 0x19: //aload
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[codePtr[1]]));
			codePtr += 2;
			break;
		case 0x1A: //iload_0
			if (env->staticFunc)
			{
				this->GetLVName(sbuff, 0, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
				env->stacks->Add(Text::StrCopyNew(sbuff));
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"this"));
			}
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[0]));
			codePtr++;
			break;
		case 0x1B: //iload_1
			this->GetLVName(sbuff, 1, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[1]));
			codePtr++;
			break;
		case 0x1C: //iload_2
			this->GetLVName(sbuff, 2, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[2]));
			codePtr++;
			break;
		case 0x1D: //iload_3
			this->GetLVName(sbuff, 3, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[3]));
			codePtr++;
			break;
		case 0x1E: //lload_0
			if (env->staticFunc)
			{
				this->GetLVName(sbuff, 0, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
				env->stacks->Add(Text::StrCopyNew(sbuff));
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"this"));
			}
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[0]));
			codePtr++;
			break;
		case 0x1F: //lload_1
			this->GetLVName(sbuff, 1, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[1]));
			codePtr++;
			break;
		case 0x20: //lload_2
			this->GetLVName(sbuff, 2, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[2]));
			codePtr++;
			break;
		case 0x21: //lload_3
			this->GetLVName(sbuff, 3, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[3]));
			codePtr++;
			break;
		case 0x22: //fload_0
			if (env->staticFunc)
			{
				this->GetLVName(sbuff, 0, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
				env->stacks->Add(Text::StrCopyNew(sbuff));
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"this"));
			}
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[0]));
			codePtr++;
			break;
		case 0x23: //fload_1
			this->GetLVName(sbuff, 1, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[1]));
			codePtr++;
			break;
		case 0x24: //fload_2
			this->GetLVName(sbuff, 2, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[2]));
			codePtr++;
			break;
		case 0x25: //fload_3
			this->GetLVName(sbuff, 3, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[3]));
			codePtr++;
			break;
		case 0x26: //dload_0
			if (env->staticFunc)
			{
				this->GetLVName(sbuff, 0, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
				env->stacks->Add(Text::StrCopyNew(sbuff));
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"this"));
			}
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[0]));
			codePtr++;
			break;
		case 0x27: //dload_1
			this->GetLVName(sbuff, 1, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[1]));
			codePtr++;
			break;
		case 0x28: //dload_2
			this->GetLVName(sbuff, 2, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[2]));
			codePtr++;
			break;
		case 0x29: //dload_3
			this->GetLVName(sbuff, 3, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[3]));
			codePtr++;
			break;
		case 0x2A: //aload_0
			if (env->staticFunc)
			{
				this->GetLVName(sbuff, 0, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
				env->stacks->Add(Text::StrCopyNew(sbuff));
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"this"));
			}
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[0]));
			codePtr++;
			break;
		case 0x2B: //aload_1
			this->GetLVName(sbuff, 1, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[1]));
			codePtr++;
			break;
		case 0x2C: //aload_2
			this->GetLVName(sbuff, 2, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[2]));
			codePtr++;
			break;
		case 0x2D: //aload_3
			this->GetLVName(sbuff, 3, env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			env->stacks->Add(Text::StrCopyNew(sbuff));
			env->stackTypes->Add(Text::StrCopyNew(env->localTypes[3]));
			codePtr++;
			break;
		case 0x2E: //iaload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// iaload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x2F: //laload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// laload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x30: //faload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// faload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x31: //daload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// daload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x32: //aaload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// aaload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
			if (!sbTmp2.EndsWith((const UTF8Char*)"[]"))
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// aaload stack not array type: ");
				sb->Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp2.RemoveChars(2);
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew(sbTmp2.ToString()));
			codePtr++;
			break;
		case 0x33: //baload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// baload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"byte"));
			codePtr++;
			break;
		case 0x34: //caload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// caload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"char"));
			codePtr++;
			break;
		case 0x35: //saload
			if (env->stacks->GetCount() < 2)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// saload stack invalid: count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"short"));
			codePtr++;
			break;
		case 0x36: //istore
			this->DecompileStore(codePtr[1], env, lev, sb, codePtr - env->codeStart + 2);
			codePtr += 2;
			break;
		case 0x37: //lstore
			this->DecompileStore(codePtr[1], env, lev, sb, codePtr - env->codeStart + 2);
			codePtr += 2;
			break;
		case 0x38: //fstore
			this->DecompileStore(codePtr[1], env, lev, sb, codePtr - env->codeStart + 2);
			codePtr += 2;
			break;
		case 0x39: //dstore
			this->DecompileStore(codePtr[1], env, lev, sb, codePtr - env->codeStart + 2);
			codePtr += 2;
			break;
		case 0x3A: //astore
			this->DecompileStore(codePtr[1], env, lev, sb, codePtr - env->codeStart + 2);
			codePtr += 2;
			break;
		case 0x3B: //istore_0
			this->DecompileStore(0, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x3C: //istore_1
			this->DecompileStore(1, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x3D: //istore_2
			this->DecompileStore(2, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x3E: //istore_3
			this->DecompileStore(3, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x3F: //lstore_0
			this->DecompileStore(0, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x40: //lstore_1
			this->DecompileStore(1, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x41: //lstore_2
			this->DecompileStore(2, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x42: //lstore_3
			this->DecompileStore(3, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x43: //fstore_0
			this->DecompileStore(0, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x44: //fstore_1
			this->DecompileStore(1, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x45: //fstore_2
			this->DecompileStore(2, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x46: //fstore_3
			this->DecompileStore(3, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x47: //dstore_0
			this->DecompileStore(0, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x48: //dstore_1
			this->DecompileStore(1, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x49: //dstore_2
			this->DecompileStore(2, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x4A: //dstore_3
			this->DecompileStore(3, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x4B: //astore_0
			this->DecompileStore(0, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x4C: //astore_1
			this->DecompileStore(1, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x4D: //astore_2
			this->DecompileStore(2, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x4E: //astore_3
			this->DecompileStore(3, env, lev, sb, codePtr - env->codeStart + 1);
			codePtr++;
			break;
		case 0x53: //aastore
			if (env->stacks->GetCount() < 3)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// pop stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				const UTF8Char *nameStr = env->stacks->GetItem(env->stacks->GetCount() - 3);
				const UTF8Char *indexStr = env->stacks->GetItem(env->stacks->GetCount() - 2);
				const UTF8Char *valueStr = env->stacks->GetItem(env->stacks->GetCount() - 1);
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				env->stacks->RemoveAt(env->stacks->GetCount() - 1);
				env->stacks->RemoveAt(env->stacks->GetCount() - 1);
				env->stacks->RemoveAt(env->stacks->GetCount() - 1);

				if (Text::StrStartsWith(nameStr, (const UTF8Char*)"new ") && env->stacks->GetCount() > 0 && Text::StrStartsWith(env->stacks->GetItem(env->stacks->GetCount()- 1), nameStr))
				{
					const UTF8Char *arrStr = env->stacks->RemoveAt(env->stacks->GetCount()- 1);
					sbTmp.ClearStr();
					sbTmp.Append(arrStr);
					if (sbTmp.ToString()[sbTmp.GetLength() - 1] == '}')
					{
						sbTmp.RemoveChars(1);
						sbTmp.Append((const UTF8Char*)", ");
						sbTmp.Append(valueStr);
						sbTmp.AppendChar('}', 1);
					}
					else
					{
						sbTmp.Append((const UTF8Char*)" {");
						sbTmp.Append(valueStr);
						sbTmp.AppendChar('}', 1);
					}
					env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
					Text::StrDelNew(arrStr);
				}
				else
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append(nameStr);
					sb->AppendChar('[', 1);
					sb->Append(indexStr);
					sb->Append((const UTF8Char*)"] = ");
					sb->Append(valueStr);
					sb->Append((const UTF8Char*)";\r\n");
				}
				Text::StrDelNew(nameStr);
				Text::StrDelNew(indexStr);
				Text::StrDelNew(valueStr);
			}
			
			codePtr++;
			break;
		case 0x57: //pop
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// pop stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			codePtr++;
			break;
		case 0x59: //dup
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// dup stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			env->stacks->Add(Text::StrCopyNew(env->stacks->GetItem(env->stacks->GetCount() - 1)));
			env->stackTypes->Add(Text::StrCopyNew(env->stackTypes->GetItem(env->stackTypes->GetCount() - 1)));
			codePtr++;
			break;
		case 0x5A: //dup_x1
			{
				if (env->stacks->GetCount() <= 1)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// dup_x1 stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				UOSInt cnt = env->stacks->GetCount();
				env->stacks->Insert(cnt - 2, Text::StrCopyNew(env->stacks->GetItem(cnt - 1)));
				env->stackTypes->Insert(cnt - 2, Text::StrCopyNew(env->stackTypes->GetItem(cnt - 1)));
				codePtr++;
			}
			break;
		case 0x60: //iadd
			{
				if (env->stacks->GetCount() <= 1)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// iadd stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				const UTF8Char *csptr = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(csptr);
				Text::StrDelNew(csptr);
				sbTmp.Append((const UTF8Char*)" + ");
				csptr = env->stacks->RemoveAt(env->stacks->GetCount() - 1);
				sbTmp.Append(csptr);
				Text::StrDelNew(csptr);
				env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				codePtr++;
			}
			break;
		case 0x70: //irem
			{
				if (env->stacks->GetCount() <= 1)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// irem stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				const UTF8Char *csptr = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(csptr);
				Text::StrDelNew(csptr);
				sbTmp.Append((const UTF8Char*)" % ");
				csptr = env->stacks->RemoveAt(env->stacks->GetCount() - 1);
				sbTmp.Append(csptr);
				Text::StrDelNew(csptr);
				env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				codePtr++;
			}
			break;
		case 0x84: //iinc
			sb->AppendChar(' ', lev << 2);
			this->GetLVName(sbuff, codePtr[1], env->lvTable, env->lvTableLen, codePtr - env->codeStart);
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)" += ");
			sb->AppendI16((Int16)codePtr[2]);
			sb->Append((const UTF8Char*)";\r\n");
			codePtr += 3;
			break;
		case 0x94: //lcmp
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// lcmp stack invalid: ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				CondType ct;
				switch (codePtr[1])
				{
				case 0x99: //ifeq
					ct = CT_EQ;
					break;
				case 0x9A: //ifne
					ct = CT_NE;
					break;
				case 0x9B: //iflt
					ct = CT_LT;
					break;
				case 0x9C: //ifge
					ct = CT_GE;
					break;
				case 0x9D: //ifgt
					ct = CT_GT;
					break;
				case 0x9E: //ifle
					ct = CT_LE;
					break;
				default:
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// lcmp next opcode invalid: ");
					sb->AppendHex8(codePtr[1]);
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				EndType et = this->DecompileCondBranch(codePtr + 4, codePtr + ReadMInt16(&codePtr[2]) + 1, ct, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x99: //ifeq
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifeq stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_EQ, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9A: //ifne
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifne stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_NE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9B: //iflt
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// iflt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_LT, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9C: //ifge
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_GE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9D: //ifgt
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifgt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_GT, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9E: //ifle
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifle stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_LE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9F: //if_icmpeq
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_EQ, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA0: //if_icmpne
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_NE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA1: //if_icmplt
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_LT, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA2: //if_icmpge
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_GE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA3: //if_icmpgt
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmpgt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_GT, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA4: //if_icmple
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_icmple stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_LE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA5: //if_acmpeq
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_acmpeq stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_EQ, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA6: //if_acmpne
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// if_acmpne stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_NE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA7: //goto
			{
				Int16 i = ReadMInt16(&codePtr[1]);
				if (i < 0)
				{
					env->endPtr = codePtr + i;
					return ET_GOTO;
				}
				if (codePtr + i >= codeEnd)
				{
					env->endPtr = codePtr + i;
					return ET_GOTO;
				}
				sbTmp.ClearStr();
				UOSInt stackCnt = env->stacks->GetCount();
				EndType et = ET_CODEEND;//this->DecompileCode(codePtr + 3, codePtr + i, env, lev + 1, &sbTmp);
				if (et == ET_ERROR)
				{
					sb->Append(sbTmp.ToString());
					return et;
				}
				if (stackCnt == env->stacks->GetCount() && (et == ET_CODEEND || et == ET_THROW || et == ET_RETURN))
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"if (false)\r\n");
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"{\r\n");
					sb->Append(sbTmp.ToString());
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"}\r\n");
					codePtr = codePtr + i;
				}
				else
				{
					env->endPtr = codePtr + i;
					return ET_GOTO;
				}
			}
			break;
		case 0xAC: //ireturn
			codePtr++;
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ireturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return ET_RETURN;
		case 0xAD: //lreturn
			codePtr++;
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// lreturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return ET_RETURN;
		case 0xAE: //freturn
			codePtr++;
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// freturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return ET_RETURN;
		case 0xB0: //areturn
			codePtr++;
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// areturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return ET_RETURN;
		case 0xB1: //return
			codePtr++;
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"return;\r\n");
			env->endPtr = codePtr;
			return ET_RETURN;
		case 0xB2: //getstatic
			val = ReadMUInt16(&codePtr[1]);
			if (val == 0 || val >= this->constPoolCnt)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// getstatic index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// getstatic const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				UInt16 classIndex = ReadMUInt16(&this->constPool[val][1]);
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				sbTmp.ClearStr();
				this->ClassNameString(classIndex, &sbTmp);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// getstatic const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sbTmp2.ClearStr();
				if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
				{
					sbTmp2.Append(sbTmp.ToString() + 10);
				}
				else
				{
					sbTmp2.Append(sbTmp.ToString());
				}
				sbTmp2.AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sbTmp2.Append(sbuff);
				this->GetConstName(sbuff, nameTypeIndex);
				sbTmp.ClearStr();
				Type2String(sbuff, &sbTmp);
				env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
				env->stackTypes->Add(Text::StrCopyNew(sbTmp.ToString()));
			}
			codePtr += 3;
			break;
		case 0xB3: //putstatic
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putstatic stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putstatic index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putstatic const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				UInt16 classIndex = ReadMUInt16(&this->constPool[val][1]);
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				sbTmp.ClearStr();
				this->ClassNameString(classIndex, &sbTmp);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// putstatic const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sb->AppendChar(' ', lev << 2);
				if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
				{
					sb->Append(sbTmp.ToString() + 10);
				}
				else
				{
					sb->Append(sbTmp.ToString());
				}
				sb->AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)" = ");
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sb->Append((const UTF8Char*)";\r\n");
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			}
			codePtr += 3;
			break;
		case 0xB4: //getfield
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// getfield stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// getfield index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// getfield const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				UInt16 classIndex;
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// getfield const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sbTmp2.Append(sbuff);
				this->GetConstName(sbuff, nameTypeIndex);
				sbTmp.ClearStr();
				Type2String(sbuff, &sbTmp);
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
				env->stackTypes->Add(Text::StrCopyNew(sbTmp.ToString()));
			}
			codePtr += 3;
			break;
		case 0xB5: //putfield
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 1)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putfield stack invalid: ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putfield index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// putfield const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				UInt16 classIndex;
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append((const UTF8Char*)"// putfield const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return ET_ERROR;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sb->AppendChar(' ', lev << 2);
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
				sb->AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)" = ");
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sb->Append((const UTF8Char*)";\r\n");
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			}
			codePtr += 3;
			break;
		case 0xB6: //invokevirtual
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append(sbTmp.ToString());
				return ET_ERROR;
			}
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// invokevirtual Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendChar('.', 1);
				sbTmp2.Append(sbuff);
				sbTmp2.Append(sbTmp.ToString());
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				if (typeBuff[0])
				{
					env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
					env->stackTypes->Add(Text::StrCopyNew(typeBuff));
				}
				else
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";\r\n");
				}
			}
			codePtr += 3;
			break;
		case 0xB7: //invokespecial
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append(sbTmp.ToString());
				return ET_ERROR;
			}
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// invokespecial Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				Bool isInit = false;
				sbTmp2.ClearStr();
				if (Text::StrEquals(sbuff, (const UTF8Char*)"<init>"))
				{
					isInit = true;
					if (Text::StrEquals(env->stacks->GetItem(env->stacks->GetCount() - 1), (const UTF8Char*)"this"))
					{
						if (classIndex == this->thisClass)
						{
							sbTmp2.Append((const UTF8Char*)"this");
						}
						else
						{
							sbTmp2.Append((const UTF8Char*)"super");
						}
					}
					else
					{
						sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
					}
				}
				else
				{
					if (Text::StrEquals(env->stacks->GetItem(env->stacks->GetCount() - 1), (const UTF8Char*)"this"))
					{
						if (classIndex == this->thisClass)
						{
							sbTmp2.Append((const UTF8Char*)"this.");
						}
						else
						{
							sbTmp2.Append((const UTF8Char*)"super.");
						}
					}
					else
					{
						sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
						sbTmp2.AppendChar('.', 1);
					}
					sbTmp2.Append(sbuff);
				}
				sbTmp2.Append(sbTmp.ToString());
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				if (typeBuff[0])
				{
					env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
					env->stackTypes->Add(Text::StrCopyNew(typeBuff));
				}
				else if (isInit && env->stacks->GetCount() > 0 && sbTmp2.StartsWith(env->stacks->GetItem(env->stacks->GetCount() - 1)))
				{
					Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
					env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
				}
				else
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";\r\n");
				}
			}
			codePtr += 3;
			break;
		case 0xB8: //invokestatic
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append(sbTmp.ToString());
				return ET_ERROR;
			}
			sbTmp2.ClearStr();
			this->ClassNameString(classIndex, &sbTmp2);
			sbTmp2.AppendChar('.', 1);
			sbTmp2.Append(sbuff);
			sbTmp2.Append(sbTmp.ToString());
			if (typeBuff[0])
			{
				env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
				env->stackTypes->Add(Text::StrCopyNew(typeBuff));
			}
			else
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append(sbTmp2.ToString());
				sb->Append((const UTF8Char*)";\r\n");
			}
			codePtr += 3;
			break;
		case 0xB9: //invokeinterface
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append(sbTmp.ToString());
				return ET_ERROR;
			}
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// invokeinterface Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendChar('.', 1);
				sbTmp2.Append(sbuff);
				sbTmp2.Append(sbTmp.ToString());
				Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
				Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
				if (typeBuff[0])
				{
					env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
					env->stackTypes->Add(Text::StrCopyNew(typeBuff));
				}
				else
				{
					sb->AppendChar(' ', lev << 2);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";\r\n");
				}
			}
			codePtr += 5;
			break;
		case 0xBB: //new
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.Append((const UTF8Char*)"new ");
			if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
			{
				sbTmp2.Append(sbTmp.ToString() + 10);
			}
			else
			{
				sbTmp2.Append(sbTmp.ToString());
			}
			env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
			env->stackTypes->Add(Text::StrCopyNew(sbTmp.ToString()));
			codePtr += 3;
			break;
		case 0xBC: //newarray
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// newarray stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp2.ClearStr();
			sbTmp.ClearStr();
			sbTmp.Append((const UTF8Char*)"new ");
			switch (codePtr[1])
			{
			case 4: //T_BOOLEAN
				sbTmp.Append((const UTF8Char*)"boolean[");
				sbTmp2.Append((const UTF8Char*)"boolean[]");
				break;
			case 5: //T_CHAR
				sbTmp.Append((const UTF8Char*)"char[");
				sbTmp2.Append((const UTF8Char*)"char[]");
				break;
			case 6: //T_FLOAT
				sbTmp.Append((const UTF8Char*)"float[");
				sbTmp2.Append((const UTF8Char*)"float[]");
				break;
			case 7: //T_DOUBLE
				sbTmp.Append((const UTF8Char*)"double[");
				sbTmp2.Append((const UTF8Char*)"double[]");
				break;
			case 8: //T_BYTE
				sbTmp.Append((const UTF8Char*)"byte[");
				sbTmp2.Append((const UTF8Char*)"byte[]");
				break;
			case 9: //T_SHORT
				sbTmp.Append((const UTF8Char*)"short[");
				sbTmp2.Append((const UTF8Char*)"short[]");
				break;
			case 10: //T_INT
				sbTmp.Append((const UTF8Char*)"int[");
				sbTmp2.Append((const UTF8Char*)"int[]");
				break;
			case 11: //T_LONG
				sbTmp.Append((const UTF8Char*)"long[");
				sbTmp2.Append((const UTF8Char*)"long[]");
				break;
			default:
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// newarray invalid type: ");
				sb->AppendU16(codePtr[1]);
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			env->stackTypes->Add(Text::StrCopyNew(sbTmp2.ToString()));
			codePtr += 2;
			break;
		case 0xBD: //anewarray
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// anewarray stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.Append((const UTF8Char*)"new ");
			if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
			{
				sbTmp2.Append(sbTmp.ToString() + 10);
			}
			else
			{
				sbTmp2.Append(sbTmp.ToString());
			}
			sbTmp.Append((const UTF8Char*)"[]");
			sbTmp2.AppendChar('[', 1);
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.AppendChar(']', 1);
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
			env->stackTypes->Add(Text::StrCopyNew(sbTmp.ToString()));
			codePtr += 3;
			break;
		case 0xBE: //arraylength
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// arraylength Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.Append((const UTF8Char*)".length");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0xBF: //athrow
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// athrow Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"throw ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr + 1;
			return ET_THROW;
		case 0xC0: //checkcast
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// checkcast Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.AppendChar('(', 1);
			if (sbTmp.StartsWith((const UTF8Char*)"java.lang."))
			{
				sbTmp2.Append(sbTmp.ToString() + 10);
			}
			else
			{
				sbTmp2.Append(sbTmp.ToString());
			}
			sbTmp2.AppendChar(')', 1);
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->stacks->Add(Text::StrCopyNew(sbTmp2.ToString()));
			env->stackTypes->Add(Text::StrCopyNew(sbTmp.ToString()));
			codePtr += 3;
			break;
		case 0xC2: //monitorenter
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// monitorenter Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"synchronized (");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)")\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"{\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			lev++;
			codePtr++;
			break;
		case 0xC3: //monitorexit
			if (env->stacks->GetCount() == 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// monitorexit Stack error: stack count = ");
				sb->AppendOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			lev--;
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"}\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			codePtr++;
			break;
		case 0xC6: //ifnull
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifnull stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"null"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"java.lang.Object"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_EQ, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xC7: //ifnonnull
			if (env->stacks->GetCount() <= 0)
			{
				sb->AppendChar(' ', lev << 2);
				sb->Append((const UTF8Char*)"// ifnonnull stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return ET_ERROR;
			}
			else
			{
				env->stacks->Add(Text::StrCopyNew((const UTF8Char*)"null"));
				env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"java.lang.Object"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CT_NE, env, lev, sb);
				if (et == ET_ERROR)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xC8: //goto_w
			env->endPtr = codePtr + ReadMInt32(&codePtr[1]);
			return ET_GOTO;
		default:
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"// Unsupported opcode ");
			sb->AppendHex8(codePtr[0]);
			sb->Append((const UTF8Char*)"\r\n");
			return ET_ERROR;
		}
	}
	env->endPtr = codePtr;
	return ET_CODEEND;
}

void IO::JavaClass::DecompileLDC(UInt16 index, IO::JavaClass::DecompileEnv *env)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return;
	}
	UInt8 *ptr = this->constPool[index];
	Text::StringBuilderUTF8 sb;
	if (ptr[0] == 3)
	{
		sb.AppendI32(ReadMUInt32(&ptr[1]));
		env->stacks->Add(Text::StrCopyNew(sb.ToString()));
		env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"int"));
	}
	else if (ptr[0] == 4)
	{
		Text::SBAppendF32(&sb, ReadMFloat(&ptr[1]));
		env->stacks->Add(Text::StrCopyNew(sb.ToString()));
		env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"float"));
	}
	else if (ptr[0] == 5)
	{
		sb.AppendI64(ReadMUInt64(&ptr[1]));
		env->stacks->Add(Text::StrCopyNew(sb.ToString()));
		env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"long"));
	}
	else if (ptr[0] == 6)
	{
		Text::SBAppendF64(&sb, ReadMDouble(&ptr[1]));
		env->stacks->Add(Text::StrCopyNew(sb.ToString()));
		env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"double"));
	}
	else if (ptr[0] == 7) //Class
	{
		UInt16 sindex = ReadMUInt16(&ptr[1]);
		UInt16 strLen;
		ptr = this->constPool[sindex];
		if (ptr[0] == 1)
		{
			strLen = ReadMUInt16(&ptr[1]);
			sb.AppendC(ptr + 3, strLen);
			sb.Replace('/', '.');
			sb.Append((const UTF8Char*)".class");
			env->stacks->Add(Text::StrCopyNew(sb.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"java.lang.Class"));
		}
	}
	else if (ptr[0] == 8) //String
	{
		UInt16 sindex = ReadMUInt16(&ptr[1]);
		UInt16 strLen;
		ptr = this->constPool[sindex];
		if (ptr[0] == 1)
		{
			strLen = ReadMUInt16(&ptr[1]);
			sb.AppendC(ptr + 3, strLen);
			env->stacks->Add(Text::JSText::ToNewJSTextDQuote(sb.ToString()));
			env->stackTypes->Add(Text::StrCopyNew((const UTF8Char*)"java.lang.String"));
		}
	}
}

void IO::JavaClass::DecompileStore(UInt16 index, IO::JavaClass::DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb, UOSInt codeOfst)
{
	UTF8Char sbuff[256];
	sb->AppendChar(' ', lev << 2);
	if (env->stacks->GetCount() <= 0)
	{
		sb->Append((const UTF8Char*)"// Store stack is empty\r\n");
		return;
	}
	if (env->localTypes[index] == 0)
	{
		env->localTypes[index] = Text::StrCopyNew(env->stackTypes->GetItem(env->stackTypes->GetCount() - 1));
		sb->Append(env->localTypes[index]);
		sb->AppendChar(' ', 1);
	}
	this->GetLVName(sbuff, index, env->lvTable, env->lvTableLen, codeOfst);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" = ");
	sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
	sb->Append((const UTF8Char*)";\r\n");
	Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
	Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
}

IO::JavaClass::EndType IO::JavaClass::DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, IO::JavaClass::DecompileEnv *env, OSInt lev, Text::StringBuilderUTF *sb)
{
	if (codePtr > codeEnd)
	{
		Text::StringBuilderUTF8 sbTmp;
		UOSInt initStackCnt = env->stacks->GetCount();
		EndType et = DecompileCode(codeEnd, codePtr - 3, env, lev + 1, &sbTmp);
		if (et == ET_ERROR)
		{
			sb->Append(sbTmp.ToString());
			return et;
		}
		if (initStackCnt + 1 == env->stacks->GetCount() && (et == ET_RETURN || et == ET_THROW || et == ET_CODEEND))
		{
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"while (");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sb->AppendChar(' ', 1);
			sb->Append(CondType2String(ct));
			sb->AppendChar(' ', 1);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)")\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"}\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return et;
		}
		else if (initStackCnt + 2 == env->stacks->GetCount() && (et == ET_CODEEND))
		{
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"while (");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sb->AppendChar(' ', 1);
			sb->Append(CondType2String(ct));
			sb->AppendChar(' ', 1);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)")\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"}\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
			env->endPtr = codePtr;
			return et;
		}
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"// CondBranch: unknown type2: Stacks = ");
		sb->AppendOSInt(initStackCnt);
		sb->Append((const UTF8Char*)", ");
		sb->AppendOSInt(env->stacks->GetCount());
		sb->Append((const UTF8Char*)", et = ");
		sb->AppendI32(et);
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append(sbTmp.ToString());
		return ET_ERROR;
	}
	else if (codePtr == codeEnd)
	{
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"if (");
		sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
		sb->AppendChar(' ', 1);
		sb->Append(CondType2IString(ct));
		sb->AppendChar(' ', 1);
		sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
		sb->Append((const UTF8Char*)")\r\n");
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"{\r\n");
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"}\r\n");
		Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
		Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
		Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
		Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
		env->endPtr = codePtr;
		return ET_CODEEND;
	}
	Text::StringBuilderUTF8 sbTmp;
	UOSInt initStackCnt = env->stacks->GetCount();
	EndType et = DecompileCode(codePtr, codeEnd, env, lev + 1, &sbTmp);
	if (et == ET_ERROR)
	{
		sb->Append(sbTmp.ToString());
		return et;
	}
	if (initStackCnt + 1 == env->stacks->GetCount() && et == ET_GOTO && sbTmp.GetLength() == 0)
	{
		et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, &sbTmp);
		if (et == ET_ERROR)
		{
			sb->Append(sbTmp.ToString());
			return et;
		}
		else if (et == ET_CODEEND && initStackCnt + 2 == env->stacks->GetCount() && sbTmp.GetLength() == 0)
		{
			sbTmp.ClearStr();
			sbTmp.AppendChar('(', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt - 2));
			sbTmp.AppendChar(' ', 1);
			sbTmp.Append(CondType2IString(ct));
			sbTmp.AppendChar(' ', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt - 1));
			sbTmp.AppendChar(')', 1);
			sbTmp.AppendChar('?', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt + 0));
			sbTmp.AppendChar(':', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt + 1));
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt + 1));
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt + 0));
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt - 1));
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt - 2));
			Text::StrDelNew(env->stackTypes->RemoveAt(initStackCnt + 0));
			Text::StrDelNew(env->stackTypes->RemoveAt(initStackCnt - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(initStackCnt - 2));
			env->stacks->Add(Text::StrCopyNew(sbTmp.ToString()));
			return ET_CODEEND;
		}
	}
	else if (initStackCnt == env->stacks->GetCount() && et == ET_GOTO)
	{
		Text::StringBuilderUTF8 sbTmp2;
		et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, &sbTmp2);
		if (et == ET_ERROR)
		{
			sb->Append(sbTmp2.ToString());
			return et;
		}
		else if (et == ET_CODEEND && initStackCnt == env->stacks->GetCount())
		{
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"if (");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sb->AppendChar(' ', 1);
			sb->Append(CondType2IString(ct));
			sb->AppendChar(' ', 1);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)")\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"}\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"else\r\n");
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp2.ToString());
			sb->AppendChar(' ', lev << 2);
			sb->Append((const UTF8Char*)"}\r\n");
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt - 1));
			Text::StrDelNew(env->stacks->RemoveAt(initStackCnt - 2));
			Text::StrDelNew(env->stackTypes->RemoveAt(initStackCnt - 1));
			Text::StrDelNew(env->stackTypes->RemoveAt(initStackCnt - 2));
			return ET_CODEEND;
		}
	}
	else if (initStackCnt == env->stacks->GetCount() && (et == ET_RETURN || et == ET_THROW || et == ET_CODEEND))
	{
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"if (");
		sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
		sb->AppendChar(' ', 1);
		sb->Append(CondType2IString(ct));
		sb->AppendChar(' ', 1);
		sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
		sb->Append((const UTF8Char*)")\r\n");
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"{\r\n");
		sb->Append(sbTmp.ToString());
		sb->AppendChar(' ', lev << 2);
		sb->Append((const UTF8Char*)"}\r\n");
		Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
		Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
		Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
		Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
		env->endPtr = codeEnd;
		return et;
	}

	sb->AppendChar(' ', lev << 2);
	sb->Append((const UTF8Char*)"// CondBranch: unknown type: Stacks = ");
	sb->AppendOSInt(initStackCnt);
	sb->Append((const UTF8Char*)", ");
	sb->AppendOSInt(env->stacks->GetCount());
	sb->Append((const UTF8Char*)", et = ");
	sb->AppendI32(et);
	sb->Append((const UTF8Char*)"\r\n");
	return ET_ERROR;
}

UTF8Char *IO::JavaClass::DecompileMethod(UInt16 methodIndex, UTF8Char *nameBuff, UInt16 *classIndex, UTF8Char *retType, IO::JavaClass::DecompileEnv *env, Text::StringBuilderUTF *sb)
{
	if (methodIndex == 0 || methodIndex >= this->constPoolCnt)
	{
		sb->Append((const UTF8Char*)"// method index out of range: ");
		sb->AppendU16(methodIndex);
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}
	const UInt8 *constPtr = this->constPool[methodIndex];
	if (constPtr == 0)
	{
		sb->Append((const UTF8Char*)"// method constPool is not used: ");
		sb->AppendU16(methodIndex);
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}
	else if (constPtr[0] == 10) //methodRef
	{
	}
	else if (constPtr[0] == 11) //interfaceMethodRef
	{
	}
	else
	{
		sb->Append((const UTF8Char*)"// method constPool is not supported: ");
		sb->AppendU16(methodIndex);
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}

	*classIndex = ReadMUInt16(&constPtr[1]);
	UInt16 nameTypeIndex = ReadMUInt16(&constPtr[3]);
	constPtr = this->constPool[nameTypeIndex];
	if (constPtr == 0 || constPtr[0] != 12)
	{
		sb->Append((const UTF8Char*)"// method constPool is not nameAndType: ");
		sb->AppendU16(constPtr[0]);
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}

	retType[0] = 0;
	UOSInt paramCnt = 0;
	UInt8 c;
	UTF8Char *sptr;
	UTF8Char params[32];
	this->GetConstName(nameBuff, ReadMUInt16(&constPtr[3]));
	sptr = nameBuff;
	if (sptr[0] != '(')
	{
		sb->Append((const UTF8Char*)"// method type is not valid: ");
		sb->AppendU16(ReadMUInt16(&constPtr[3]));
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}

	sptr++;
	while (true)
	{
		c = *sptr++;
		if (c == 0)
		{
			break;
		}
		else if (c == ')')
		{
			c = *sptr;
			if (c != 'V' && c != 0)
			{
				Text::StringBuilderUTF8 sbTmp;
				Type2String(sptr, &sbTmp);
				Text::StrConcat(retType, sbTmp.ToString());
			}
			break;
		}
		else if (c == 'L')
		{
			params[paramCnt] = c;
			paramCnt++;
			while (true)
			{
				c = *sptr++;
				if (c == 0)
				{
					sptr--;
					break;
				}
				else if (c == ';')
				{
					break;
				}
			}
		}
		else if (c == '[')
		{

		}
		else
		{
			params[paramCnt] = c;
			paramCnt++;
		}
	}
	
	if (paramCnt > env->stacks->GetCount())
	{
		sb->Append((const UTF8Char*)"// invokespecial Stack error: paramCnt = ");
		sb->AppendOSInt(paramCnt);
		sb->Append((const UTF8Char*)", stack count = ");
		sb->AppendOSInt(env->stacks->GetCount());
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}

	UOSInt i;
	const UTF8Char *paramStr;
	sb->AppendChar('(', 1);
	i = 0;
	while (i < paramCnt)
	{
		if (i > 0)
		{
			sb->Append((const UTF8Char*)", ");
		}
		paramStr = env->stacks->GetItem(env->stacks->GetCount() - paramCnt + i);
		if (params[i] == 'Z') //boolean
		{
			if (Text::StrEquals(paramStr, (const UTF8Char*)"1"))
			{
				sb->Append((const UTF8Char*)"true");
			}
			else if (Text::StrEquals(paramStr, (const UTF8Char*)"0"))
			{
				sb->Append((const UTF8Char*)"false");
			}
			else
			{
				sb->Append(paramStr);
			}
		}
		else
		{
			sb->Append(paramStr);
		}
		i++;
	}
	sb->AppendChar(')', 1);
	i = env->stacks->GetCount() - paramCnt;
	while (env->stacks->GetCount() > i)
	{
		Text::StrDelNew(env->stacks->RemoveAt(env->stacks->GetCount() - 1));
		Text::StrDelNew(env->stackTypes->RemoveAt(env->stackTypes->GetCount() - 1));
	}
	return this->GetConstName(nameBuff, ReadMUInt16(&constPtr[1]));
}

IO::JavaClass *IO::JavaClass::ParseFile(const UTF8Char *fileName)
{
	IO::FileStream *fs;
	IO::JavaClass *cls = 0;
	UInt64 fileLen;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	fileLen = fs->GetLength();
	if (fileLen >= 26 && fileLen <= 1048576)
	{
		UInt8 *buff = MemAlloc(UInt8, (UOSInt)fileLen);
		if (fs->Read(buff, (UOSInt)fileLen) == fileLen)
		{
			cls = ParseBuff(fileName, buff, (OSInt)fileLen);
		}
		MemFree(buff);
	}
	DEL_CLASS(fs);
	return cls;
}

IO::JavaClass *IO::JavaClass::ParseBuff(const UTF8Char *sourceName, const UInt8 *buff, OSInt buffSize)
{
	if (buffSize < 26)
	{
		return 0;
	}
	if (ReadMUInt32(&buff[0]) != 0xCAFEBABE)
	{
		return 0;
	}
	IO::JavaClass *cls;
	NEW_CLASS(cls, IO::JavaClass(sourceName, buff, buffSize));
	return cls;
}
