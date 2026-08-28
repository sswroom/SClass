#include "Stdafx.h"
#include "DB/SQL/SQLValueI64.h"

DB::SQL::SQLValueI64::SQLValueI64(Int64 value)
{
	this->value = value;
}

DB::SQL::SQLValueI64::~SQLValueI64()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueI64::GetValueType() const
{
	return ValueType::I64;
}

Int64 DB::SQL::SQLValueI64::GetValue() const
{
	return this->value;
}
