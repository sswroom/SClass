#include "Stdafx.h"
#include "IO/Java/JavaClassValue.h"

IO::Java::JavaClassValue::JavaClassValue(NN<JavaType> type)
{
	this->type = type;
}

IO::Java::JavaClassValue::~JavaClassValue()
{
	this->type.Delete();
}

NN<IO::Java::JavaType> IO::Java::JavaClassValue::GetType() const
{
	return this->type;
}

void IO::Java::JavaClassValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	this->type->ToString(sb, importList, packageName);
	sb->AppendC(UTF8STRC(".class"));
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaClassValue::GetElementType() const
{
	return ElementType::Class;
}