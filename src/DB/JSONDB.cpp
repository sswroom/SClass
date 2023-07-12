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
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		return this->obj->GetValueAsInt32(col->GetColName()->ToCString());
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		return this->obj->GetValueAsInt64(col->GetColName()->ToCString());
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		Text::String *s = this->obj->GetValueString(col->GetColName()->ToCString());
		if (s)
		{
			return Text::StrUTF8_WChar(buff, s->v, 0);
		}
		s = this->obj->GetValueNewString(col->GetColName()->ToCString());
		if (s)
		{
			buff = Text::StrUTF8_WChar(buff, s->v, 0);
			s->Release();
			return buff;
		}
		return 0;
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
	{
		if (this->obj == 0)
			return false;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return false;
		Text::String *s = this->obj->GetValueString(col->GetColName()->ToCString());
		if (s)
		{
			sb->Append(s);
			return true;
		}
		s = this->obj->GetValueNewString(col->GetColName()->ToCString());
		if (s)
		{
			sb->Append(s);
			s->Release();
			return true;
		}
		return false;
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		return this->obj->GetValueNewString(col->GetColName()->ToCString());
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		Text::String *s = this->obj->GetValueString(col->GetColName()->ToCString());
		if (s)
		{
			return s->ConcatToS(buff, buffSize);
		}
		s = this->obj->GetValueNewString(col->GetColName()->ToCString());
		if (s)
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
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		Text::String *s = this->obj->GetValueString(col->GetColName()->ToCString());
		if (s)
		{
			return Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		}
		return 0;
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		return this->obj->GetValueAsDouble(col->GetColName()->ToCString());
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->obj == 0)
			return 0;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
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

	virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex)
	{
		return 0;
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		return false;
	}
	
	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->obj == 0)
			return true;
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return true;
		Text::JSONBase *o = this->obj->GetObjectValue(col->GetColName()->ToCString());
		return o == 0 || o->GetType() == Text::JSONType::Null;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return 0;
		return col->GetColName()->ConcatTo(buff);
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return DB::DBUtil::CT_Unknown;
		return col->GetColType();
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		DB::ColDef *col = this->tab->GetCol(colIndex);
		if (col == 0)
			return false;
		colDef->Set(col);
		return true;
	}
};

DB::JSONDB::JSONDB(NotNullPtr<Text::String> sourceName, Text::CString layerName, Text::JSONArray *data) : DB::ReadingDB(sourceName)
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

UOSInt DB::JSONDB::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	names->Add(this->layerName->Clone());
	return 1;
}

DB::DBReader *DB::JSONDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	if (tableName.Equals(this->layerName->ToCString()))
	{
		JSONDBReader *r;
		UOSInt endOfst;
		if (maxCnt == 0)
			endOfst = this->data->GetArrayLength();
		else
			endOfst = ofst + maxCnt;
		NEW_CLASS(r, JSONDBReader(this->GetTableDef(schemaName, tableName), this->data, ofst, endOfst));
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
		Data::ArrayList<Text::String*> names;
		Text::JSONObject *obj = (Text::JSONObject*)json;
		Text::JSONType type;
		DB::ColDef *col;
		obj->GetObjectNames(&names);
		UOSInt i = 0;
		UOSInt j = names.GetCount();
		while (i < j)
		{
			json = obj->GetObjectValue(names.GetItem(i)->ToCString());
			type = json->GetType();
			if (type == Text::JSONType::Array)
			{

			}
			else if (type == Text::JSONType::BOOL)
			{
				NEW_CLASS(col, DB::ColDef(Text::String::OrEmpty(names.GetItem(i))));
				col->SetColType(DB::DBUtil::ColType::CT_Bool);
				tab->AddCol(col);
			}
			else if (type == Text::JSONType::INT32)
			{
				NEW_CLASS(col, DB::ColDef(Text::String::OrEmpty(names.GetItem(i))));
				col->SetColType(DB::DBUtil::ColType::CT_Int32);
				tab->AddCol(col);
			}
			else if (type == Text::JSONType::INT64)
			{
				NEW_CLASS(col, DB::ColDef(Text::String::OrEmpty(names.GetItem(i))));
				col->SetColType(DB::DBUtil::ColType::CT_Int64);
				tab->AddCol(col);
			}
			else if (type == Text::JSONType::Number)
			{
				NEW_CLASS(col, DB::ColDef(Text::String::OrEmpty(names.GetItem(i))));
				col->SetColType(DB::DBUtil::ColType::CT_Double);
				tab->AddCol(col);
			}
			else if (type == Text::JSONType::String)
			{
				NEW_CLASS(col, DB::ColDef(Text::String::OrEmpty(names.GetItem(i))));
				col->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
				col->SetColSize((UOSInt)-1);
				tab->AddCol(col);
			}
			i++;
		}
	}
	return tab;
}

void DB::JSONDB::CloseReader(DBReader *r)
{
	if (r)
	{
		JSONDBReader *reader = (JSONDBReader*)r;
		DEL_CLASS(reader);
	}
}

void DB::JSONDB::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
{
}

void DB::JSONDB::Reconnect()
{
}