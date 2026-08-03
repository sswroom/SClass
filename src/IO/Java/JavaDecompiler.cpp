#include "Stdafx.h"
#include "IO/Java/JavaDecompiler.h"
#include "IO/Java/JavaUtil.h"

void IO::Java::JavaDecompiler::AppendIndent(NN<Text::StringBuilderUTF8> sb, UIntOS lev)
{
	sb->AppendChar('\t', lev);
}

void IO::Java::JavaDecompiler::AppendCodeClassContent(NN<Text::StringBuilderUTF8> sb, NN<IO::Java::JavaClass> cls, UIntOS lev, UnsafeArray<const UTF8Char> className, NN<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	UInt16 accessFlags = cls->GetAccessFlags();


	this->AppendIndent(sb, lev);
	if (accessFlags & 1) //ACC_PUBLIC
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
	UIntOS i;
	UIntOS j;
	if (cls->GetSignature(sbuff).NotNull())
	{
		UnsafeArray<const UTF8Char> ptr = sbuff;
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
		ptr = JavaUtil::TypeString(sbTmp, ptr, importList, packageName);
		if (!sbTmp.Equals(UTF8STRC("Object")))
		{
			sb->AppendC(UTF8STRC(" extends "));
			sb->AppendC(sbTmp.ToString(), sbTmp.GetLength());
		}
/*		if (this->interfaces && this->interfaceCnt > 0)
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
				ptr = AppendType(sb, ptr, importList, packageName);
				i++;
			}
		}*/
	}
	else
	{
		sbTmp.ClearStr();
/*		if (this->GetSuperClass(sbTmp))
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
		}*/
	}
	sb->AppendC(UTF8STRC(" {\r\n"));
/*	i = 0;
	j = this->FieldsGetCount();
	while (i < j)
	{
		AppendIndent(sb, lev + 1);
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
	}*/
	AppendIndent(sb, lev);
	sb->AppendC(UTF8STRC("}\r\n"));
}

IO::Java::JavaDecompiler::JavaDecompiler()
{
}

IO::Java::JavaDecompiler::~JavaDecompiler()
{
}

Bool IO::Java::JavaDecompiler::Decompile(NN<Text::StringBuilderUTF8> sb, NN<IO::Java::JavaClass> cls)
{
	Text::StringBuilderUTF8 sbTmp;
	if (!cls->GetClassNameFull(sbTmp))
	{
		return false;
	}
	UnsafeArrayOpt<const UTF8Char> packageName = nullptr;
	Data::ArrayListStringNN importList;
	UIntOS i = sbTmp.LastIndexOf('.');
	UIntOS j;
	if (i != INVALID_INDEX)
	{
		sb->AppendC(UTF8STRC("package "));
		sb->AppendC(sbTmp.ToString(), i);
		sb->AppendC(UTF8STRC(";\r\n"));
		packageName = Text::StrCopyNewC(sbTmp.ToString(), i).Ptr();
	}
	sb->AppendC(UTF8STRC("\r\n"));

	Text::StringBuilderUTF8 sbClass;
	this->AppendCodeClassContent(sbClass, cls, 0, sbTmp.ToString() + i + 1, importList, packageName);

	if (importList.GetCount() > 0)
	{
		i = 0;
		j = importList.GetCount();
		while (i < j)
		{
			sb->AppendC(UTF8STRC("import "));
			sb->Append(importList.GetItemNoCheck(i));
			sb->AppendC(UTF8STRC(";\r\n"));
			i++;
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	sb->AppendC(sbClass.ToString(), sbClass.GetLength());

	importList.FreeAll();
	SDEL_TEXT(packageName);
	return true;
}
