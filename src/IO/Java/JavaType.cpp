#include "Stdafx.h"
#include "Data/ArrayListNative.hpp"
#include "IO/Java/JavaType.h"
#include "IO/Java/JavaUtil.h"

IO::Java::JavaType::JavaType(Text::CStringNN typeStr)
{
	this->typeStr = Text::String::New(typeStr);
}

IO::Java::JavaType::~JavaType()
{
	this->typeStr->Release();
}

NN<Text::String> IO::Java::JavaType::GetTypeStr() const
{
	return this->typeStr;
}

void IO::Java::JavaType::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName) const
{
	IO::Java::JavaUtil::TypeString(sb, this->typeStr->v, importList, packageName);
}

void IO::Java::JavaType::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	IO::Java::JavaUtil::TypeString(sb, this->typeStr->v, nullptr, nullptr);
}

Optional<IO::Java::JavaType> IO::Java::JavaType::ParseType(InOutParam<UnsafeArray<UTF8Char>> typeStr, UnsafeArray<UTF8Char> endPtr)
{

	UnsafeArray<UTF8Char> typeStart = typeStr.Get();
	UnsafeArray<UTF8Char> typePtr = typeStart;
	if (typePtr >= endPtr)
		return nullptr;
	NN<IO::Java::JavaType> jType;
	UTF8Char c = typePtr[0];
	UTF8Char currType;
	if (c == '[')
	{
		typePtr++;
		while (typePtr < endPtr && typePtr[0] == '[')
		{
			typePtr++;
		}
		if (typePtr >= endPtr)
			return nullptr;
		c = typePtr[0];
	}
	switch (c)
	{
	case 'L':
		{
			Data::ArrayListNative<UTF8Char> typeName;
			typePtr++;
			typeName.Add(currType = 'L');
			while (true)
			{
				if (typePtr >= endPtr)
					return nullptr;
				if (typePtr[0] == ';')
				{
					if (typeName.Pop() != 'L')
					{
						printf("JavaType.ParseType: Mismatched type name\r\n");
						typeStr.Set(typePtr + 1);
						return nullptr;
					}
					if (typeName.GetCount() == 0)
					{
						typeStr.Set(typePtr + 1);
						NEW_CLASSNN(jType, IO::Java::JavaType(CSTRP(typeStart, typePtr + 1)));
						return jType;
					}
					currType = typeName.GetItem(typeName.GetCount() - 1);
				}
				else if (typePtr[0] == '<')
				{
					typeName.Add(currType = '<');
				}
				else if (typePtr[0] == '>')
				{
					if (typeName.Pop() != '<')
					{
						printf("JavaType.ParseType: Mismatched type name\r\n");
						typeStr.Set(typePtr + 1);
						return nullptr;
					}
					currType = typeName.GetItem(typeName.GetCount() - 1);
				}
				else if (currType == 'L')
				{

				}
				else if (typePtr[0] == 'L')
				{
					typeName.Add(currType = 'L');
				}
				else if (typePtr[0] == 'V')
				{

				}
				else if (typePtr[0] == '*')
				{

				}
				else
				{
					printf("JavaType.ParseType: Unknown type name: %c\r\n", typePtr[0]);
					typeStr.Set(typePtr + 1);
					return nullptr;
				}
				typePtr++;
			}
		}
	case 'V': //void
		NEW_CLASSNN(jType, IO::Java::JavaType(CSTRP(typeStart, typePtr + 1)));
		typeStr.Set(typePtr + 1);
		return jType;
	default:
		printf("JavaType.ParseType: Unknown type: %c\r\n", c);
		printf("JavaType.ParseType: TypeStr: %s\r\n", typeStart.Ptr());
		return nullptr;
	}
}
