#include "Stdafx.h"
#include "IO/Java/JavaStringValue.h"
#include "Text/JSText.h"

IO::Java::JavaStringValue::JavaStringValue(Text::CStringNN s)
{
	this->s = Text::String::New(s);
}

IO::Java::JavaStringValue::~JavaStringValue()
{
	this->s->Release();
}

NN<Text::String> IO::Java::JavaStringValue::GetString() const
{
	return this->s;
}

void IO::Java::JavaStringValue::ToString(NN<Text::StringBuilderUTF8> sb, Optional<Data::ArrayListStringNN> importList, UnsafeArrayOpt<const UTF8Char> packageName)
{
	NN<Text::String> s = Text::JSText::ToNewJSTextDQuote(this->s->v);
	sb->Append(s);
	s->Release();
}

IO::Java::JavaElementValue::ElementType IO::Java::JavaStringValue::GetElementType() const
{
	return ElementType::String;
}
