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
	UnsafeArrayOpt<Optional<Text::String>> row;
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
		if (this->row.NotNull())
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
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return s->ToInt32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return s->ToInt64();
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return Text::StrUTF8_WChar(buff, s->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return false;
		if (colIndex >= this->data->colList.GetCount())
			return false;
		if (!row[colIndex].SetTo(s))
			return false;
		sb->Append(s);
		return true;
	}

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return s->Clone();
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return Text::StrConcatS(buff, s->v, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return Data::Timestamp(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	}

	virtual Double GetDblOrNAN(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return NAN;
		if (colIndex >= this->data->colList.GetCount())
			return NAN;
		if (!row[colIndex].SetTo(s))
			return NAN;
		return s->ToDoubleOrNAN();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return false;
		if (colIndex >= this->data->colList.GetCount())
			return false;
		if (!row[colIndex].SetTo(s))
			return false;
		if (s->EqualsICase(UTF8STRC("TRUE")))
			return true;
		else if (s->EqualsICase(UTF8STRC("FALSE")))
			return false;
		return s->ToInt32() != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		return s->leng;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		UOSInt len = s->leng;
		MemCopyNO(buff.Ptr(), s->v.Ptr(), len);
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
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return true;
		if (colIndex >= this->data->colList.GetCount())
			return true;
		if (!row[colIndex].SetTo(s))
			return true;
		return false;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
	{
		NN<Text::String> name;
		if (this->data->colList.GetItem(colIndex).SetTo(name))
		{
			return name->ConcatTo(buff);
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
		NN<Text::String> colName;
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
	NN<DBData> data;
	UnsafeArray<Optional<Text::String>> vals;
	k = this->dbMap.GetCount();
	while (k-- > 0)
	{
		data = this->dbMap.GetItemNoCheck(k);
		i = data->valList.GetCount();
		while (i-- > 0)
		{
			vals = data->valList.GetItemNoCheck(i);
			j = data->colList.GetCount();
			while (j-- > 0)
			{
				OPTSTR_DEL(vals[j]);
			}
			MemFreeArr(vals);
		}
		data->colList.FreeAll();
		data->name->Release();
		data.Delete();
	}
}

UOSInt DB::TextDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<Text::String> key;
	UOSInt i = 0;
	UOSInt j = this->dbMap.GetCount();
	while (i < j)
	{
		if (this->dbMap.GetKey(i).SetTo(key))
		{
			names->Add(key->Clone());
		}
		i++;
	}
	return j;
}

Optional<DB::DBReader> DB::TextDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DBData> data;
	if (!this->dbMap.GetC(tableName).SetTo(data))
	{
		return 0;
	}
	NN<DB::DBReader> reader;
	NEW_CLASSNN(reader, TextDBReader(data));
	return reader;
}

Optional<DB::TableDef> DB::TextDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DBData> data;
	if (!this->dbMap.GetC(tableName).SetTo(data))
	{
		return 0;
	}
	DB::TableDef *tab;
	NN<DB::ColDef> colDef;
	NEW_CLASS(tab, DB::TableDef(schemaName, data->name->ToCString()));
	Data::ArrayIterator<NN<Text::String>> it = data->colList.Iterator();
	while (it.HasNext())
	{
		NEW_CLASSNN(colDef, DB::ColDef(it.Next()));
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

Bool DB::TextDB::AddTable(Text::CStringNN tableName, NN<Data::ArrayListStringNN> colList)
{
	NN<DBData> data;
	if (this->dbMap.GetC(tableName).SetTo(data))
		return false;
	UOSInt i;
	UOSInt j;
	NEW_CLASSNN(data, DBData());
	data->name = Text::String::New(tableName);
	i = 0;
	j = colList->GetCount();
	while (i < j)
	{
		data->colList.Add(colList->GetItemNoCheck(i)->Clone());
		i++;
	}
	this->dbMap.PutC(tableName, data);
	this->currDB = data;
	return true;
}

Bool DB::TextDB::AddTable(Text::CStringNN tableName, UnsafeArray<Text::CStringNN> colArr, UOSInt colCount)
{
	NN<DBData> data;
	if (this->dbMap.GetC(tableName).SetTo(data))
		return false;
	UOSInt i;
	UOSInt j;
	NEW_CLASSNN(data, DBData());
	data->name = Text::String::New(tableName);
	i = 0;
	j = colCount;
	while (i < j)
	{
		data->colList.Add(Text::String::New(colArr[i]));
		i++;
	}
	this->dbMap.PutC(tableName, data);
	this->currDB = data;
	return true;
}

Bool DB::TextDB::AddTableData(NN<Data::ArrayList<Optional<Text::String>>> valList)
{
	NN<DBData> currDB;
	if (!this->currDB.SetTo(currDB))
		return false;
	if (currDB->colList.GetCount() != valList->GetCount())
	{
		return false;
	}
	UOSInt i = 0;
	UOSInt j = valList->GetCount();
	UnsafeArray<Optional<Text::String>> vals = MemAllocArr(Optional<Text::String>, j);
	while (i < j)
	{
		vals[i] = Text::String::CopyOrNull(valList->GetItem(i));
		i++;
	}
	currDB->valList.Add(vals);
	return true;
}

Bool DB::TextDB::AddTableData(UnsafeArray<Text::CString> valArr, UOSInt colCount)
{
	NN<DBData> currDB;
	if (!this->currDB.SetTo(currDB))
		return false;
	if (currDB->colList.GetCount() != colCount)
	{
		return false;
	}
	UOSInt i = 0;
	UnsafeArray<Optional<Text::String>> vals = MemAllocArr(Optional<Text::String>, colCount);
	while (i < colCount)
	{
		vals[i] = Text::String::NewOrNull(valArr[i]);
		i++;
	}
	currDB->valList.Add(vals);
	return true;
}
