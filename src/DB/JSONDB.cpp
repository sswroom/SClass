#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/JSONDB.h"
#include "DB/TableDef.h"
#include "Text/MyStringW.h"

class JSONDBReader : public DB::DBReader
{
private:
	Text::JSONArray *data;
	DB::TableDef *tab;
	Text::JSONObject *obj;
	UOSInt nextIndex;
	UOSInt endOfst;
public:
	JSONDBReader(DB::TableDef *tab, Text::JSONArray *data, UOSInt ofst, UOSInt endOfst)
	{
		this->data = data;
		this->tab = tab;
		this->obj = 0;
		this->nextIndex = ofst;
		this->endOfst = endOfst;
	}

	virtual ~JSONDBReader()
	{
		DEL_CLASS(this->tab);
	}

	virtual Bool ReadNext()
	{
		this->obj = 0;
		if (this->nextIndex >= this->endOfst)
			return false;
		Text::JSONBase *obj = this->data->GetArrayValue(this->nextIndex++);
		if (obj == 0)
			return false;
		if (obj->GetType() != Text::JSONType::Object)
			return false;
		this->obj = (Text::JSONObject*)obj;
		return true;
	}

	virtual UOSInt ColCount()
	{
		return this->tab->GetColCnt();
	}

	virtual OSInt GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return this->obj->GetValueAsInt32(col->GetColName()->ToCString());
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return this->obj->GetValueAsInt64(col->GetColName()->ToCString());
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		NN<Text::String> s;
		if (this->obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return Text::StrUTF8_WChar(buff, s->v, 0);
		}
		if (this->obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			buff = Text::StrUTF8_WChar(buff, s->v, 0);
			s->Release();
			return buff;
		}
		return 0;
	}

	virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		if (this->obj == 0)
			return false;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return false;
		NN<Text::String> s;
		if (this->obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			sb->Append(s);
			return true;
		}
		if (this->obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			sb->Append(s);
			s->Release();
			return true;
		}
		return false;
	}

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return this->obj->GetValueNewString(col->GetColName()->ToCString());
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		NN<Text::String> s;
		if (this->obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return s->ConcatToS(buff, buffSize);
		}
		if (this->obj->GetValueNewString(col->GetColName()->ToCString()).SetTo(s))
		{
			buff = s->ConcatToS(buff, buffSize);
			s->Release();
			return buff;
		}
		return 0;
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		NN<Text::String> s;
		if (this->obj->GetValueString(col->GetColName()->ToCString()).SetTo(s))
		{
			return Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		}
		return 0;
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return this->obj->GetValueAsDouble(col->GetColName()->ToCString());
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return this->obj->GetValueAsBool(col->GetColName()->ToCString());
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		return 0;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		return 0;
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
	{
		return 0;
	}

	virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
	{
		return false;
	}
	
	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->obj == 0)
			return true;
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return true;
		Text::JSONBase *o = this->obj->GetObjectValue(col->GetColName()->ToCString());
		return o == 0 || o->GetType() == Text::JSONType::Null;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return 0;
		return col->GetColName()->ConcatTo(buff);
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return DB::DBUtil::CT_Unknown;
		colSize.Set(col->GetColSize());
		return col->GetColType();
	}

	virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
	{
		NN<DB::ColDef> col;
		if (!this->tab->GetCol(colIndex).SetTo(col))
			return false;
		colDef->Set(col);
		return true;
	}
};

DB::JSONDB::JSONDB(NN<Text::String> sourceName, Text::CString layerName, Text::JSONArray *data) : DB::ReadingDB(sourceName)
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

UOSInt DB::JSONDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	names->Add(this->layerName->Clone());
	return 1;
}

Optional<DB::DBReader> DB::JSONDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	if (tableName.Equals(this->layerName->ToCString()))
	{
		NN<JSONDBReader> r;
		UOSInt endOfst;
		if (maxCnt == 0)
			endOfst = this->data->GetArrayLength();
		else
			endOfst = ofst + maxCnt;
		NEW_CLASSNN(r, JSONDBReader(this->GetTableDef(schemaName, tableName), this->data, ofst, endOfst));
		return r;
	}
	return 0;
}

DB::TableDef *DB::JSONDB::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	DB::TableDef *tab;
	if (!tableName.Equals(this->layerName->ToCString()))
		return 0;
	NEW_CLASS(tab, DB::TableDef(schemaName, this->layerName->ToCString()));
	Text::JSONBase *json = this->data->GetArrayValue(0);
	if (json != 0 && json->GetType() == Text::JSONType::Object)
	{
		Data::ArrayListStringNN names;
		Text::JSONObject *obj = (Text::JSONObject*)json;
		Text::JSONType type;
		NN<DB::ColDef> col;
		obj->GetObjectNames(names);
		Data::ArrayIterator<NN<Text::String>> it = names.Iterator();
		NN<Text::String> name;
		while (it.HasNext())
		{
			name = it.Next();
			json = obj->GetObjectValue(name->ToCString());
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
				col->SetColSize((UOSInt)-1);
				tab->AddCol(col);
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