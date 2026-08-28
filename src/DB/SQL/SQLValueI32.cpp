#include "Stdafx.h"
#include "DB/SQL/SQLValueI32.h"

DB::SQL::SQLValueI32::SQLValueI32(Int32 value)
{
	this->value = value;
}

DB::SQL::SQLValueI32::~SQLValueI32()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueI32::GetValueType() const
{
	return ValueType::I32;
}

Int32 DB::SQL::SQLValueI32::GetValue() const
{
	return this->value;
}
