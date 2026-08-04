#include "Stdafx.h"
#include "IO/Java/JavaEnumValue.h"

IO::Java::JavaEnumValue::JavaEnumValue(NN<JavaType> type, Text::CStringNN constName)
{
	this->type = type;
	this->constName = Text::String::New(constName);
}

IO::Java::JavaEnumValue::~JavaEnumValue()
{
	this->type.Delete();
	this->constName->Release();
}

NN<IO::Java::JavaType> IO::Java::JavaEnumValue::GetType() const
{
	return this->type;
}

NN<Text::String> IO::Java::JavaEnumValue::GetConstName() const
{
	return this->constName;
}

void IO::Java::JavaEnumValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	this->type->ToString(sb, importList, packageName);
	sb->AppendC(UTF8STRC("."));
	sb->Append(this->constName);
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaEnumValue::GetElementType() const
{
	return ElementType::Enum;
}