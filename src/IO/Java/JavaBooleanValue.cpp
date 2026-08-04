#include "Stdafx.h"
#include "IO/Java/JavaBooleanValue.h"

IO::Java::JavaBooleanValue::JavaBooleanValue(Bool value)
{
	this->value = value;
}

IO::Java::JavaBooleanValue::~JavaBooleanValue()
{
}

Bool IO::Java::JavaBooleanValue::GetValue() const
{
	return this->value;
}

void IO::Java::JavaBooleanValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	sb->Append(this->value? CSTR("true") : CSTR("false"));
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaBooleanValue::GetElementType() const
{
	return ElementType::Boolean;
}
