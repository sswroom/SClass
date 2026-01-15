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
	UIntOS index;
	UnsafeArrayOpt<Optional<Text::String>> row;
public:
	TextDBReader(NN<DB::TextDB::DBData> data)
	{
		this->row = nullptr;
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

	virtual UIntOS ColCount()
	{
		return this->data->colList.GetCount();
	}

	virtual IntOS GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UIntOS colIndex)
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

	virtual Int64 GetInt64(UIntOS colIndex)
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

	virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return nullptr;
		if (colIndex >= this->data->colList.GetCount())
			return nullptr;
		if (!row[colIndex].SetTo(s))
			return nullptr;
		return Text::StrUTF8_WChar(buff, s->v, 0);
	}

	virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
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

	virtual Optional<Text::String> GetNewStr(UIntOS colIndex)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return nullptr;
		if (colIndex >= this->data->colList.GetCount())
			return nullptr;
		if (!row[colIndex].SetTo(s))
			return nullptr;
		return s->Clone();
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return nullptr;
		if (colIndex >= this->data->colList.GetCount())
			return nullptr;
		if (!row[colIndex].SetTo(s))
			return nullptr;
		return Text::StrConcatS(buff, s->v, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UIntOS colIndex)
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

	virtual Double GetDblOrNAN(UIntOS colIndex)
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

	virtual Bool GetBool(UIntOS colIndex)
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

	virtual UIntOS GetBinarySize(UIntOS colIndex)
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

	virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
	{
		UnsafeArray<Optional<Text::String>> row;
		NN<Text::String> s;
		if (!this->row.SetTo(row))
			return 0;
		if (colIndex >= this->data->colList.GetCount())
			return 0;
		if (!row[colIndex].SetTo(s))
			return 0;
		UIntOS len = s->leng;
		MemCopyNO(buff.Ptr(), s->v.Ptr(), len);
		return len;
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

	virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
	{
		NN<Text::String> name;
		if (this->data->colList.GetItem(colIndex).SetTo(name))
		{
			return name->ConcatTo(buff);
		}
		return nullptr;
	}

	virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
	{
		if (colIndex >= this->data->colList.GetCount())
			return DB::DBUtil::CT_Unknown;
		colSize.Set(256);
		return DB::DBUtil::CT_VarUTF8Char;
	}

	virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
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
		colDef->SetAttr(Text::CString(nullptr));
		colDef->SetColDP(0);
		colDef->SetDefVal(Text::CString(nullptr));
		colDef->SetAutoIncNone();
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		return true;
	}
};

DB::TextDB::TextDB(Text::CStringNN sourceName) : DB::ReadingDB(sourceName)
{
	this->currDB = nullptr;
}

DB::TextDB::~TextDB()
{
	UIntOS i;
	UIntOS j;
	UIntOS k;
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

UIntOS DB::TextDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<Text::String> key;
	UIntOS i = 0;
	UIntOS j = this->dbMap.GetCount();
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

Optional<DB::DBReader> DB::TextDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DBData> data;
	if (!this->dbMap.GetC(tableName).SetTo(data))
	{
		return nullptr;
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
		return nullptr;
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
		colDef->SetAttr(Text::CString(nullptr));
		colDef->SetColDP(0);
		colDef->SetDefVal(Text::CString(nullptr));
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
	UIntOS i;
	UIntOS j;
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

Bool DB::TextDB::AddTable(Text::CStringNN tableName, UnsafeArray<Text::CStringNN> colArr, UIntOS colCount)
{
	NN<DBData> data;
	if (this->dbMap.GetC(tableName).SetTo(data))
		return false;
	UIntOS i;
	UIntOS j;
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

Bool DB::TextDB::AddTableData(NN<Data::ArrayListObj<Optional<Text::String>>> valList)
{
	NN<DBData> currDB;
	if (!this->currDB.SetTo(currDB))
		return false;
	if (currDB->colList.GetCount() != valList->GetCount())
	{
		return false;
	}
	UIntOS i = 0;
	UIntOS j = valList->GetCount();
	UnsafeArray<Optional<Text::String>> vals = MemAllocArr(Optional<Text::String>, j);
	while (i < j)
	{
		vals[i] = Text::String::CopyOrNull(valList->GetItem(i));
		i++;
	}
	currDB->valList.Add(vals);
	return true;
}

Bool DB::TextDB::AddTableData(UnsafeArray<Text::CString> valArr, UIntOS colCount)
{
	NN<DBData> currDB;
	if (!this->currDB.SetTo(currDB))
		return false;
	if (currDB->colList.GetCount() != colCount)
	{
		return false;
	}
	UIntOS i = 0;
	UnsafeArray<Optional<Text::String>> vals = MemAllocArr(Optional<Text::String>, colCount);
	while (i < colCount)
	{
		vals[i] = Text::String::NewOrNull(valArr[i]);
		i++;
	}
	currDB->valList.Add(vals);
	return true;
}
