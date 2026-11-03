#include "Stdafx.h"
#include "IO/Java/JavaShortValue.h"

IO::Java::JavaShortValue::JavaShortValue(Int16 value)
{
	this->value = value;
}

IO::Java::JavaShortValue::~JavaShortValue()
{
}

Int16 IO::Java::JavaShortValue::GetValue() const
{
	return this->value;
}

void IO::Java::JavaShortValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->AppendI16(this->value);
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaShortValue::GetElementType() const
{
	return ElementType::Short;
}
