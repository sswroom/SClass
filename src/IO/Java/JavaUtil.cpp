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


UnsafeArray<const UTF8Char> IO::Java::JavaUtil::TypeString(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> typeStr, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	NN<Data::ArrayListStringNN> nnimportList;
	IntOS arrLev = 0;
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
		if (importList.NotNull() || packageName.NotNull())
		{
			Text::StringBuilderUTF8 sbTmp;
			UIntOS subcls = INVALID_INDEX;
			UIntOS tmpIndex = INVALID_INDEX;
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
							typeStr = TypeString(sbTmp, typeStr, importList, packageName);
							found = true;
						}
					}
				}
				else
				{
					sbTmp.AppendUTF8Char(c);
				}
			}
			UnsafeArray<UTF8Char> sptr = sbTmp.v;
			UIntOS sptrLen = sbTmp.GetLength();
			UIntOS i;
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
				UnsafeArray<const UTF8Char> nnpackageName;
				if (packageName.SetTo(nnpackageName) && Text::StrStartsWith(sptr, nnpackageName) && Text::StrCharCnt(nnpackageName) == i)
				{
					
				}
				else if (Text::StrStartsWithC(sptr, sptrLen, UTF8STRC("java.lang.")) && i == 9)
				{

				}
				else if (importList.SetTo(nnimportList))
				{
					if (nnimportList->SortedIndexOfC(Text::CStringNN(sptr, sptrLen)) < 0)
					{
						nnimportList->SortedInsert(Text::String::New(sptr, sptrLen));
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
					sptrLen = sbTmp.GetLength();
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
							typeStr = TypeString(sb, typeStr, importList, packageName);
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
	case '*':
		sb->AppendC(UTF8STRC("?"));
		break;
	default:
		printf("JavaUtil.TypeString: Unknown type: %c\r\n", c);
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