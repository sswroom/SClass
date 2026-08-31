#include "Stdafx.h"
#include "DB/SQL/SQLValueNull.h"

DB::SQL::SQLValueNull::SQLValueNull()
{
}

DB::SQL::SQLValueNull::~SQLValueNull()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLValueNull::GetValueType() const
{
	return ValueType::Null;
}
