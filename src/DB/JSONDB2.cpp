#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/JSONDB2.h"
#include "DB/TableDef.h"
#include "Text/MyStringW.h"

class JSONDB2Reader : public DB::DBReader
{
private:
	NN<Text::JSONArray> rows;
	NN<DB::TableDef> tab;
	UIntOS currRow;
	Int8 tzQhr;
public:
	JSONDB2Reader(NN<DB::TableDef> tab, NN<Text::JSONArray> rows, Int8 tzQhr)
	{
		this->rows = rows;
		this->tab = tab;
		this->tzQhr = tzQhr;
		this->currRow = INVALID_INDEX;
	}

	virtual ~JSONDB2Reader()
	{
	}

	virtual Bool ReadNext()
	{
		if (this->currRow + 1 >= this->rows->GetArrayLength())
		{
			this->currRow = this->rows->GetArrayLength();
			return false;
		}
		this->currRow++;
		return true;
	}

	virtual UIntOS ColCount()
	{
		return this->tab->GetColCnt();
	}

	virtual IntOS GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return 0;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return 0;
		return val->GetAsInt32();
	}

	virtual Int64 GetInt64(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return 0;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return 0;
		return val->GetAsInt64();
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return nullptr;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return nullptr;
		switch (val->GetType())
		{
		case Text::JSONType::BOOL:
			return NN<Text::JSONBool>::ConvertFrom(val)->GetValue()?Text::StrConcat(buff, L"true"):Text::StrConcat(buff, L"false");
		case Text::JSONType::INT32:
			return Text::StrInt32(buff, NN<Text::JSONInt32>::ConvertFrom(val)->GetValue());
		case Text::JSONType::INT64:
			return Text::StrInt64(buff, NN<Text::JSONInt64>::ConvertFrom(val)->GetValue());
		case Text::JSONType::Number:
			return Text::StrDoubleW(buff, NN<Text::JSONNumber>::ConvertFrom(val)->GetValue());
		case Text::JSONType::String:
			return Text::StrUTF8_WChar(buff, NN<Text::JSONString>::ConvertFrom(val)->GetValue()->v, nullptr);
		case Text::JSONType::Array:
		case Text::JSONType::Object:
		case Text::JSONType::Null:
		default:
			return nullptr;
		}
	}

	virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return false;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return false;
		val->ToString(sb);
		return true;
	}

	virtual Optional<Text::String> GetNewStr(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return nullptr;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return nullptr;
		else if (val->GetType() == Text::JSONType::Null)
			return nullptr;
		Text::StringBuilderUTF8 sb;
		val->ToString(sb);
		return Text::String::New(sb.ToCString());
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return nullptr;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return nullptr;
		switch (val->GetType())
		{
		case Text::JSONType::BOOL:
			return NN<Text::JSONBool>::ConvertFrom(val)->GetValue()?Text::StrConcatC(buff, UTF8STRC("true")):Text::StrConcatC(buff, UTF8STRC("false"));
		case Text::JSONType::INT32:
			return Text::StrInt32(buff, NN<Text::JSONInt32>::ConvertFrom(val)->GetValue());
		case Text::JSONType::INT64:
			return Text::StrInt64(buff, NN<Text::JSONInt64>::ConvertFrom(val)->GetValue());
		case Text::JSONType::Number:
			return Text::StrDouble(buff, NN<Text::JSONNumber>::ConvertFrom(val)->GetValue());
		case Text::JSONType::String:
			return NN<Text::JSONString>::ConvertFrom(val)->GetValue()->ConcatToS(buff, buffSize);
		case Text::JSONType::Array:
		case Text::JSONType::Object:
		case Text::JSONType::Null:
		default:
			return nullptr;
		}
	}

	virtual Data::Timestamp GetTimestamp(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return nullptr;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return nullptr;
		switch (val->GetType())
		{
		case Text::JSONType::BOOL:
		case Text::JSONType::INT32:
		case Text::JSONType::INT64:
		case Text::JSONType::Number:
		case Text::JSONType::Array:
		case Text::JSONType::Object:
		case Text::JSONType::Null:
		default:
			return nullptr;
		case Text::JSONType::String:
			return Data::Timestamp::FromStr(NN<Text::JSONString>::ConvertFrom(val)->GetValue()->ToCString(), this->tzQhr);
		}
	}

	virtual Double GetDblOrNAN(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return NAN;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return NAN;
		return val->GetAsDoubleOrNAN();
	}

	virtual Bool GetBool(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return false;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return false;
		return val->GetAsBool();
	}

	virtual UIntOS GetBinarySize(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return 0;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return 0;
		if (val->GetType() != Text::JSONType::String)
			return 0;
		if (colDef->GetColType() == DB::DBUtil::ColType::CT_Binary)
			return NN<Text::JSONString>::ConvertFrom(val)->GetValue()->leng >> 1;
		else
			return NN<Text::JSONString>::ConvertFrom(val)->GetValue()->leng;
	}

	virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return 0;
		if (!row->GetArrayValue(colIndex).SetTo(val))
			return 0;
		if (val->GetType() != Text::JSONType::String)
			return 0;
		if (colDef->GetColType() == DB::DBUtil::ColType::CT_Binary)
			return NN<Text::JSONString>::ConvertFrom(val)->GetValue()->Hex2Bytes(buff);
		else
		{
			NN<Text::String> strVal = NN<Text::JSONString>::ConvertFrom(val)->GetValue();
			MemCopyNO(buff.Ptr(), strVal->v.Ptr(), strVal->leng);
			return strVal->leng;
		}
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex)
	{
		return nullptr;
	}

	virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
	{
		return false;
	}

	virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
	{
		NN<Text::JSONArray> row;
		NN<DB::ColDef> colDef;
		NN<Text::JSONBase> val;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || !this->tab->GetCol(colIndex).SetTo(colDef))
			return false;
		if (!row->GetArrayValue(colIndex).SetTo(val))
		{
			item->SetNull();
			return true;
		}
		if (val->GetType() == Text::JSONType::Null)
		{
			item->SetNull();
			return true;
		}
		DB::DBUtil::ColType colType = colDef->GetColType();
		switch (colType)
		{
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt16:
		{
			Int32 v;
			if (val->GetAsInt32(v))
			{
				item->SetI32(v);
				return true;
			}
			else
				return false;
		}
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_UInt64:
		case DB::DBUtil::CT_Int64:
		{
			Int64 v;
			if (val->GetAsInt64(v))
			{
				item->SetI64(v);
				return true;
			}
			else
				return false;
		}
		case DB::DBUtil::CT_Float:
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Decimal:
		{
			Double v;
			if (val->GetAsDouble(v))
			{
				item->SetF64(v);
				return true;
			}
			else
				return false;
		}
		case DB::DBUtil::CT_Date:
			if (val->GetType() == Text::JSONType::String)
			{
				Data::Timestamp ts = Data::Timestamp::FromStr(NN<Text::JSONString>::ConvertFrom(val)->GetValue()->ToCString(), this->tzQhr);
				if (ts.IsNull())
					return false;
				item->SetDate(ts.ToDate());
				return true;
			}
			else
				return false;
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTimeTZ:
			if (val->GetType() == Text::JSONType::String)
			{
				Data::Timestamp ts = Data::Timestamp::FromStr(NN<Text::JSONString>::ConvertFrom(val)->GetValue()->ToCString(), this->tzQhr);
				if (ts.IsNull())
					return false;
				item->SetDate(ts);
				return true;
			}
			else
				return false;
		case DB::DBUtil::CT_UTF8Char:
		case DB::DBUtil::CT_UTF16Char:
		case DB::DBUtil::CT_UTF32Char:
		case DB::DBUtil::CT_VarUTF8Char:
		case DB::DBUtil::CT_VarUTF16Char:
		case DB::DBUtil::CT_VarUTF32Char:
		{
			NN<Text::String> s;
			if (this->GetNewStr(colIndex).SetTo(s))
			{
				item->SetStr(s);
				return true;
			}
			else
				return false;
		}
		case DB::DBUtil::CT_Bool:
			item->SetBool(val->GetAsBool());
			return true;
		case DB::DBUtil::CT_Binary:
		{
			if (val->GetType() != Text::JSONType::String)
			{
				return false;
			}
			NN<Text::String> s = NN<Text::JSONString>::ConvertFrom(val)->GetValue();
			UnsafeArray<UInt8> buff = MemAllocArr(UInt8, s->leng >> 1);
			UIntOS len = s->Hex2Bytes(buff);
			item->SetByteArr(buff, len);
			MemFreeArr(buff);
			return true;
		}
		case DB::DBUtil::CT_Vector:
			return false;
		case DB::DBUtil::CT_UUID:
			return false;
		case DB::DBUtil::CT_Unknown:
		default:
			return false;
		}
	}

	virtual Bool IsNull(UIntOS colIndex)
	{
		NN<Text::JSONArray> row;
		if (!this->rows->GetArrayArray(this->currRow).SetTo(row) || this->tab->GetCol(colIndex).IsNull())
		{
			return true;
		}
		NN<Text::JSONBase> val;
		return !row->GetArrayValue(colIndex).SetTo(val) || val->GetType() == Text::JSONType::Null;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
	{
		NN<DB::ColDef> colDef;
		if (!this->tab->GetCol(colIndex).SetTo(colDef))
			return nullptr;
		return colDef->GetColName()->ConcatTo(buff);
	}

	virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
	{
		NN<DB::ColDef> colDef;
		if (!this->tab->GetCol(colIndex).SetTo(colDef))
			return DB::DBUtil::CT_Unknown;
		colSize.Set(colDef->GetColSize());
		return colDef->GetColType();
	}

	virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
	{
		NN<DB::ColDef> colDef2;
		if (!this->tab->GetCol(colIndex).SetTo(colDef2))
			return false;
		colDef->Set(colDef2);
		return true;
	}
};

Bool DB::JSONDB2::IsSchema(Text::CString schemaName)
{
	NN<Text::String> sname;
	Text::CStringNN scName;
	if (this->schemaName.SetTo(sname) && schemaName.SetTo(scName) && sname->Equals(scName))
	{
		return true;
	}
	else if (this->schemaName.IsNull() && schemaName.IsNull())
	{
		return true;
	}
	else
	{
		return false;
	}
}

DB::JSONDB2::JSONDB2(Text::CString schemaName, Text::CStringNN tableName, NN<Text::JSONArray> headers, NN<Text::JSONArray> types, NN<Text::JSONArray> rows) : DB::ReadingDB(tableName), tableDef(schemaName, tableName)
{
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->tzQhr = Data::DateTimeUtil::GetLocalTzQhr();
	NN<Text::String> header;
	NN<Text::String> sType;
	NN<DB::ColDef> colDef;
	UIntOS i = 0;
	UIntOS j = headers->GetArrayLength();
	while (i < j)
	{
		this->headers.Add(header = Text::String::OrEmpty(headers->GetArrayString(i))->Clone());
		this->types.Add(sType = Text::String::OrEmpty(types->GetArrayString(i))->Clone());
		NEW_CLASSNN(colDef, DB::ColDef(header));
		colDef->SetNativeType(sType);
		if (sType->Equals(CSTR("I32")))
		{
			colDef->SetColType(DB::DBUtil::CT_Int32);
			colDef->SetColSize(11);
		}
		else if (sType->Equals(CSTR("I64")))
		{
			colDef->SetColType(DB::DBUtil::CT_Int64);
			colDef->SetColSize(20);
		}
		else if (sType->Equals(CSTR("F32")))
		{
			colDef->SetColType(DB::DBUtil::CT_Float);
			colDef->SetColSize(23);
			colDef->SetColDP(6);
		}
		else if (sType->Equals(CSTR("F64")))
		{
			colDef->SetColType(DB::DBUtil::CT_Double);
			colDef->SetColSize(53);
			colDef->SetColDP(15);
		}
		else if (sType->Equals(CSTR("Date")))
		{
			colDef->SetColType(DB::DBUtil::CT_Date);
			colDef->SetColSize(10);
		}
		else if (sType->Equals(CSTR("DT")))
		{
			colDef->SetColType(DB::DBUtil::CT_DateTime);
			colDef->SetColSize(19);
		}
		else if (sType->Equals(CSTR("Str")))
		{
			colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
			colDef->SetColSize(0x7fffffff);
		}
		else if (sType->Equals(CSTR("Bool")))
		{
			colDef->SetColType(DB::DBUtil::CT_Bool);
			colDef->SetColSize(1);
		}
		else if (sType->Equals(CSTR("I8")))
		{
			colDef->SetColType(DB::DBUtil::CT_Byte);
			colDef->SetColSize(4);
		}
		else if (sType->Equals(CSTR("I16")))
		{
			colDef->SetColType(DB::DBUtil::CT_Int16);
			colDef->SetColSize(6);
		}
		else if (sType->Equals(CSTR("U16")))
		{
			colDef->SetColType(DB::DBUtil::CT_UInt16);
			colDef->SetColSize(5);
		}
		else if (sType->Equals(CSTR("U32")))
		{
			colDef->SetColType(DB::DBUtil::CT_UInt32);
			colDef->SetColSize(10);
		}
		else if (sType->Equals(CSTR("U64")))
		{
			colDef->SetColType(DB::DBUtil::CT_UInt64);
			colDef->SetColSize(20);
		}
		else if (sType->Equals(CSTR("Bin")))
		{
			colDef->SetColType(DB::DBUtil::CT_Binary);
			colDef->SetColSize(0x7fffffff);
		}
		else if (sType->Equals(CSTR("Geometry")))
		{
			colDef->SetColType(DB::DBUtil::CT_Vector);
			colDef->SetColSize(0x7fffffff);
		}
		else if (sType->Equals(CSTR("UUID")))
		{
			colDef->SetColType(DB::DBUtil::CT_UUID);
			colDef->SetColSize(36);
		}
		else
		{
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			colDef->SetColSize(0);
		}
		this->tableDef.AddCol(colDef);
		i++;
	}
	this->rows = rows;
	this->rows->BeginUse();
}

DB::JSONDB2::~JSONDB2()
{
	this->rows->EndUse();
	this->headers.FreeAll();
	this->types.FreeAll();
	OPTSTR_DEL(this->schemaName);
}

UIntOS DB::JSONDB2::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (this->IsSchema(schemaName))
	{
		names->Add(this->GetSourceNameObj()->Clone());
		return 1;
	}
	else
	{
		return 0;
	}
}

Optional<DB::DBReader> DB::JSONDB2::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	if (this->IsSchema(schemaName) && this->GetSourceNameObj()->Equals(tableName))
	{
		NN<JSONDB2Reader> r;
		NEW_CLASSNN(r, JSONDB2Reader(this->tableDef, this->rows, this->tzQhr));
		return r;
	}
	else
	{
		return nullptr;
	}

}

Optional<DB::TableDef> DB::JSONDB2::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	if (this->IsSchema(schemaName) && this->GetSourceNameObj()->Equals(tableName))
	{
		return this->tableDef.Clone();
	}
	else
	{
		return nullptr;
	}
}

void DB::JSONDB2::CloseReader(NN<DBReader> r)
{
	NN<JSONDB2Reader> reader = NN<JSONDB2Reader>::ConvertFrom(r);
	reader.Delete();
}

void DB::JSONDB2::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::JSONDB2::Reconnect()
{
}

Int8 DB::JSONDB2::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::JSONDB2::ForceTzQhr(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

Bool DB::JSONDB2::BuildJSON(NN<Text::JSONBuilder> builder, NN<DB::DBReader> r, NN<DB::TableDef> tabDef)
{
	NN<DB::ColDef> colDef;
	UIntOS i = 0;
	UIntOS j = r->ColCount();
	builder->ObjectBeginArray(CSTR("headers"));
	while (i < j)
	{
		if (tabDef->GetCol(i).SetTo(colDef))
		{
			builder->ArrayAddStr(colDef->GetColName());
		}
		i++;
	}
	builder->ArrayEnd();
	builder->ObjectBeginArray(CSTR("types"));
	i = 0;
	while (i < j)
	{
		if (tabDef->GetCol(i).SetTo(colDef))
		{
			builder->ArrayAddStr(ColType2Str(colDef->GetColType()));
		}
		i++;
	}
	builder->ArrayEnd();
	builder->ObjectBeginArray(CSTR("rows"));
	Text::StringBuilderUTF8 sb;
	while (r->ReadNext())
	{
		builder->ArrayBeginArray();
		i = 0;
		while (i < j)
		{
			if (tabDef->GetCol(i).SetTo(colDef))
			{
				if (r->IsNull(i))
				{
					builder->ArrayAddNull();
				}
				else
				{
					switch (colDef->GetColType())
					{
					case DB::DBUtil::CT_Byte:
					case DB::DBUtil::CT_Int16:
					case DB::DBUtil::CT_UInt16:
					case DB::DBUtil::CT_Int32:
						builder->ArrayAddInt32(r->GetInt32(i));
						break;
					case DB::DBUtil::CT_UInt32:
					case DB::DBUtil::CT_Int64:
						builder->ArrayAddInt64(r->GetInt64(i));
						break;
					case DB::DBUtil::CT_UInt64:
						builder->ArrayAddInt64(r->GetInt64(i));
						break;
					case DB::DBUtil::CT_Float:
					case DB::DBUtil::CT_Double:
					case DB::DBUtil::CT_Decimal:
						builder->ArrayAddFloat64(r->GetDblOrNAN(i));
						break;
					case DB::DBUtil::CT_Date:
						builder->ArrayAddDateStr(r->GetDate(i));
						break;
					case DB::DBUtil::CT_DateTime:
					case DB::DBUtil::CT_DateTimeTZ:
						builder->ArrayAddTSStr(r->GetTimestamp(i));
						break;
					case DB::DBUtil::CT_UTF8Char:
					case DB::DBUtil::CT_UTF16Char:
					case DB::DBUtil::CT_UTF32Char:
					case DB::DBUtil::CT_VarUTF8Char:
					case DB::DBUtil::CT_VarUTF16Char:
					case DB::DBUtil::CT_VarUTF32Char:
						{
							NN<Text::String> s = r->GetNewStrNN(i);
							builder->ArrayAddStr(s);
							s->Release();
						}
						break;
					case DB::DBUtil::CT_Bool:
						builder->ArrayAddBool(r->GetBool(i));
						break;
					case DB::DBUtil::CT_Binary:
						{
							UIntOS binSize = r->GetBinarySize(i);
							UnsafeArray<UInt8> binBuff = MemAllocArr(UInt8, binSize);
							binSize = r->GetBinary(i, binBuff);
							sb.ClearStr();
							sb.AppendHexBuff(binBuff, binSize, 0, Text::LineBreakType::None);
							builder->ArrayAddStr(sb.ToCString());
							MemFreeArr(binBuff);
						}
						break;
					case DB::DBUtil::CT_Vector:
						{
							NN<Math::Geometry::Vector2D> vec;
							if (r->GetVector(i).SetTo(vec))
							{
								builder->ArrayAddGeometry(vec);
								vec.Delete();
							}
							else
							{
								builder->ArrayAddNull();
							}
						}
						break;
					case DB::DBUtil::CT_UUID:
						{
							Data::UUID uuid;
							if (r->GetUUID(i, uuid))
							{
								sb.ClearStr();
								uuid.ToString(sb);
								builder->ArrayAddStr(sb.ToCString());
							}
							else
							{
								builder->ArrayAddNull();
							}
						}
						break;
					case DB::DBUtil::CT_Unknown:
					default:
						builder->ArrayAddNull();
						break;
					}
				}
			}
			i++;
		}
		builder->ArrayEnd();
	}
	builder->ArrayEnd();
	return true;
}

Text::CStringNN DB::JSONDB2::ColType2Str(DB::DBUtil::ColType colType)
{
	switch (colType)
	{
	case DB::DBUtil::CT_Int32:
		return CSTR("I32");
	case DB::DBUtil::CT_Int64:
		return CSTR("I64");
	case DB::DBUtil::CT_Float:
		return CSTR("F32");
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Decimal:
		return CSTR("F64");
	case DB::DBUtil::CT_Date:
		return CSTR("Date");
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		return CSTR("DT");
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
		return CSTR("Str");
	case DB::DBUtil::CT_Bool:
		return CSTR("Bool");
	case DB::DBUtil::CT_Byte:
		return CSTR("I8");
	case DB::DBUtil::CT_Int16:
		return CSTR("I16");
	case DB::DBUtil::CT_UInt16:
		return CSTR("U16");
	case DB::DBUtil::CT_UInt32:
		return CSTR("U32");
	case DB::DBUtil::CT_UInt64:
		return CSTR("U64");
	case DB::DBUtil::CT_Binary:
		return CSTR("Bin");
	case DB::DBUtil::CT_Vector:
		return CSTR("Geometry");
	case DB::DBUtil::CT_UUID:
		return CSTR("UUID");
	case DB::DBUtil::CT_Unknown:
	default:
		return CSTR("UNK");
	}
}

DB::DBUtil::ColType DB::JSONDB2::Str2ColType(Text::CStringNN colType)
{
	if (colType.Equals(CSTR("I32")))
	{
		return DB::DBUtil::CT_Int32;
	}
	else if (colType.Equals(CSTR("I64")))
	{
		return DB::DBUtil::CT_Int64;
	}
	else if (colType.Equals(CSTR("F32")))
	{
		return DB::DBUtil::CT_Float;
	}
	else if (colType.Equals(CSTR("F64")))
	{
		return DB::DBUtil::CT_Double;
	}
	else if (colType.Equals(CSTR("Date")))
	{
		return DB::DBUtil::CT_Date;
	}
	else if (colType.Equals(CSTR("DT")))
	{
		return DB::DBUtil::CT_DateTime;
	}
	else if (colType.Equals(CSTR("Str")))
	{
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else if (colType.Equals(CSTR("Bool")))
	{
		return DB::DBUtil::CT_Bool;
	}
	else if (colType.Equals(CSTR("I8")))
	{
		return DB::DBUtil::CT_Byte;
	}
	else if (colType.Equals(CSTR("I16")))
	{
		return DB::DBUtil::CT_Int16;
	}
	else if (colType.Equals(CSTR("U16")))
	{
		return DB::DBUtil::CT_UInt16;
	}
	else if (colType.Equals(CSTR("U32")))
	{
		return DB::DBUtil::CT_UInt32;
	}
	else if (colType.Equals(CSTR("U64")))
	{
		return DB::DBUtil::CT_UInt64;
	}
	else if (colType.Equals(CSTR("Bin")))
	{
		return DB::DBUtil::CT_Binary;
	}
	else if (colType.Equals(CSTR("Geometry")))
	{
		return DB::DBUtil::CT_Vector;
	}
	else if (colType.Equals(CSTR("UUID")))
	{
		return DB::DBUtil::CT_UUID;
	}
	else
	{
		return DB::DBUtil::CT_Unknown;
	}
}
