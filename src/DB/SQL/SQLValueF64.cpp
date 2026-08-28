#include "Stdafx.h"
#include "DB/SQL/SQLValueF64.h"

DB::SQL::SQLValueF64::SQLValueF64(Double value)
{
	this->value = value;
}

DB::SQL::SQLValueF64::~SQLValueF64()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueF64::GetValueType() const
{
	return ValueType::F64;
}

Double DB::SQL::SQLValueF64::GetValue() const
{
	return this->value;
}
