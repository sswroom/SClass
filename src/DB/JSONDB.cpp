#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/JSONDB.h"
#include "DB/TableDef.h"
#include "Text/MyStringW.h"

class JSONDBReader : public DB::DBReader
{
private:
	NN<Text::JSONArray> data;
	NN<DB::TableDef> tab;
	Optional<Text::JSONObject> obj;
	UIntOS nextIndex;
	UIntOS endOfst;
public:
	JSONDBReader(NN<DB::TableDef> tab, NN<Text::JSONArray> data, UIntOS ofst, UIntOS endOfst)
	{
		this->data = data;
		this->tab = tab;
		this->obj = nullptr;
		this->nextIndex = ofst;
		this->endOfst = endOfst;
	}

	virtual ~JSONDBReader()
	{
		this->tab.Delete();
	}

	virtual Bool ReadNext()
	{
		this->obj = nullptr;
		if (this->nextIndex >= this->endOfst)
			return false;
		NN<Text::JSONBase> obj;
		if (!this->data->GetArrayValue(this->nextIndex++).SetTo(obj))
			return false;
		if (obj->GetType() != Text::JSONType::Object)
			return false;
		this->obj = NN<Text::JSONObject>::ConvertFrom(obj);
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
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return obj->GetValueAsInt32(col->GetColName()->ToCString());
	}

	virtual Int64 GetInt64(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return obj->GetValueAsInt64(col->GetColName()->ToCString());
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return nullptr;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return nullptr;
		NN<Text::String> s;
		if (obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return Text::StrUTF8_WChar(buff, s->v, 0);
		}
		if (obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			buff = Text::StrUTF8_WChar(buff, s->v, 0);
			s->Release();
			return buff;
		}
		return nullptr;
	}

	virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return false;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return false;
		NN<Text::String> s;
		if (obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			sb->Append(s);
			return true;
		}
		if (obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			sb->Append(s);
			s->Release();
			return true;
		}
		return false;
	}

	virtual Optional<Text::String> GetNewStr(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return nullptr;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return nullptr;
		return obj->GetValueNewString(col->GetColName()->ToCString());
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return nullptr;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return nullptr;
		NN<Text::String> s;
		if (obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return s->ConcatToS(buff, buffSize);
		}
		if (obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			buff = s->ConcatToS(buff, buffSize);
			s->Release();
			return buff;
		}
		return nullptr;
	}

	virtual Data::Timestamp GetTimestamp(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		NN<Text::String> s;
		if (obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		}
		return 0;
	}

	virtual Double GetDblOrNAN(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return NAN;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return NAN;
		return obj->GetValueAsDoubleOrNAN(col->GetColName()->ToCString());
	}

	virtual Bool GetBool(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return obj->GetValueAsBool(col->GetColName()->ToCString());
	}

	virtual UIntOS GetBinarySize(UIntOS colIndex)
	{
		return 0;
	}

	virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
	{
		return 0;
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex)
	{
		return nullptr;
	}

	virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
	{
		return false;
	}
	
	virtual Bool IsNull(UIntOS colIndex)
	{
		NN<Text::JSONObject> obj;
		if (!this->obj.SetTo(obj))
			return true;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return true;
		NN<Text::JSONBase> o;
		return !obj->GetObjectValue(col->GetColName()->ToCString()).SetTo(o) || o->GetType() == Text::JSONType::Null;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return nullptr;
		return col->GetColName()->ConcatTo(buff);
	}

	virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return DB::DBUtil::CT_Unknown;
		colSize.Set(col->GetColSize());
		return col->GetColType();
	}

	virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return false;
		colDef->Set(col);
		return true;
	}
};

DB::JSONDB::JSONDB(NN<Text::String> sourceName, Text::CStringNN layerName, NN<Text::JSONArray> data) : DB::ReadingDB(sourceName)
{
	this->layerName = Text::String::New(layerName);
	this->data = data;
	this->data->BeginUse();
}

DB::JSONDB::~JSONDB()
{
	this->layerName->Release();
	this->data->EndUse();
}

UIntOS DB::JSONDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	names->Add(this->layerName->Clone());
	return 1;
}

Optional<DB::DBReader> DB::JSONDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DB::TableDef> tabDef;
	if (tableName.Equals(this->layerName->ToCString()) && this->GetTableDef(schemaName, tableName).SetTo(tabDef))
	{
		NN<JSONDBReader> r;
		UIntOS endOfst;
		if (maxCnt == 0)
			endOfst = this->data->GetArrayLength();
		else
			endOfst = ofst + maxCnt;
		NEW_CLASSNN(r, JSONDBReader(tabDef, this->data, ofst, endOfst));
		return r;
	}
	return nullptr;
}

Optional<DB::TableDef> DB::JSONDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	DB::TableDef *tab;
	if (!tableName.Equals(this->layerName->ToCString()))
		return nullptr;
	NEW_CLASS(tab, DB::TableDef(schemaName, this->layerName->ToCString()));
	NN<Text::JSONBase> json;
	if (this->data->GetArrayValue(0).SetTo(json) && json->GetType() == Text::JSONType::Object)
	{
		Data::ArrayListStringNN names;
		NN<Text::JSONObject> obj = NN<Text::JSONObject>::ConvertFrom(json);
		Text::JSONType type;
		NN<DB::ColDef> col;
		obj->GetObjectNames(names);
		Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
		NN<Text::String> name;
		while (it.HasNext())
		{
			name = it.Next();
			if (obj->GetObjectValue(name->ToCString()).SetTo(json))
			{
				type = json->GetType();
				if (type == Text::JSONType::Array)
				{

				}
				else if (type == Text::JSONType::BOOL)
				{
					NEW_CLASSNN(col, DB::ColDef(name));
					col->SetColType(DB::DBUtil::ColType::CT_Bool);
					tab->AddCol(col);
				}
				else if (type == Text::JSONType::INT32)
				{
					NEW_CLASSNN(col, DB::ColDef(name));
					col->SetColType(DB::DBUtil::ColType::CT_Int32);
					tab->AddCol(col);
				}
				else if (type == Text::JSONType::INT64)
				{
					NEW_CLASSNN(col, DB::ColDef(name));
					col->SetColType(DB::DBUtil::ColType::CT_Int64);
					tab->AddCol(col);
				}
				else if (type == Text::JSONType::Number)
				{
					NEW_CLASSNN(col, DB::ColDef(name));
					col->SetColType(DB::DBUtil::ColType::CT_Double);
					tab->AddCol(col);
				}
				else if (type == Text::JSONType::String)
				{
					NEW_CLASSNN(col, DB::ColDef(name));
					col->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
					col->SetColSize((UIntOS)-1);
					tab->AddCol(col);
				}
			}
		}
	}
	return tab;
}

void DB::JSONDB::CloseReader(NN<DBReader> r)
{
	JSONDBReader *reader = (JSONDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::JSONDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::JSONDB::Reconnect()
{
}