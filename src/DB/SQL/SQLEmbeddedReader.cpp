#include "Stdafx.h"
#include "DB/SQL/SQLEmbeddedReader.h"

DB::SQL::SQLEmbeddedReader::SQLEmbeddedReader(NN<DBReader> r, NN<SQLEngineTable> table)
{
	this->r = r;
	this->table = table;
}

DB::SQL::SQLEmbeddedReader::~SQLEmbeddedReader()
{
	this->table->CloseReader(this->r);
}

Bool DB::SQL::SQLEmbeddedReader::ReadNext()
{
	return this->r->ReadNext();
}

UIntOS DB::SQL::SQLEmbeddedReader::ColCount()
{
	return this->r->ColCount();
}

IntOS DB::SQL::SQLEmbeddedReader::GetRowChanged()
{
	return this->r->GetRowChanged();
}

Int32 DB::SQL::SQLEmbeddedReader::GetInt32(UIntOS colIndex)
{
	return this->r->GetInt32(colIndex);
}

Int64 DB::SQL::SQLEmbeddedReader::GetInt64(UIntOS colIndex)
{
	return this->r->GetInt64(colIndex);
}

UnsafeArrayOpt<WChar> DB::SQL::SQLEmbeddedReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	return this->r->GetStr(colIndex, buff);
}

Bool DB::SQL::SQLEmbeddedReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	return this->r->GetStr(colIndex, sb);
}

Optional<Text::String> DB::SQL::SQLEmbeddedReader::GetNewStr(UIntOS colIndex)
{
	return this->r->GetNewStr(colIndex);
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLEmbeddedReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	return this->r->GetStr(colIndex, buff, buffSize);
}

Data::Timestamp DB::SQL::SQLEmbeddedReader::GetTimestamp(UIntOS colIndex)
{
	return this->r->GetTimestamp(colIndex);
}

Double DB::SQL::SQLEmbeddedReader::GetDblOrNAN(UIntOS colIndex)
{
	return this->r->GetDblOrNAN(colIndex);
}

Bool DB::SQL::SQLEmbeddedReader::GetBool(UIntOS colIndex)
{
	return this->r->GetBool(colIndex);
}

UIntOS DB::SQL::SQLEmbeddedReader::GetBinarySize(UIntOS colIndex)
{
	return this->r->GetBinarySize(colIndex);
}

UIntOS DB::SQL::SQLEmbeddedReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	return this->r->GetBinary(colIndex, buff);
}

Optional<Math::Geometry::Vector2D> DB::SQL::SQLEmbeddedReader::GetVector(UIntOS colIndex)
{
	return this->r->GetVector(colIndex);
}

Bool DB::SQL::SQLEmbeddedReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	return this->r->GetUUID(colIndex, uuid);
}

Bool DB::SQL::SQLEmbeddedReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	return this->r->GetVariItem(colIndex, item);
}

Bool DB::SQL::SQLEmbeddedReader::IsNull(UIntOS colIndex)
{
	return this->r->IsNull(colIndex);
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLEmbeddedReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	return this->r->GetName(colIndex, buff);
}

DB::DBUtil::ColType DB::SQL::SQLEmbeddedReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	return this->r->GetColType(colIndex, colSize);
}

Bool DB::SQL::SQLEmbeddedReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	return this->r->GetColDef(colIndex, colDef);
}
