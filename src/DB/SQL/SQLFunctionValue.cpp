#include "Stdafx.h"
#include "DB/SQL/SQLFunctionValue.h"

DB::SQL::SQLFunctionValue::SQLFunctionValue(NN<SQLObjectPath> objPath)
{
	this->objectPath = objPath;
}

DB::SQL::SQLFunctionValue::~SQLFunctionValue()
{
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLFunctionValue::GetValueType() const
{
	return DB::SQL::SQLValue::ValueType::Function;
}

NN<DB::SQL::SQLObjectPath> DB::SQL::SQLFunctionValue::GetObjectPath() const
{
	return this->objectPath;
}

void DB::SQL::SQLFunctionValue::AddParam(NN<SQLValue> param)
{
	this->paramList.Add(param);
}

UIntOS DB::SQL::SQLFunctionValue::GetParamCount() const
{
	return this->paramList.GetCount();
}

Optional<DB::SQL::SQLValue> DB::SQL::SQLFunctionValue::GetParam(UIntOS index) const
{
	return this->paramList.GetItem(index);
}
