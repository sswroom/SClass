#include "Stdafx.h"
#include "DB/SQL/SQLValueString.h"

DB::SQL::SQLValueString::SQLValueString(Text::CStringNN value)
{
	this->value = Text::String::New(value);
}

DB::SQL::SQLValueString::~SQLValueString()
{
	this->value->Release();
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueString::GetValueType() const
{
	return ValueType::String;
}

NN<Text::String> DB::SQL::SQLValueString::GetValue() const
{
	return this->value;
}
