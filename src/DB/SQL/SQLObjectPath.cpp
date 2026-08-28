#include "Stdafx.h"
#include "DB/SQL/SQLObjectPath.h"

DB::SQL::SQLObjectPath::SQLObjectPath(Text::CStringNN objName, Optional<SQLObjectPath> parentPath)
{
	this->objectName = Text::String::New(objName);
	this->parentPath = parentPath;
}

DB::SQL::SQLObjectPath::~SQLObjectPath()
{
	this->objectName->Release();
	this->parentPath.Delete();
}

DB::SQL::SQLValue::ValueType DB::SQL::SQLObjectPath::GetValueType() const
{
	return ValueType::ObjectPath;
}

NN<Text::String> DB::SQL::SQLObjectPath::GetObjectName() const
{
	return this->objectName;
}

Optional<DB::SQL::SQLObjectPath> DB::SQL::SQLObjectPath::GetParentPath() const
{
	return this->parentPath;
}
