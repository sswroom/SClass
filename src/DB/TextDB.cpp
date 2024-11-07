#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "DB/TextDB.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

class TextDBReader : public DB::DBReader
{
private:
	NN<DB::TextDB::DBData> data;
	UOSInt index;
	Text::String **row;
public:
	TextDBReader(NN<DB::TextDB::DBData> data)
	{
		this->row = 0;
		this->index = 0;
		this->data = data;
	}

	virtual ~TextDBReader()
	{

	}

	virtual Bool ReadNext()
	{
		this->row = data->valList.GetItem(this->index);
		if (this->row)
		{
			this->index++;
			return true;
		}
		return false;
	}

	virtual UOSInt ColCount()
	{
		return this->data->colList.GetCount();
	}

	virtual OSInt GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return this->row[colIndex]->ToInt32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return this->row[colIndex]->ToInt64();
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrUTF8_WChar(buff, this->row[colIndex]->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		if (this->row == 0)
			return false;
		if (colIndex >= this->data->colList.GetCount())
			return false;
		if (this->row[colIndex] == 0)
			return false;
		sb->Append(this->row[colIndex]);
		return true;
	}

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return this->row[colIndex]->Clone();
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrConcatS(buff, this->row[colIndex]->v, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		if (this->row == 0)
			return Data::Timestamp(0);
		if (colIndex >= this->data->colList.GetCount())
			return Data::Timestamp(0);
		if (this->row[colIndex] == 0)
			return Data::Timestamp(0);

		return Data::Timestamp(this->row[colIndex]->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	}

	virtual Double GetDblOrNAN(UOSInt colIndex)
	{
		if (this->row == 0)
			return NAN;
		if (colIndex >= this->data->colList.GetCount())
			return NAN;
		if (this->row[colIndex] == 0)
			return NAN;
		return this->row[colIndex]->ToDoubleOrNAN();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->row == 0)
			return false;
		if (colIndex >= this->data->colList.GetCount())
			return false;
		if (this->row[colIndex] == 0)
			return false;
		if (this->row[colIndex]->EqualsICase(UTF8STRC("TRUE")))
			return true;
		else if (this->row[colIndex]->EqualsICase(UTF8STRC("FALSE")))
			return false;
		return this->row[colIndex]->ToInt32() != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return this->row[colIndex]->leng;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		UOSInt len = this->row[colIndex]->leng;
		MemCopyNO(buff.Ptr(), this->row[colIndex]->v.Ptr(), len);
		return len;
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
		if (this->row == 0)
			return true;
		if (colIndex >= this->data->colList.GetCount())
			return true;
		if (this->row[colIndex] == 0)
			return true;
		return false;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
	{
		UnsafeArray<const UTF8Char> name;
		if (this->data->colList.GetItem(colIndex).SetTo(name))
		{
			return Text::StrConcat(buff, name.Ptr());
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
	{
		if (colIndex >= this->data->colList.GetCount())
			return DB::DBUtil::CT_Unknown;
		colSize.Set(256);
		return DB::DBUtil::CT_VarUTF8Char;
	}

	virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
	{
		if (colIndex >= this->data->colList.GetCount())
		{
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			return false;
		}
		UnsafeArray<const UTF8Char> colName;
		if (!this->data->colList.GetItem(colIndex).SetTo(colName))
		{
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			return false;
		}
		colDef->SetColName(colName);
		colDef->SetColSize(256);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetAttr(CSTR_NULL);
		colDef->SetColDP(0);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		return true;
	}
};

DB::TextDB::TextDB(Text::CStringNN sourceName) : DB::ReadingDB(sourceName)
{
	this->currDB = 0;
}

DB::TextDB::~TextDB()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<const Data::ArrayList<DBData*>> dbList = this->dbMap.GetValues();
	DBData *data;
	Text::String **vals;
	UnsafeArray<const UTF8Char> sptr;
	k = dbList->GetCount();
	while (k-- > 0)
	{
		data = dbList->GetItem(k);
		i = data->valList.GetCount();
		while (i-- > 0)
		{
			vals = data->valList.GetItem(i);
			j = data->colList.GetCount();
			while (j-- > 0)
			{
				SDEL_STRING(vals[j]);
			}
			MemFree(vals);
		}
		i = data->colList.GetCount();
		while (i-- > 0)
		{
			if (data->colList.GetItem(i).SetTo(sptr))
				Text::StrDelNew(sptr.Ptr());
		}
		data->name->Release();
		DEL_CLASS(data);
	}
}

UOSInt DB::TextDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<Data::ArrayList<Text::String*>> keys = this->dbMap.GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		names->Add(keys->GetItem(i)->Clone());
		i++;
	}
	return j;
}

Optional<DB::DBReader> DB::TextDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DBData> data;
	if (!data.Set(this->dbMap.Get(tableName)))
	{
		return 0;
	}
	NN<DB::DBReader> reader;
	NEW_CLASSNN(reader, TextDBReader(data));
	return reader;
}

Optional<DB::TableDef> DB::TextDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	DBData *data;
	data = this->dbMap.Get(tableName);
	if (data == 0)
	{
		return 0;
	}
	DB::TableDef *tab;
	NN<DB::ColDef> colDef;
	NEW_CLASS(tab, DB::TableDef(schemaName, data->name->ToCString()));
	Data::ArrayIterator<UnsafeArray<const UTF8Char>> it = data->colList.Iterator();
	while (it.HasNext())
	{
		NEW_CLASSNN(colDef, DB::ColDef(Text::String::NewEmpty()));
		colDef->SetColName(it.Next());
		colDef->SetColSize(256);
		colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
		colDef->SetAttr(CSTR_NULL);
		colDef->SetColDP(0);
		colDef->SetDefVal(CSTR_NULL);
		colDef->SetAutoIncNone();
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		tab->AddCol(colDef);
	}
	return tab;
}

void DB::TextDB::CloseReader(NN<DBReader> r)
{
	TextDBReader *reader = (TextDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::TextDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{

}

void DB::TextDB::Reconnect()
{

}

Bool DB::TextDB::AddTable(Text::CStringNN tableName, Data::ArrayList<const UTF8Char*> *colList)
{
	DBData *data = this->dbMap.Get(tableName);
	if (data)
		return false;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(data, DBData());
	data->name = Text::String::New(tableName);
	i = 0;
	j = colList->GetCount();
	while (i < j)
	{
		data->colList.Add(Text::StrCopyNew(colList->GetItem(i)));
		i++;
	}
	this->dbMap.Put(tableName, data);
	this->currDB = data;
	return true;
}

Bool DB::TextDB::AddTableData(Data::ArrayList<const UTF8Char*> *valList)
{
	if (this->currDB == 0)
		return false;
	if (this->currDB->colList.GetCount() != valList->GetCount())
	{
		return false;
	}
	UOSInt i = 0;
	UOSInt j = valList->GetCount();
	const UTF8Char *csptr;
	Text::String **vals = MemAlloc(Text::String*, j);
	while (i < j)
	{
		csptr = valList->GetItem(i);
		vals[i] = Text::String::NewOrNullSlow(csptr).OrNull();
		i++;
	}
	this->currDB->valList.Add(vals);
	return true;
}
