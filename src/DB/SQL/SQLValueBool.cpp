#include "Stdafx.h"
#include "DB/SQL/SQLValueBool.h"

DB::SQL::SQLValueBool::SQLValueBool(Bool value)
{
	this->value = value;
}

DB::SQL::SQLValueBool::~SQLValueBool()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueBool::GetValueType() const
{
	return ValueType::Bool;
}

Bool DB::SQL::SQLValueBool::GetValue() const
{
	return this->value;
}
