#include "Stdafx.h"
#include "IO/Java/JavaLongValue.h"

IO::Java::JavaLongValue::JavaLongValue(Int64 value)
{
	this->value = value;
}

IO::Java::JavaLongValue::~JavaLongValue()
{
}

Int64 IO::Java::JavaLongValue::GetValue() const
{
	return this->value;
}

void IO::Java::JavaLongValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->AppendI64(this->value);
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaLongValue::GetElementType() const
{
	return ElementType::Long;
}
