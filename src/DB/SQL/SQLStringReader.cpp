#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/SQL/SQLStringReader.h"
#include "Text/MyStringW.h"

DB::SQL::SQLStringReader::SQLStringReader(NN<Data::ArrayListStringNN> colNames, NN<Data::ArrayListObj<Optional<Text::String>>> values)
{
	this->colNames.AddAll(colNames);
	this->values.AddAll(values);
	this->currRow = INVALID_INDEX;
	UIntOS i = 0;
	UIntOS j = colNames->GetCount();
	UIntOS k;
	UIntOS l;
	UnsafeArray<UIntOS> colSizes = MemAllocArr(UIntOS, j);
	while (i < j)
	{
		colSizes[i] = 0;
		i++;
	}
	NN<Text::String> s;
	k = 0;
	l = this->values.GetCount();
	i = 0;
	while (k < l)
	{
		if (this->values.GetItem(k).SetTo(s))
		{
			if (s->leng > colSizes[i])
			{
				colSizes[i] = s->leng;
			}
		}
		if (++i >= j)
		{
			i = 0;
		}
		k++;
	}
	this->colSizes = colSizes;
}

DB::SQL::SQLStringReader::~SQLStringReader()
{
	MemFreeArr(this->colSizes);
	this->colNames.FreeAll();
	NN<Text::String> s;
	UIntOS i = this->values.GetCount();
	while (i-- > 0)
	{
		if (this->values.GetItem(i).SetTo(s))
		{
			s->Release();
		}
	}
}

Bool DB::SQL::SQLStringReader::ReadNext()
{
	if ((this->currRow + 1) * this->colNames.GetCount() >= this->values.GetCount())
		return false;
	this->currRow++;
	return true;
}

UIntOS DB::SQL::SQLStringReader::ColCount()
{
	return this->colNames.GetCount();
}

IntOS DB::SQL::SQLStringReader::GetRowChanged()
{
	return 0;
}

Int32 DB::SQL::SQLStringReader::GetInt32(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return 0;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->ToInt32();
	return 0;
}

Int64 DB::SQL::SQLStringReader::GetInt64(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return 0;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->ToInt64();
	return 0;
}

UnsafeArrayOpt<WChar> DB::SQL::SQLStringReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return nullptr;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return Text::StrUTF8_WChar(buff, s->v, nullptr);
	return nullptr;
}

Bool DB::SQL::SQLStringReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return false;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
	{
		sb->Append(s);
		return true;
	}
	return false;
}
Optional<Text::String> DB::SQL::SQLStringReader::GetNewStr(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return nullptr;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->Clone();
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLStringReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return nullptr;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->ConcatToS(buff, buffSize);
	return nullptr;
}

Data::Timestamp DB::SQL::SQLStringReader::GetTimestamp(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return nullptr;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	return nullptr;
}

Double DB::SQL::SQLStringReader::GetDblOrNAN(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return NAN;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->ToDoubleOrNAN();
	return NAN;
}

Bool DB::SQL::SQLStringReader::GetBool(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return false;
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
		return s->ToBool();
	return false;
}

UIntOS DB::SQL::SQLStringReader::GetBinarySize(UIntOS colIndex)
{
	return 0;
}
UIntOS DB::SQL::SQLStringReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}
Optional<Math::Geometry::Vector2D> DB::SQL::SQLStringReader::GetVector(UIntOS colIndex)
{
	return nullptr;
}
Bool DB::SQL::SQLStringReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	return false;
}

Bool DB::SQL::SQLStringReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
	{
		item->SetNull();
		return false;
	}
	NN<Text::String> s;
	if (this->values.GetItem(index).SetTo(s))
	{
		item->SetStrCopy(UnsafeArray<const UTF8Char>(s->v), s->leng);
		return true;
	}
	item->SetNull();
	return false;
}

Bool DB::SQL::SQLStringReader::IsNull(UIntOS colIndex)
{
	UIntOS index = this->currRow * this->colNames.GetCount() + colIndex;
	if (index >= this->values.GetCount())
		return true;
	return this->values.GetItem(index).IsNull();
}

UnsafeArrayOpt<UTF8Char> DB::SQL::SQLStringReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	if (this->colNames.GetCount() <= colIndex)
	{
		return nullptr;
	}
	return this->colNames.GetItemNoCheck(colIndex)->ConcatTo(buff);
}

DB::DBUtil::ColType DB::SQL::SQLStringReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	if (this->colNames.GetCount() <= colIndex)
	{
		colSize.Set(0);
		return DB::DBUtil::ColType::CT_Unknown;
	}
	colSize.Set(this->colSizes[colIndex]);
	return DB::DBUtil::ColType::CT_VarUTF8Char;
}

Bool DB::SQL::SQLStringReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	if (this->colNames.GetCount() <= colIndex)
	{
		return false;
	}
	colDef->SetColName(this->colNames.GetItemNoCheck(colIndex)->ToCString());
	colDef->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
	colDef->SetColSize(this->colSizes[colIndex]);
	colDef->SetNativeType(CSTR("VARCHAR"));
	colDef->SetNotNull(false);
	colDef->SetColDP(0);
	return true;
}
