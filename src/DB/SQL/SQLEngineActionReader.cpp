#include "Stdafx.h"
#include "DB/SQL/SQLEngineActionReader.h"

DB::SQL::SQLEngineActionReader::SQLEngineActionReader(IntOS rowChanged)
{
	this->rowChanged = rowChanged;
}

DB::SQL::SQLEngineActionReader::~SQLEngineActionReader()
{
}

Bool DB::SQL::SQLEngineActionReader::ReadNext()
{
	return false;
}

UIntOS DB::SQL::SQLEngineActionReader::ColCount()
{
	return 0;
}

IntOS DB::SQL::SQLEngineActionReader::GetRowChanged()
{
	return this->rowChanged;
}

Int32 DB::SQL::SQLEngineActionReader::GetInt32(UIntOS colIndex)
{
	return 0;
}

Int64 DB::SQL::SQLEngineActionReader::GetInt64(UIntOS colIndex)
{
	return 0;
}

UnsafeArrayOpt<WChar> DB::SQL::SQLEngineActionReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	return nullptr;
}
Bool DB::SQL::SQLEngineActionReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}
Optional<Text::String> DB::SQL::SQLEngineActionReader::GetNewStr(UIntOS colIndex)
{
	return nullptr;
}
UnsafeArrayOpt<UTF8Char> DB::SQL::SQLEngineActionReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	return nullptr;
}
Data::Timestamp DB::SQL::SQLEngineActionReader::GetTimestamp(UIntOS colIndex)
{
	return nullptr;
}
Double DB::SQL::SQLEngineActionReader::GetDblOrNAN(UIntOS colIndex)
{
	return NAN;
}
Bool DB::SQL::SQLEngineActionReader::GetBool(UIntOS colIndex)
{
	return false;
}
UIntOS DB::SQL::SQLEngineActionReader::GetBinarySize(UIntOS colIndex)
{
	return 0;
}
UIntOS DB::SQL::SQLEngineActionReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}
Optional<Math::Geometry::Vector2D> DB::SQL::SQLEngineActionReader::GetVector(UIntOS colIndex)
{
	return nullptr;
}

Bool DB::SQL::SQLEngineActionReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	return false;
}

Bool DB::SQL::SQLEngineActionReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	return false;
}

Bool DB::SQL::SQLEngineActionReader::IsNull(UIntOS colIndex)
{
	return false;
}
UnsafeArrayOpt<UTF8Char> DB::SQL::SQLEngineActionReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	return nullptr;
}

DB::DBUtil::ColType DB::SQL::SQLEngineActionReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	return DB::DBUtil::ColType::CT_Unknown;
}

Bool DB::SQL::SQLEngineActionReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	return false;
}
