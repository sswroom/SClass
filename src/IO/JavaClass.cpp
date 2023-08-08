#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/JavaClass.h"
#include "Text/JSText.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

const UInt8 *IO::JavaClass::Type2String(const UInt8 *typeStr, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	return AppendCodeType2String(sb, typeStr, 0, 0);
}

Text::CString IO::JavaClass::CondType2String(CondType ct)
{
	switch (ct)
	{
	case CondType::EQ:
		return CSTR("==");
	case CondType::NE:
		return CSTR("!=");
	case CondType::LE:
		return CSTR("<=");
	case CondType::GE:
		return CSTR(">=");
	case CondType::LT:
		return CSTR("<");
	case CondType::GT:
		return CSTR(">");
	default:
		return CSTR("?");
	}
}

Text::CString IO::JavaClass::CondType2IString(CondType ct)
{
	switch (ct)
	{
	case CondType::EQ:
		return CSTR("!=");
	case CondType::NE:
		return CSTR("==");
	case CondType::LE:
		return CSTR(">");
	case CondType::GE:
		return CSTR("<");
	case CondType::LT:
		return CSTR(">=");
	case CondType::GT:
		return CSTR("<=");
	default:
		return CSTR("?");
	}
}

void IO::JavaClass::DetailAccessFlags(UInt16 accessFlags, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("0x"));
	sb->AppendHex16(accessFlags);
	if (accessFlags & 1) //ACC_PUBLIC
	{
		sb->AppendC(UTF8STRC(" Public"));
	}
	if (accessFlags & 2) //ACC_PRIVATE
	{
		sb->AppendC(UTF8STRC(" Private"));
	}
	if (accessFlags & 4) //ACC_PROTECTED
	{
		sb->AppendC(UTF8STRC(" Protected"));
	}
	if (accessFlags & 8) //ACC_STATIC
	{
		sb->AppendC(UTF8STRC(" Static"));
	}
	if (accessFlags & 0x10) //ACC_FINAL
	{
		sb->AppendC(UTF8STRC(" Final"));
	}
	if (accessFlags & 0x20) //ACC_SUPER
	{
		sb->AppendC(UTF8STRC(" Super"));
	}
	if (accessFlags & 0x40) //ACC_VOLATILE
	{
		sb->AppendC(UTF8STRC(" Volatile"));
	}
	if (accessFlags & 0x80) //ACC_TRANSIENT
	{
		sb->AppendC(UTF8STRC(" Transient"));
	}
	if (accessFlags & 0x0100) //ACC_NATIVE
	{
		sb->AppendC(UTF8STRC(" Interface"));
	}
	if (accessFlags & 0x0200) //ACC_INTERFACE
	{
		sb->AppendC(UTF8STRC(" Interface"));
	}
	if (accessFlags & 0x0400) //ACC_ABSTRACT
	{
		sb->AppendC(UTF8STRC(" Abstract"));
	}
	if (accessFlags & 0x0800) //ACC_STRICT
	{
		sb->AppendC(UTF8STRC(" Strict"));
	}
	if (accessFlags & 0x1000) //ACC_SYNTHETIC
	{
		sb->AppendC(UTF8STRC(" Synthetic"));
	}
	if (accessFlags & 0x2000) //ACC_ANNOTATION
	{
		sb->AppendC(UTF8STRC(" Annotation"));
	}
	if (accessFlags & 0x4000) //ACC_ENUM
	{
		sb->AppendC(UTF8STRC(" Enum"));
	}
}

void IO::JavaClass::AppendCond(NotNullPtr<Text::StringBuilderUTF8> sb, DecompileEnv *env, UOSInt index, CondType ct, Bool inv)
{
	if (env->stackTypes->GetItem(index)->Equals(UTF8STRC("boolean")))
	{
		Text::String *s = env->stacks->GetItem(index);
		if ((ct == CondType::NE && !inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("0"))) ||
			(ct == CondType::EQ && inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("0"))))
		{
			sb->Append(s);
			return;	
		}
		else if ((ct == CondType::EQ && !inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("0"))) ||
				(ct == CondType::NE && inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("0"))))
		{
			sb->AppendUTF8Char('!');
			if (s->IndexOf(UTF8STRC(" instanceof ")) != INVALID_INDEX)
			{
				sb->AppendUTF8Char('(');
				sb->Append(s);
				sb->AppendUTF8Char(')');
			}
			else
			{
				sb->Append(s);
			}
			return;	
		}
		else if ((ct == CondType::EQ && inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("1"))) ||
				(ct == CondType::NE && !inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("1"))))
		{
			if (s->IndexOf(UTF8STRC(" instanceof ")) != INVALID_INDEX || s->ContainChars((const UTF8Char*)"><=|&^"))
			{
				sb->AppendUTF8Char('(');
				sb->Append(s);
				sb->AppendUTF8Char(')');
			}
			else
			{
				sb->Append(s);
			}
			sb->Append(env->stacks->GetItem(index));
			sb->AppendC(UTF8STRC(" != true"));
			return;	
		}
		else if ((ct == CondType::EQ && !inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("1"))) ||
				(ct == CondType::NE && inv && env->stacks->GetItem(index + 1)->Equals(UTF8STRC("1"))))
		{
			if (s->IndexOf(UTF8STRC(" instanceof ")) != INVALID_INDEX || s->ContainChars((const UTF8Char*)"><=|&^"))
			{
				sb->AppendUTF8Char('(');
				sb->Append(s);
				sb->AppendUTF8Char(')');
			}
			else
			{
				sb->Append(s);
			}
			sb->AppendC(UTF8STRC(" == true"));
			return;	
		}
	}
	sb->Append(env->stacks->GetItem(index));
	sb->AppendUTF8Char(' ');
	if (inv)
	{
		sb->Append(CondType2IString(ct));
	}
	else
	{
		sb->Append(CondType2String(ct));
	}
	sb->AppendUTF8Char(' ');
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

void IO::JavaClass::AppendIndent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev) const
{
	sb->AppendChar('\t', lev);
}

void IO::JavaClass::AppendLineNum(NotNullPtr<Text::StringBuilderUTF8> sb, DecompileEnv *env, const UInt8 *codePtr) const
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
		sb->AppendC(UTF8STRC(" // LineNum "));
		sb->AppendU16(lineNum->lineNumber);
	}
}

const UInt8 *IO::JavaClass::DetailAttribute(const UInt8 *attr, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt32 len = ReadMUInt32(&attr[2]);
	UInt16 nameIndex = ReadMUInt16(&attr[0]);
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("Attr Name Index = "));
	sb->AppendU16(nameIndex);
	sbuff[0] = 0;
	if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
	{
		sb->AppendC(UTF8STRC(" ("));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendUTF8Char(')');
	}
	else
	{
		sptr = sbuff;
	}
	sb->AppendC(UTF8STRC("\r\n"));
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("Attr Length = "));
	sb->AppendU32(len);
	sb->AppendC(UTF8STRC("\r\n"));
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ConstantValue")))
	{
		UInt16 i = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr ConstantValue index = "));
		sb->AppendU16(i);
		this->DetailConstVal(i, sb, true);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Code")))
	{
		const UInt8 *ptr;
		UInt32 codeLen = ReadMUInt32(&attr[10]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr Max Stack = "));
		sb->AppendU16(ReadMUInt16(&attr[6]));
		sb->AppendC(UTF8STRC("\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr Max Locals = "));
		sb->AppendU16(ReadMUInt16(&attr[8]));
		sb->AppendC(UTF8STRC("\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr Code Length = "));
		sb->AppendU32(codeLen);
		sb->AppendC(UTF8STRC("\r\n"));
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
			sb->AppendC(UTF8STRC("Attr Ex "));
			sb->AppendU16(i);
			sb->AppendC(UTF8STRC(" start_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->AppendC(UTF8STRC("\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr Ex "));
			sb->AppendU16(i);
			sb->AppendC(UTF8STRC(" end_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr Ex "));
			sb->AppendU16(i);
			sb->AppendC(UTF8STRC(" handler_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->AppendC(UTF8STRC("\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr Ex "));
			sb->AppendU16(i);
			sb->AppendC(UTF8STRC(" catch_type = "));
			type = ReadMUInt16(&ptr[6]);
			sb->AppendU16(type);
			if (type != 0)
			{
				this->DetailClassName(type, sb);
			}
			sb->AppendC(UTF8STRC("\r\n"));
			i++;
			ptr += 8;
		}
		UInt16 attrCount = ReadMUInt16(&ptr[0]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr attributes_count = "));
		sb->AppendU16(attrCount);
		sb->AppendC(UTF8STRC("\r\n"));
		ptr += 2;
		i = 0;
		while (i < attrCount)
		{
			ptr = this->DetailAttribute(ptr, lev + 1, sb);
			i++;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Exceptions")))
	{
		UInt16 number_of_exceptions = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr number_of_exceptions = "));
		sb->AppendU16(number_of_exceptions);
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < number_of_exceptions)
		{
			UInt16 clsIndex = ReadMUInt16(&ptr[0]);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr exception index = "));
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			i++;
			ptr += 2;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("InnerClasses")))
	{
		UInt16 number_of_classes = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr number_of_classes = "));
		sb->AppendU16(number_of_classes);
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < number_of_classes)
		{
			UInt16 clsIndex;
			clsIndex = ReadMUInt16(&ptr[0]);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr inner class info index = "));
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->AppendC(UTF8STRC("\r\n"));

			clsIndex = ReadMUInt16(&ptr[2]);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr outer class info index = "));
			sb->AppendU16(clsIndex);
			this->DetailClassName(clsIndex, sb);
			sb->AppendC(UTF8STRC("\r\n"));

			clsIndex = ReadMUInt16(&ptr[4]);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr inner name index = "));
			sb->AppendU16(clsIndex);
			if ((sptr = this->GetConstName(sbuff, clsIndex)) != 0)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendUTF8Char(')');
			}
			sb->AppendC(UTF8STRC("\r\n"));

			clsIndex = ReadMUInt16(&ptr[6]);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr inner class access flags = "));
			this->DetailAccessFlags(clsIndex, sb);
			sb->AppendC(UTF8STRC("\r\n"));

			i++;
			ptr += 8;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EnclosingMethod")))
	{
		UInt16 clsIndex;
		const UInt8 *ptr = &attr[6];
		clsIndex = ReadMUInt16(&ptr[0]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr class index = "));
		sb->AppendU16(clsIndex);
		this->DetailClassName(clsIndex, sb);
		sb->AppendC(UTF8STRC("\r\n"));

		clsIndex = ReadMUInt16(&ptr[2]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr method index = "));
		sb->AppendU16(clsIndex);
		if (clsIndex != 0)
		{
			this->DetailNameAndType(clsIndex, ReadMUInt16(&ptr[0]), sb);
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LineNumberTable")))
	{
		UInt16 line_number_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr line_number_table_length = "));
		sb->AppendU16(line_number_table_length);
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < line_number_table_length)
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr start_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->AppendC(UTF8STRC(", line_number = "));
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->AppendC(UTF8STRC("\r\n"));
			i++;
			ptr += 4;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LocalVariableTable")))
	{
		UInt16 local_variable_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr local_variable_table_length = "));
		sb->AppendU16(local_variable_table_length);
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < local_variable_table_length)
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr start_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->AppendC(UTF8STRC(", length = "));
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->AppendC(UTF8STRC(", name_index = "));
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->AppendC(UTF8STRC(", descriptor_index = "));
			sb->AppendU16(ReadMUInt16(&ptr[6]));
			sb->AppendC(UTF8STRC(", index = "));
			sb->AppendU16(ReadMUInt16(&ptr[8]));

			if ((sptr = this->GetConstName(sbuff, ReadMUInt16(&ptr[4]))) != 0)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC(")"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			i++;
			ptr += 10;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LocalVariableTypeTable")))
	{
		UInt16 local_variable_type_table_length = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr local_variable_type_table_length = "));
		sb->AppendU16(local_variable_type_table_length);
		sb->AppendC(UTF8STRC("\r\n"));
		while (i < local_variable_type_table_length)
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Attr start_pc = "));
			sb->AppendU16(ReadMUInt16(&ptr[0]));
			sb->AppendC(UTF8STRC(", length = "));
			sb->AppendU16(ReadMUInt16(&ptr[2]));
			sb->AppendC(UTF8STRC(", name_index = "));
			sb->AppendU16(ReadMUInt16(&ptr[4]));
			sb->AppendC(UTF8STRC(", signature_index = "));
			sb->AppendU16(ReadMUInt16(&ptr[6]));
			sb->AppendC(UTF8STRC(", index = "));
			sb->AppendU16(ReadMUInt16(&ptr[8]));

			if ((sptr = this->GetConstName(sbuff, ReadMUInt16(&ptr[4]))) != 0)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC(")"));
			}
			sb->AppendC(UTF8STRC("\r\n"));
			i++;
			ptr += 10;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Signature")))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr Signature Index = "));
		sb->AppendU16(snameIndex);
		this->DetailName(snameIndex, sb, true);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("SourceFile")))
	{
		UInt16 snameIndex = ReadMUInt16(&attr[6]);
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr SourceFile Index = "));
		sb->AppendU16(snameIndex);
		this->DetailName(snameIndex, sb, true);
		sb->AppendC(UTF8STRC("\r\n"));
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RuntimeVisibleAnnotations")))
	{
		UInt16 num_annotations = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr RuntimeVisibleAnnotations num_annotations = "));
		sb->AppendU16(num_annotations);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendHexBuff(&attr[6], len, ' ', Text::LineBreakType::CRLF);
		sb->AppendC(UTF8STRC("\r\n"));

		while (i < num_annotations)
		{
			this->AppendIndent(sb, lev + 1);
			ptr = this->DetailAnnotation(ptr, &attr[6 + len], sb, 0, 0);
			sb->AppendC(UTF8STRC("\r\n"));

			i++;
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("StackMapTable")))
	{
		UInt16 number_of_entries = ReadMUInt16(&attr[6]);
		UInt16 i = 0;
		const UInt8 *ptr = &attr[8];
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr StackMapTable number_of_entries = "));
		sb->AppendU16(number_of_entries);
		sb->AppendC(UTF8STRC("\r\n"));

		while (i < number_of_entries)
		{
			ptr = this->DetailStackMapFrame(ptr, &attr[6 + len], lev + 1, sb);

			i++;
		}
	}
	else
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Attr "));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendC(UTF8STRC(" (not supported)\r\n"));
	}

	attr += 6 + len;
	return attr;
}

void IO::JavaClass::DetailConstVal(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb, Bool brankets) const
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
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		sb->AppendU32(ReadMUInt32(&ptr[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (ptr[0] == 4)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		Text::SBAppendF32(sb, ReadMFloat(&ptr[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (ptr[0] == 5)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		sb->AppendU64(ReadMUInt64(&ptr[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (ptr[0] == 6)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		Text::SBAppendF64(sb, ReadMDouble(&ptr[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
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
				sb->AppendUTF8Char(' ');
				sb->AppendUTF8Char('(');
			}
			sb->AppendC(ptr + 3, strLen);
			if (brankets)
			{
				sb->AppendUTF8Char(')');
			}
		}
	}
	else if (ptr[0] == 1) //String
	{
		UInt16 strLen;
		strLen = ReadMUInt16(&ptr[1]);
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
			sb->AppendC(ptr + 3, strLen);
			sb->AppendUTF8Char(')');
		}
		else
		{
			const UTF8Char *s = Text::StrCopyNewC(ptr + 3, strLen).Ptr();
			NotNullPtr<Text::String> j = Text::JSText::ToNewJSTextDQuote(s);
			sb->Append(j);
			j->Release();
			Text::StrDelNew(s);
		}
	}
}

void IO::JavaClass::DetailName(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb, Bool brankets) const
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
		sb->AppendUTF8Char(' ');
		sb->AppendUTF8Char('(');
	}
	sb->AppendC(ptr + 3, strLen);
	if (brankets)
	{
		sb->AppendUTF8Char(')');
	}
}

void IO::JavaClass::DetailClassName(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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

void IO::JavaClass::DetailClassNameStr(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
	sb->AppendUTF8Char(' ');
	sb->AppendUTF8Char('(');
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
				sb->AppendUTF8Char('.');
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	}
	sb->AppendUTF8Char(')');
}

void IO::JavaClass::DetailFieldRef(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
	sb->AppendUTF8Char(' ');
	while (ptr < strEnd)
	{
		c = *ptr++;
		if (c == '/')
		{
			sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
			sb->AppendUTF8Char('.');
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	sb->AppendUTF8Char(',');
	this->DetailNameAndType(nameIndex, this->thisClass, sb);
}

void IO::JavaClass::DetailMethodRef(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
	sb->AppendUTF8Char(' ');
	while (ptr < strEnd)
	{
		c = *ptr++;
		if (c == '/' || c == '$')
		{
			sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
			sb->AppendUTF8Char('.');
			strStart = ptr;
		}
	}
	sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	sb->AppendUTF8Char(',');
	this->DetailNameAndType(nameIndex, classIndex, sb);
}

Bool IO::JavaClass::MethodGetReturnType(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
			ptr = Type2String(ptr, sbTmp);
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

void IO::JavaClass::DetailNameAndType(UInt16 index, UInt16 classIndex, NotNullPtr<Text::StringBuilderUTF8> sb) const
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

void IO::JavaClass::DetailNameType(UInt16 nameIndex, UInt16 typeIndex, UInt16 classIndex, const UTF8Char *prefix, NotNullPtr<Text::StringBuilderUTF8> sb, UTF8Char *typeBuff, MethodInfo *method, Data::ArrayListString *importList, const UTF8Char *packageName) const
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
	UTF8Char *sptr;
	Data::ArrayList<Text::String*> typeNames;
	Text::StringBuilderUTF8 sbParam;
	Text::StringBuilderUTF8 sbTmp;
	const UInt8 *ptr = this->constPool[typeIndex];
	UInt16 strLen = ReadMUInt16(&ptr[1]);
	const UInt8 *strEnd;
	UInt32 paramIndex;
	UInt32 paramId;
	UOSInt i;
	UOSInt j;
	Text::String *paramName;
	if (method)
	{
		i = 0;
		j = method->lvList->GetCount();
		while (i < j)
		{
			if ((sptr = this->GetConstName(sbuff, method->lvList->GetItem(i)->nameIndex)) != 0)
			{
				typeNames.Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr());
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
		sbParam.AppendUTF8Char('(');
		while (ptr < strEnd && ptr[0] != ')')
		{
			if (sbParam.GetLength() > 1)
			{
				sbParam.AppendUTF8Char(',');
				sbParam.AppendUTF8Char(' ');
			}
			sbTmp.ClearStr();
			ptr = Type2String(ptr, sbTmp);
			this->AppendCodeClassName(sbParam, sbTmp.ToString(), importList, packageName);
			sbParam.AppendUTF8Char(' ');
			paramId = GetParamId(paramIndex, method);
			paramName = typeNames.GetItem(paramId);
			if (paramName)
			{
				sbParam.Append(paramName);
			}
			else
			{
				sbParam.AppendUTF8Char('v');
				sbParam.AppendU32(paramId);
			}
			paramIndex++;
		}
		sbParam.AppendUTF8Char(')');
		ptr++;
	}
	i = typeNames.GetCount();
	while (i-- > 0)
	{
		paramName = typeNames.GetItem(i);
		SDEL_STRING(paramName);
	}
	if (ptr >= strEnd)
	{
		return;
	}
	if (prefix)
	{
		sb->AppendSlow(prefix);
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
		sb->AppendUTF8Char(' ');
		sb->AppendC(&ptr2[3], strLen);
	}
	sb->AppendC(sbParam.ToString(), sbParam.GetLength());
}

void IO::JavaClass::DetailType(UInt16 typeIndex, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const
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

void IO::JavaClass::DetailCode(const UInt8 *codePtr, UOSInt codeLen, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("nop\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x01:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aconst_null\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x02:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_m1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x03:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x04:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x05:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x06:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x07:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_4\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x08:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iconst_5\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x09:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lconst_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lconst_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fconst_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fconst_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fconst_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dconst_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x0F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dconst_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x10:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("bipush "));
			val = codePtr[codeOfst + 1];
			sb->AppendU16(val);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x11:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("sipush "));
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x12:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("ldc "));
			val = codePtr[codeOfst + 1];
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x13:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ldc_w "));
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x14:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ldc2_w "));
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendU16(val);
			this->DetailConstVal(val, sb, true);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x15:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("iload "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x16:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("lload "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x17:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("fload "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x18:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("dload "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x19:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("aload "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x1A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iload_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x1B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iload_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x1C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iload_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x1D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iload_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x1E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lload_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x1F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lload_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x20:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lload_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x21:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lload_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x22:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fload_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x23:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fload_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x24:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fload_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x25:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fload_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x26:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dload_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x27:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dload_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x28:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dload_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x29:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dload_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aload_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aload_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aload_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aload_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iaload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x2F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("laload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x30:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("faload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x31:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("daload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x32:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aaload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x33:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("baload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x34:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("caload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x35:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("saload\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x36:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("istore "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x37:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("lstore "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x38:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("fstore "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x39:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("dstore "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x3A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("astore "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0x3B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("istore_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x3C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("istore_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x3D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("istore_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x3E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("istore_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x3F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lstore_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x40:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lstore_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x41:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lstore_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x42:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lstore_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x43:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fstore_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x44:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fstore_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x45:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fstore_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x46:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fstore_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x47:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dstore_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x48:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dstore_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x49:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dstore_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dstore_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("astore_0\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("astore_1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("astore_2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("astore_3\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x4F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x50:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x51:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x52:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x53:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("aastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x54:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("bastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x55:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("castore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x56:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("sastore\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x57:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("pop\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x58:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("pop2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x59:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup_x1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup_x2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup2_x1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dup2_x2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x5F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("swap\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x60:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iadd\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x61:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ladd\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x62:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fadd\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x63:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dadd\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x64:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("isub\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x65:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lsub\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x66:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fsub\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x67:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dsub\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x68:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("imul\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x69:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lmul\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fmul\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dmul\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("idiv\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ldiv\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fdiv\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x6F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ddiv\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x70:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("irem\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x71:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lrem\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x72:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("frem\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x73:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("drem\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x74:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ineg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x75:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lneg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x76:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fneg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x77:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dneg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x78:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ishl\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x79:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lshl\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ishr\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lshr\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iushr\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lushr\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("iand\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x7F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("land\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x80:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ior\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x81:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lor\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x82:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ixor\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x83:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lxor\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x84:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("iinc "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC(", "));
			sb->AppendI16((Int8)codePtr[codeOfst + 2]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x85:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2l\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x86:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2f\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x87:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2d\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x88:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("l2i\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x89:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("l2f\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("l2d\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("f2i\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("f2l\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("f2d\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("d2i\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x8F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("d2l\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x90:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("d2f\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x91:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2b\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x92:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2c\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x93:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("i2s\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x94:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lcmp\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x95:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fcmpl\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x96:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("fcmpg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x97:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dcmpl\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x98:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dcmpg\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0x99:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifeq "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9A:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifne "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9B:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("iflt "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9C:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifge "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9D:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifgt "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9E:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifle "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0x9F:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmpeq "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmpne "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmplt "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmpge "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmpgt "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_icmple "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_acmpeq "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("if_acmpne "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("goto "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("jsr "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xA9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			sb->AppendC(UTF8STRC("ret "));
			sb->AppendU16(codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0xAA:
			{
				this->AppendIndent(sb, lev);
				sb->AppendUOSInt(codeOfst);
				sb->AppendUTF8Char('\t');
				UOSInt currOfst = codeOfst + 1;
				while (currOfst & 3)
				{
					currOfst += 1;
				}
				Int32 defaultPC = ReadMInt32(&codePtr[currOfst]);
				UInt32 npairs = ReadMUInt32(&codePtr[currOfst + 4]);
				sb->AppendHexBuff(&codePtr[codeOfst], (currOfst + 8 - codeOfst), ' ', Text::LineBreakType::None);
				sb->AppendUTF8Char(' ');
				sb->AppendC(UTF8STRC("tableswitch default = "));
				sb->AppendI32(defaultPC);
				sb->AppendC(UTF8STRC(", npairs = "));
				sb->AppendU32(npairs);
				sb->AppendC(UTF8STRC("\r\n"));
				codeLen -= (currOfst + 8 - codeOfst);
				codeOfst = currOfst + 8;
				UInt32 currIndex = 0;
				while (currIndex < npairs && codeLen >= 8)
				{
					this->AppendIndent(sb, lev);
					sb->AppendUOSInt(codeOfst);
					sb->AppendUTF8Char('\t');
					sb->AppendHexBuff(&codePtr[codeOfst], 8, ' ', Text::LineBreakType::None);
					sb->AppendUTF8Char(' ');
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst]));
					sb->AppendC(UTF8STRC(": "));
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 4]));
					sb->AppendC(UTF8STRC("\r\n"));
					currIndex++;
					codeOfst += 8;
					codeLen -= 8;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			break;
		case 0xAB:
			{
				this->AppendIndent(sb, lev);
				sb->AppendUOSInt(codeOfst);
				sb->AppendUTF8Char('\t');
				UOSInt currOfst = codeOfst + 1;
				while (currOfst & 3)
				{
					currOfst += 1;
				}
				Int32 defaultPC = ReadMInt32(&codePtr[currOfst]);
				UInt32 npairs = ReadMUInt32(&codePtr[currOfst + 4]);
				sb->AppendHexBuff(&codePtr[codeOfst], (currOfst + 8 - codeOfst), ' ', Text::LineBreakType::None);
				sb->AppendUTF8Char(' ');
				sb->AppendC(UTF8STRC("lookupswitch default = "));
				sb->AppendI32(defaultPC);
				sb->AppendC(UTF8STRC(", npairs = "));
				sb->AppendU32(npairs);
				sb->AppendC(UTF8STRC("\r\n"));
				codeLen -= (currOfst + 8 - codeOfst);
				codeOfst = currOfst + 8;
				UInt32 currIndex = 0;
				while (currIndex < npairs && codeLen >= 8)
				{
					this->AppendIndent(sb, lev);
					sb->AppendUOSInt(codeOfst);
					sb->AppendUTF8Char('\t');
					sb->AppendHexBuff(&codePtr[codeOfst], 8, ' ', Text::LineBreakType::None);
					sb->AppendUTF8Char(' ');
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst]));
					sb->AppendC(UTF8STRC(": "));
					sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 4]));
					sb->AppendC(UTF8STRC("\r\n"));
					currIndex++;
					codeOfst += 8;
					codeLen -= 8;
				}
				sb->AppendC(UTF8STRC("\r\n"));
			}
			break;
		case 0xAC:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("ireturn\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xAD:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("lreturn\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xAE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("freturn\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xAF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("dreturn\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xB0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("areturn\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xB1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("return\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xB2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("getstatic "));
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("putstatic "));
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("getfield "));
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("putfield "));
			sb->AppendU16(val);
			this->DetailFieldRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("invokevirtual "));
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("invokespecial "));
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("invokestatic "));
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xB9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendUTF8Char(' ');
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("invokeinterface "));
			sb->AppendU16(val);
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
			sb->AppendU16(codePtr[codeOfst + 3]);
			this->DetailMethodRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xBA:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendUTF8Char(' ');
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("invokedynamic "));
			sb->AppendU16(val);
			this->DetailMethodRef(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xBB:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("new "));
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xBC:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 2, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 10);
			val = codePtr[codeOfst + 1];
			sb->AppendC(UTF8STRC("newarray "));
			sb->AppendU16(val);
			switch (val)
			{
			case 4: //T_BOOLEAN
				sb->AppendC(UTF8STRC(" (T_BOOLEAN)"));
				break;
			case 5: //T_CHAR
				sb->AppendC(UTF8STRC(" (T_CHAR)"));
				break;
			case 6: //T_FLOAT
				sb->AppendC(UTF8STRC(" (T_FLOAT)"));
				break;
			case 7: //T_DOUBLE
				sb->AppendC(UTF8STRC(" (T_DOUBLE)"));
				break;
			case 8: //T_BYTE
				sb->AppendC(UTF8STRC(" (T_BYTE)"));
				break;
			case 9: //T_SHORT
				sb->AppendC(UTF8STRC(" (T_SHORT)"));
				break;
			case 10: //T_INT
				sb->AppendC(UTF8STRC(" (T_INT)"));
				break;
			case 11: //T_LONG
				sb->AppendC(UTF8STRC(" (T_LONG)"));
				break;
			}
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 2;
			codeLen -= 2;
			break;
		case 0xBD:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("anewarray "));
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xBE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("arraylength\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xBF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("athrow\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xC0:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("checkcast "));
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC1:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("instanceof "));
			sb->AppendU16(val);
			this->DetailClassName(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC2:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("monitorenter\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xC3:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("monitorexit\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xC4:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			//////////////////////////////////////////
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("wide\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xC5:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 4, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 4);
			val = ReadMUInt16(&codePtr[codeOfst + 1]);
			sb->AppendC(UTF8STRC("multianewarray "));
			sb->AppendU16(val);
			sb->AppendUTF8Char(',');
			sb->AppendUTF8Char(' ');
			sb->AppendU16(codePtr[codeOfst + 3]);
			this->DetailClassName(val, sb);
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 4;
			codeLen -= 4;
			break;
		case 0xC6:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifnull "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC7:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 3, ' ', Text::LineBreakType::None);
			sb->AppendChar(' ', 7);
			sb->AppendC(UTF8STRC("ifnonnull "));
			sb->AppendI16(ReadMInt16(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 3;
			codeLen -= 3;
			break;
		case 0xC8:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendUTF8Char(' ');
			sb->AppendC(UTF8STRC("goto_w "));
			sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xC9:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHexBuff(&codePtr[codeOfst], 5, ' ', Text::LineBreakType::None);
			sb->AppendUTF8Char(' ');
			sb->AppendC(UTF8STRC("jsr_w "));
			sb->AppendI32(ReadMInt32(&codePtr[codeOfst + 1]));
			sb->AppendC(UTF8STRC("\r\n"));
			codeOfst += 5;
			codeLen -= 5;
			break;
		case 0xCA:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("breakpoint\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xFE:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("impdep1\r\n"));
			codeOfst++;
			codeLen--;
			break;
		case 0xFF:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("impdep2\r\n"));
			codeOfst++;
			codeLen--;
			break;
		default:
			this->AppendIndent(sb, lev);
			sb->AppendUOSInt(codeOfst);
			sb->AppendUTF8Char('\t');
			sb->AppendHex8(codePtr[codeOfst + 0]);
			sb->AppendChar(' ', 13);
			sb->AppendC(UTF8STRC("Unknown opcode\r\n"));
			codeOfst++;
			codeLen--;
			break;
		}
	}
}

const UInt8 *IO::JavaClass::DetailAnnotation(const UInt8 *annoPtr, const UInt8 *annoEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const
{
	sb->AppendUTF8Char('@');
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
		sb->AppendUTF8Char('(');
		i = 0;
		while (i < num_element_value_pairs)
		{
			if (annoEnd - annoPtr < 4)
			{
				break;
			}
			if (i > 0)
			{
				sb->AppendUTF8Char(',');
			}
			UInt16 element_name_index = ReadMUInt16(&annoPtr[0]);
			this->DetailName(element_name_index, sb, false);
			sb->AppendUTF8Char('=');
			annoPtr = this->DetailElementValue(&annoPtr[2], annoEnd, sb, importList, packageName);
			i++;
		}
		sb->AppendUTF8Char(')');
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
const UInt8 *IO::JavaClass::DetailElementValue(const UInt8 *annoPtr, const UInt8 *annoEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListString *importList, const UTF8Char *packageName) const
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (annoEnd - annoPtr < 3)
	{
		return annoPtr;
	}

	if (annoPtr[0] == '[')
	{
		UInt16 num_values = ReadMUInt16(&annoPtr[1]);
		UOSInt i;
		sb->AppendUTF8Char('{');
		annoPtr += 3;
		i = 0;
		while (i < num_values)
		{
			if (i > 0)
			{
				sb->AppendUTF8Char(',');
			}
			annoPtr = this->DetailElementValue(annoPtr, annoEnd, sb, importList, packageName);
			i++;
		}
		sb->AppendUTF8Char('}');
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
		sb->AppendUTF8Char('.');
		sptr = this->GetConstName(sbuff, const_name_index);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		annoPtr += 5;
		return annoPtr;
	}
	else
	{
		sb->AppendUTF8Char('?');
		return annoPtr + 1;
	}
}

const UInt8 *IO::JavaClass::DetailStackMapFrame(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (currPtr >= ptrEnd)
	{
		return currPtr;
	}
	if (currPtr[0] >= 0 && currPtr[0] < 64)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = SAME ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendC(UTF8STRC(")\r\n"));
		return currPtr + 1;
	}
	else if (currPtr[0] >= 64 && currPtr[0] < 128)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = SAME_LOCALS_1_STACK_ITEM ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendC(UTF8STRC(")\r\n"));
		return this->DetailVerificationTypeInfo(currPtr + 1, ptrEnd, lev + 1, sb);
	}
	else if (currPtr[0] == 247)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = SAME_LOCALS_1_STACK_ITEM_EXTENDED\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame offset_delta = "));
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->AppendC(UTF8STRC("\r\n"));
		return this->DetailVerificationTypeInfo(currPtr + 3, ptrEnd, lev + 1, sb);
	}
	else if (currPtr[0] >= 248 && currPtr[0] <= 250)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = CHOP ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendC(UTF8STRC(")\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame offset_delta = "));
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->AppendC(UTF8STRC("\r\n"));
		return currPtr + 3;
	}
	else if (currPtr[0] == 251)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = SAME_FRAME_EXTENDED\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame offset_delta = "));
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->AppendC(UTF8STRC("\r\n"));
		return currPtr + 3;
	}
	else if (currPtr[0] >= 252 && currPtr[0] <= 254)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame Type = APPEND ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendC(UTF8STRC(")\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame offset_delta = "));
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->AppendC(UTF8STRC("\r\n"));
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
		sb->AppendC(UTF8STRC("Frame Type = FULL_FRAME\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Frame offset_delta = "));
		sb->AppendU16(ReadMUInt16(&currPtr[1]));
		sb->AppendC(UTF8STRC("\r\n"));
		currPtr += 3;
		if (currPtr + 2 <= ptrEnd)
		{
			i = ReadMUInt16(currPtr);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Frame number_of_locals = "));
			sb->AppendUOSInt(i);
			sb->AppendC(UTF8STRC("\r\n"));
			while (i-- > 0)
			{
				currPtr = this->DetailVerificationTypeInfo(currPtr, ptrEnd, lev + 1, sb);
			}
		}
		if (currPtr + 2 <= ptrEnd)
		{
			i = ReadMUInt16(currPtr);
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Frame number_of_stack_items = "));
			sb->AppendUOSInt(i);
			sb->AppendC(UTF8STRC("\r\n"));
			while (i-- > 0)
			{
				currPtr = this->DetailVerificationTypeInfo(currPtr, ptrEnd, lev + 1, sb);
			}
		}
		return currPtr;
	}
	else
	{
		sb->AppendC(UTF8STRC("Frame Type = UNKNOWN ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendUTF8Char(')');
		return currPtr + 1;
	}
}

const UInt8 *IO::JavaClass::DetailVerificationTypeInfo(const UInt8 *currPtr, const UInt8 *ptrEnd, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (currPtr >= ptrEnd)
	{
		return currPtr;
	}
	UInt16 sindex;
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("VerificationType = "));
	switch (currPtr[0])
	{
	case 0:
		sb->AppendC(UTF8STRC("Top\r\n"));
		return currPtr + 1;
	case 1:
		sb->AppendC(UTF8STRC("Integer\r\n"));
		return currPtr + 1;
	case 2:
		sb->AppendC(UTF8STRC("Float\r\n"));
		return currPtr + 1;
	case 3:
		sb->AppendC(UTF8STRC("Double\r\n"));
		return currPtr + 1;
	case 4:
		sb->AppendC(UTF8STRC("Long\r\n"));
		return currPtr + 1;
	case 5:
		sb->AppendC(UTF8STRC("Null\r\n"));
		return currPtr + 1;
	case 6:
		sb->AppendC(UTF8STRC("UninitializedThis\r\n"));
		return currPtr + 1;
	case 7:
		{
			sb->AppendC(UTF8STRC("Object\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("Verification cinfo_index = "));
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
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				sb->AppendC(UTF8STRC(")"));
			}*/
			sb->AppendC(UTF8STRC("\r\n"));
			return currPtr + 3;
		}
	case 8:
		sb->AppendC(UTF8STRC("Uninitialized\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("Verification offset = "));
		sindex = ReadMUInt16(&currPtr[1]);
		sb->AppendU16(sindex);
		sb->AppendC(UTF8STRC("\r\n"));
		return currPtr + 3;
	default:
		sb->AppendC(UTF8STRC("Unknown ("));
		sb->AppendU16(currPtr[0]);
		sb->AppendC(UTF8STRC("\r\n"));
		return currPtr + 1;
	}
}

UTF8Char *IO::JavaClass::GetConstName(UTF8Char *sbuff, UInt16 index) const
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

Bool IO::JavaClass::ClassNameString(UInt16 index, NotNullPtr<Text::StringBuilderUTF8> sb) const
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
				sb->AppendUTF8Char('.');
				strStart = ptr;
			}
			else if (c == '$')
			{
				sb->AppendC(strStart, (UOSInt)(ptr - strStart - 1));
				sb->AppendUTF8Char('.');
				strStart = ptr;
			}
		}
		sb->AppendC(strStart, (UOSInt)(strEnd - strStart));
	}
	return true;
}

UTF8Char *IO::JavaClass::GetLVName(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst) const
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

UTF8Char *IO::JavaClass::GetLVType(UTF8Char *sbuff, UInt16 index, const MethodInfo *method, UOSInt codeOfst, Data::ArrayListString *importList, const UTF8Char *packageName) const
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
				this->DetailType(lvt->signatureIndex, sbTmp, importList, packageName);
				return Text::StrConcatC(sbuff, sbTmp.ToString(), sbTmp.GetLength());
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
				this->DetailType(lv->descriptorIndex, sbTmp, importList, packageName);
				return Text::StrConcatC(sbuff, sbTmp.ToString(), sbTmp.GetLength());
			}
			i++;
		}
	}
	return Text::StrConcatC(sbuff, UTF8STRC("java.lang.Object"));
}

Bool IO::JavaClass::MethodParse(MethodInfo *method, const UInt8 *methodBuff) const
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
	UTF8Char *sptr;
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
		if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
		{
			if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Code")))
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
					if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
					{
						if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LocalVariableTable")))
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
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LocalVariableTypeTable")))
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
						else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LineNumberTable")))
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
			else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Exceptions")))
			{
				UInt16 exCnt = ReadMUInt16(&ptr[6]);
				UInt16 j = 0;
				while (j < exCnt)
				{
					method->exList->Add(ReadMUInt16(&ptr[8 + j * 2]));
					j++;
				}
			}
			else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Signature")))
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

void IO::JavaClass::MethodFree(MethodInfo *method) const
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

void IO::JavaClass::AppendCodeClassName(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *className, Data::ArrayListString *importList, const UTF8Char *packageName) const
{
	UOSInt classNameLen = Text::StrCharCnt(className);
	UOSInt i = Text::StrLastIndexOfCharC(className, classNameLen, '.');
	if (i == INVALID_INDEX)
	{
		sb->AppendC(className, classNameLen);
		return;
	}
	if (packageName && Text::StrStartsWith(className, packageName) && i == Text::StrCharCnt(packageName))
	{
		sb->AppendC(className + i + 1, classNameLen - i - 1);
		return;
	}
	if (Text::StrStartsWithC(className, classNameLen, UTF8STRC("java.lang.")) && i == 9)
	{
		sb->AppendC(className + i + 1, classNameLen - i - 1);
		return;
	}
	if (importList == 0)
	{
		sb->AppendC(className, classNameLen);
		return;
	}
	if (Text::StrEndsWithC(className, classNameLen, UTF8STRC("[]")))
	{
		Text::StringBuilderUTF8 sbCls;
		sbCls.AppendC(className, classNameLen - 2);
		if (importList->SortedIndexOfPtr(sbCls.ToString(), sbCls.GetLength()) >= 0)
		{
			sb->AppendC(className + i + 1, classNameLen - i - 1);
			return;
		}
		importList->SortedInsert(Text::String::New(sbCls.ToString(), sbCls.GetLength()).Ptr());
		sb->AppendC(className + i + 1, classNameLen - i - 1);
	}
	else
	{
		if (importList->SortedIndexOfPtr(className, classNameLen) >= 0)
		{
			sb->AppendC(className + i + 1, classNameLen - i - 1);
			return;
		}
		importList->SortedInsert(Text::String::New(className, classNameLen).Ptr());
		sb->AppendC(className + i + 1, classNameLen - i - 1);
	}
}

void IO::JavaClass::AppendCodeClassContent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev, const UTF8Char *className, Data::ArrayListString *importList, const UTF8Char *packageName)
{
	this->AppendIndent(sb, lev);
	if (this->accessFlags & 1) //ACC_PUBLIC
	{
		sb->AppendC(UTF8STRC("public "));
	}
	else if (accessFlags & 2) //ACC_PRIVATE
	{
		sb->AppendC(UTF8STRC("private "));
	}
	else if (accessFlags & 4) //ACC_PROTECTED
	{
		sb->AppendC(UTF8STRC("protected "));
	}

	if (accessFlags & 8) //ACC_STATIC
	{
		sb->AppendC(UTF8STRC("static "));
	}
	if (accessFlags & 0x10) //ACC_FINAL
	{
		sb->AppendC(UTF8STRC("final "));
	}
	if (accessFlags & 0x40) //ACC_VOLATILE
	{
		sb->AppendC(UTF8STRC("volatile "));
	}
	if (accessFlags & 0x80) //ACC_TRANSIENT
	{
		sb->AppendC(UTF8STRC("transient "));
	}
	if (accessFlags & 0x0100) //ACC_NATIVE
	{
		sb->AppendC(UTF8STRC("native "));
	}

	if (accessFlags & 0x0200) //ACC_INTERFACE
	{
		sb->AppendC(UTF8STRC("interface "));
	}
	else if (accessFlags & 0x0400) //ACC_ABSTRACT
	{
		sb->AppendC(UTF8STRC("abstract class "));
	}
	else if (accessFlags & 0x0800) //ACC_STRICT
	{
		sb->AppendC(UTF8STRC("strict class"));
	}
	else if (accessFlags & 0x1000) //ACC_SYNTHETIC
	{
		sb->AppendC(UTF8STRC("synthetic class"));
	}
	else if (accessFlags & 0x2000) //ACC_ANNOTATION
	{
		sb->AppendC(UTF8STRC("annotation class"));
	}
	else if (accessFlags & 0x4000) //ACC_ENUM
	{
		sb->AppendC(UTF8STRC("enum "));
	}
	else
	{
		sb->AppendC(UTF8STRC("class "));
	}
	sb->AppendSlow(className);
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
			sb->AppendUTF8Char('<');
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
					sb->AppendUTF8Char(c);
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
						sb->AppendC(UTF8STRC(", "));
						isComma = false;
					}
					sb->AppendUTF8Char(c);
				}
			}
		}
		sbTmp.ClearStr();
		ptr = AppendCodeType2String(sbTmp, ptr, importList, packageName);
		if (!sbTmp.Equals(UTF8STRC("Object")))
		{
			sb->AppendC(UTF8STRC(" extends "));
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		}
		if (this->interfaces && this->interfaceCnt > 0)
		{
			i = 0;
			while (i < this->interfaceCnt)
			{
				if (i == 0)
				{
					sb->AppendC(UTF8STRC(" implements "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				ptr = AppendCodeType2String(sb, ptr, importList, packageName);
				i++;
			}
		}
	}
	else
	{
		sbTmp.ClearStr();
		if (this->GetSuperClass(sbTmp))
		{
			if (!sbTmp.Equals(UTF8STRC("java.lang.Object")))
			{
				sb->AppendC(UTF8STRC(" extends "));
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
					sb->AppendC(UTF8STRC(" implements "));
				}
				else
				{
					sb->AppendC(UTF8STRC(", "));
				}
				sbTmp.ClearStr();
				this->ClassNameString(ReadMUInt16(&this->interfaces[i * 2]), sbTmp);
				this->AppendCodeClassName(sb, sbTmp.ToString(), importList, packageName);
				i++;
			}
		}
	}
	sb->AppendC(UTF8STRC(" {\r\n"));
	i = 0;
	j = this->FieldsGetCount();
	while (i < j)
	{
		this->AppendIndent(sb, lev + 1);
		this->AppendCodeField(sb, i, importList, packageName);
		sb->AppendC(UTF8STRC(";\r\n"));
		i++;
	}

	i = 0;
	j = this->MethodsGetCount();
	while (i < j)
	{
		sb->AppendC(UTF8STRC("\r\n"));
		this->AppendCodeMethod(sb, i, lev + 1, false, true, importList, packageName);
		i++;
	}
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("}\r\n"));
}

void IO::JavaClass::AppendCodeField(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt index, Data::ArrayListString *importList, const UTF8Char *packageName) const
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sbValue;
	UInt8 *ptr = this->fields[index];
	UInt16 accessFlags = ReadMUInt16(ptr);
	UInt16 nameIndex = ReadMUInt16(&ptr[2]);
	UInt16 descriptorIndex = ReadMUInt16(&ptr[4]);
	UInt16 signatureIndex = 0;
	UInt16 attrCnt = ReadMUInt16(&ptr[6]);
	this->GetConstName(sbuff, descriptorIndex);
	Text::StringBuilderUTF8 sbTypeName;
	Type2String(sbuff, sbTypeName);
	UInt16 i = 0;
	ptr += 8;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		sbuff[0] = 0;
		if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
		{
			if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ConstantValue")))
			{
				nameIndex = ReadMUInt16(&ptr[6]);
				if (nameIndex > 0 && nameIndex < this->constPoolCnt && this->constPool[nameIndex] != 0)
				{
					if (this->constPool[nameIndex][0] == 3)
					{
						sbValue.AppendC(UTF8STRC(" = "));
						if (sbTypeName.Equals(UTF8STRC("boolean")))
						{
							if (ReadMInt32(&this->constPool[nameIndex][1]) != 0)
							{
								sbValue.AppendC(UTF8STRC("true"));
							}
							else
							{
								sbValue.AppendC(UTF8STRC("false"));
							}
						}
						else
						{
							sbValue.AppendI32(ReadMInt32(&this->constPool[nameIndex][1]));
						}
					}
					else if (this->constPool[nameIndex][0] == 4)
					{
						sbValue.AppendC(UTF8STRC(" = "));
						Text::SBAppendF32(sbValue, ReadMFloat(&this->constPool[nameIndex][1]));
						sbValue.AppendUTF8Char('f');
					}
					else if (this->constPool[nameIndex][0] == 5)
					{
						sbValue.AppendC(UTF8STRC(" = "));
						sbValue.AppendI64(ReadMInt64(&this->constPool[nameIndex][1]));
						sbValue.AppendUTF8Char('L');
					}
					else if (this->constPool[nameIndex][0] == 6)
					{
						sbValue.AppendC(UTF8STRC(" = "));
						Text::SBAppendF64(sbValue, ReadMDouble(&this->constPool[nameIndex][1]));
					}
					else if (this->constPool[nameIndex][0] == 8)
					{
						UInt16 sindex = ReadMUInt16(&this->constPool[nameIndex][1]);
						UInt16 strLen;
						ptr = this->constPool[sindex];
						if (ptr[0] == 1)
						{
							sbValue.AppendC(UTF8STRC(" = "));
							strLen = ReadMUInt16(&ptr[1]);
							const UTF8Char *s = Text::StrCopyNewC(ptr + 3, strLen).Ptr();
							Text::JSText::ToJSTextDQuote(sbValue, s);
							Text::StrDelNew(s);
						}
					}
					else
					{
						printf("AppendCodeField: Unsupported type %d\r\n", this->constPool[nameIndex][0]);
					}
				}
			}
			else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Signature")))
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
		sb->AppendC(UTF8STRC("public "));
	}
	if (accessFlags & 2)
	{
		sb->AppendC(UTF8STRC("private "));
	}
	if (accessFlags & 4)
	{
		sb->AppendC(UTF8STRC("protected "));
	}
	if (accessFlags & 8)
	{
		sb->AppendC(UTF8STRC("static "));
	}
	if (accessFlags & 0x10)
	{
		sb->AppendC(UTF8STRC("final "));
	}
	if (accessFlags & 0x40)
	{
		sb->AppendC(UTF8STRC("volatile "));
	}
	if (signatureIndex != 0)
	{
		this->GetConstName(sbuff, signatureIndex);
		AppendCodeType2String(sb, sbuff, importList, packageName);
	}
	else if (importList == 0 && packageName == 0)
	{
		sb->AppendC(sbTypeName.ToString(), sbTypeName.GetLength());
	}
	else
	{
		this->AppendCodeClassName(sb, sbTypeName.ToString(), importList, packageName);
	}
	sb->AppendUTF8Char(' ');
	sptr = this->GetConstName(sbuff, nameIndex);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(sbValue.ToString(), sbValue.GetLength());
}

void IO::JavaClass::AppendCodeMethod(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt index, UOSInt lev, Bool disasm, Bool decompile, Data::ArrayListString *importList, const UTF8Char *packageName) const
{
	Text::StringBuilderUTF8 sbTmp;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
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
		if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
		{
			if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RuntimeVisibleAnnotations")))
			{
				UInt16 num_annotations = ReadMUInt16(&ptr[6]);
				const UInt8 *annptr = &ptr[8];
				j = 0;

				while (j < num_annotations)
				{
					this->AppendIndent(sb, lev);
					annptr = this->DetailAnnotation(annptr, &ptr[6 + len], sb, importList, packageName);
					sb->AppendC(UTF8STRC("\r\n"));

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
		sb->AppendC(UTF8STRC("public "));
	}
	if (method.accessFlags & 2)
	{
		sb->AppendC(UTF8STRC("private "));
	}
	if (method.accessFlags & 4)
	{
		sb->AppendC(UTF8STRC("protected "));
	}
	if (method.accessFlags & 8)
	{
		sb->AppendC(UTF8STRC("static "));
	}
	if (method.accessFlags & 0x10)
	{
		sb->AppendC(UTF8STRC("final "));
	}
	if (method.accessFlags & 0x20)
	{
		sb->AppendC(UTF8STRC("synchronized "));
	}
	if (method.accessFlags & 0x100)
	{
		sb->AppendC(UTF8STRC("native "));
	}
	if (method.accessFlags & 0x400)
	{
		sb->AppendC(UTF8STRC("abstract "));
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
			sb->AppendC(UTF8STRC(" throws "));
		}
		else
		{
			sb->AppendC(UTF8STRC(", "));
		}
		sbTmp.ClearStr();
		this->ClassNameString(method.exList->GetItem(i), sbTmp);
		this->AppendCodeClassName(sb, sbTmp.ToString(), importList, packageName);
		i++;
	}

	sb->AppendC(UTF8STRC("\r\n"));
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("{\r\n"));
	if (disasm)
	{
		this->AppendIndent(sb, (lev + 1));
		sb->AppendC(UTF8STRC("/*\r\n"));

		ptr = &this->methods[index][8];
		i = 0;
		while (i < attrCnt)
		{
			UInt32 len = ReadMUInt32(&ptr[2]);
			UInt16 nameIndex = ReadMUInt16(&ptr[0]);
			sbuff[0] = 0;
			if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Code")))
				{
					UInt32 codeLen = ReadMUInt32(&ptr[10]);
					this->DetailCode(&ptr[14], codeLen, (lev + 1), sb);
				}
			}
			ptr += len + 6;

			i++;
		}
		this->AppendIndent(sb, (lev + 1));
		sb->AppendC(UTF8STRC("*/\r\n"));
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
			if ((sptr = this->GetConstName(sbuff, nameIndex)))
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Code")))
				{
					this->AppendCodeMethodCodes(sb, lev + 1, importList ,packageName, ptr, typeBuff, &method);
				}
			}
			ptr += len + 6;

			i++;
		}
	}
	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("}\r\n"));
	MethodFree(&method);
}

void IO::JavaClass::AppendCodeMethodCodes(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt lev, Data::ArrayListString *importList, const UTF8Char *packageName, const UInt8 *codeAttr, const UTF8Char *typeBuff, const MethodInfo *method) const
{
	IO::JavaClass::DecompileEnv env;
	UInt16 maxLocal = ReadMUInt16(&codeAttr[8]);
	UInt32 codeLen = ReadMUInt32(&codeAttr[10]);
	Data::ArrayListNN<Text::String> stackVal;
	Data::ArrayListNN<Text::String> stackTypes;
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
	if (this->MethodGetReturnType(method->descriptorIndex, sbTmp))
	{
		env.returnType = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
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
		this->ClassNameString(this->thisClass, sbTmp);
		env.localTypes[0] = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
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
		typeBuff = Type2String(typeBuff, sbTmp);
		env.localTypes[i] = Text::String::New(sbTmp.ToString(), sbTmp.GetLength()).Ptr();
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
			sb->AppendC(UTF8STRC("// Unprocessed stack: "));
			sb->Append(stackVal.GetItem(i));
			sb->AppendC(UTF8STRC("\r\n"));
			stackVal.GetItem(i)->Release();
		}
		i = stackTypes.GetCount();
		while (i-- > 0)
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("// Unprocessed stack type: "));
			sb->Append(stackTypes.GetItem(i));
			sb->AppendC(UTF8STRC("\r\n"));
			stackTypes.GetItem(i)->Release();
		}
	}
	else
	{
		i = stackVal.GetCount();
		while (i-- > 0)
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("// Unprocessed stack: "));
			sb->Append(stackTypes.GetItem(i));
			sb->AppendC(UTF8STRC(" "));
			sb->Append(stackVal.GetItem(i));
			sb->AppendC(UTF8STRC("\r\n"));
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

const UTF8Char *IO::JavaClass::AppendCodeType2String(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *typeStr, Data::ArrayListString *importList, const UTF8Char *packageName)
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
		sb->AppendC(UTF8STRC("? extends "));
		c = *typeStr++;
	}
	switch (c)
	{
	case 'B':
		sb->AppendC(UTF8STRC("byte"));
		break;
	case 'C':
		sb->AppendC(UTF8STRC("char"));
		break;
	case 'D':
		sb->AppendC(UTF8STRC("double"));
		break;
	case 'F':
		sb->AppendC(UTF8STRC("float"));
		break;
	case 'I':
		sb->AppendC(UTF8STRC("int"));
		break;
	case 'J':
		sb->AppendC(UTF8STRC("long"));
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
					sbTmp.AppendUTF8Char('.');
				}
				else if (c == '$')
				{
					subcls = sbTmp.GetLength();
					sbTmp.AppendUTF8Char('.');
				}
				else if (c == '<')
				{
					tmpIndex = sbTmp.GetLength();
					sbTmp.AppendUTF8Char('<');
					Bool found = false;
					while (true)
					{
						if (*typeStr == 0)
						{
							break;
						}
						else if (*typeStr == '>')
						{
							sbTmp.AppendUTF8Char('>');
							typeStr++;
							break;
						}
						else
						{
							if (found) sbTmp.AppendC(UTF8STRC(", "));
							typeStr = AppendCodeType2String(sbTmp, typeStr, importList, packageName);
							found = true;
						}
					}
				}
				else
				{
					sbTmp.AppendUTF8Char(c);
				}
			}
			UTF8Char *sptr = sbTmp.v;
			UOSInt sptrLen = sbTmp.GetLength();
			UOSInt i;
			if (subcls != INVALID_INDEX)
			{
				sptr[subcls] = 0;
				sptrLen = subcls;
			}
			else if (tmpIndex != INVALID_INDEX)
			{
				sptr[tmpIndex] = 0;
				sptrLen = tmpIndex;
			}
			i = Text::StrLastIndexOfCharC(sptr, sptrLen, '.');
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
				sb->AppendC(sptr, sptrLen);
			}
			else
			{
				Bool fullName = false;
				if (packageName && Text::StrStartsWith(sptr, packageName) && Text::StrCharCnt(packageName) == i)
				{
					
				}
				else if (Text::StrStartsWithC(sptr, sptrLen, UTF8STRC("java.lang.")) && i == 9)
				{

				}
				else if (importList)
				{
					if (importList->SortedIndexOfPtr(sptr, sptrLen) < 0)
					{
						importList->SortedInsert(Text::String::New(sptr, sptrLen).Ptr());
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
					sb->AppendC(sptr, sptrLen);
				}
				else
				{
					sb->AppendC(&sptr[i + 1], sptrLen - i - 1);
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
					sb->AppendUTF8Char('.');
				}
				else if (c == '$')
				{
					sb->AppendUTF8Char('.');
				}
				else if (c == '<')
				{
					sb->AppendUTF8Char('<');
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
							sb->AppendUTF8Char('>');
							break;
						}
						else
						{
							if (found) sb->AppendC(UTF8STRC(", "));
							typeStr = AppendCodeType2String(sb, typeStr, importList, packageName);
							found = true;
						}
					}
				}
				else
				{
					sb->AppendUTF8Char(c);
				}
			}
		}
		break;
	case 'S':
		sb->AppendC(UTF8STRC("short"));
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
				sb->AppendUTF8Char(c);
			}
		}
		break;
	case 'V':
		sb->AppendC(UTF8STRC("void"));
		break;
	case 'Z':
		sb->AppendC(UTF8STRC("boolean"));
		break;
	default:
		sb->AppendC(UTF8STRC("?"));
		break;
	}

	while (arrLev-- > 0)
	{
		sb->AppendUTF8Char('[');
		sb->AppendUTF8Char(']');
	}
	return typeStr;
}

void IO::JavaClass::Init(Data::ByteArrayR buff)
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
	if (buff.GetSize() < 26)
	{
		return;
	}
	if (ReadMUInt32(&buff[0]) != 0xCAFEBABE)
	{
		return;
	}
	this->fileBuffSize = buff.GetSize();
	this->fileBuff = MemAlloc(UInt8, buff.GetSize());
	MemCopyNO(this->fileBuff, buff.Ptr(), buff.GetSize());

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
	UTF8Char *sptr;
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
			if ((sptr = this->GetConstName(sbuff, nameIndex)) != 0)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Signature")) && len == 2)
				{
					this->signatureIndex = ReadMUInt16(&this->fileBuff[ofst + 6]);
				}
			}
			ofst += 6 + len;
			j++;
		}
	}
}

IO::JavaClass::JavaClass(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff) : IO::ParsedObject(sourceName)
{
	this->Init(buff);
}

IO::JavaClass::JavaClass(Text::CString sourceName, Data::ByteArrayR buff) : IO::ParsedObject(sourceName)
{
	this->Init(buff);
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

IO::ParserType IO::JavaClass::GetParserType() const
{
	return IO::ParserType::JavaClass;
}

Bool IO::JavaClass::GetClassNameFull(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	return ClassNameString(this->thisClass, sb);
}

Bool IO::JavaClass::GetSuperClass(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	return ClassNameString(this->superClass, sb);
}

Bool IO::JavaClass::FileStructDetail(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	sb->AppendC(UTF8STRC("Version = "));
	sb->AppendU16(ReadMUInt16(&this->fileBuff[6]));
	sb->AppendUTF8Char('.');
	sb->AppendU16(ReadMUInt16(&this->fileBuff[4]));
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("constant_pool_count = "));
	sb->AppendUOSInt(this->constPoolCnt);
	sb->AppendC(UTF8STRC("\r\n"));
	UOSInt i;
	UInt16 j;
	i = 1;
	while (i < this->constPoolCnt)
	{
		UInt16 strLen;
		sb->AppendC(UTF8STRC("Const "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(": "));
		if (this->constPool[i] == 0)
		{
			sb->AppendC(UTF8STRC("unusable"));
		}
		else
		{
			switch (this->constPool[i][0])
			{
			case 1: //CONSTANT_Utf8
				strLen = ReadMUInt16(&this->constPool[i][1]);
				sb->AppendC(UTF8STRC("UTF8 = "));
				sb->AppendC(&this->constPool[i][3], strLen);
				break;
			case 3: //CONSTANT_Integer
				sb->AppendC(UTF8STRC("Integer, value = "));
				sb->AppendI32(ReadMInt32(&this->constPool[i][1]));
				break;
			case 4: //CONSTANT_Float
				sb->AppendC(UTF8STRC("Float, value = "));
				Text::SBAppendF32(sb, ReadMFloat(&this->constPool[i][1]));
				break;
			case 5: //CONSTANT_Long
				sb->AppendC(UTF8STRC("Long, value = "));
				sb->AppendI64(ReadMInt64(&this->constPool[i][1]));
				break;
			case 6: //CONSTANT_Float
				sb->AppendC(UTF8STRC("Double, value = "));
				Text::SBAppendF64(sb, ReadMDouble(&this->constPool[i][1]));
				break;
			case 7: //CONSTANT_Class
				sb->AppendC(UTF8STRC("Class, Name index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 8: //CONSTANT_String
				sb->AppendC(UTF8STRC("String, String index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 9: //CONSTANT_Fieldref
				sb->AppendC(UTF8STRC("Fieldref, Class index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->AppendC(UTF8STRC(", Name and type index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 10: //CONSTANT_Methodref
				sb->AppendC(UTF8STRC("Methodref, Class index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->AppendC(UTF8STRC(", Name and type index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 11: //CONSTANT_InterfaceMethodref
				sb->AppendC(UTF8STRC("InterfaceMethodref, Class index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->AppendC(UTF8STRC(", Name and type index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 12: //CONSTANT_NameAndType
				sb->AppendC(UTF8STRC("NameAndType, Name index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->AppendC(UTF8STRC(", Descriptor index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			case 15: //CONSTANT_MethodHandle
				sb->AppendC(UTF8STRC("MethodHandle, Ref Kind = "));
				sb->AppendU16(this->constPool[i][1]);
				sb->AppendC(UTF8STRC(", Ref index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][2]));
				break;
			case 16: //CONSTANT_MethodType
				sb->AppendC(UTF8STRC("MethodType, Descriptor index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				break;
			case 18: //CONSTANT_InvokeDynamic
				sb->AppendC(UTF8STRC("InvokeDynamic, bootstrap method attr index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][1]));
				sb->AppendC(UTF8STRC(", Name and Type index = "));
				sb->AppendU16(ReadMUInt16(&this->constPool[i][3]));
				break;
			}
		}
		sb->AppendC(UTF8STRC("\r\n"));
		i++;
	}

	sb->AppendC(UTF8STRC("Access Flags = "));
	DetailAccessFlags(this->accessFlags, sb);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("this_class = "));
	sb->AppendU16(this->thisClass);
	this->DetailClassName(this->thisClass, sb);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("super_class = "));
	sb->AppendU16(this->superClass);
	this->DetailClassName(this->superClass, sb);
	sb->AppendC(UTF8STRC("\r\n"));
	sb->AppendC(UTF8STRC("interfaces_count = "));
	sb->AppendUOSInt(this->interfaceCnt);
	sb->AppendC(UTF8STRC("\r\n"));
	i = 0;
	while (i < this->interfaceCnt)
	{
		sb->AppendC(UTF8STRC("interfaces["));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC("] = "));
		sb->AppendU16(ReadMUInt16(&this->interfaces[i * 2]));
		sb->AppendC(UTF8STRC("\r\n"));
		i++;
	}
	sb->AppendC(UTF8STRC("fields_count = "));
	sb->AppendUOSInt(this->fieldsCnt);
	sb->AppendC(UTF8STRC("\r\n"));
	i = 0;
	while (i < this->fieldsCnt)
	{
		sb->AppendC(UTF8STRC("Field "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" Access Flags = "));
		DetailAccessFlags(ReadMUInt16(&this->fields[i][0]), sb);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("Field "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" name index = "));
		sb->AppendU16(ReadMUInt16(&this->fields[i][2]));
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("Field "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" descriptor index = "));
		sb->AppendU16(ReadMUInt16(&this->fields[i][4]));
		sb->AppendC(UTF8STRC("\r\n"));
		UInt16 attributes_count = ReadMUInt16(&this->fields[i][6]);
		sb->AppendC(UTF8STRC("Field "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" attributes count = "));
		sb->AppendU16(attributes_count);
		sb->AppendC(UTF8STRC("\r\n"));
		const UInt8 *attr = &this->fields[i][8];
		j = 0;
		while (j < attributes_count)
		{
			attr = this->DetailAttribute(attr, 1, sb);
			j++;
		}
		i++;
	}
	sb->AppendC(UTF8STRC("methods_count = "));
	sb->AppendUOSInt(this->methodCnt);
	sb->AppendC(UTF8STRC("\r\n"));
	i = 0;
	while (i < this->methodCnt)
	{
		MethodInfo method;
		this->MethodParse(&method, this->methods[i]);
		sb->AppendC(UTF8STRC("Method "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" Access Flags = "));
		DetailAccessFlags(accessFlags, sb);
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("Method "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" name index = "));
		sb->AppendU16(ReadMUInt16(&this->methods[i][2]));
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(UTF8STRC("Method "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" descriptor index = "));
		sb->AppendU16(ReadMUInt16(&this->methods[i][4]));
		this->DetailNameType(ReadMUInt16(&this->methods[i][2]), ReadMUInt16(&this->methods[i][4]), this->thisClass, (const UTF8Char*)" ", sb, 0, &method, 0, 0);
		sb->AppendC(UTF8STRC("\r\n"));
		UInt16 attributes_count = ReadMUInt16(&this->methods[i][6]);
		sb->AppendC(UTF8STRC("Method "));
		sb->AppendUOSInt(i);
		sb->AppendC(UTF8STRC(" attributes count = "));
		sb->AppendU16(attributes_count);
		sb->AppendC(UTF8STRC("\r\n"));
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

	sb->AppendC(UTF8STRC("attributes_count = "));
	sb->AppendUOSInt(this->attrCnt);
	sb->AppendC(UTF8STRC("\r\n"));
	i = 0;
	while (i < this->attrCnt)
	{
		this->DetailAttribute(this->attrs[i], 1, sb);
		i++;
	}
	return true;
}

UOSInt IO::JavaClass::FieldsGetCount() const
{
	return this->fieldsCnt;
}

Bool IO::JavaClass::FieldsGetDecl(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (index >= this->fieldsCnt)
	{
		return false;
	}
	this->AppendCodeField(sb, index, 0, 0);
	return true;
}

UOSInt IO::JavaClass::MethodsGetCount() const
{
	return this->methodCnt;
}

Bool IO::JavaClass::MethodsGetDecl(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (index >= this->methodCnt)
	{
		return false;
	}

	MethodInfo method;
	this->MethodParse(&method, this->methods[index]);
	if (method.accessFlags & 1)
	{
		sb->AppendC(UTF8STRC("public "));
	}
	if (method.accessFlags & 2)
	{
		sb->AppendC(UTF8STRC("private "));
	}
	if (method.accessFlags & 4)
	{
		sb->AppendC(UTF8STRC("protected "));
	}
	if (method.accessFlags & 8)
	{
		sb->AppendC(UTF8STRC("static "));
	}
	if (method.accessFlags & 0x10)
	{
		sb->AppendC(UTF8STRC("final "));
	}
	if (method.accessFlags & 0x20)
	{
		sb->AppendC(UTF8STRC("synchronized "));
	}
	if (method.accessFlags & 0x100)
	{
		sb->AppendC(UTF8STRC("native "));
	}
	if (method.accessFlags & 0x400)
	{
		sb->AppendC(UTF8STRC("abstract "));
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

Bool IO::JavaClass::MethodsGetDetail(UOSInt index, UOSInt lev, Bool disasm, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (index >= this->methodCnt)
	{
		return false;
	}
	this->AppendCodeMethod(sb, index, lev, disasm, disasm, 0, 0);
	return true;
}

void IO::JavaClass::DecompileFile(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	Text::StringBuilderUTF8 sbTmp;
	if (!this->GetClassNameFull(sbTmp))
	{
		return;
	}
	const UTF8Char *packageName = 0;
	Data::ArrayListString importList;
	UOSInt i = sbTmp.LastIndexOf('.');
	UOSInt j;
	if (i != INVALID_INDEX)
	{
		sb->AppendC(UTF8STRC("package "));
		sb->AppendC(sbTmp.ToString(), i);
		sb->AppendC(UTF8STRC(";\r\n"));
		packageName = Text::StrCopyNewC(sbTmp.ToString(), i).Ptr();
	}
	sb->AppendC(UTF8STRC("\r\n"));

	Text::StringBuilderUTF8 sbClass;
	this->AppendCodeClassContent(sbClass, 0, sbTmp.ToString() + i + 1, &importList, packageName);

	if (importList.GetCount() > 0)
	{
		i = 0;
		j = importList.GetCount();
		while (i < j)
		{
			sb->AppendC(UTF8STRC("import "));
			sb->Append(importList.GetItem(i));
			sb->AppendC(UTF8STRC(";\r\n"));
			i++;
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	sb->AppendC(sbClass.ToString(), sbClass.GetLength());

	LIST_FREE_STRING(&importList);
	SDEL_TEXT(packageName);
}

IO::JavaClass::EndType IO::JavaClass::DecompileCode(const UInt8 *codePtr, const UInt8 *codeEnd, IO::JavaClass::DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	Text::StringBuilderUTF8 sbTmp;
	Text::StringBuilderUTF8 sbTmp2;
	Text::StringBuilderUTF8 sbTmp3;
	UInt16 classIndex;
	UInt16 val;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char typeBuff[128];
	while (codePtr < codeEnd)
	{
		if (env->stacks->GetCount() != env->stackTypes->GetCount())
		{
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("// stack and type count mismatch: "));
			sb->AppendUOSInt(env->stacks->GetCount());
			sb->AppendC(UTF8STRC(" vs "));
			sb->AppendUOSInt(env->stackTypes->GetCount());
			sb->AppendC(UTF8STRC(", next code = "));
			sb->AppendHex8(codePtr[0]);
			sb->AppendC(UTF8STRC("\r\n"));
			return EndType::Error;
		}
		switch (codePtr[0])
		{
		case 0x00: //nop
			codePtr++;
			break;
		case 0x01: //aconst_null
			env->stacks->Add(Text::String::New(UTF8STRC("null")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("Object")));
			codePtr++;
			break;
		case 0x02: //iconst_m1
			env->stacks->Add(Text::String::New(UTF8STRC("-1")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x03: //iconst_0
			env->stacks->Add(Text::String::New(UTF8STRC("0")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x04: //iconst_1
			env->stacks->Add(Text::String::New(UTF8STRC("1")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x05: //iconst_2
			env->stacks->Add(Text::String::New(UTF8STRC("2")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x06: //iconst_3
			env->stacks->Add(Text::String::New(UTF8STRC("3")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x07: //iconst_4
			env->stacks->Add(Text::String::New(UTF8STRC("4")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x08: //iconst_5
			env->stacks->Add(Text::String::New(UTF8STRC("5")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x09: //lconst_0
			env->stacks->Add(Text::String::New(UTF8STRC("0")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("long")));
			codePtr++;
			break;
		case 0x0A: //lconst_1
			env->stacks->Add(Text::String::New(UTF8STRC("1")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("long")));
			codePtr++;
			break;
		case 0x0B: //fconst_0
			env->stacks->Add(Text::String::New(UTF8STRC("0.0f")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("float")));
			codePtr++;
			break;
		case 0x0C: //fconst_1
			env->stacks->Add(Text::String::New(UTF8STRC("1.0f")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("float")));
			codePtr++;
			break;
		case 0x0D: //fconst_2
			env->stacks->Add(Text::String::New(UTF8STRC("2.0f")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("float")));
			codePtr++;
			break;
		case 0x0E: //dconst_0
			env->stacks->Add(Text::String::New(UTF8STRC("0.0")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("double")));
			codePtr++;
			break;
		case 0x0F: //dconst_1
			env->stacks->Add(Text::String::New(UTF8STRC("1.0")));
			env->stackTypes->Add(Text::String::New(UTF8STRC("double")));
			codePtr++;
			break;
		case 0x10: //bipush
			sbTmp.ClearStr();
			sbTmp.AppendU16(codePtr[1]);
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr += 2;
			break;
		case 0x11: //sipush
			sbTmp.ClearStr();
			sbTmp.AppendI16(ReadMInt16(&codePtr[1]));
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
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
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x16: //lload
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x17: //fload
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x18: //dload
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x19: //aload
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			env->stackTypes->Add(env->localTypes[codePtr[1]]->Clone());
			codePtr += 2;
			break;
		case 0x1A: //iload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				sptr = this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewP(sbuff, sptr));
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("this")));
			}
			sptr = this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x1B: //iload_1
			sptr = this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x1C: //iload_2
			sptr = this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x1D: //iload_3
			sptr = this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x1E: //lload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				sptr = this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewP(sbuff, sptr));
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("this")));
			}
			sptr = this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x1F: //lload_1
			sptr = this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x20: //lload_2
			sptr = this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x21: //lload_3
			sptr = this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x22: //fload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				sptr = this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewP(sbuff, sptr));
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("this")));
			}
			sptr = this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x23: //fload_1
			sptr = this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x24: //fload_2
			sptr = this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x25: //fload_3
			sptr = this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x26: //dload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				sptr = this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewP(sbuff, sptr));
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("this")));
			}
			sptr = this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x27: //dload_1
			sptr = this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x28: //dload_2
			sptr = this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x29: //dload_3
			sptr = this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x2A: //aload_0
			if (env->method && (env->method->accessFlags & 8) != 0)
			{
				sptr = this->GetLVName(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart));
				env->stacks->Add(Text::String::NewP(sbuff, sptr));
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("this")));
			}
			sptr = this->GetLVType(sbuff, 0, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x2B: //aload_1
			sptr = this->GetLVName(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 1, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x2C: //aload_2
			sptr = this->GetLVName(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 2, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x2D: //aload_3
			sptr = this->GetLVName(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart));
			env->stacks->Add(Text::String::NewP(sbuff, sptr));
			sptr = this->GetLVType(sbuff, 3, env->method, (UOSInt)(codePtr - env->codeStart), env->importList, env->packageName);
			env->stackTypes->Add(Text::String::NewP(sbuff, sptr));
			codePtr++;
			break;
		case 0x2E: //iaload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// iaload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();;
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0x2F: //laload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// laload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("long")));
			codePtr++;
			break;
		case 0x30: //faload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// faload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("float")));
			codePtr++;
			break;
		case 0x31: //daload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// daload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("double")));
			codePtr++;
			break;
		case 0x32: //aaload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// aaload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
			if (!sbTmp2.EndsWith(UTF8STRC("[]")))
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// aaload stack not array type: "));
				sb->Append(env->stackTypes->GetItem(env->stackTypes->GetCount() - 2));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp2.RemoveChars(2);
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
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
				sb->AppendC(UTF8STRC("// baload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("byte")));
			codePtr++;
			break;
		case 0x34: //caload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// caload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("char")));
			codePtr++;
			break;
		case 0x35: //saload
			if (env->stacks->GetCount() < 2)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// saload stack invalid: count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.ClearStr();
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
			sbTmp.AppendUTF8Char('[');
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("short")));
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
				sb->AppendC(UTF8STRC("// iastore stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendUTF8Char('[');
				sb->Append(indexStr);
				sb->AppendC(UTF8STRC("] = "));
				sb->Append(valueStr);
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// dastore stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendUTF8Char('[');
				sb->Append(indexStr);
				sb->AppendC(UTF8STRC("] = "));
				sb->Append(valueStr);
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// aastore stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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

				if (nameStr->StartsWith(UTF8STRC("new ")) && env->stacks->GetCount() > 0 && env->stacks->GetItem(env->stacks->GetCount()- 1)->StartsWith(nameStr))
				{
					Text::String *arrStr = env->stacks->RemoveAt(env->stacks->GetCount()- 1);
					sbTmp.ClearStr();
					sbTmp.Append(arrStr);
					if (sbTmp.ToString()[sbTmp.GetLength() - 1] == '}')
					{
						sbTmp.RemoveChars(1);
						sbTmp.AppendC(UTF8STRC(", "));
						sbTmp.Append(valueStr);
						sbTmp.AppendUTF8Char('}');
					}
					else
					{
						if (sbTmp.ToString()[sbTmp.GetLength() - 1] == ']')
						{
							UOSInt tmpI = sbTmp.LastIndexOf('[');
							if (tmpI != INVALID_INDEX)
							{
								sbTmp.TrimToLength(tmpI + 1);
								sbTmp.AppendUTF8Char(']');
							}
						}
						sbTmp.AppendC(UTF8STRC(" {"));
						sbTmp.Append(valueStr);
						sbTmp.AppendUTF8Char('}');
					}
					env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
					arrStr->Release();
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(nameStr);
					sb->AppendUTF8Char('[');
					sb->Append(indexStr);
					sb->AppendC(UTF8STRC("] = "));
					sb->Append(valueStr);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// bastore stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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

				if (nameStr->StartsWith(UTF8STRC("new ")) && env->stacks->GetCount() > 0 && env->stacks->GetItem(env->stacks->GetCount()- 1)->StartsWith(nameStr))
				{
					Text::String *arrStr = env->stacks->RemoveAt(env->stacks->GetCount()- 1);
					sbTmp.ClearStr();
					sbTmp.Append(arrStr);
					if (sbTmp.ToString()[sbTmp.GetLength() - 1] == '}')
					{
						sbTmp.RemoveChars(1);
						sbTmp.AppendC(UTF8STRC(", "));
						sbTmp.Append(valueStr);
						sbTmp.AppendUTF8Char('}');
					}
					else
					{
						sbTmp.AppendC(UTF8STRC(" {"));
						sbTmp.Append(valueStr);
						sbTmp.AppendUTF8Char('}');
					}
					env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
					arrStr->Release();
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->Append(nameStr);
					sb->AppendUTF8Char('[');
					sb->Append(indexStr);
					sb->AppendC(UTF8STRC("] = "));
					sb->Append(valueStr);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// pop stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			break;
		case 0x59: //dup
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// dup stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// dup_x1 stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// iadd stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"<>?"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.AppendC(UTF8STRC(" + "));
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"<>?"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
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
					sb->AppendC(UTF8STRC("// ladd stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" + "));
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
					sb->AppendC(UTF8STRC("// fadd stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" + "));
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
					sb->AppendC(UTF8STRC("// dadd stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" + "));
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
					sb->AppendC(UTF8STRC("// isub stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" - "));
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
					sb->AppendC(UTF8STRC("// lsub stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" - "));
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
					sb->AppendC(UTF8STRC("// fsub stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" - "));
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
					sb->AppendC(UTF8STRC("// dsub stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" - "));
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
					sb->AppendC(UTF8STRC("// mul stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.AppendC(UTF8STRC(" * "));
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
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
					sb->AppendC(UTF8STRC("// div stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s);
				}
				s->Release();
				sbTmp.AppendC(UTF8STRC(" * "));
				s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
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
					sb->AppendC(UTF8STRC("// irem stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				sbTmp.Append(s);
				s->Release();
				sbTmp.AppendC(UTF8STRC(" % "));
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
					sb->AppendC(UTF8STRC("// dneg stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				sbTmp.ClearStr();
				sbTmp.AppendUTF8Char('-');
				if (s->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendUTF8Char(')');
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
					sb->AppendC(UTF8STRC("// ishl stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.AppendC(UTF8STRC(" << "));
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				codePtr++;
			}
			break;
		case 0x7C: //iushr
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// iushr stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.AppendC(UTF8STRC(" >> "));
				sbTmp.Append(s);
				s->Release();
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				codePtr++;
			}
			break;
		case 0x7E: //iand
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// iand stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.AppendC(UTF8STRC(" & "));
				if (s->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				codePtr++;
			}
			break;
		case 0x80: //ior
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// ior stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->Pop();
				Text::String *s2 = env->stacks->Pop();
				sbTmp.ClearStr();
				if (s2->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s2);
					s2->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s2);
					s2->Release();
				}
				sbTmp.AppendC(UTF8STRC(" | "));
				if (s->ContainChars((const UTF8Char*)"+-*/><"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendUTF8Char(')');
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
				}
				env->stacks->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
				env->stackTypes->Pop()->Release();
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				codePtr++;
			}
			break;
		case 0x82: //ixor
			{
				if (env->stacks->GetCount() <= 1)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// ixor stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				Text::String *s = env->stacks->RemoveAt(env->stacks->GetCount() - 2);
				sbTmp.ClearStr();
				if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals(UTF8STRC("-1")))
				{
					sbTmp.AppendUTF8Char('~');
					sbTmp.Append(s);
					s->Release();
					env->stacks->Pop()->Release();
				}
				else
				{
					sbTmp.Append(s);
					s->Release();
					sbTmp.AppendC(UTF8STRC(" % "));
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
			sptr = this->GetLVName(sbuff, codePtr[1], env->method, (UOSInt)(codePtr - env->codeStart));
			sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb->AppendC(UTF8STRC(" += "));
			sb->AppendI16((Int16)codePtr[2]);
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			codePtr += 3;
			break;
		case 0x85: //i2l
			env->stackTypes->Pop()->Release();
			env->stackTypes->Add(Text::String::New(UTF8STRC("long")));
			codePtr += 1;
			break;
		case 0x91: //i2b
			{
				env->stackTypes->Pop()->Release();
				env->stackTypes->Add(Text::String::New(UTF8STRC("byte")));
				sbTmp.ClearStr();
				sbTmp.AppendC(UTF8STRC("(byte)"));
				Text::String *s = env->stacks->Pop();
				if (s->ContainChars((const UTF8Char*)"+-*/<>?|&"))
				{
					sbTmp.AppendUTF8Char('(');
					sbTmp.Append(s);
					sbTmp.AppendUTF8Char(')');
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
				sb->AppendC(UTF8STRC("// lcmp stack invalid: "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// lcmp next opcode invalid: "));
					sb->AppendHex8(codePtr[1]);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// lcmpl stack invalid: "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// lcmpl next opcode invalid: "));
					sb->AppendHex8(codePtr[1]);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// ifeq stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::EQ, true);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// ifne stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::NE, true);
					sb->AppendC(UTF8STRC(";"));
					AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// iflt stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::LT, true);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// ifge stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::GE, true);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// ifgt stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::GT, true);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// ifle stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Int16 codeOfst = ReadMInt16(&codePtr[1]);
				env->stacks->Add(Text::String::New(UTF8STRC("0")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
				if (codeOfst == 5 && codePtr + 7 <= codeEnd && codePtr[3] == 0x04 && codePtr[4] == 0xAC && codePtr[5] == 0x03 && codePtr[6] == 0xAC)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("return "));
					AppendCond(sb, env, env->stacks->GetCount() - 2, CondType::LE, true);
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmpge stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmpge stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmpge stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmpge stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmpgt stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_icmple stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_acmpeq stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// if_acmpne stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
					return et;
				}
				if (stackCnt == env->stacks->GetCount() && (et == EndType::CodeEnd || et == EndType::Throw || et == EndType::Return))
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("if (false)\r\n"));
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("{\r\n"));
					sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("}\r\n"));
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
					sb->AppendC(UTF8STRC("// ireturn stack invalid"));
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("return "));
				Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
				if (env->returnType && env->returnType->Equals(UTF8STRC("boolean")))
				{
					if (s->Equals(UTF8STRC("0")))
					{
						sb->AppendC(UTF8STRC("false"));
					}
					else if (s->Equals(UTF8STRC("1")))
					{
						sb->AppendC(UTF8STRC("true"));
					}
					else if (s->EndsWith(UTF8STRC("?1:0")))
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
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// lreturn stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("return "));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xAE: //freturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// freturn stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("return "));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xAF: //dreturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// dreturn stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("return "));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB0: //areturn
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// areturn stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("return "));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB1: //return
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("return;"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			codePtr++;
			env->endPtr = codePtr;
			return EndType::Return;
		case 0xB2: //getstatic
			val = ReadMUInt16(&codePtr[1]);
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// getstatic index invalid:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// getstatic const pool not fieldref:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				UInt16 classIndex = ReadMUInt16(&this->constPool[val][1]);
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				sbTmp.ClearStr();
				this->ClassNameString(classIndex, sbTmp);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// getstatic const pool not nameAndType:"));
					sb->AppendU16(nameTypeIndex);
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sbTmp2.ClearStr();
				this->AppendCodeClassName(sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
				sbTmp2.AppendUTF8Char('.');
				sptr = this->GetConstName(sbuff, classIndex);
				sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				this->GetConstName(sbuff, nameTypeIndex);
				sbTmp.ClearStr();
				Type2String(sbuff, sbTmp);
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
				sb->AppendC(UTF8STRC("// putstatic stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// putstatic index invalid:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// putstatic const pool not fieldref:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				UInt16 classIndex = ReadMUInt16(&this->constPool[val][1]);
				UInt16 nameTypeIndex = ReadMUInt16(&this->constPool[val][3]);
				sbTmp.ClearStr();
				this->ClassNameString(classIndex, sbTmp);
				UInt8 *ptr = this->constPool[nameTypeIndex];
				if (ptr == 0 || ptr[0] != 12)
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(UTF8STRC("// putstatic const pool not nameAndType:"));
					sb->AppendU16(nameTypeIndex);
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				this->AppendIndent(sb, lev);
				this->AppendCodeClassName(sb, sbTmp.ToString(), env->importList, env->packageName);
				sb->AppendUTF8Char('.');
				sptr = this->GetConstName(sbuff, classIndex);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC(" = "));
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
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
				sb->AppendC(UTF8STRC("// getfield stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// getfield index invalid:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// getfield const pool not fieldref:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// getfield const pool not nameAndType:"));
					sb->AppendU16(nameTypeIndex);
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendUTF8Char('.');
				sptr = this->GetConstName(sbuff, classIndex);
				sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				this->GetConstName(sbuff, nameTypeIndex);
				sbTmp.ClearStr();
				Type2String(sbuff, sbTmp);
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
				sb->AppendC(UTF8STRC("// putfield stack invalid: "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (val == 0 || val >= this->constPoolCnt)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// putfield index invalid:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			if (this->constPool[val] == 0 || this->constPool[val][0] != 9)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// putfield const pool not fieldref:"));
				sb->AppendU16(val);
				sb->AppendC(UTF8STRC("\r\n"));
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
					sb->AppendC(UTF8STRC("// putfield const pool not nameAndType:"));
					sb->AppendU16(nameTypeIndex);
					sb->AppendC(UTF8STRC("\r\n"));
					return EndType::Error;
				}
				classIndex = ReadMUInt16(&ptr[1]);
				nameTypeIndex = ReadMUInt16(&ptr[3]);
				this->AppendIndent(sb, lev);
				sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 2));
				sb->AppendUTF8Char('.');
				sptr = this->GetConstName(sbuff, classIndex);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sb->AppendC(UTF8STRC(" = "));
				Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
				this->GetConstName(sbuff, nameTypeIndex);
				if (sbuff[0] == 'Z') //boolean
				{
					if (s->Equals(UTF8STRC("0")))
					{
						sb->AppendC(UTF8STRC("false"));
					}
					else if (s->Equals(UTF8STRC("1")))
					{
						sb->AppendC(UTF8STRC("true"));
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
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
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
			if ((sptr = this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, sbTmp)) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// invokevirtual Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendUTF8Char('.');
				sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sbTmp2.AppendC(sbTmp.ToString(), sbTmp.GetLength());
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNullSlow(typeBuff));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			codePtr += 3;
			break;
		case 0xB7: //invokespecial
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if ((sptr = this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, sbTmp)) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// invokespecial Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				Bool isInit = false;
				sbTmp2.ClearStr();
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("<init>")))
				{
					isInit = true;
					if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals(UTF8STRC("this")))
					{
						if (classIndex == this->thisClass)
						{
							sbTmp2.AppendC(UTF8STRC("this"));
						}
						else
						{
							sbTmp2.AppendC(UTF8STRC("super"));
						}
					}
					else
					{
						sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
					}
				}
				else
				{
					if (env->stacks->GetItem(env->stacks->GetCount() - 1)->Equals(UTF8STRC("this")))
					{
						if (classIndex == this->thisClass)
						{
							sbTmp2.AppendC(UTF8STRC("this."));
						}
						else
						{
							sbTmp2.AppendC(UTF8STRC("super."));
						}
					}
					else
					{
						sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
						sbTmp2.AppendUTF8Char('.');
					}
					sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				}
				sbTmp2.AppendC(sbTmp.ToString(), sbTmp.GetLength());
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNullSlow(typeBuff));
				}
				else if (isInit && env->stacks->GetCount() > 0 && sbTmp2.StartsWith(env->stacks->GetItem(env->stacks->GetCount() - 1)))
				{
					env->stacks->Pop()->Release();
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			codePtr += 3;
			break;
		case 0xB8: //invokestatic
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if ((sptr = this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, sbTmp)) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp3.ClearStr();
			this->ClassNameString(classIndex, sbTmp3);
			this->AppendCodeClassName(sbTmp2, sbTmp3.ToString(), env->importList, env->packageName);
			sbTmp2.AppendUTF8Char('.');
			sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sbTmp2.AppendC(sbTmp.ToString(), sbTmp.GetLength());
			if (typeBuff[0])
			{
				env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
				env->stackTypes->Add(Text::String::NewNotNullSlow(typeBuff));
			}
			else
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
				sb->AppendC(UTF8STRC(";"));
				this->AppendLineNum(sb, env, codePtr);
				sb->AppendC(UTF8STRC("\r\n"));
			}
			codePtr += 3;
			break;
		case 0xB9: //invokeinterface
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			typeBuff[0] = 0;
			if ((sptr = this->DecompileMethod(val, sbuff, &classIndex, typeBuff, env, sbTmp)) == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				return EndType::Error;
			}
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// invokeinterface Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				sbTmp2.ClearStr();
				sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
				sbTmp2.AppendUTF8Char('.');
				sbTmp2.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				sbTmp2.AppendC(sbTmp.ToString(), sbTmp.GetLength());
				env->stacks->Pop()->Release();
				env->stackTypes->Pop()->Release();
				if (typeBuff[0])
				{
					env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
					env->stackTypes->Add(Text::String::NewNotNullSlow(typeBuff));
				}
				else
				{
					this->AppendIndent(sb, lev);
					sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
					sb->AppendC(UTF8STRC(";"));
					this->AppendLineNum(sb, env, codePtr);
					sb->AppendC(UTF8STRC("\r\n"));
				}
			}
			codePtr += 5;
			break;
		case 0xBB: //new
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.AppendC(UTF8STRC("new "));
			this->AppendCodeClassName(sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(sbTmp.ToString(), sbTmp.GetLength()));
			codePtr += 3;
			break;
		case 0xBC: //newarray
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// newarray stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp.ClearStr();
			sbTmp.AppendC(UTF8STRC("new "));
			switch (codePtr[1])
			{
			case 4: //T_BOOLEAN
				sbTmp.AppendC(UTF8STRC("boolean["));
				sbTmp2.AppendC(UTF8STRC("boolean[]"));
				break;
			case 5: //T_CHAR
				sbTmp.AppendC(UTF8STRC("char["));
				sbTmp2.AppendC(UTF8STRC("char[]"));
				break;
			case 6: //T_FLOAT
				sbTmp.AppendC(UTF8STRC("float["));
				sbTmp2.AppendC(UTF8STRC("float[]"));
				break;
			case 7: //T_DOUBLE
				sbTmp.AppendC(UTF8STRC("double["));
				sbTmp2.AppendC(UTF8STRC("double[]"));
				break;
			case 8: //T_BYTE
				sbTmp.AppendC(UTF8STRC("byte["));
				sbTmp2.AppendC(UTF8STRC("byte[]"));
				break;
			case 9: //T_SHORT
				sbTmp.AppendC(UTF8STRC("short["));
				sbTmp2.AppendC(UTF8STRC("short[]"));
				break;
			case 10: //T_INT
				sbTmp.AppendC(UTF8STRC("int["));
				sbTmp2.AppendC(UTF8STRC("int[]"));
				break;
			case 11: //T_LONG
				sbTmp.AppendC(UTF8STRC("long["));
				sbTmp2.AppendC(UTF8STRC("long[]"));
				break;
			default:
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// newarray invalid type: "));
				sb->AppendU16(codePtr[1]);
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp.AppendUTF8Char(']');
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
				sb->AppendC(UTF8STRC("// anewarray stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.AppendC(UTF8STRC("new "));
			this->AppendCodeClassName(sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			sbTmp.AppendC(UTF8STRC("[]"));
			sbTmp2.AppendUTF8Char('[');
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.AppendUTF8Char(']');
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
				sb->AppendC(UTF8STRC("// arraylength Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.AppendC(UTF8STRC(".length"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
			codePtr++;
			break;
		case 0xBF: //athrow
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// athrow Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("throw "));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(";"));
			this->AppendLineNum(sb, env, codePtr);
			sb->AppendC(UTF8STRC("\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->endPtr = codePtr + 1;
			return EndType::Throw;
		case 0xC0: //checkcast
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// checkcast Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			val = ReadMUInt16(&codePtr[1]);
			sbTmp.ClearStr();
			this->ClassNameString(val, sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.AppendUTF8Char('(');
			this->AppendCodeClassName(sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			sbTmp2.AppendUTF8Char(')');
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
			this->ClassNameString(val, sbTmp);
			sbTmp2.ClearStr();
			sbTmp2.Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sbTmp2.AppendC(UTF8STRC(" instanceof "));
			this->AppendCodeClassName(sbTmp2, sbTmp.ToString(), env->importList, env->packageName);
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stacks->Add(Text::String::New(sbTmp2.ToString(), sbTmp2.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("boolean")));
			codePtr += 3;
			break;
		case 0xC2: //monitorenter
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// monitorenter Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("synchronized ("));
			sb->Append(env->stacks->GetItem(env->stacks->GetCount() - 1));
			sb->AppendC(UTF8STRC(")\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("{\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			lev++;
			codePtr++;
			break;
		case 0xC3: //monitorexit
			if (env->stacks->GetCount() == 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// monitorexit Stack error: stack count = "));
				sb->AppendUOSInt(env->stacks->GetCount());
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			lev--;
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("}\r\n"));
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			codePtr++;
			break;
		case 0xC6: //ifnull
			if (env->stacks->GetCount() <= 0)
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("// ifnull stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("null")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("java.lang.Object")));
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
				sb->AppendC(UTF8STRC("// ifnonnull stack invalid"));
				sb->AppendC(UTF8STRC("\r\n"));
				return EndType::Error;
			}
			else
			{
				env->stacks->Add(Text::String::New(UTF8STRC("null")));
				env->stackTypes->Add(Text::String::New(UTF8STRC("java.lang.Object")));
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
			sb->AppendC(UTF8STRC("// Unsupported opcode "));
			sb->AppendHex8(codePtr[0]);
			sb->AppendC(UTF8STRC("\r\n"));
			return EndType::Error;
		}
	}
	env->endPtr = codePtr;
	return EndType::CodeEnd;
}

void IO::JavaClass::DecompileLDC(UInt16 index, IO::JavaClass::DecompileEnv *env) const
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
		env->stackTypes->Add(Text::String::New(UTF8STRC("int")));
	}
	else if (ptr[0] == 4)
	{
		Text::SBAppendF32(sb, ReadMFloat(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::New(UTF8STRC("float")));
	}
	else if (ptr[0] == 5)
	{
		sb.AppendU64(ReadMUInt64(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::New(UTF8STRC("long")));
	}
	else if (ptr[0] == 6)
	{
		sb.AppendDouble(ReadMDouble(&ptr[1]));
		env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
		env->stackTypes->Add(Text::String::New(UTF8STRC("double")));
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
			this->AppendCodeClassName(sb, sbTmp.ToString(), env->importList, env->packageName);
			sb.AppendC(UTF8STRC(".class"));
			env->stacks->Add(Text::String::New(sb.ToString(), sb.GetLength()));
			env->stackTypes->Add(Text::String::New(UTF8STRC("java.lang.Class")));
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
			env->stackTypes->Add(Text::String::New(UTF8STRC("java.lang.String")));
		}
	}
}

void IO::JavaClass::DecompileStore(UInt16 index, IO::JavaClass::DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt codeOfst) const
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	this->AppendIndent(sb, lev);
	if (env->stacks->GetCount() <= 0)
	{
		sb->AppendC(UTF8STRC("// Store stack is empty\r\n"));
		return;
	}
	if (env->localTypes[index] == 0)
	{
		sptr = this->GetLVType(sbuff, index, env->method, codeOfst, env->importList, env->packageName);
		env->localTypes[index] = Text::String::NewP(sbuff, sptr).Ptr();
		this->AppendCodeClassName(sb, env->localTypes[index]->v, env->importList, env->packageName);
		sb->AppendUTF8Char(' ');
	}
	sptr = this->GetLVName(sbuff, index, env->method, codeOfst);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" = "));
	Text::String *s = env->stacks->GetItem(env->stacks->GetCount() - 1);
	if (env->localTypes[index]->Equals(UTF8STRC("boolean")))
	{
		if (s->Equals(UTF8STRC("0")))
		{
			sb->AppendC(UTF8STRC("false"));
		}
		else if (s->Equals(UTF8STRC("1")))
		{
			sb->AppendC(UTF8STRC("true"));
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
	sb->AppendC(UTF8STRC(";"));
	this->AppendLineNum(sb, env, env->codeStart + codeOfst - 1);
	sb->AppendC(UTF8STRC("\r\n"));
	env->stackTypes->Pop()->Release();
	env->stacks->Pop()->Release();
}

IO::JavaClass::EndType IO::JavaClass::DecompileCondBranch(const UInt8 *codePtr, const UInt8 *codeEnd, CondType ct, IO::JavaClass::DecompileEnv *env, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (codePtr > codeEnd)
	{
		Text::StringBuilderUTF8 sbTmp;
		UOSInt initStackCnt = env->stacks->GetCount();
		if (env->method && initStackCnt > env->method->maxStacks)
		{
			sb->AppendC(UTF8STRC("//Stack overflow\r\n"));
			return EndType::Error;
		}
		EndType et = DecompileCode(codeEnd, codePtr - 3, env, lev + 1, sbTmp);
		if (et == EndType::Error)
		{
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
			return et;
		}
		if (initStackCnt + 1 == env->stacks->GetCount() && (et == EndType::Return || et == EndType::Throw || et == EndType::CodeEnd))
		{
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("while ("));
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, false);
			sb->AppendC(UTF8STRC(")\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("}\r\n"));
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
			sb->AppendC(UTF8STRC("while ("));
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, false);
			sb->AppendC(UTF8STRC(")\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("}\r\n"));
			env->stacks->Pop()->Release();
			env->stacks->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->stackTypes->Pop()->Release();
			env->endPtr = codePtr;
			return et;
		}
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("// CondBranch: unknown type2: Stacks = "));
		sb->AppendUOSInt(initStackCnt);
		sb->AppendC(UTF8STRC(", "));
		sb->AppendUOSInt(env->stacks->GetCount());
		sb->AppendC(UTF8STRC(", et = "));
		sb->Append(EndTypeGetName(et));
		sb->AppendC(UTF8STRC("\r\n"));
		sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		return EndType::Error;
	}
	else if (codePtr == codeEnd)
	{
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("if ("));
		AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
		sb->AppendC(UTF8STRC(")\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("{\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("}\r\n"));
		env->stacks->Pop()->Release();
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->endPtr = codePtr;
		return EndType::CodeEnd;
	}
	Text::StringBuilderUTF8 sbTmp;
	UOSInt initStackCnt = env->stacks->GetCount();
	EndType et = DecompileCode(codePtr, codeEnd, env, lev + 1, sbTmp);
	if (et == EndType::Error)
	{
		sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		return et;
	}
	if (initStackCnt + 1 == env->stacks->GetCount() && et == EndType::Goto && sbTmp.GetLength() == 0)
	{
		et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, sbTmp);
		if (et == EndType::Error)
		{
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
			return et;
		}
		else if (et == EndType::CodeEnd && initStackCnt + 2 == env->stacks->GetCount() && sbTmp.GetLength() == 0)
		{
			sbTmp.ClearStr();
			sbTmp.AppendUTF8Char('(');
			AppendCond(sbTmp, env, initStackCnt - 2, ct, true);
			sbTmp.AppendUTF8Char(')');
			sbTmp.AppendUTF8Char('?');
			sbTmp.Append(env->stacks->GetItem(initStackCnt + 0));
			sbTmp.AppendUTF8Char(':');
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
			sb->AppendC(UTF8STRC("while ("));
			AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
			sb->AppendC(UTF8STRC(")\r\n"));
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("{\r\n"));
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
			this->AppendIndent(sb, lev);
			sb->AppendC(UTF8STRC("}\r\n"));
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
			et = DecompileCode(codeEnd, env->endPtr, env, lev + 1, sbTmp2);
			if (et == EndType::Error)
			{
				sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
				return et;
			}
			else if (et == EndType::CodeEnd && initStackCnt == env->stacks->GetCount())
			{
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("if ("));
				AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
				sb->AppendC(UTF8STRC(")\r\n"));
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("{\r\n"));
				sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("}\r\n"));
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("else\r\n"));
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("{\r\n"));
				sb->AppendC(sbTmp2.ToString(), sbTmp2.GetLength());
				this->AppendIndent(sb, lev);
				sb->AppendC(UTF8STRC("}\r\n"));
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
		sb->AppendC(UTF8STRC("if ("));
		AppendCond(sb, env, env->stacks->GetCount() - 2, ct, true);
		sb->AppendC(UTF8STRC(")\r\n"));
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("{\r\n"));
		sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		this->AppendIndent(sb, lev);
		sb->AppendC(UTF8STRC("}\r\n"));
		env->stacks->Pop()->Release();
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->stackTypes->Pop()->Release();
		env->endPtr = codeEnd;
		return et;
	}

	this->AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("// CondBranch: unknown type: Stacks = "));
	sb->AppendUOSInt(initStackCnt);
	sb->AppendC(UTF8STRC(", "));
	sb->AppendUOSInt(env->stacks->GetCount());
	sb->AppendC(UTF8STRC(", et = "));
	sb->Append(EndTypeGetName(et));
	sb->AppendC(UTF8STRC("\r\n"));
	return EndType::Error;
}

UTF8Char *IO::JavaClass::DecompileMethod(UInt16 methodIndex, UTF8Char *nameBuff, UInt16 *classIndex, UTF8Char *retType, IO::JavaClass::DecompileEnv *env, NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	if (methodIndex == 0 || methodIndex >= this->constPoolCnt)
	{
		sb->AppendC(UTF8STRC("// method index out of range: "));
		sb->AppendU16(methodIndex);
		sb->AppendC(UTF8STRC("\r\n"));
		return 0;
	}
	const UInt8 *constPtr = this->constPool[methodIndex];
	if (constPtr == 0)
	{
		sb->AppendC(UTF8STRC("// method constPool is not used: "));
		sb->AppendU16(methodIndex);
		sb->AppendC(UTF8STRC("\r\n"));
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
		sb->AppendC(UTF8STRC("// method constPool is not supported: "));
		sb->AppendU16(methodIndex);
		sb->AppendC(UTF8STRC("\r\n"));
		return 0;
	}

	*classIndex = ReadMUInt16(&constPtr[1]);
	UInt16 nameTypeIndex = ReadMUInt16(&constPtr[3]);
	constPtr = this->constPool[nameTypeIndex];
	if (constPtr == 0 || constPtr[0] != 12)
	{
		sb->AppendC(UTF8STRC("// method constPool is not nameAndType: "));
		sb->AppendU16(constPtr[0]);
		sb->AppendC(UTF8STRC("\r\n"));
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
		sb->AppendC(UTF8STRC("// method type is not valid: "));
		sb->AppendU16(ReadMUInt16(&constPtr[3]));
		sb->AppendC(UTF8STRC("\r\n"));
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
				Type2String(sptr, sbTmp);
				Text::StrConcatC(retType, sbTmp.ToString(), sbTmp.GetLength());
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
		sb->AppendC(UTF8STRC("// invokespecial Stack error: paramCnt = "));
		sb->AppendUOSInt(paramCnt);
		sb->AppendC(UTF8STRC(", stack count = "));
		sb->AppendUOSInt(env->stacks->GetCount());
		sb->AppendC(UTF8STRC("\r\n"));
		return 0;
	}

	UOSInt i;
	Text::String *paramStr;
	sb->AppendUTF8Char('(');
	i = 0;
	while (i < paramCnt)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		paramStr = env->stacks->GetItem(env->stacks->GetCount() - paramCnt + i);
		if (params[i] == 'Z') //boolean
		{
			if (paramStr->Equals(UTF8STRC("1")))
			{
				sb->AppendC(UTF8STRC("true"));
			}
			else if (paramStr->Equals(UTF8STRC("0")))
			{
				sb->AppendC(UTF8STRC("false"));
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
			UTF8Char *sptr;
			UInt32 c;
			if (paramStr->ToUInt32(&c))
			{
				Text::StrWriteChar(sbuff, (UTF32Char)c)[0] = 0;
				sptr = Text::JSText::ToJSText(sbuff2, sbuff);
				sb->AppendC(sbuff2, (UOSInt)(sptr - sbuff2));
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
	sb->AppendUTF8Char(')');
	i = env->stacks->GetCount() - paramCnt;
	while (env->stacks->GetCount() > i)
	{
		env->stacks->Pop()->Release();
		env->stackTypes->Pop()->Release();
	}
	return this->GetConstName(nameBuff, ReadMUInt16(&constPtr[1]));
}

IO::JavaClass *IO::JavaClass::ParseFile(Text::CStringNN fileName)
{
	IO::JavaClass *cls = 0;
	UInt64 fileLen;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fileLen = fs.GetLength();
	if (fileLen >= 26 && fileLen <= 1048576)
	{
		Data::ByteBuffer buff((UOSInt)fileLen);
		if (fs.Read(buff) == fileLen)
		{
			cls = ParseBuff(fileName, buff);
		}
	}
	return cls;
}

IO::JavaClass *IO::JavaClass::ParseBuff(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff)
{
	if (buff.GetSize() < 26)
	{
		return 0;
	}
	if (ReadMUInt32(&buff[0]) != 0xCAFEBABE)
	{
		return 0;
	}
	IO::JavaClass *cls;
	NEW_CLASS(cls, IO::JavaClass(sourceName, buff));
	return cls;
}

IO::JavaClass *IO::JavaClass::ParseBuff(Text::CString sourceName, Data::ByteArrayR buff)
{
	if (buff.GetSize() < 26)
	{
		return 0;
	}
	if (ReadMUInt32(&buff[0]) != 0xCAFEBABE)
	{
		return 0;
	}
	IO::JavaClass *cls;
	NEW_CLASS(cls, IO::JavaClass(sourceName, buff));
	return cls;
}

Text::CString IO::JavaClass::EndTypeGetName(EndType et)
{
	switch (et)
	{
	case EndType::Error:
		return CSTR("Error");
	case EndType::CodeEnd:
		return CSTR("CodeEnd");
	case EndType::Return:
		return CSTR("Return");
	case EndType::Goto:
		return CSTR("Goto");
	case EndType::Throw:
		return CSTR("Throw");
	default:
		return CSTR("Unknown");
	}
}
