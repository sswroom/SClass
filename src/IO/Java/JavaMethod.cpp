#include "Stdafx.h"
#include "IO/Java/JavaMethod.h"

IO::Java::JavaMethod::JavaMethod(UInt16 accessFlags, Text::CStringNN name, NN<JavaType> returnType)
{
	this->accessFlags = accessFlags;
	this->name = Text::String::New(name);
	this->returnType = returnType;
}

IO::Java::JavaMethod::~JavaMethod()
{
	this->name->Release();
	this->returnType.Delete();
	this->annoList.DeleteAll();
	this->paramNames.FreeAll();
	this->paramTypes.DeleteAll();
}

void IO::Java::JavaMethod::AddParam(NN<JavaType> paramType, Text::CStringNN paramName)
{
	this->paramTypes.Add(paramType);
	this->paramNames.Add(Text::String::New(paramName));
}

void IO::Java::JavaMethod::AddAnnotation(NN<JavaAnnotation> anno)
{
	this->annoList.Add(anno);
}

UInt16 IO::Java::JavaMethod::GetAccessFlags() const
{
	return this->accessFlags;
}

NN<Text::String> IO::Java::JavaMethod::GetName() const
{
	return this->name;
}

NN<IO::Java::JavaType> IO::Java::JavaMethod::GetReturnType() const
{
	return this->returnType;
}

UIntOS IO::Java::JavaMethod::GetAnnotationCount() const
{
	return this->annoList.GetCount();
}

Optional<IO::Java::JavaAnnotation> IO::Java::JavaMethod::GetAnnotation(UIntOS index) const
{
	return this->annoList.GetItem(index);
}

void IO::Java::JavaMethod::ToAnnotation(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const
{
	UIntOS i = 0;
	UIntOS j = this->annoList.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC("\r\n"));
		}
		this->annoList.GetItemNoCheck(i)->ToString(sb, importList, packageName);
		i++;
	}
}

void IO::Java::JavaMethod::ToDeclaration(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const
{
	if (this->accessFlags & 1)
	{
		sb->AppendC(UTF8STRC("public "));
	}
	if (this->accessFlags & 2)
	{
		sb->AppendC(UTF8STRC("private "));
	}
	if (this->accessFlags & 4)
	{
		sb->AppendC(UTF8STRC("protected "));
	}
	if (this->accessFlags & 8)
	{
		sb->AppendC(UTF8STRC("static "));
	}
	if (this->accessFlags & 0x10)
	{
		sb->AppendC(UTF8STRC("final "));
	}
	if (this->accessFlags & 0x20)
	{
		sb->AppendC(UTF8STRC("synchronized "));
	}
	if (this->accessFlags & 0x100)
	{
		sb->AppendC(UTF8STRC("native "));
	}
	if (this->accessFlags & 0x400)
	{
		sb->AppendC(UTF8STRC("abstract "));
	}
	this->returnType->ToString(sb, importList, packageName);
	sb->AppendUTF8Char(' ');
	this->ToDeclarationNameParams(sb, importList, packageName);
}

void IO::Java::JavaMethod::ToDeclarationNameParams(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const
{
	sb->Append(this->name);
	sb->AppendUTF8Char('(');
	UIntOS i = 0;
	UIntOS j = this->paramTypes.GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendC(UTF8STRC(", "));
		}
		this->paramTypes.GetItemNoCheck(i)->ToString(sb, importList, packageName);
		sb->AppendUTF8Char(' ');
		sb->Append(this->paramNames.GetItemNoCheck(i));
		i++;
	}
	sb->AppendUTF8Char(')');
}

Optional<IO::Java::JavaMethod> IO::Java::JavaMethod::ParseMethod(NN<JavaClass> cls, UnsafeArray<const UInt8> methodPtr)
{
	UInt16 accessFlags = ReadMUInt16(&methodPtr[0]);
	UInt16 nameIndex = ReadMUInt16(&methodPtr[2]);
	UInt16 descriptorIndex = ReadMUInt16(&methodPtr[4]);
	UnsafeArray<UInt8> funcDef;
	UIntOS funcDefLen;
	if (!cls->GetConstUTF8(descriptorIndex, funcDefLen).SetTo(funcDef))
	{
		return nullptr;
	}
	UnsafeArrayOpt<const UInt8> optannoPtr = nullptr;
	UnsafeArray<const UInt8> annoPtr;
	UIntOS annoLen = 0;
	UIntOS nParam = 0;
	Data::ArrayListNative<UInt16> exList;
	Data::ArrayListStringNN paramNames;
	Data::ArrayListNN<JavaType> paramTypes;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt16 attrCnt = ReadMUInt16(&methodPtr[6]);
	UInt16 i;
	UIntOS j;
	UnsafeArray<const UInt8> ptr = &methodPtr[8];
	i = 0;
	while (i < attrCnt)
	{
		UInt32 len = ReadMUInt32(&ptr[2]);
		UInt16 nameIndex = ReadMUInt16(&ptr[0]);
		UInt16 j;
		sbuff[0] = 0;
		if (cls->GetConstUTF8(sbuff, nameIndex).SetTo(sptr))
		{
			if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Code")))
			{

			}
			else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Exceptions")))
			{
				UInt16 exCnt = ReadMUInt16(&ptr[6]);
				UInt16 j = 0;
				while (j < exCnt)
				{
					exList.Add(ReadMUInt16(&ptr[8 + j * 2]));
					j++;
				}
			}
			else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Signature")))
			{
				if (len == 2)
				{ 
					UInt16 signatureIndex = ReadMUInt16(&ptr[6]);
					cls->GetConstUTF8(signatureIndex, funcDefLen).SetTo(funcDef);
				}
			}
			else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("RuntimeVisibleAnnotations")))
			{
				optannoPtr = &ptr[6];
				annoLen = len;
			}
			else if (Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("MethodParameters")))
			{
				UnsafeArray<const UInt8> paramPtr = &ptr[6];
				nParam = paramPtr[0];
				paramPtr++;
				j = 0;
				while (j < nParam)
				{
					UInt16 nameIndex = ReadMUInt16(&paramPtr[0]);
					if (cls->GetConstUTF8(sbuff, nameIndex).SetTo(sptr))
					{
						paramNames.Add(Text::String::NewP(sbuff, sptr));
					}
					paramPtr += 4;
					j++;
				}
			}
		}
		ptr += len + 6;

		i++;
	}
	NN<JavaMethod> method;
	Optional<JavaMethod> optMethod = nullptr;
	Bool succ = true;
	if (funcDef[0] != '(')
	{
		succ = false;
	}
	else
	{
		UnsafeArray<UInt8> funcPtr = funcDef + 1;
		UnsafeArray<UInt8> funcEnd = funcDef + funcDefLen;
		while (funcPtr[0] != ')')
		{
			if (funcPtr >= funcEnd)
			{
				succ = false;
				break;
			}
			else
			{
				NN<JavaType> paramType;
				if (!JavaType::ParseType(funcPtr, funcEnd).SetTo(paramType))
				{
					succ = false;
					break;
				}
				paramTypes.Add(paramType);
			}
		}
		if (succ)
		{
			NN<JavaType> returnType;
			funcPtr++;
			if (!JavaType::ParseType(funcPtr, funcEnd).SetTo(returnType))
			{
				succ = false;
			}
			else
			{
				if (!cls->GetConstUTF8(sbuff, nameIndex).SetTo(sptr))
				{
					succ = false;
					returnType.Delete();
				}
				else
				{
					NEW_CLASSNN(method, JavaMethod(accessFlags, CSTRP(sbuff, sptr), returnType));
					if (paramTypes.GetCount() == paramNames.GetCount())
					{
						j = 0;
						while (j < paramTypes.GetCount())
						{
							method->AddParam(paramTypes.GetItemNoCheck(j), paramNames.GetItemNoCheck(j)->ToCString());
							j++;
						}
					}
					else
					{
						paramTypes.DeleteAll();
					}
					optMethod = method;

					if (optannoPtr.SetTo(annoPtr))
					{
						Data::ArrayListNN<JavaAnnotation> annoList;
						IO::Java::JavaElementValue::ParseAnnotations(annoList, cls, annoPtr, annoPtr + annoLen);
						NN<JavaAnnotation> anno;
						j = 0;
						while (j < annoList.GetCount())
						{
							anno = annoList.GetItemNoCheck(j);
							method->AddAnnotation(anno);
							j++;
						}
					}
				}
			}
		}
	}

	if (optMethod.IsNull())
	{
		paramTypes.DeleteAll();
	}
	paramNames.FreeAll();
	return optMethod;
}
