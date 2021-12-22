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
	return AppendCodeType2String(sb, typeStr, 0, 0);
}

const UInt8 *IO::JavaClass::CondType2String(CondType ct)
{
	switch (ct)
	{
	case CondType::EQ:
		return (const UTF8Char*)"==";
	case CondType::NE:
		return (const UTF8Char*)"!=";
	case CondType::LE:
		return (const UTF8Char*)"<=";
	case CondType::GE:
		return (const UTF8Char*)">=";
	case CondType::LT:
		return (const UTF8Char*)"<";
	case CondType::GT:
		return (const UTF8Char*)">";
	default:
		return (const UTF8Char*)"?";
	}
}

const UInt8 *IO::JavaClass::CondType2IString(CondType ct)
{
	switch (ct)
	{
	case CondType::EQ:
		return (const UTF8Char*)"!=";
	case CondType::NE:
		return (const UTF8Char*)"==";
	case CondType::LE:
		return (const UTF8Char*)">";
	case CondType::GE:
		return (const UTF8Char*)"<";
	case CondType::LT:
		return (const UTF8Char*)">=";
	case CondType::GT:
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

void IO::JavaClass::AppendCond(Text::StringBuilderUTF *sb, DecompileEnv *env, UOSInt index, CondType ct, Bool inv)
{
	if (env->stackTypes->GetItem(index)->Equals((const UTF8Char*)"boolean"))
	{
		Text::String *s = env->stacks->GetItem(index);
		if ((ct == CondType::NE && !inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"0")) ||
			(ct == CondType::EQ && inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"0")))
		{
			sb->Append(s);
			return;	
		}
		else if ((ct == CondType::EQ && !inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"0")) ||
				(ct == CondType::NE && inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"0")))
		{
			sb->AppendChar('!', 1);
			if (s->IndexOf((const UTF8Char *)" instanceof ") != INVALID_INDEX)
			{
				sb->AppendChar('(', 1);
				sb->Append(s);
				sb->AppendChar(')', 1);
			}
			else
			{
				sb->Append(s);
			}
			return;	
		}
		else if ((ct == CondType::EQ && inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"1")) ||
				(ct == CondType::NE && !inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"1")))
		{
			if (s->IndexOf((const UTF8Char *)" instanceof ") != INVALID_INDEX || s->ContainChars((const UTF8Char*)"><=|&^"))
			{
				sb->AppendChar('(', 1);
				sb->Append(s);
				sb->AppendChar(')', 1);
			}
			else
			{
				sb->Append(s);
			}
			sb->Append(env->stacks->GetItem(index));
			sb->Append((const UTF8Char*)" != true");
			return;	
		}
		else if ((ct == CondType::EQ && !inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"1")) ||
				(ct == CondType::NE && inv && env->stacks->GetItem(index + 1)->Equals((const UTF8Char*)"1")))
		{
			if (s->IndexOf((const UTF8Char *)" instanceof ") != INVALID_INDEX || s->ContainChars((const UTF8Char*)"><=|&^"))
			{
				sb->AppendChar('(', 1);
				sb->Append(s);
				sb->AppendChar(')', 1);
			}
			else
			{
				sb->Append(s);
			}
			sb->Append((const UTF8Char*)" == true");
			return;	
		}
	}
	sb->Append(env->stacks->GetItem(index));
	sb->AppendChar(' ', 1);
	if (inv)
	{
		sb->Append(CondType2IString(ct));
	}
	else
	{
		sb->Append(CondType2String(ct));
	}
	sb->AppendChar(' ', 1);
	sb->Append(env->stacks->GetItem(index + 1));
}

UInt32 IO::JavaClass::GetParamId(UInt32 paramIndex, const MethodInfo *method)
{
	if (method == 0)
	{
		return paramIndex;
	}
	UInt32 i = 0;
	UOSInt j = method->lvList->GetCount();
	while (i < j)
	{
		LocalVariableInfo *lv = method->lvList->GetItem(i);
		if (lv->startPC == 0 && lv->index == paramIndex)
		{
			return i;
		}
		i++;
	}
	return paramIndex;
}

void IO::JavaClass::AppendIndent(Text::StringBuilderUTF *sb, UOSInt lev)
{
	sb->AppendChar('\t', lev);
}

void IO::JavaClass::AppendLineNum(Text::StringBuilderUTF *sb, DecompileEnv *env, const UInt8 *codePtr)
{
	UInt16 codePC = (UInt16)(codePtr - env->codeStart);
	if (env->method == 0 || env->method->lineNumList->GetCount() == 0)
	{
		return;
	}
	LineNumberInfo *lineNum;
	UOSInt i = 0;
	UOSInt j = env->method->lineNumList->GetCount();
	UOSInt k = INVALID_INDEX;
	while (i < j)
	{
		lineNum = env->method->lineNumList->GetItem(i);
		if (lineNum->startPC > codePC)
		{
			break;
		}
		k = i;
		i++;;
	}
	if (k != INVALID_INDEX)
	{
		lineNum = env->method->lineNumList->GetItem(k);
		sb->Append((const UTF8Char*)" // LineNum ");
		sb->AppendU16(lineNum->lineNumber);
	}
}

const UInt8 *IO::JavaClass::DetailAttribute(const UInt8 *attr, UOSInt lev, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[256];
	UInt32 len = ReadMUInt32(&attr[2]);
	UInt16 nameIndex = ReadMUInt16(&attr[0]);
	this->AppendIndent(sb, lev);
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
	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"Attr Length = ");
	sb->AppendU32(len);
	sb->Append((const UTF8Char*)"\r\n");
	if (Text::StrEquals(sbuff, (const UTF8Char*)"ConstantValue"))
	{
		UInt16 i = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr ConstantValue index = ");
		sb->AppendU16(i);
		this->DetailConstVal(i, sb, true);
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
	{
		const UInt8 *ptr;
		UInt32 codeLen = ReadMUInt32(&attr[10]);
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr Max Stack = ");
		sb->AppendU16(ReadMUInt16(&attr[6]));
		sb->Append((const UTF8Char*)"\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr Max Locals = ");
		sb->AppendU16(ReadMUInt16(&attr[8]));
		sb->Append((const UTF8Char*)"\r\n");
		this->AppendIndent(sb, lev);
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
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" start_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->Append((const UTF8Char*)"\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" end_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->Append((const UTF8Char*)"\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr Ex ");
			sb->AppendU16(i);
			sb->Append((const UTF8Char*)" handler_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->Append((const UTF8Char*)"\r\n");
			this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr number_of_exceptions = ");
		sb->AppendU16(number_of_exceptions);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < number_of_exceptions)
		{
			UInt16 clsIndex = ReadMUInt16(&ptr[0]);
			this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr number_of_classes = ");
		sb->AppendU16(number_of_classes);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < number_of_classes)
		{
			UInt16 clsIndex;
			clsIndex = ReadMUInt16(&ptr[0]);
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr inner class info index = ");
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");

			clsIndex = ReadMUInt16(&ptr[2]);
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr outer class info index = ");
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->Append((const UTF8Char*)"\r\n");

			clsIndex = ReadMUInt16(&ptr[4]);
			this->AppendIndent(sb, lev);
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
			this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr class index = ");
		sb->AppendU16(clsIndex);
		this->DetailClassName(clsIndex, sb);
		sb->Append((const UTF8Char*)"\r\n");

		clsIndex = ReadMUInt16(&ptr[2]);
		this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr line_number_table_length = ");
		sb->AppendU16(line_number_table_length);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < line_number_table_length)
		{
			this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr local_variable_table_length = ");
		sb->AppendU16(local_variable_table_length);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < local_variable_table_length)
		{
			this->AppendIndent(sb, lev);
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

			if (this->GetConstName(sbuff, ReadMUInt16(&ptr[4])))
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)")");
			}
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 10;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTypeTable"))
	{
		UInt16 local_variable_type_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr local_variable_type_table_length = ");
		sb->AppendU16(local_variable_type_table_length);
		sb->Append((const UTF8Char*)"\r\n");
		while (i < local_variable_type_table_length)
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Attr start_pc = ");
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->Append((const UTF8Char*)", length = ");
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->Append((const UTF8Char*)", name_index = ");
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->Append((const UTF8Char*)", signature_index = ");
			sb->AppendU16(ReadMUInt16(&ptr[6]));
			sb->Append((const UTF8Char*)", index = ");
			sb->AppendU16(ReadMUInt16(&ptr[8]));

			if (this->GetConstName(sbuff, ReadMUInt16(&ptr[4])))
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)")");
			}
			sb->Append((const UTF8Char*)"\r\n");
			i++;
			ptr += 10;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"Signature"))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr Signature Index = ");
		sb->AppendU16(snameIndex);
		this->DetailName(snameIndex, sb, true);
		sb->Append((const UTF8Char*)"\r\n");
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"SourceFile"))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
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
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr RuntimeVisibleAnnotations num_annotations = ");
		sb->AppendU16(num_annotations);
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(&attr[6], len, ' ', Text::LineBreakType::CRLF);
		sb->Append((const UTF8Char*)"\r\n");

		while (i < num_annotations)
		{
			this->AppendIndent(sb, lev + 1);
			ptr = this->DetailAnnotation(ptr, &attr[6 + len], sb, 0, 0);
			sb->Append((const UTF8Char*)"\r\n");

			i++;
		}
	}
	else if (Text::StrEquals(sbuff, (const UTF8Char*)"StackMapTable"))
	{
		UInt16 number_of_entries = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Attr StackMapTable number_of_entries = ");
		sb->AppendU16(number_of_entries);
		sb->Append((const UTF8Char*)"\r\n");

		while (i < number_of_entries)
		{
			ptr = this->DetailStackMapFrame(ptr, &attr[6 + len], lev + 1, sb);

			i++;
		}
	}
	else
	{
		this->AppendIndent(sb, lev);
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
		sb->AppendU32(ReadMUInt32(&ptr[1]));
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
		sb->AppendU64(ReadMUInt64(&ptr[1]));
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
			Text::String *j = Text::JSText::ToNewJSTextDQuote(s);
			sb->Append(j);
			j->Release();
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
				sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
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
			sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
			sb->AppendChar('.', 1);
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
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
			sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
			sb->AppendChar('.', 1);
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	sb->AppendChar(',', 1);
	this->DetailNameAndType(nameIndex, classIndex, sb);
}

Bool IO::JavaClass::MethodGetReturnType(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return false;
	}
	if (this->constPool[index][0] != 1)
	{
		return false;
	}

	const UInt8 *ptr = this->constPool[index];
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	const UInt8 *strEnd;
	Text::StringBuilderUTF8 sbTmp;
	ptr += 3;
	strEnd = ptr + strLen;
	if (ptr[0] == '(')
	{
		ptr++;
		while (ptr < strEnd && ptr[0] != ')')
		{
			sbTmp.ClearStr();
			ptr = Type2String(ptr, &sbTmp);
		}
		ptr++;
	}
	else
	{
		return false;
	}
	Type2String(ptr, sb);
	return true;
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
	this->DetailNameType(nameIndex, typeIndex, classIndex, (const UTF8Char*)" ", sb, 0, 0, 0, 0);
}

void IO::JavaClass::DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, Text::StringBuilderUTF *sb, UTF8Char *typeBuff, MethodInfo *method, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
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
	Text::StringBuilderUTF8 sbTmp;
	const UInt8 *ptr = this->constPool[typeIndex];
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	const UInt8 *strEnd;
	UOSInt cnt = 0;
	UInt32 paramIndex;
	UInt32 paramId;
	UOSInt i;
	UOSInt j;
	const UTF8Char *paramName;
	if (method)
	{
		i = 0;
		j = method->lvList->GetCount();
		while (i < j)
		{
			if (this->GetConstName(sbuff, method->lvList->GetItem(i)->nameIndex))
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

	if (method && (method->accessFlags & 8) != 0)
	{
		paramIndex = 0;
	}
	else
	{
		paramIndex = 1;
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
			sbTmp.ClearStr();
			ptr = Type2String(ptr, &sbTmp);
			this->AppendCodeClassName(&sbParam, sbTmp.ToString(), importList, packageName);
			sbParam.AppendChar(' ', 1);
			paramId = GetParamId(paramIndex, method);
			paramName = typeNames.GetItem(paramId);
			if (paramName)
			{
				sbParam.Append(paramName);
			}
			else
			{
				sbParam.AppendChar('v', 1);
				sbParam.AppendU32(paramId);
			}
			paramIndex++;
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
		sb->AppendC(startPtr, (UOSInt)(endPtr - startPtr));
	}
	else
	{
		AppendCodeType2String(sb, ptr, importList, packageName);
		sb->AppendChar(' ', 1);
		sb->AppendC(&ptr2[3], strLen);
	}
	sb->Append(sbParam.ToString());
}

void IO::JavaClass::DetailType(UInt16 typeIndex, Text::StringBuilderUTF *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
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
	AppendCodeType2String(sb, ptr, importList, packageName);
}

void IO::JavaClass::DetailCode(const UInt8 *codePtr, UOSInt codeLen, UOSInt lev, Text::StringBuilderUTF *sb)
{
	UInt16 val;
	UOSInt codeOfst = 0;
	while (codeLen > 0)
	{
		switch (codePtr[codeOfst + 0])
		{
		case 0x00:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"nop\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x01:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aconst_null\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x02:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_m1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x03:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x04:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x05:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x06:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x07:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_4\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x08:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iconst_5\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x09:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lconst_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lconst_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fconst_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dconst_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x0F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dconst_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x10:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"bipush ");
			val = codePtr[codeOfst + 1];
			sb->AppendU16(val);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x11:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"sipush ");
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x12:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"ldc ");
			val = codePtr[codeOfst + 1];
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x13:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ldc_w ");
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x14:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ldc2_w ");
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x15:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"iload ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x16:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"lload ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x17:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"fload ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x18:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"dload ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x19:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"aload ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x1A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x1B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x1C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x1D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iload_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x1E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x1F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x20:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x21:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lload_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x22:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x23:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x24:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x25:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fload_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x26:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x27:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x28:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x29:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dload_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aload_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iaload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x2F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"laload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x30:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"faload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x31:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"daload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x32:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aaload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x33:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"baload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x34:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"caload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x35:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"saload\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x36:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"istore ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x37:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"lstore ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x38:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"fstore ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x39:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"dstore ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x3A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"astore ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x3B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x3C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x3D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x3E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"istore_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x3F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x40:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x41:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x42:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lstore_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x43:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x44:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x45:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x46:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fstore_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x47:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x48:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x49:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dstore_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_0\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"astore_3\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x4F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x50:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x51:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x52:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x53:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"aastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x54:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"bastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x55:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"castore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x56:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"sastore\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x57:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"pop\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x58:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"pop2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x59:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup_x1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup_x2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2_x1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dup2_x2\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x5F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"swap\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x60:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iadd\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x61:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ladd\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x62:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fadd\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x63:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dadd\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x64:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"isub\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x65:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lsub\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x66:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fsub\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x67:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dsub\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x68:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"imul\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x69:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lmul\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fmul\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dmul\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"idiv\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ldiv\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fdiv\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x6F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ddiv\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x70:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"irem\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x71:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lrem\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x72:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"frem\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x73:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"drem\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x74:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ineg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x75:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lneg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x76:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fneg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x77:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dneg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x78:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ishl\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x79:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lshl\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ishr\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lshr\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iushr\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lushr\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"iand\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x7F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"land\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x80:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ior\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x81:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lor\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x82:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ixor\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x83:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lxor\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x84:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"iinc ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)", ");
			sb->AppendI16((Int8)codePtr[codeOfst + 2]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x85:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2l\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x86:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2f\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x87:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2d\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x88:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2i\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x89:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2f\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"l2d\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2i\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2l\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"f2d\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2i\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x8F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2l\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x90:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"d2f\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x91:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2b\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x92:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2c\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x93:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"i2s\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x94:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lcmp\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x95:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fcmpl\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x96:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"fcmpg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x97:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dcmpl\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x98:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dcmpg\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0x99:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifeq ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifne ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"iflt ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifge ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifgt ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifle ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpeq ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpne ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmplt ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpge ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmpgt ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_icmple ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_acmpeq ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"if_acmpne ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"goto ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"jsr ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->Append((const UTF8Char*)"ret ");
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0xAA:
			{
				this->AppendIndent(sb, lev);
				sb->AppendUOSInt(codeOfst);
				sb->AppendChar('\t', 1);
				UOSInt currOfst = codeOfst + 1;
				while (currOfst & 3)
				{
					currOfst += 1;
				}
				Int32 defaultPC = ReadMInt32(&codePtr[currOfst]);
				UInt32 npairs = ReadMUInt32(&codePtr[currOfst + 4]);
				sb->AppendHexBuff(&codePtr[codeOfst], (currOfst + 8 - codeOfst), ' ', Text::LineBreakType::None);
				sb->AppendChar(' ', 1);
				sb->Append((const UTF8Char*)"tableswitch default = ");
				sb->AppendI32(defaultPC);
				sb->Append((const UTF8Char*)", npairs = ");
				sb->AppendU32(npairs);
				sb->Append((const UTF8Char*)"\r\n");
				codeLen -= (currOfst + 8 - codeOfst);
				codeOfst = currOfst + 8;
				UInt32 currIndex = 0;
				while (currIndex < npairs && codeLen >= 8)
				{
					this->AppendIndent(sb, lev);
					sb->AppendUOSInt(codeOfst);
					sb->AppendChar('\t', 1);
					sb->AppendHexBuff(&codePtr[codeOfst], 8, ' ', Text::LineBreakType::None);
					sb->AppendChar(' ', 1);
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst]));
					sb->Append((const UTF8Char*)": ");
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 4]));
					sb->Append((const UTF8Char*)"\r\n");
					currIndex++;
					codeOfst += 8;
					codeLen -= 8;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			break;
		case 0xAB:
			{
				this->AppendIndent(sb, lev);
				sb->AppendUOSInt(codeOfst);
				sb->AppendChar('\t', 1);
				UOSInt currOfst = codeOfst + 1;
				while (currOfst & 3)
				{
					currOfst += 1;
				}
				Int32 defaultPC = ReadMInt32(&codePtr[currOfst]);
				UInt32 npairs = ReadMUInt32(&codePtr[currOfst + 4]);
				sb->AppendHexBuff(&codePtr[codeOfst], (currOfst + 8 - codeOfst), ' ', Text::LineBreakType::None);
				sb->AppendChar(' ', 1);
				sb->Append((const UTF8Char*)"lookupswitch default = ");
				sb->AppendI32(defaultPC);
				sb->Append((const UTF8Char*)", npairs = ");
				sb->AppendU32(npairs);
				sb->Append((const UTF8Char*)"\r\n");
				codeLen -= (currOfst + 8 - codeOfst);
				codeOfst = currOfst + 8;
				UInt32 currIndex = 0;
				while (currIndex < npairs && codeLen >= 8)
				{
					this->AppendIndent(sb, lev);
					sb->AppendUOSInt(codeOfst);
					sb->AppendChar('\t', 1);
					sb->AppendHexBuff(&codePtr[codeOfst], 8, ' ', Text::LineBreakType::None);
					sb->AppendChar(' ', 1);
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst]));
					sb->Append((const UTF8Char*)": ");
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 4]));
					sb->Append((const UTF8Char*)"\r\n");
					currIndex++;
					codeOfst += 8;
					codeLen -= 8;
				}
				sb->Append((const UTF8Char*)"\r\n");
			}
			break;
		case 0xAC:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"ireturn\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xAD:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"lreturn\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xAE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"freturn\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xAF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"dreturn\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xB0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"areturn\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xB1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"return\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xB2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"getstatic ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"putstatic ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"getfield ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"putfield ");
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"invokevirtual ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"invokespecial ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"invokestatic ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 1);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"invokeinterface ");
			sb->AppendU16(val);
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
			sb->AppendU16(codePtr[codeOfst + 3]);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xBA:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 1);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"invokedynamic ");
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xBB:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"new ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xBC:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			val = codePtr[codeOfst + 1];
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
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0xBD:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"anewarray ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xBE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"arraylength\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xBF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"athrow\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xC0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"checkcast ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"instanceof ");
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"monitorenter\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xC3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"monitorexit\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xC4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			//////////////////////////////////////////
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"wide\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xC5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 4, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 4);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->Append((const UTF8Char*)"multianewarray ");
			sb->AppendU16(val);
			sb->AppendChar(',', 1);
			sb->AppendChar(' ', 1);
			sb->AppendU16(codePtr[codeOfst + 3]);
			this->DetailClassName(val, sb);
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 4;
			codeLen -= 4;
			break;
		case 0xC6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifnull ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->Append((const UTF8Char*)"ifnonnull ");
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"goto_w ");
			sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xC9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 1);
			sb->Append((const UTF8Char*)"jsr_w ");
			sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 1]));
			sb->Append((const UTF8Char*)"\r\n");
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xCA:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"breakpoint\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xFE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"impdep1\r\n");
			codeOfst++;
			codeLen--;
			break;
		case 0xFF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"impdep2\r\n");
			codeOfst++;
			codeLen--;
			break;
		default:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendChar('\t', 1);
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->Append((const UTF8Char*)"Unknown opcode\r\n");
			codeOfst++;
			codeLen--;
			break;
		}
	}
}

const UInt8 *IO::JavaClass::DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
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
	this->DetailType(type_index, sb, importList, packageName);
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
			annoPtr = this->DetailElementValue(&annoPtr[2], annoEnd, sb, importList, packageName);
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
const UInt8 *IO::JavaClass::DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, Text::StringBuilderUTF *sb, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
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
			annoPtr = this->DetailElementValue(annoPtr, annoEnd, sb, importList, packageName);
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
		this->DetailType(type_name_index, sb, importList, packageName);
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

const UInt8 *IO::JavaClass::DetailStackMapFrame(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, Text::StringBuilderUTF *sb)
{
	if (currPtr >= ptrEnd)
	{
		return currPtr;
	}
	if (currPtr[0] >= 0 && currPtr[0] < 64)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = SAME (");
		sb->AppendU16(currPtr[0]);
		sb->Append((const UTF8Char*)")\r\n");
		return currPtr + 1;
	}
	else if (currPtr[0] >= 64 && currPtr[0] < 128)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = SAME_LOCALS_1_STACK_ITEM (");
		sb->AppendU16(currPtr[0]);
		sb->Append((const UTF8Char*)")\r\n");
		return this->DetailVerificationTypeInfo(currPtr + 1, ptrEnd, lev + 1, sb);
	}
	else if (currPtr[0] == 247)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = SAME_LOCALS_1_STACK_ITEM_EXTENDED\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame offset_delta = ");
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->Append((const UTF8Char*)"\r\n");
		return this->DetailVerificationTypeInfo(currPtr + 3, ptrEnd, lev + 1, sb);
	}
	else if (currPtr[0] >= 248 && currPtr[0] <= 250)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = CHOP (");
		sb->AppendU16(currPtr[0]);
		sb->Append((const UTF8Char*)")\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame offset_delta = ");
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->Append((const UTF8Char*)"\r\n");
		return currPtr + 3;
	}
	else if (currPtr[0] == 251)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = SAME_FRAME_EXTENDED\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame offset_delta = ");
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->Append((const UTF8Char*)"\r\n");
		return currPtr + 3;
	}
	else if (currPtr[0] >= 252 && currPtr[0] <= 254)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = APPEND (");
		sb->AppendU16(currPtr[0]);
		sb->Append((const UTF8Char*)")\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame offset_delta = ");
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->Append((const UTF8Char*)"\r\n");
		UOSInt i = (UOSInt)currPtr[0];
		i -= 251;
		currPtr += 3;
		while (i-- > 0)
		{
			currPtr = this->DetailVerificationTypeInfo(currPtr, ptrEnd, lev + 1, sb);
		}
		return currPtr;
	}
	else if (currPtr[0] == 255)
	{
		UOSInt i;
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame Type = FULL_FRAME\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Frame offset_delta = ");
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->Append((const UTF8Char*)"\r\n");
		currPtr += 3;
		if (currPtr + 2 <= ptrEnd)
		{
			i = ReadMUInt16(currPtr);
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Frame number_of_locals = ");
			sb->AppendUOSInt(i);
			sb->Append((const UTF8Char*)"\r\n");
			while (i-- > 0)
			{
				currPtr = this->DetailVerificationTypeInfo(currPtr, ptrEnd, lev + 1, sb);
			}
		}
		if (currPtr + 2 <= ptrEnd)
		{
			i = ReadMUInt16(currPtr);
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Frame number_of_stack_items = ");
			sb->AppendUOSInt(i);
			sb->Append((const UTF8Char*)"\r\n");
			while (i-- > 0)
			{
				currPtr = this->DetailVerificationTypeInfo(currPtr, ptrEnd, lev + 1, sb);
			}
		}
		return currPtr;
	}
	else
	{
		sb->Append((const UTF8Char*)"Frame Type = UNKNOWN (");
		sb->AppendU16(currPtr[0]);
		sb->AppendChar(')', 1);
		return currPtr + 1;
	}
}

const UInt8 *IO::JavaClass::DetailVerificationTypeInfo(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, Text::StringBuilderUTF *sb)
{
	if (currPtr >= ptrEnd)
	{
		return currPtr;
	}
	UInt16 sindex;
	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"VerificationType = ");
	switch (currPtr[0])
	{
	case 0:
		sb->Append((const UTF8Char*)"Top\r\n");
		return currPtr + 1;
	case 1:
		sb->Append((const UTF8Char*)"Integer\r\n");
		return currPtr + 1;
	case 2:
		sb->Append((const UTF8Char*)"Float\r\n");
		return currPtr + 1;
	case 3:
		sb->Append((const UTF8Char*)"Double\r\n");
		return currPtr + 1;
	case 4:
		sb->Append((const UTF8Char*)"Long\r\n");
		return currPtr + 1;
	case 5:
		sb->Append((const UTF8Char*)"Null\r\n");
		return currPtr + 1;
	case 6:
		sb->Append((const UTF8Char*)"UninitializedThis\r\n");
		return currPtr + 1;
	case 7:
		{
			sb->Append((const UTF8Char*)"Object\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"Verification cinfo_index = ");
			sindex = ReadMUInt16(&currPtr[1]);
			sb->AppendU16(sindex);
	/*		UInt16 strLen;
			UInt8 *ptr;
			ptr = this->constPool[sindex];
			if (ptr[0] == 1)
			{
				strLen = ReadMUInt16(&ptr[1]);
				Text::StringBuilderUTF8 sbTmp;
				sbTmp.AppendC(ptr + 3, strLen);
				sbTmp.Replace('/', '.');
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbTmp.ToString());
				sb->Append((const UTF8Char*)")");
			}*/
			sb->Append((const UTF8Char*)"\r\n");
			return currPtr + 3;
		}
	case 8:
		sb->Append((const UTF8Char*)"Uninitialized\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"Verification offset = ");
		sindex = ReadMUInt16(&currPtr[1]);
		sb->AppendU16(sindex);
		sb->Append((const UTF8Char*)"\r\n");
		return currPtr + 3;
	default:
		sb->Append((const UTF8Char*)"Unknown (");
		sb->AppendU16(currPtr[0]);
		sb->Append((const UTF8Char*)"\r\n");
		return currPtr + 1;
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

Bool IO::JavaClass::ClassNameString(UInt16 index, Text::StringBuilderUTF *sb)
{
	if (index == 0 || index >= this->constPoolCnt)
	{
		return false;
	}
	if (this->constPool[index][0] != 7)
	{
		return false;
	}
	UInt16 nameIndex = ReadMUInt16(&this->constPool[index][1]);
	UInt8 *ptr = this->constPool[nameIndex];
	if (ptr[0] != 1)
	{
		return false;
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
				sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
			else if (c == '$')
			{
				sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
				sb->AppendChar('.', 1);
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	}
	return true;
}

UTF8Char *IO::JavaClass::GetLVName(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst)
{
	if (method)
	{
		UOSInt i = 0;
		UOSInt j = method->lvList->GetCount();
		while (i < j)
		{
			LocalVariableInfo *lv = method->lvList->GetItem(i);
			if (lv->index == index && lv->startPC <= codeOfst && lv->startPC + (UOSInt)lv->length > codeOfst)
			{
				UTF8Char *ret = this->GetConstName(sbuff, lv->nameIndex);
				if (ret)
				{
					return ret;
				}
			}
			i++;
		}
	}
	*sbuff++ = 'v';
	return Text::StrUInt16(sbuff, index);
}

UTF8Char *IO::JavaClass::GetLVType(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	if (method)
	{
		UOSInt i;
		UOSInt j;
		i = 0;
		j = method->lvtList->GetCount();
		while (i < j)
		{
			LocalVariableTypeInfo *lvt = method->lvtList->GetItem(i);
			if (lvt->index == index && lvt->startPC <= codeOfst && lvt->startPC + (UOSInt)lvt->length > codeOfst)
			{
				Text::StringBuilderUTF8 sbTmp;
				this->DetailType(lvt->signatureIndex, &sbTmp, importList, packageName);
				return Text::StrConcat(sbuff, sbTmp.ToString());
			}
			i++;
		}

		i = 0;
		j = method->lvList->GetCount();
		while (i < j)
		{
			LocalVariableInfo *lv = method->lvList->GetItem(i);
			if (lv->index == index && lv->startPC <= codeOfst && lv->startPC + (UOSInt)lv->length > codeOfst)
			{
				Text::StringBuilderUTF8 sbTmp;
				this->DetailType(lv->descriptorIndex, &sbTmp, importList, packageName);
				return Text::StrConcat(sbuff, sbTmp.ToString());
			}
			i++;
		}
	}
	return Text::StrConcat(sbuff, (const UTF8Char*)"java.lang.Object");
}

Bool IO::JavaClass::MethodParse(MethodInfo *method, const UInt8 *methodBuff)
{
	NEW_CLASS(method->exHdlrList, Data::ArrayList<ExceptionHdlrInfo*>());
	NEW_CLASS(method->lvList, Data::ArrayList<LocalVariableInfo*>());
	NEW_CLASS(method->lvtList, Data::ArrayList<LocalVariableTypeInfo*>());
	NEW_CLASS(method->lineNumList, Data::ArrayList<LineNumberInfo*>());
	NEW_CLASS(method->exList, Data::ArrayList<UInt16>());
	method->accessFlags = ReadMUInt16(methodBuff);
	method->nameIndex = ReadMUInt16(&methodBuff[2]);
	method->descriptorIndex = ReadMUInt16(&methodBuff[4]);
	method->code = 0;
	method->maxStacks = 0;
	method->maxLocals = 0;
	method->signatureIndex = 0;
	UTF8Char sbuff[256];
	UInt16 attrCnt = ReadMUInt16(&methodBuff[6]);
	UInt16 i;
	const UInt8 *ptr = &methodBuff[8];
	i = 0;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		UInt16 j;
		sbuff[0] = 0;
		if (this->GetConstName(sbuff, nameIndex))
		{
			if (Text::StrEquals(sbuff, (const UTF8Char*)"Code"))
			{
				UInt32 codeLen = ReadMUInt32(&ptr[10]);
				const UInt8 *ptr2;
				method->maxStacks = ReadMUInt16(&ptr[6]);
				method->maxLocals = ReadMUInt16(&ptr[8]);
				method->code = MemAlloc(UInt8, codeLen);
				method->codeLen = codeLen;
				MemCopyNO(method->code, &ptr[14], codeLen);
				ptr2 = &ptr[14 + codeLen];
				UInt16 exceptLen = ReadMUInt16(&ptr2[0]);
				ptr2 += 2;
				while (exceptLen-- > 0)
				{
					ExceptionHdlrInfo *exHdlr = MemAlloc(ExceptionHdlrInfo, 1);
					exHdlr->startPC = ReadMUInt16(&ptr2[0]);
					exHdlr->endPC = ReadMUInt16(&ptr2[2]);
					exHdlr->handlerPC = ReadMUInt16(&ptr2[4]);
					exHdlr->catchType = ReadMUInt16(&ptr2[6]);
					method->exHdlrList->Add(exHdlr);
					ptr2 += 8;
				}
				UInt16 attrCount = ReadMUInt16(&ptr2[0]);
				ptr2 += 2;
				j = 0;
				while (j < attrCount)
				{
					UInt32 len = ReadMUInt32(&ptr2[2]);
					UInt16 nameIndex = ReadMUInt16(&ptr2[0]);
					sbuff[0] = 0;
					if (this->GetConstName(sbuff, nameIndex))
					{
						if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTable"))
						{
							const UInt8 *lvTable = &ptr2[6];
							UOSInt local_variable_table_length = ReadMUInt16(lvTable);
							UOSInt k;
							LocalVariableInfo *lv;
							if (local_variable_table_length * 10 + 2 <= len)
							{
								k = 0;
								while (k < local_variable_table_length)
								{
									lv = MemAlloc(LocalVariableInfo, 1);
									lv->startPC = ReadMUInt16(&lvTable[2 + k * 10 + 0]);
									lv->length = ReadMUInt16(&lvTable[2 + k * 10 + 2]);
									lv->nameIndex = ReadMUInt16(&lvTable[2 + k * 10 + 4]);
									lv->descriptorIndex = ReadMUInt16(&lvTable[2 + k * 10 + 6]);
									lv->index = ReadMUInt16(&lvTable[2 + k * 10 + 8]);
									method->lvList->Add(lv);
									k++;
								}
							}
						}
						else if (Text::StrEquals(sbuff, (const UTF8Char*)"LocalVariableTypeTable"))
						{
							const UInt8 *lvtTable = &ptr2[6];
							UOSInt local_variable_type_table_length = ReadMUInt16(lvtTable);
							UOSInt k;
							LocalVariableTypeInfo *lvt;
							if (local_variable_type_table_length * 10 + 2 <= len)
							{
								k = 0;
								while (k < local_variable_type_table_length)
								{
									lvt = MemAlloc(LocalVariableTypeInfo, 1);
									lvt->startPC = ReadMUInt16(&lvtTable[2 + k * 10 + 0]);
									lvt->length = ReadMUInt16(&lvtTable[2 + k * 10 + 2]);
									lvt->nameIndex = ReadMUInt16(&lvtTable[2 + k * 10 + 4]);
									lvt->signatureIndex = ReadMUInt16(&lvtTable[2 + k * 10 + 6]);
									lvt->index = ReadMUInt16(&lvtTable[2 + k * 10 + 8]);
									method->lvtList->Add(lvt);
									k++;
								}
							}
						}
						else if (Text::StrEquals(sbuff, (const UTF8Char*)"LineNumberTable"))
						{
							UInt16 line_number_table_length = ReadMUInt16(&ptr2[6]);
							UInt16 k = 0;
							const UInt8 *ptr3 = &ptr2[8];
							while (k < line_number_table_length)
							{
								LineNumberInfo *lineNumber = MemAlloc(LineNumberInfo, 1);
								lineNumber->startPC = ReadMUInt16(&ptr3[0]);
								lineNumber->lineNumber = ReadMUInt16(&ptr3[2]);
								method->lineNumList->Add(lineNumber);
								k++;
								ptr3 += 4;
							}
						}
					}
					ptr2 += 6 + len;
					j++;
				}
			}
			else if (Text::StrEquals(sbuff, (const UTF8Char*)"Exceptions"))
			{
				UInt16 exCnt = ReadMUInt16(&ptr[6]);
				UInt16 j = 0;
				while (j < exCnt)
				{
					method->exList->Add(ReadMUInt16(&ptr[8 + j * 2]));
					j++;
				}
			}
			else if (Text::StrEquals(sbuff, (const UTF8Char*)"Signature"))
			{
				if (len == 2)
				{
					method->signatureIndex = ReadMUInt16(&ptr[6]);
				}
			}
		}
		ptr += len + 6;

		i++;
	}
	return true;
}

void IO::JavaClass::MethodFree(MethodInfo *method)
{
	if (method->code)
	{
		MemFree(method->code);
		method->code = 0;
	}
	LIST_FREE_FUNC(method->exHdlrList, MemFree);
	DEL_CLASS(method->exHdlrList);
	LIST_FREE_FUNC(method->lvList, MemFree);
	DEL_CLASS(method->lvList);
	LIST_FREE_FUNC(method->lvtList, MemFree);
	DEL_CLASS(method->lvtList);
	LIST_FREE_FUNC(method->lineNumList, MemFree);
	DEL_CLASS(method->lineNumList);
	DEL_CLASS(method->exList);
}

void IO::JavaClass::AppendCodeClassName(Text::StringBuilderUTF *sb, const UTF8Char *className, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	UOSInt i = Text::StrLastIndexOf(className, '.');
	if (i == INVALID_INDEX)
	{
		sb->Append(className);
		return;
	}
	if (packageName && Text::StrStartsWith(className, packageName) && i == Text::StrCharCnt(packageName))
	{
		sb->Append(className + i + 1);
		return;
	}
	if (Text::StrStartsWith(className, (const UTF8Char*)"java.lang.") && i == 9)
	{
		sb->Append(className + i + 1);
		return;
	}
	if (importList == 0)
	{
		sb->Append(className);
		return;
	}
	if (Text::StrEndsWith(className, (const UTF8Char*)"[]"))
	{
		Text::StringBuilderUTF8 sbCls;
		sbCls.Append(className);
		sbCls.RemoveChars(2);
		if (importList->SortedIndexOf(sbCls.ToString()) >= 0)
		{
			sb->Append(className + i + 1);
			return;
		}
		importList->SortedInsert(Text::StrCopyNew(sbCls.ToString()));
		sb->Append(className + i + 1);
	}
	else
	{
		if (importList->SortedIndexOf(className) >= 0)
		{
			sb->Append(className + i + 1);
			return;
		}
		importList->SortedInsert(Text::StrCopyNew(className));
		sb->Append(className + i + 1);
	}
}

void IO::JavaClass::AppendCodeClassContent(Text::StringBuilderUTF *sb, UOSInt lev, const UTF8Char *className, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	this->AppendIndent(sb, lev);
	if (this->accessFlags & 1) //ACC_PUBLIC
	{
		sb->Append((const UTF8Char*)"public ");
	}
	else if (accessFlags & 2) //ACC_PRIVATE
	{
		sb->Append((const UTF8Char*)"private ");
	}
	else if (accessFlags & 4) //ACC_PROTECTED
	{
		sb->Append((const UTF8Char*)"protected ");
	}

	if (accessFlags & 8) //ACC_STATIC
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (accessFlags & 0x10) //ACC_FINAL
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (accessFlags & 0x40) //ACC_VOLATILE
	{
		sb->Append((const UTF8Char*)"volatile ");
	}
	if (accessFlags & 0x80) //ACC_TRANSIENT
	{
		sb->Append((const UTF8Char*)"transient ");
	}
	if (accessFlags & 0x0100) //ACC_NATIVE
	{
		sb->Append((const UTF8Char*)"native ");
	}

	if (accessFlags & 0x0200) //ACC_INTERFACE
	{
		sb->Append((const UTF8Char*)"interface ");
	}
	else if (accessFlags & 0x0400) //ACC_ABSTRACT
	{
		sb->Append((const UTF8Char*)"abstract class ");
	}
	else if (accessFlags & 0x0800) //ACC_STRICT
	{
		sb->Append((const UTF8Char*)"strict class");
	}
	else if (accessFlags & 0x1000) //ACC_SYNTHETIC
	{
		sb->Append((const UTF8Char*)"synthetic class");
	}
	else if (accessFlags & 0x2000) //ACC_ANNOTATION
	{
		sb->Append((const UTF8Char*)"annotation class");
	}
	else if (accessFlags & 0x4000) //ACC_ENUM
	{
		sb->Append((const UTF8Char*)"enum ");
	}
	else
	{
		sb->Append((const UTF8Char*)"class ");
	}
	sb->Append(className);
	Text::StringBuilderUTF8 sbTmp;
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	if (this->signatureIndex != 0 && this->GetConstName(sbuff, this->signatureIndex))
	{
		const UTF8Char *ptr = sbuff;
		UTF8Char c;
		Bool isType = false;
		Bool isComma = false;
		if (ptr[0] == '<')
		{
			sb->AppendChar('<', 1);
			ptr++;
			while (true)
			{
				c = *ptr++;
				if (c == 0)
				{
					ptr--;
					break;
				}
				else if (c == '>')
				{
					sb->AppendChar(c, 1);
					break;
				}
				else if (c == ':')
				{
					isType = true;
				}
				else if (c == ';')
				{
					isType = false;
					isComma = true;
				}
				else if (!isType)
				{
					if (isComma)
					{
						sb->Append((const UTF8Char*)", ");
						isComma = false;
					}
					sb->AppendChar(c, 1);
				}
			}
		}
		sbTmp.ClearStr();
		ptr = AppendCodeType2String(&sbTmp, ptr, importList, packageName);
		if (!sbTmp.Equals((const UTF8Char*)"Object"))
		{
			sb->Append((const UTF8Char*)" extends ");
			sb->Append(sbTmp.ToString());
		}
		if (this->interfaces && this->interfaceCnt > 0)
		{
			i = 0;
			while (i < this->interfaceCnt)
			{
				if (i == 0)
				{
					sb->Append((const UTF8Char*)" implements ");
				}
				else
				{
					sb->Append((const UTF8Char*)", ");
				}
				ptr = AppendCodeType2String(sb, ptr, importList, packageName);
				i++;
			}
		}
	}
	else
	{
		sbTmp.ClearStr();
		if (this->GetSuperClass(&sbTmp))
		{
			if (!sbTmp.Equals((const UTF8Char*)"java.lang.Object"))
			{
				sb->Append((const UTF8Char*)" extends ");
				this->AppendCodeClassName(sb, sbTmp.ToString(), importList, packageName);
			}
		}
		if (this->interfaces && this->interfaceCnt > 0)
		{
			i = 0;
			while (i < this->interfaceCnt)
			{
				if (i == 0)
				{
					sb->Append((const UTF8Char*)" implements ");
				}
				else
				{
					sb->Append((const UTF8Char*)", ");
				}
				sbTmp.ClearStr();
				this->ClassNameString(ReadMUInt16(&this->interfaces[i * 2]), &sbTmp);
				this->AppendCodeClassName(sb, sbTmp.ToString(), importList, packageName);
				i++;
			}
		}
	}
	sb->Append((const UTF8Char*)" {\r\n");
	i = 0;
	j = this->FieldsGetCount();
	while (i < j)
	{
		this->AppendIndent(sb, lev + 1);
		this->AppendCodeField(sb, i, importList, packageName);
		sb->Append((const UTF8Char*)";\r\n");
		i++;
	}

	i = 0;
	j = this->MethodsGetCount();
	while (i < j)
	{
		sb->Append((const UTF8Char*)"\r\n");
		this->AppendCodeMethod(sb, i, lev + 1, false, true, importList, packageName);
		i++;
	}
	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"}\r\n");
}

void IO::JavaClass::AppendCodeField(Text::StringBuilderUTF *sb, UOSInt index, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	UTF8Char sbuff[256];
	Text::StringBuilderUTF8 sbValue;
	UInt8 *ptr = this->fields[index];
	UInt16 accessFlags = ReadMUInt16(ptr);
	UInt16 nameIndex = ReadMUInt16(&ptr[2]);
	UInt16 descriptorIndex = ReadMUInt16(&ptr[4]);
	UInt16 signatureIndex = 0;
	UInt16 attrCnt = ReadMUInt16(&ptr[6]);
	this->GetConstName(sbuff, descriptorIndex);
	Text::StringBuilderUTF8 sbTypeName;
	Type2String(sbuff, &sbTypeName);
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
						sbValue.Append((const UTF8Char*)" = ");
						if (sbTypeName.Equals((const UTF8Char*)"boolean"))
						{
							if (ReadMInt32(&this->constPool[nameIndex][1]) != 0)
							{
								sbValue.Append((const UTF8Char*)"true");
							}
							else
							{
								sbValue.Append((const UTF8Char*)"false");
							}
						}
						else
						{
							sbValue.AppendI32(ReadMInt32(&this->constPool[nameIndex][1]));
						}
					}
					else if (this->constPool[nameIndex][0] == 4)
					{
						sbValue.Append((const UTF8Char*)" = ");
						Text::SBAppendF32(&sbValue, ReadMFloat(&this->constPool[nameIndex][1]));
						sbValue.AppendChar('f', 1);
					}
					else if (this->constPool[nameIndex][0] == 5)
					{
						sbValue.Append((const UTF8Char*)" = ");
						sbValue.AppendI64(ReadMInt64(&this->constPool[nameIndex][1]));
						sbValue.AppendChar('L', 1);
					}
					else if (this->constPool[nameIndex][0] == 6)
					{
						sbValue.Append((const UTF8Char*)" = ");
						Text::SBAppendF64(&sbValue, ReadMDouble(&this->constPool[nameIndex][1]));
					}
					else if (this->constPool[nameIndex][0] == 8)
					{
						UInt16 sindex = ReadMUInt16(&this->constPool[nameIndex][1]);
						UInt16 strLen;
						ptr = this->constPool[sindex];
						if (ptr[0] == 1)
						{
							sbValue.Append((const UTF8Char*)" = ");
							strLen = ReadMUInt16(&ptr[1]);
							const UTF8Char *s = Text::StrCopyNewC(ptr + 3, strLen);
							Text::JSText::ToJSTextDQuote(&sbValue, s);
							Text::StrDelNew(s);
						}
					}
					else
					{
						printf("AppendCodeField: Unsupported type %d\r\n", this->constPool[nameIndex][0]);
					}
				}
			}
			else if (Text::StrEquals(sbuff, (const UTF8Char*)"Signature"))
			{
				if (len == 2)
				{
					signatureIndex = ReadMUInt16(&ptr[6]);
				}
			}
		}
		ptr += len + 6;

		i++;
	}

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
	if (signatureIndex != 0)
	{
		this->GetConstName(sbuff, signatureIndex);
		AppendCodeType2String(sb, sbuff, importList, packageName);
	}
	else if (importList == 0 && packageName == 0)
	{
		sb->Append(sbTypeName.ToString());
	}
	else
	{
		this->AppendCodeClassName(sb, sbTypeName.ToString(), importList, packageName);
	}
	sb->AppendChar(' ', 1);
	this->GetConstName(sbuff, nameIndex);
	sb->Append(sbuff);
	sb->Append(sbValue.ToString());
}

void IO::JavaClass::AppendCodeMethod(Text::StringBuilderUTF *sb, UOSInt index, UOSInt lev, Bool disasm, Bool decompile, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	Text::StringBuilderUTF8 sbTmp;
	UTF8Char sbuff[256];
	UInt8 *ptr = this->methods[index];
	UInt16 i;
	UOSInt j;
	UTF8Char typeBuff[256];
	MethodInfo method;
	MethodParse(&method, ptr);
	UInt16 attrCnt = ReadMUInt16(&this->methods[index][6]);

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
					this->AppendIndent(sb, lev);
					annptr = this->DetailAnnotation(annptr, &ptr[6 + len], sb, importList, packageName);
					sb->Append((const UTF8Char*)"\r\n");

					j++;
				}
			}
		}
		ptr += len + 6;

		i++;
	}

	ptr = this->methods[index];
	this->AppendIndent(sb, lev);
	if (method.accessFlags & 1)
	{
		sb->Append((const UTF8Char*)"public ");
	}
	if (method.accessFlags & 2)
	{
		sb->Append((const UTF8Char*)"private ");
	}
	if (method.accessFlags & 4)
	{
		sb->Append((const UTF8Char*)"protected ");
	}
	if (method.accessFlags & 8)
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (method.accessFlags & 0x10)
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (method.accessFlags & 0x20)
	{
		sb->Append((const UTF8Char*)"synchronized ");
	}
	if (method.accessFlags & 0x100)
	{
		sb->Append((const UTF8Char*)"native ");
	}
	if (method.accessFlags & 0x400)
	{
		sb->Append((const UTF8Char*)"abstract ");
	}
	if (method.signatureIndex != 0)
	{
		this->DetailNameType(method.nameIndex, method.signatureIndex, this->thisClass, 0, sb, typeBuff, &method, importList, packageName);
	}
	else
	{
		this->DetailNameType(method.nameIndex, method.descriptorIndex, this->thisClass, 0, sb, typeBuff, &method, importList, packageName);
	}
	ptr = &this->methods[index][8];
	i = 0;
	j = method.exList->GetCount();
	while (i < j)
	{
		if (i == 0)
		{
			sb->Append((const UTF8Char*)" throws ");
		}
		else
		{
			sb->Append((const UTF8Char*)", ");
		}
		sbTmp.ClearStr();
		this->ClassNameString(method.exList->GetItem(i), &sbTmp);
		this->AppendCodeClassName(sb, sbTmp.ToString(), importList, packageName);
		i++;
	}

	sb->Append((const UTF8Char*)"\r\n");
	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"{\r\n");
	if (disasm)
	{
		this->AppendIndent(sb, (lev + 1));
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
					this->DetailCode(&ptr[14], codeLen, (lev + 1), sb);
				}
			}
			ptr += len + 6;

			i++;
		}
		this->AppendIndent(sb, (lev + 1));
		sb->Append((const UTF8Char*)"*/\r\n");
	}
	if (decompile)
	{
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
					this->AppendCodeMethodCodes(sb, lev + 1, importList ,packageName, ptr, typeBuff, &method);
				}
			}
			ptr += len + 6;

			i++;
		}
	}
	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"}\r\n");
	MethodFree(&method);
}

void IO::JavaClass::AppendCodeMethodCodes(Text::StringBuilderUTF *sb, UOSInt lev, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName, const UInt8 *codeAttr, const UTF8Char *typeBuff, const MethodInfo *method)
{
	IO::JavaClass::DecompileEnv env;
	UInt16 maxLocal = ReadMUInt16(&codeAttr[8]);
	UInt32 codeLen = ReadMUInt32(&codeAttr[10]);
	Data::ArrayList<Text::String *> stackVal;
	Data::ArrayList<Text::String *> stackTypes;
	UTF8Char sbuff[256];
	UOSInt i;
	Text::StringBuilderUTF8 sbTmp;
	env.localTypes = MemAlloc(Text::String *, maxLocal);
	env.method = method;
	env.stacks = &stackVal;
	env.stackTypes = &stackTypes;
	env.importList = importList;
	env.packageName = packageName;
	env.returnType = 0;
	sbTmp.ClearStr();
	if (this->MethodGetReturnType(method->descriptorIndex, &sbTmp))
	{
		env.returnType = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
	}

	if (maxLocal > 0)
	{
		i = maxLocal;
		while (i-- > 0)
		{
			env.localTypes[i] = 0;
			sbuff[0] = 'v';
			Text::StrUOSInt(&sbuff[1], i);
		}
	}

	if (method && (method->accessFlags & 8) != 0)
	{
		i = 0;
	}
	else
	{
		sbTmp.ClearStr();
		this->ClassNameString(this->thisClass, &sbTmp);
		env.localTypes[0] = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
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
		env.localTypes[i] = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
		i++;
	}

	const UInt8 *codePtr = &codeAttr[14];
	env.codeStart = codePtr;
	this->DecompileCode(codePtr, codePtr + codeLen, &env, lev, sb);
	if (stackVal.GetCount() != stackTypes.GetCount())
	{
		i = stackVal.GetCount();
		while (i-- > 0)
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"// Unprocessed stack: ");
			sb->Append(stackVal.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			stackVal.GetItem(i)->Release();
		}
		i = stackTypes.GetCount();
		while (i-- > 0)
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"// Unprocessed stack type: ");
			sb->Append(stackTypes.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			stackTypes.GetItem(i)->Release();
		}
	}
	else
	{
		i = stackVal.GetCount();
		while (i-- > 0)
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"// Unprocessed stack: ");
			sb->Append(stackTypes.GetItem(i));
			sb->Append((const UTF8Char*)" ");
			sb->Append(stackVal.GetItem(i));
			sb->Append((const UTF8Char*)"\r\n");
			stackTypes.GetItem(i)->Release();
			stackVal.GetItem(i)->Release();
		}
	}

	i = maxLocal;
	while (i-- > 0)
	{
		if (env.localTypes[i])
		{
			env.localTypes[i]->Release();
		}
	}
	MemFree(env.localTypes);
	SDEL_STRING(env.returnType);
}

const UTF8Char *IO::JavaClass::AppendCodeType2String(Text::StringBuilderUTF *sb, const UTF8Char *typeStr, Data::ArrayListStrUTF8 *importList, const UTF8Char *packageName)
{
	OSInt arrLev = 0;
	UInt8 c;
	c = *typeStr++;
	while (c == '[')
	{
		arrLev++;
		c = *typeStr++;
	}
	if (c == '+')
	{
		sb->Append((const UTF8Char*)"? extends ");
		c = *typeStr++;
	}
	switch (c)
	{
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
	case 'L':
		if (importList || packageName)
		{
			Text::StringBuilderUTF8 sbTmp;
			UOSInt subcls = INVALID_INDEX;
			UOSInt tmpIndex = INVALID_INDEX;
			while (true)
			{
				c = *typeStr++;
				if (c == 0)
				{
					typeStr--;
					break;
				}
				else if (c == ';')
				{
					break;
				}
				else if (c == '/')
				{
					sbTmp.AppendChar('.', 1);
				}
				else if (c == '$')
				{
					subcls = sbTmp.GetLength();
					sbTmp.AppendChar('.', 1);
				}
				else if (c == '<')
				{
					tmpIndex = sbTmp.GetLength();
					sbTmp.AppendChar('<', 1);
					Bool found = false;
					while (true)
					{
						if (*typeStr == 0)
						{
							break;
						}
						else if (*typeStr == '>')
						{
							sbTmp.AppendChar('>', 1);
							typeStr++;
							break;
						}
						else
						{
							if (found) sbTmp.Append((const UTF8Char*)", ");
							typeStr = AppendCodeType2String(&sbTmp, typeStr, importList, packageName);
							found = true;
						}
					}
				}
				else
				{
					sbTmp.AppendChar(c, 1);
				}
			}
			UTF8Char *sptr = sbTmp.ToString();
			UOSInt i;
			if (subcls != INVALID_INDEX)
			{
				sptr[subcls] = 0;
			}
			else if (tmpIndex != INVALID_INDEX)
			{
				sptr[tmpIndex] = 0;
			}
			i = Text::StrLastIndexOf(sptr, '.');
			if (i == INVALID_INDEX)
			{
				if (subcls != INVALID_INDEX)
				{
					sptr[subcls] = '.';
				}
				else if (tmpIndex != INVALID_INDEX)
				{
					sptr[tmpIndex] = '<';
				}
				sb->Append(sptr);
			}
			else
			{
				Bool fullName = false;
				if (packageName && Text::StrStartsWith(sptr, packageName) && Text::StrCharCnt(packageName) == i)
				{
					
				}
				else if (Text::StrStartsWith(sptr, (const UTF8Char*)"java.lang.") && i == 9)
				{

				}
				else if (importList)
				{
					if (importList->SortedIndexOf(sptr) < 0)
					{
						importList->SortedInsert(Text::StrCopyNew(sptr));
					}
				}
				else
				{
					fullName = true;
				}

				if (subcls != INVALID_INDEX)
				{
					sptr[subcls] = '.';
				}
				else if (tmpIndex != INVALID_INDEX)
				{
					sptr[tmpIndex] = '<';
				}
				if (fullName)
				{
					sb->Append(sptr);
				}
				else
				{
					sb->Append(&sptr[i + 1]);
				}
			}
		}
		else
		{
			while (true)
			{
				c = *typeStr++;
				if (c == 0)
				{
					typeStr--;
					break;
				}
				else if (c == ';')
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
				else if (c == '<')
				{
					sb->AppendChar('<', 1);
					Bool found = false;
					while (true)
					{
						if (*typeStr == 0)
						{
							break;
						}
						else if (*typeStr == '>')
						{
							typeStr++;
							sb->AppendChar('>', 1);
							break;
						}
						else
						{
							if (found) sb->Append((const UTF8Char*)", ");
							typeStr = AppendCodeType2String(sb, typeStr, importList, packageName);
							found = true;
						}
					}
				}
				else
				{
					sb->AppendChar(c, 1);
				}
			}
		}
		break;
	case 'S':
		sb->Append((const UTF8Char*)"short");
		break;
	case 'T':
		while (true)
		{
			c = *typeStr++;
			if (c == 0)
			{
				typeStr--;
				break;
			}
			else if (c == ';')
			{
				break;
			}
			else
			{
				sb->AppendChar(c, 1);
			}
		}
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

void IO::JavaClass::Init(const UInt8 *buff, UOSInt buffSize)
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
	this->signatureIndex = 0;
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

	UOSInt ofst;
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
			ofst += 3 + (UOSInt)strLen;
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
	UTF8Char sbuff[256];
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
			UInt32 len = ReadMUInt32(&this->fileBuff[ofst + 2]);
			UInt16 nameIndex = ReadMUInt16(&this->fileBuff[ofst + 0]);
			if (this->GetConstName(sbuff, nameIndex))
			{
				if (Text::StrEquals(sbuff, (const UTF8Char*)"Signature") && len == 2)
				{
					this->signatureIndex = ReadMUInt16(&this->fileBuff[ofst + 6]);
				}
			}
			ofst += 6 + len;
			j++;
		}
	}
}

IO::JavaClass::JavaClass(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize) : IO::ParsedObject(sourceName)
{
	this->Init(buff, buffSize);
}

IO::JavaClass::JavaClass(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize) : IO::ParsedObject(sourceName)
{
	this->Init(buff, buffSize);
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

IO::ParserType IO::JavaClass::GetParserType()
{
	return IO::ParserType::JavaClass;
}

Bool IO::JavaClass::GetClassNameFull(Text::StringBuilderUTF *sb)
{
	return ClassNameString(this->thisClass, sb);
}

Bool IO::JavaClass::GetSuperClass(Text::StringBuilderUTF *sb)
{
	return ClassNameString(this->superClass, sb);
}

Bool IO::JavaClass::FileStructDetail(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"Version = ");
	sb->AppendU16(ReadMUInt16(&this->fileBuff[6]));
	sb->AppendChar('.', 1);
	sb->AppendU16(ReadMUInt16(&this->fileBuff[4]));
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"constant_pool_count = ");
	sb->AppendUOSInt(this->constPoolCnt);
	sb->Append((const UTF8Char*)"\r\n");
	UOSInt i;
	UInt16 j;
	i = 1;
	while (i < this->constPoolCnt)
	{
		UInt16 strLen;
		sb->Append((const UTF8Char*)"Const ");
		sb->AppendUOSInt(i);
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
	sb->AppendUOSInt(this->interfaceCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->interfaceCnt)
	{
		sb->Append((const UTF8Char*)"interfaces[");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)"] = ");
		sb->AppendU16(ReadMUInt16(&this->interfaces[i * 2]));
		sb->Append((const UTF8Char*)"\r\n");
		i++;
	}
	sb->Append((const UTF8Char*)"fields_count = ");
	sb->AppendUOSInt(this->fieldsCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->fieldsCnt)
	{
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" Access Flags = ");
		DetailAccessFlags(ReadMUInt16(&this->fields[i][0]), sb);
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" name index = ");
		sb->AppendU16(ReadMUInt16(&this->fields[i][2]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" descriptor index = ");
		sb->AppendU16(ReadMUInt16(&this->fields[i][4]));
		sb->Append((const UTF8Char*)"\r\n");
		UInt16 attributes_count = ReadMUInt16(&this->fields[i][6]);
		sb->Append((const UTF8Char*)"Field ");
		sb->AppendUOSInt(i);
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
	sb->AppendUOSInt(this->methodCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->methodCnt)
	{
		MethodInfo method;
		this->MethodParse(&method, this->methods[i]);
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" Access Flags = ");
		DetailAccessFlags(accessFlags, sb);
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" name index = ");
		sb->AppendU16(ReadMUInt16(&this->methods[i][2]));
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendUOSInt(i);
		sb->Append((const UTF8Char*)" descriptor index = ");
		sb->AppendU16(ReadMUInt16(&this->methods[i][4]));
		this->DetailNameType(ReadMUInt16(&this->methods[i][2]), ReadMUInt16(&this->methods[i][4]), this->thisClass, (const UTF8Char*)" ", sb, 0, &method, 0, 0);
		sb->Append((const UTF8Char*)"\r\n");
		UInt16 attributes_count = ReadMUInt16(&this->methods[i][6]);
		sb->Append((const UTF8Char*)"Method ");
		sb->AppendUOSInt(i);
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
		this->MethodFree(&method);
		i++;
	}

	sb->Append((const UTF8Char*)"attributes_count = ");
	sb->AppendUOSInt(this->attrCnt);
	sb->Append((const UTF8Char*)"\r\n");
	i = 0;
	while (i < this->attrCnt)
	{
		this->DetailAttribute(this->attrs[i], 1, sb);
		i++;
	}
	return true;
}

UOSInt IO::JavaClass::FieldsGetCount()
{
	return this->fieldsCnt;
}

Bool IO::JavaClass::FieldsGetDecl(UOSInt index, Text::StringBuilderUTF *sb)
{
	if (index >= this->fieldsCnt)
	{
		return false;
	}
	this->AppendCodeField(sb, index, 0, 0);
	return true;
}

UOSInt IO::JavaClass::MethodsGetCount()
{
	return this->methodCnt;
}

Bool IO::JavaClass::MethodsGetDecl(UOSInt index, Text::StringBuilderUTF *sb)
{
	if (index >= this->methodCnt)
	{
		return false;
	}

	MethodInfo method;
	this->MethodParse(&method, this->methods[index]);
	if (method.accessFlags & 1)
	{
		sb->Append((const UTF8Char*)"public ");
	}
	if (method.accessFlags & 2)
	{
		sb->Append((const UTF8Char*)"private ");
	}
	if (method.accessFlags & 4)
	{
		sb->Append((const UTF8Char*)"protected ");
	}
	if (method.accessFlags & 8)
	{
		sb->Append((const UTF8Char*)"static ");
	}
	if (method.accessFlags & 0x10)
	{
		sb->Append((const UTF8Char*)"final ");
	}
	if (method.accessFlags & 0x20)
	{
		sb->Append((const UTF8Char*)"synchronized ");
	}
	if (method.accessFlags & 0x100)
	{
		sb->Append((const UTF8Char*)"native ");
	}
	if (method.accessFlags & 0x400)
	{
		sb->Append((const UTF8Char*)"abstract ");
	}
	if (method.signatureIndex != 0)
	{
		this->DetailNameType(method.nameIndex, method.signatureIndex, thisClass, 0, sb, 0, &method, 0, 0);
	}
	else
	{
		this->DetailNameType(method.nameIndex, method.descriptorIndex, thisClass, 0, sb, 0, &method, 0, 0);
	}
	this->MethodFree(&method);
	return true;
}

Bool IO::JavaClass::MethodsGetDetail(UOSInt index, UOSInt lev, Bool disasm, Text::StringBuilderUTF *sb)
{
	if (index >= this->methodCnt)
	{
		return false;
	}
	this->AppendCodeMethod(sb, index, lev, disasm, disasm, 0, 0);
	return true;
}

void IO::JavaClass::DecompileFile(Text::StringBuilderUTF *sb)
{
	Text::StringBuilderUTF8 sbTmp;
	if (!this->GetClassNameFull(&sbTmp))
	{
		return;
	}
	const UTF8Char *packageName = 0;
	Data::ArrayListStrUTF8 importList;
	UOSInt i = sbTmp.LastIndexOf('.');
	UOSInt j;
	if (i != INVALID_INDEX)
	{
		sb->Append((const UTF8Char*)"package ");
		sb->AppendC(sbTmp.ToString(), i);
		sb->Append((const UTF8Char*)";\r\n");
		packageName = Text::StrCopyNewC(sbTmp.ToString(), i);
	}
	sb->Append((const UTF8Char*)"\r\n");

	Text::StringBuilderUTF8 sbClass;
	this->AppendCodeClassContent(&sbClass, 0, sbTmp.ToString() + i + 1, &importList, packageName);

	if (importList.GetCount() > 0)
	{
		i = 0;
		j = importList.GetCount();
		while (i < j)
		{
			sb->Append((const UTF8Char*)"import ");
			sb->Append(importList.GetItem(i));
			sb->Append((const UTF8Char*)";\r\n");
			i++;
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
	sb->Append(sbClass.ToString());

	LIST_FREE_FUNC(&importList, Text::StrDelNew);
	SDEL_TEXT(packageName);
}

IO::JavaClass::EndType IO::JavaClass::DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, IO::JavaClass::DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF *sb)
{
	Text::StringBuilderUTF8 sbTmp;
	Text::StringBuilderUTF8 sbTmp2;
	Text::StringBuilderUTF8 sbTmp3;
	UInt16 classIndex;
	UInt16 val;
	UTF8Char sbuff[512];
	UTF8Char typeBuff[128];
	while (codePtr < codeEnd)
	{
		if (env->stacks->GetCount() != env->stackTypes->GetCount())
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"// stack and type count mismatch: ");
			sb->AppendUOSInt(env->stacks->GetCount());
			sb->Append((const UTF8Char*)" vs ");
			sb->AppendUOSInt(env->stackTypes->GetCount());
			sb->Append((const UTF8Char*)", next code = ");
			sb->AppendHex8(codePtr[0]);
			sb->Append((const UTF8Char*)"\r\n");
			return EndType::Error;
		}
		switch (codePtr[0])
		{
		case 0x00: //nop
			codePtr++;
			break;
		case 0x01: //aconst_null
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"null"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"Object"));
			codePtr++;
			break;
		case 0x02: //iconst_m1
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"-1"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x03: //iconst_0
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x04: //iconst_1
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"1"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x05: //iconst_2
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"2"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x06: //iconst_3
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"3"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x07: //iconst_4
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"4"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x08: //iconst_5
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"5"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x09: //lconst_0
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x0A: //lconst_1
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"1"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x0B: //fconst_0
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0.0f"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0C: //fconst_1
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"1.0f"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0D: //fconst_2
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"2.0f"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x0E: //dconst_0
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0.0"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x0F: //dconst_1
			env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"1.0"));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x10: //bipush
			sbTmp.ClearStr();
			sbTmp.AppendU16(codePtr[1]);
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr += 2;
			break;
		case 0x11: //sipush
			sbTmp.ClearStr();
			sbTmp.AppendI16(ReadMInt16(&codePtr[1]));
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
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
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x16: //lload
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x17: //fload
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x18: //dload
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x19: //aload
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x1A: //iload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewNotNull(sbuff));
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"this"));
			}
			this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x1B: //iload_1
			this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x1C: //iload_2
			this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x1D: //iload_3
			this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x1E: //lload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewNotNull(sbuff));
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"this"));
			}
			this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x1F: //lload_1
			this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x20: //lload_2
			this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x21: //lload_3
			this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x22: //fload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewNotNull(sbuff));
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"this"));
			}
			this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x23: //fload_1
			this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x24: //fload_2
			this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x25: //fload_3
			this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x26: //dload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewNotNull(sbuff));
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"this"));
			}
			this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x27: //dload_1
			this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x28: //dload_2
			this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x29: //dload_3
			this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x2A: //aload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewNotNull(sbuff));
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"this"));
			}
			this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x2B: //aload_1
			this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x2C: //aload_2
			this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x2D: //aload_3
			this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewNotNull(sbuff));
			this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewNotNull(sbuff));
			codePtr++;
			break;
		case 0x2E: //iaload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// iaload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();;
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0x2F: //laload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// laload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"long"));
			codePtr++;
			break;
		case 0x30: //faload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// faload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"float"));
			codePtr++;
			break;
		case 0x31: //daload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// daload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"double"));
			codePtr++;
			break;
		case 0x32: //aaload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// aaload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
			if (!sbTmp2.EndsWith((const UTF8Char*)"[]"))
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// aaload stack not array type: ");
				sb->Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp2.RemoveChars(2);
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			codePtr++;
			break;
		case 0x33: //baload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// baload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"byte"));
			codePtr++;
			break;
		case 0x34: //caload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// caload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"char"));
			codePtr++;
			break;
		case 0x35: //saload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// saload stack invalid: count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendChar('[', 1);
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"short"));
			codePtr++;
			break;
		case 0x36: //istore
			this->DecompileStore(codePtr[1], env, lev, sb, (UOSInt)(codePtr - env->codeStart + 2));
			codePtr += 2;
			break;
		case 0x37: //lstore
			this->DecompileStore(codePtr[1], env, lev, sb, (UOSInt)(codePtr - env->codeStart + 2));
			codePtr += 2;
			break;
		case 0x38: //fstore
			this->DecompileStore(codePtr[1], env, lev, sb, (UOSInt)(codePtr - env->codeStart + 2));
			codePtr += 2;
			break;
		case 0x39: //dstore
			this->DecompileStore(codePtr[1], env, lev, sb, (UOSInt)(codePtr - env->codeStart + 2));
			codePtr += 2;
			break;
		case 0x3A: //astore
			this->DecompileStore(codePtr[1], env, lev, sb, (UOSInt)(codePtr - env->codeStart + 2));
			codePtr += 2;
			break;
		case 0x3B: //istore_0
			this->DecompileStore(0, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x3C: //istore_1
			this->DecompileStore(1, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x3D: //istore_2
			this->DecompileStore(2, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x3E: //istore_3
			this->DecompileStore(3, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x3F: //lstore_0
			this->DecompileStore(0, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x40: //lstore_1
			this->DecompileStore(1, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x41: //lstore_2
			this->DecompileStore(2, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x42: //lstore_3
			this->DecompileStore(3, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x43: //fstore_0
			this->DecompileStore(0, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x44: //fstore_1
			this->DecompileStore(1, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x45: //fstore_2
			this->DecompileStore(2, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x46: //fstore_3
			this->DecompileStore(3, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x47: //dstore_0
			this->DecompileStore(0, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x48: //dstore_1
			this->DecompileStore(1, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x49: //dstore_2
			this->DecompileStore(2, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4A: //dstore_3
			this->DecompileStore(3, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4B: //astore_0
			this->DecompileStore(0, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4C: //astore_1
			this->DecompileStore(1, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4D: //astore_2
			this->DecompileStore(2, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4E: //astore_3
			this->DecompileStore(3, env, lev, sb, (UOSInt)(codePtr - env->codeStart + 1));
			codePtr++;
			break;
		case 0x4F: //iastore
			if (env->stacks->GetCount() < 3)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// iastore stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Text::String *nameStr = env->stacks->GetItem(env->stacks->GetCount() - 3);
				Text::String *indexStr = env->stacks->GetItem(env->stacks->GetCount() - 2);
				Text::String *valueStr = env->stacks->GetItem(env->stacks->GetCount() - 1);
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stacks->Pop();
				env->stacks->Pop();
				env->stacks->Pop();

				this->AppendIndent(sb, lev);
				sb->Append(nameStr);
				sb->AppendChar('[', 1);
				sb->Append(indexStr);
				sb->Append((const UTF8Char*)"] = ");
				sb->Append(valueStr);
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
				nameStr->Release();
				indexStr->Release();
				valueStr->Release();
			}
			
			codePtr++;
			break;
		case 0x52: //dastore
			if (env->stacks->GetCount() < 3)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// dastore stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Text::String *nameStr = env->stacks->GetItem(env->stacks->GetCount() - 3);
				Text::String *indexStr = env->stacks->GetItem(env->stacks->GetCount() - 2);
				Text::String *valueStr = env->stacks->GetItem(env->stacks->GetCount() - 1);
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stacks->Pop();
				env->stacks->Pop();
				env->stacks->Pop();

				this->AppendIndent(sb, lev);
				sb->Append(nameStr);
				sb->AppendChar('[', 1);
				sb->Append(indexStr);
				sb->Append((const UTF8Char*)"] = ");
				sb->Append(valueStr);
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
				nameStr->Release();
				indexStr->Release();
				valueStr->Release();
			}
			
			codePtr++;
			break;
		case 0x53: //aastore
			if (env->stacks->GetCount() < 3)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// aastore stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Text::String *nameStr = env->stacks->GetItem(env->stacks->GetCount() - 3);
				Text::String *indexStr = env->stacks->GetItem(env->stacks->GetCount() - 2);
				Text::String *valueStr = env->stacks->GetItem(env->stacks->GetCount() - 1);
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stacks->Pop();
				env->stacks->Pop();
				env->stacks->Pop();

				if (nameStr->StartsWith((const UTF8Char*)"new ") && env->stacks->GetCount() > 0 && env->stacks->GetItem(env->stacks->GetCount()- 1)->StartsWith(nameStr))
				{
					Text::String *arrStr = env->stacks->RemoveAt(env->stacks->GetCount()- 1);
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
						if (sbTmp.ToString()[sbTmp.GetLength() - 1] == ']')
						{
							UOSInt tmpI = sbTmp.LastIndexOf('[');
							if (tmpI != INVALID_INDEX)
							{
								sbTmp.TrimToLength(tmpI + 1);
								sbTmp.AppendChar(']', 1);
							}
						}
						sbTmp.Append((const UTF8Char*)" {");
						sbTmp.Append(valueStr);
						sbTmp.AppendChar('}', 1);
					}
					env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
					arrStr->Release();
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(nameStr);
					sb->AppendChar('[', 1);
					sb->Append(indexStr);
					sb->Append((const UTF8Char*)"] = ");
					sb->Append(valueStr);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
				}
				nameStr->Release();
				indexStr->Release();
				valueStr->Release();
			}
			
			codePtr++;
			break;
		case 0x54: //bastore
			if (env->stacks->GetCount() < 3)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// bastore stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Text::String *nameStr = env->stacks->GetItem(env->stacks->GetCount() - 3);
				Text::String *indexStr = env->stacks->GetItem(env->stacks->GetCount() - 2);
				Text::String *valueStr = env->stacks->GetItem(env->stacks->GetCount() - 1);
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stacks->Pop();
				env->stacks->Pop();
				env->stacks->Pop();

				if (nameStr->StartsWith((const UTF8Char*)"new ") && env->stacks->GetCount() > 0 && env->stacks->GetItem(env->stacks->GetCount()- 1)->StartsWith(nameStr))
				{
					Text::String *arrStr = env->stacks->RemoveAt(env->stacks->GetCount()- 1);
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
					env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
					arrStr->Release();
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(nameStr);
					sb->AppendChar('[', 1);
					sb->Append(indexStr);
					sb->Append((const UTF8Char*)"] = ");
					sb->Append(valueStr);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
				}
				nameStr->Release();
				indexStr->Release();
				valueStr->Release();
			}
			
			codePtr++;
			break;
		case 0x57: //pop
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// pop stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			break;
		case 0x59: //dup
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// dup stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			env->stacks->Add(env->stacks->GetItem(env->stacks->GetCount() - 1)->Clone());
			env->stackTypes->Add(env->stackTypes->GetItem(env->stackTypes->GetCount() - 1)->Clone());
			codePtr++;
			break;
		case 0x5A: //dup_x1
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// dup_x1 stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				UOSInt cnt = env->stacks->GetCount();
				env->stacks->Insert(cnt - 2, env->stacks->GetItem(cnt - 1)->Clone());
				env->stackTypes->Insert(cnt - 2, env->stackTypes->GetItem(cnt - 1)->Clone());
				codePtr++;
			}
			break;
		case 0x60: //iadd
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// iadd stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"<>?"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.Append((const UTF8Char*)" + ");
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"<>?"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x61: //ladd
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// ladd stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" + ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x62: //fadd
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// fadd stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" + ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x63: //dadd
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// dadd stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" + ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x64: //isub
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// isub stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" - ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x65: //lsub
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// lsub stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" - ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x66: //fsub
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// fsub stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" - ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x67: //dsub
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// dsub stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" - ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x68: //imul
		case 0x69: //lmul
		case 0x6A: //fmul
		case 0x6B: //dmul
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// mul stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.Append((const UTF8Char*)" * ");
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x6C: //idiv
		case 0x6D: //ldiv
		case 0x6E: //fdiv
		case 0x6F: //ddiv
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// div stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.Append((const UTF8Char*)" * ");
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x70: //irem
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// irem stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.Append((const UTF8Char*)" % ");
				s = env->stacks->Pop();
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x77: //dneg
			{
				if (env->stacks->GetCount() <= 0)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// dneg stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				sbTmp.ClearStr();
				sbTmp.AppendChar('-', 1);
				if (s->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				codePtr++;
			}
			break;
		case 0x78: //ishl
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// ishl stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.Append((const UTF8Char*)" << ");
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				codePtr++;
			}
			break;
		case 0x7C: //iushr
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// iushr stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.Append((const UTF8Char*)" >> ");
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				codePtr++;
			}
			break;
		case 0x7E: //iand
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// iand stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.Append((const UTF8Char*)" & ");
				if (s->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				codePtr++;
			}
			break;
		case 0x80: //ior
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// ior stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.Append((const UTF8Char*)" | ");
				if (s->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				codePtr++;
			}
			break;
		case 0x82: //ixor
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// ixor stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals((const UTF8Char*)"-1"))
				{
					sbTmp.AppendChar('~', 1);
					sbTmp.Append(s);
					s->Release();
					env->stacks->Pop()->Release();
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
					sbTmp.Append((const UTF8Char*)" % ");
					s = env->stacks->Pop();
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				codePtr++;
			}
			break;
		case 0x84: //iinc
			this->AppendIndent(sb, lev);
			this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)" += ");
			sb->AppendI16((Int16)codePtr[2]);
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			codePtr += 3;
			break;
		case 0x85: //i2l
			env->stackTypes->Pop()->Release();
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"long"));
			codePtr += 1;
			break;
		case 0x91: //i2b
			{
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"byte"));
				sbTmp.ClearStr();
				sbTmp.Append((const UTF8Char*)"(byte)");
				Text::String *s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-*/<>?|&"))
				{
					sbTmp.AppendChar('(', 1);
					sbTmp.Append(s);
					sbTmp.AppendChar(')', 1);
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				codePtr += 1;
				break;
			}
		case 0x94: //lcmp
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// lcmp stack invalid: ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				CondType ct;
				switch (codePtr[1])
				{
				case 0x99: //ifeq
					ct = CondType::EQ;
					break;
				case 0x9A: //ifne
					ct = CondType::NE;
					break;
				case 0x9B: //iflt
					ct = CondType::LT;
					break;
				case 0x9C: //ifge
					ct = CondType::GE;
					break;
				case 0x9D: //ifgt
					ct = CondType::GT;
					break;
				case 0x9E: //ifle
					ct = CondType::LE;
					break;
				default:
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// lcmp next opcode invalid: ");
					sb->AppendHex8(codePtr[1]);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				EndType et = this->DecompileCondBranch(codePtr + 4, codePtr + ReadMInt16(&codePtr[2]) + 1, ct, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x97: //dcmpl
		case 0x98: //dcmpg
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// lcmpl stack invalid: ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				CondType ct;
				switch (codePtr[1])
				{
				case 0x99: //ifeq
					ct = CondType::EQ;
					break;
				case 0x9A: //ifne
					ct = CondType::NE;
					break;
				case 0x9B: //iflt
					ct = CondType::LT;
					break;
				case 0x9C: //ifge
					ct = CondType::GE;
					break;
				case 0x9D: //ifgt
					ct = CondType::GT;
					break;
				case 0x9E: //ifle
					ct = CondType::LE;
					break;
				default:
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// lcmpl next opcode invalid: ");
					sb->AppendHex8(codePtr[1]);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				EndType et = this->DecompileCondBranch(codePtr + 4, codePtr + ReadMInt16(&codePtr[2]) + 1, ct, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x99: //ifeq
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifeq stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::EQ, true);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::EQ, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9A: //ifne
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifne stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::NE, true);
					sb->Append((const UTF8Char*)";");
					AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::NE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9B: //iflt
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// iflt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::LT, true);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::LT, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9C: //ifge
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::GE, true);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::GE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9D: //ifgt
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifgt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::GT, true);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::GT, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9E: //ifle
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifle stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"0"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"return ");
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::LE, true);
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
					env->stacks->Pop()->Release();
					env->stacks->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->stackTypes->Pop()->Release();
					env->endPtr = codePtr + 7;
					return EndType::Return;
				}
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + codeOfst, CondType::LE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0x9F: //if_icmpeq
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::EQ, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA0: //if_icmpne
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::NE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA1: //if_icmplt
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::LT, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA2: //if_icmpge
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmpge stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::GE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA3: //if_icmpgt
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmpgt stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::GT, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA4: //if_icmple
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_icmple stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::LE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA5: //if_acmpeq
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_acmpeq stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::EQ, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xA6: //if_acmpne
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// if_acmpne stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::NE, env, lev, sb);
				if (et == EndType::Error)
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
					return EndType::Goto;
				}
				if (codePtr + i >= codeEnd)
				{
					env->endPtr = codePtr + i;
					return EndType::Goto;
				}
				sbTmp.ClearStr();
				UOSInt stackCnt = env->stacks->GetCount();
				EndType et = EndType::CodeEnd;//this->DecompileCode(codePtr + 3, codePtr + i, env, lev + 1, &sbTmp);
				if (et == EndType::Error)
				{
					sb->Append(sbTmp.ToString());
					return et;
				}
				if (stackCnt == env->stacks->GetCount() && (et == EndType::CodeEnd || et == EndType::Throw || et == EndType::Return))
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"if (false)\r\n");
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"{\r\n");
					sb->Append(sbTmp.ToString());
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"}\r\n");
					codePtr = codePtr + i;
				}
				else
				{
					env->endPtr = codePtr + i;
					return EndType::Goto;
				}
			}
			break;
		case 0xAC: //ireturn
			{
				if (env->stacks->GetCount() <= 0)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// ireturn stack invalid");
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"return ");
				Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
				if (env->returnType && env->returnType->Equals((const UTF8Char*)"boolean"))
				{
					if (s->Equals((const UTF8Char*)"0"))
					{
						sb->Append((const UTF8Char*)"false");
					}
					else if (s->Equals((const UTF8Char*)"1"))
					{
						sb->Append((const UTF8Char*)"true");
					}
					else if (s->EndsWith((const UTF8Char*)"?1:0"))
					{
						sb->AppendC(s->v, s->leng - 4);
					}
					else
					{
						sb->Append(s);
					}
				}
				else
				{
					sb->Append(s);
				}
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				codePtr++;
				env->endPtr = codePtr;
				return EndType::Return;
			}
		case 0xAD: //lreturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// lreturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xAE: //freturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// freturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xAF: //dreturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// dreturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB0: //areturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// areturn stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"return ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB1: //return
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"return;");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB2: //getstatic
			val = ReadMUInt16(&codePtr[1]);
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// getstatic index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// getstatic const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
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
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// getstatic const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sbTmp2.ClearStr();
				this->AppendCodeClassName(&sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
				sbTmp2.AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sbTmp2.Append(sbuff);
				this->GetConstName(sbuff, nameTypeIndex);
				sbTmp.ClearStr();
				Type2String(sbuff, &sbTmp);
				env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			}
			codePtr += 3;
			break;
		case 0xB3: //putstatic
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putstatic stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putstatic index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putstatic const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
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
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// putstatic const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				this->AppendIndent(sb, lev);
				this->AppendCodeClassName(sb, sbTmp.ToString(), env->importList, env->packageName);
				sb->AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)" = ");
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
			}
			codePtr += 3;
			break;
		case 0xB4: //getfield
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// getfield stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// getfield index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// getfield const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				UInt16 classIndex;
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// getfield const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
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
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			}
			codePtr += 3;
			break;
		case 0xB5: //putfield
			val = ReadMUInt16(&codePtr[1]);
			if (env->stacks->GetCount() <= 1)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putfield stack invalid: ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putfield index invalid:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// putfield const pool not fieldref:");
				sb->AppendU16(val);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				UInt16 classIndex;
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					this->AppendIndent(sb, lev);
					sb->Append((const UTF8Char*)"// putfield const pool not nameAndType:");
					sb->AppendU16(nameTypeIndex);
					sb->Append((const UTF8Char*)"\r\n");
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				this->AppendIndent(sb, lev);
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
				sb->AppendChar('.', 1);
				this->GetConstName(sbuff, classIndex);
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)" = ");
				Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
				this->GetConstName(sbuff, nameTypeIndex);
				if (sbuff[0] == 'Z') //boolean
				{
					if (s->Equals((const UTF8Char*)"0"))
					{
						sb->Append((const UTF8Char*)"false");
					}
					else if (s->Equals((const UTF8Char*)"1"))
					{
						sb->Append((const UTF8Char*)"true");
					}
					else
					{
						sb->Append(s);
					}
				}
				else
				{
					sb->Append(s);
				}
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
				env->stacks->Pop()->Release();
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
			}
			codePtr += 3;
			break;
		case 0xB6: //invokevirtual
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append(sbTmp.ToString());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// invokevirtual Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendChar('.', 1);
				sbTmp2.Append(sbuff);
				sbTmp2.Append(sbTmp.ToString());
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNull(typeBuff));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
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
				this->AppendIndent(sb, lev);
				sb->Append(sbTmp.ToString());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// invokespecial Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				Bool isInit = false;
				sbTmp2.ClearStr();
				if (Text::StrEquals(sbuff, (const UTF8Char*)"<init>"))
				{
					isInit = true;
					if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals((const UTF8Char*)"this"))
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
					if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals((const UTF8Char*)"this"))
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
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNull(typeBuff));
				}
				else if (isInit && env->stacks->GetCount() > 0 && sbTmp2.StartsWith(env->stacks->GetItem(env->stacks->GetCount() - 1)->v))
				{
					env->stacks->Pop()->Release();
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
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
				this->AppendIndent(sb, lev);
				sb->Append(sbTmp.ToString());
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp3.ClearStr();
			this->ClassNameString(classIndex, &sbTmp3);
			this->AppendCodeClassName(&sbTmp2, sbTmp3.ToString(), env->importList, env->packageName);
			sbTmp2.AppendChar('.', 1);
			sbTmp2.Append(sbuff);
			sbTmp2.Append(sbTmp.ToString());
			if (typeBuff[0])
			{
				env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				env->stackTypes->Add(Text::String::NewNotNull(typeBuff));
			}
			else
			{
				this->AppendIndent(sb, lev);
				sb->Append(sbTmp2.ToString());
				sb->Append((const UTF8Char*)";");
				this->AppendLineNum(sb, env, codePtr);
				sb->Append((const UTF8Char*)"\r\n");
			}
			codePtr += 3;
			break;
		case 0xB9: //invokeinterface
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if (this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, &sbTmp) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append(sbTmp.ToString());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// invokeinterface Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendChar('.', 1);
				sbTmp2.Append(sbuff);
				sbTmp2.Append(sbTmp.ToString());
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNull(typeBuff));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(sbTmp2.ToString());
					sb->Append((const UTF8Char*)";");
					this->AppendLineNum(sb, env, codePtr);
					sb->Append((const UTF8Char*)"\r\n");
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
			this->AppendCodeClassName(&sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			codePtr += 3;
			break;
		case 0xBC: //newarray
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// newarray stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
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
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// newarray invalid type: ");
				sb->AppendU16(codePtr[1]);
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			codePtr += 2;
			break;
		case 0xBD: //anewarray
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// anewarray stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.Append((const UTF8Char*)"new ");
			this->AppendCodeClassName(&sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			sbTmp.Append((const UTF8Char*)"[]");
			sbTmp2.AppendChar('[', 1);
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.AppendChar(']', 1);
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			codePtr += 3;
			break;
		case 0xBE: //arraylength
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// arraylength Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.Append((const UTF8Char*)".length");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
			codePtr++;
			break;
		case 0xBF: //athrow
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// athrow Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"throw ");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)";");
			this->AppendLineNum(sb, env, codePtr);
			sb->Append((const UTF8Char*)"\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->endPtr = codePtr + 1;
			return EndType::Throw;
		case 0xC0: //checkcast
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// checkcast Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.AppendChar('(', 1);
			this->AppendCodeClassName(&sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			sbTmp2.AppendChar(')', 1);
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			codePtr += 3;
			break;
		case 0xC1: //instanceof
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, &sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.Append((const UTF8Char*)" instanceof ");
			this->AppendCodeClassName(&sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"boolean"));
			codePtr += 3;
			break;
		case 0xC2: //monitorenter
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// monitorenter Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"synchronized (");
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->Append((const UTF8Char*)")\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"{\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			lev++;
			codePtr++;
			break;
		case 0xC3: //monitorexit
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// monitorexit Stack error: stack count = ");
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			lev--;
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"}\r\n");
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			break;
		case 0xC6: //ifnull
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifnull stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"null"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"java.lang.Object"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::EQ, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xC7: //ifnonnull
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"// ifnonnull stack invalid");
				sb->Append((const UTF8Char*)"\r\n");
				return EndType::Error;
			}
			else
			{
				env->stacks->Add(Text::String::NewNotNull((const UTF8Char*)"null"));
				env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"java.lang.Object"));
				EndType et = this->DecompileCondBranch(codePtr + 3, codePtr + ReadMInt16(&codePtr[1]), CondType::NE, env, lev, sb);
				if (et == EndType::Error)
				{
					return et;
				}
				codePtr = env->endPtr;
			}
			break;
		case 0xC8: //goto_w
			env->endPtr = codePtr + ReadMInt32(&codePtr[1]);
			return EndType::Goto;
		default:
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"// Unsupported opcode ");
			sb->AppendHex8(codePtr[0]);
			sb->Append((const UTF8Char*)"\r\n");
			return EndType::Error;
		}
	}
	env->endPtr = codePtr;
	return EndType::CodeEnd;
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
		sb.AppendU32(ReadMUInt32(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"int"));
	}
	else if (ptr[0] == 4)
	{
		Text::SBAppendF32(&sb, ReadMFloat(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"float"));
	}
	else if (ptr[0] == 5)
	{
		sb.AppendU64(ReadMUInt64(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"long"));
	}
	else if (ptr[0] == 6)
	{
		Text::SBAppendF64(&sb, ReadMDouble(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"double"));
	}
	else if (ptr[0] == 7) //Class
	{
		UInt16 sindex = ReadMUInt16(&ptr[1]);
		UInt16 strLen;
		ptr = this->constPool[sindex];
		if (ptr[0] == 1)
		{
			strLen = ReadMUInt16(&ptr[1]);
			Text::StringBuilderUTF8 sbTmp;
			sbTmp.AppendC(ptr + 3, strLen);
			sbTmp.Replace('/', '.');
			this->AppendCodeClassName(&sb, sbTmp.ToString(), env->importList, env->packageName);
			sb.Append((const UTF8Char*)".class");
			env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"java.lang.Class"));
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
			env->stackTypes->Add(Text::String::NewNotNull((const UTF8Char*)"java.lang.String"));
		}
	}
}

void IO::JavaClass::DecompileStore(UInt16 index, IO::JavaClass::DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF *sb, UOSInt codeOfst)
{
	UTF8Char sbuff[256];
	this->AppendIndent(sb, lev);
	if (env->stacks->GetCount() <= 0)
	{
		sb->Append((const UTF8Char*)"// Store stack is empty\r\n");
		return;
	}
	if (env->localTypes[index] == 0)
	{
		this->GetLVType(sbuff, index, env->method, codeOfst, env->importList, env->packageName);
		env->localTypes[index] = Text::String::NewNotNull(sbuff);
		this->AppendCodeClassName(sb, env->localTypes[index]->v, env->importList, env->packageName);
		sb->AppendChar(' ', 1);
	}
	this->GetLVName(sbuff, index, env->method, codeOfst);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" = ");
	Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
	if (env->localTypes[index]->Equals((const UTF8Char*)"boolean"))
	{
		if (s->Equals((const UTF8Char*)"0"))
		{
			sb->Append((const UTF8Char*)"false");
		}
		else if (s->Equals((const UTF8Char*)"1"))
		{
			sb->Append((const UTF8Char*)"true");
		}
		else
		{
			sb->Append(s);
		}
	}
	else
	{
		sb->Append(s);
	}
	sb->Append((const UTF8Char*)";");
	this->AppendLineNum(sb, env, env->codeStart + codeOfst - 1);
	sb->Append((const UTF8Char*)"\r\n");
	env->stackTypes->Pop()->Release();
	env->stacks->Pop()->Release();
}

IO::JavaClass::EndType IO::JavaClass::DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, IO::JavaClass::DecompileEnv *env, UOSInt lev, Text::StringBuilderUTF *sb)
{
	if (codePtr > codeEnd)
	{
		Text::StringBuilderUTF8 sbTmp;
		UOSInt initStackCnt = env->stacks->GetCount();
		if (env->method && initStackCnt > env->method->maxStacks)
		{
			sb->Append((const UTF8Char*)"//Stack overflow\r\n");
			return EndType::Error;
		}
		EndType et = DecompileCode(codeEnd, codePtr - 3, env, lev + 1, &sbTmp);
		if (et == EndType::Error)
		{
			sb->Append(sbTmp.ToString());
			return et;
		}
		if (initStackCnt + 1 == env->stacks->GetCount() && (et == EndType::Return || et == EndType::Throw || et == EndType::CodeEnd))
		{
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"while (");
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, false);
			sb->Append((const UTF8Char*)")\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"}\r\n");
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->endPtr = codePtr;
			return et;
		}
		else if (initStackCnt + 2 == env->stacks->GetCount() && (et == EndType::CodeEnd))
		{
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"while (");
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, false);
			sb->Append((const UTF8Char*)")\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"}\r\n");
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->endPtr = codePtr;
			return et;
		}
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"// CondBranch: unknown type2: Stacks = ");
		sb->AppendUOSInt(initStackCnt);
		sb->Append((const UTF8Char*)", ");
		sb->AppendUOSInt(env->stacks->GetCount());
		sb->Append((const UTF8Char*)", et = ");
		sb->Append(EndTypeGetName(et));
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append(sbTmp.ToString());
		return EndType::Error;
	}
	else if (codePtr == codeEnd)
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"if (");
		AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
		sb->Append((const UTF8Char*)")\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"{\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"}\r\n");
		env->stacks->Pop()->Release();
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->endPtr = codePtr;
		return EndType::CodeEnd;
	}
	Text::StringBuilderUTF8 sbTmp;
	UOSInt initStackCnt = env->stacks->GetCount();
	EndType et = DecompileCode(codePtr, codeEnd, env, lev + 1, &sbTmp);
	if (et == EndType::Error)
	{
		sb->Append(sbTmp.ToString());
		return et;
	}
	if (initStackCnt + 1 == env->stacks->GetCount() && et == EndType::Goto && sbTmp.GetLength() == 0)
	{
		et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, &sbTmp);
		if (et == EndType::Error)
		{
			sb->Append(sbTmp.ToString());
			return et;
		}
		else if (et == EndType::CodeEnd && initStackCnt + 2 == env->stacks->GetCount() && sbTmp.GetLength() == 0)
		{
			sbTmp.ClearStr();
			sbTmp.AppendChar('(', 1);
			AppendCond(&sbTmp, env, initStackCnt - 2, ct, true);
			sbTmp.AppendChar(')', 1);
			sbTmp.AppendChar('?', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt + 0));
			sbTmp.AppendChar(':', 1);
			sbTmp.Append(env->stacks->GetItem(initStackCnt + 1));
			env->stacks->RemoveAt(initStackCnt + 1)->Release();
			env->stacks->RemoveAt(initStackCnt + 0)->Release();
			env->stacks->RemoveAt(initStackCnt - 1)->Release();
			env->stacks->RemoveAt(initStackCnt - 2)->Release();
			env->stackTypes->RemoveAt(initStackCnt + 0)->Release();
			env->stackTypes->RemoveAt(initStackCnt - 1)->Release();
			env->stackTypes->RemoveAt(initStackCnt - 2)->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			return EndType::CodeEnd;
		}
	}
	else if (initStackCnt == env->stacks->GetCount() && et == EndType::Goto)
	{
		if (env->endPtr < codePtr)
		{
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"while (");
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
			sb->Append((const UTF8Char*)")\r\n");
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"{\r\n");
			sb->Append(sbTmp.ToString());
			this->AppendIndent(sb, lev);
			sb->Append((const UTF8Char*)"}\r\n");
			env->stacks->RemoveAt(initStackCnt - 1)->Release();
			env->stacks->RemoveAt(initStackCnt - 2)->Release();
			env->stackTypes->RemoveAt(initStackCnt - 1)->Release();
			env->stackTypes->RemoveAt(initStackCnt - 2)->Release();
			env->endPtr = codeEnd;
			return EndType::CodeEnd;
		}
		else
		{
			Text::StringBuilderUTF8 sbTmp2;
			et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, &sbTmp2);
			if (et == EndType::Error)
			{
				sb->Append(sbTmp2.ToString());
				return et;
			}
			else if (et == EndType::CodeEnd && initStackCnt == env->stacks->GetCount())
			{
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"if (");
				AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
				sb->Append((const UTF8Char*)")\r\n");
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"{\r\n");
				sb->Append(sbTmp.ToString());
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"}\r\n");
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"else\r\n");
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"{\r\n");
				sb->Append(sbTmp2.ToString());
				this->AppendIndent(sb, lev);
				sb->Append((const UTF8Char*)"}\r\n");
				env->stacks->RemoveAt(initStackCnt - 1)->Release();
				env->stacks->RemoveAt(initStackCnt - 2)->Release();
				env->stackTypes->RemoveAt(initStackCnt - 1)->Release();
				env->stackTypes->RemoveAt(initStackCnt - 2)->Release();
				return EndType::CodeEnd;
			}
		}
	}
	else if (initStackCnt == env->stacks->GetCount() && (et == EndType::Return || et == EndType::Throw || et == EndType::CodeEnd))
	{
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"if (");
		AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
		sb->Append((const UTF8Char*)")\r\n");
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"{\r\n");
		sb->Append(sbTmp.ToString());
		this->AppendIndent(sb, lev);
		sb->Append((const UTF8Char*)"}\r\n");
		env->stacks->Pop()->Release();
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->endPtr = codeEnd;
		return et;
	}

	this->AppendIndent(sb, lev);
	sb->Append((const UTF8Char*)"// CondBranch: unknown type: Stacks = ");
	sb->AppendUOSInt(initStackCnt);
	sb->Append((const UTF8Char*)", ");
	sb->AppendUOSInt(env->stacks->GetCount());
	sb->Append((const UTF8Char*)", et = ");
	sb->Append(EndTypeGetName(et));
	sb->Append((const UTF8Char*)"\r\n");
	return EndType::Error;
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
		sb->AppendUOSInt(paramCnt);
		sb->Append((const UTF8Char*)", stack count = ");
		sb->AppendUOSInt(env->stacks->GetCount());
		sb->Append((const UTF8Char*)"\r\n");
		return 0;
	}

	UOSInt i;
	Text::String *paramStr;
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
			if (paramStr->Equals((const UTF8Char*)"1"))
			{
				sb->Append((const UTF8Char*)"true");
			}
			else if (paramStr->Equals((const UTF8Char*)"0"))
			{
				sb->Append((const UTF8Char*)"false");
			}
			else
			{
				sb->Append(paramStr);
			}
		}
		else if (params[i] == 'C') //char
		{
			UTF8Char sbuff[32];
			UTF8Char sbuff2[32];
			UInt32 c;
			if (paramStr->ToUInt32(&c))
			{
				Text::StrWriteChar(sbuff, (UTF32Char)c)[0] = 0;
				Text::JSText::ToJSText(sbuff2, sbuff);
				sb->Append(sbuff2);
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
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
	}
	return this->GetConstName(nameBuff, ReadMUInt16(&constPtr[1]));
}

IO::JavaClass *IO::JavaClass::ParseFile(const UTF8Char *fileName)
{
	IO::FileStream *fs;
	IO::JavaClass *cls = 0;
	UInt64 fileLen;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fileLen = fs->GetLength();
	if (fileLen >= 26 && fileLen <= 1048576)
	{
		UInt8 *buff = MemAlloc(UInt8, (UOSInt)fileLen);
		if (fs->Read(buff, (UOSInt)fileLen) == fileLen)
		{
			cls = ParseBuff(fileName, buff, (UOSInt)fileLen);
		}
		MemFree(buff);
	}
	DEL_CLASS(fs);
	return cls;
}

IO::JavaClass *IO::JavaClass::ParseBuff(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize)
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

IO::JavaClass *IO::JavaClass::ParseBuff(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize)
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

const UTF8Char *IO::JavaClass::EndTypeGetName(EndType et)
{
	switch (et)
	{
	case EndType::Error:
		return (const UTF8Char*)"Error";
	case EndType::CodeEnd:
		return (const UTF8Char*)"CodeEnd";
	case EndType::Return:
		return (const UTF8Char*)"Return";
	case EndType::Goto:
		return (const UTF8Char*)"Goto";
	case EndType::Throw:
		return (const UTF8Char*)"Throw";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
