#include "Stdafx.h"
#include "IO/Java/JavaUtil.h"
#include "Text/JSText.h"

void IO::Java::JavaUtil::AccessFlags(NN<Text::StringBuilderUTF8> sb, UInt16 accessFlags)
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

void IO::Java::JavaUtil::ConstVal(NN<Text::StringBuilderUTF8> sb, UnsafeArray<UInt8> constVal, Bool brankets, UnsafeArray<UnsafeArrayOpt<UInt8>> constPool)
{
	if (constVal[0] == 3)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		sb->AppendU32(ReadMUInt32(&constVal[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (constVal[0] == 4)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		Text::SBAppendF32(sb, ReadMFloat(&constVal[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (constVal[0] == 5)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		sb->AppendU64(ReadMUInt64(&constVal[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (constVal[0] == 6)
	{
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
		}
		Text::SBAppendF64(sb, ReadMDouble(&constVal[1]));
		if (brankets)
		{
			sb->AppendUTF8Char(')');
		}
	}
	else if (constVal[0] == 8) //String
	{
		UInt16 sindex = ReadMUInt16(&constVal[1]);
		UInt16 strLen;
		if (constPool[sindex].SetTo(constVal) && constVal[0] == 1)
		{
			strLen = ReadMUInt16(&constVal[1]);
			if (brankets)
			{
				sb->AppendUTF8Char(' ');
				sb->AppendUTF8Char('(');
			}
			sb->AppendC(constVal + 3, strLen);
			if (brankets)
			{
				sb->AppendUTF8Char(')');
			}
		}
	}
	else if (constVal[0] == 1) //String
	{
		UInt16 strLen;
		strLen = ReadMUInt16(&constVal[1]);
		if (brankets)
		{
			sb->AppendUTF8Char(' ');
			sb->AppendUTF8Char('(');
			sb->AppendC(constVal + 3, strLen);
			sb->AppendUTF8Char(')');
		}
		else
		{
			UnsafeArray<const UTF8Char> s = Text::StrCopyNewC(UnsafeArray<const UInt8>(constVal) + 3, strLen);
			NN<Text::String> j = Text::JSText::ToNewJSTextDQuote(s);
			sb->Append(j);
			j->Release();
			Text::StrDelNew(s);
		}
	}
}
