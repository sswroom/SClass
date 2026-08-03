#include "Stdafx.h"
#include "IO/Java/JavaIntegerValue.h"

IO::Java::JavaIntegerValue::JavaIntegerValue(Int32 value)
{
	this->value = value;
}

IO::Java::JavaIntegerValue::~JavaIntegerValue()
{
}

Int32 IO::Java::JavaIntegerValue::GetValue() const
{
	return this->value;
}

void IO::Java::JavaIntegerValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->AppendI32(this->value);
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaIntegerValue::GetElementType() const
{
	return ElementType::Integer;
}
