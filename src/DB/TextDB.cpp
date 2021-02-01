#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/TextDB.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

class TextDBReader : public DB::DBReader
{
private:
	DB::TextDB::DBData *data;
	OSInt index;
	const UTF8Char **row;
public:
	TextDBReader(DB::TextDB::DBData *data)
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
		this->row = data->valList->GetItem(this->index);
		if (this->row)
		{
			this->index++;
			return true;
		}
		return false;
	}

	virtual UOSInt ColCount()
	{
		return this->data->colList->GetCount();
	}

	virtual OSInt GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrToInt32(this->row[colIndex]);
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrToInt64(this->row[colIndex]);
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrUTF8_WChar(buff, this->row[colIndex], -1, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
	{
		if (this->row == 0)
			return false;
		if (colIndex >= this->data->colList->GetCount())
			return false;
		if (this->row[colIndex] == 0)
			return false;
		sb->Append(this->row[colIndex]);
		return true;
	}

	virtual const UTF8Char *GetNewStr(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrCopyNew(this->row[colIndex]);
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrConcatS(buff, this->row[colIndex], buffSize);
	}

	virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
	{
		if (this->row == 0)
			return DET_NULL;
		if (colIndex >= this->data->colList->GetCount())
			return DET_NULL;
		if (this->row[colIndex] == 0)
			return DET_NULL;
		return outVal->SetValue(this->row[colIndex])?DET_OK:DET_ERROR;
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrToDouble(this->row[colIndex]);
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->row == 0)
			return false;
		if (colIndex >= this->data->colList->GetCount())
			return false;
		if (this->row[colIndex] == 0)
			return false;
		if (Text::StrEqualsICase(this->row[colIndex], (const UTF8Char*)"TRUE"))
			return true;
		else if (Text::StrEqualsICase(this->row[colIndex], (const UTF8Char*)"FALSE"))
			return false;
		return Text::StrToInt32(this->row[colIndex]) != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		return Text::StrCharCnt(this->row[colIndex]);
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		if (this->row == 0)
			return 0;
		if (colIndex >= this->data->colList->GetCount())
			return 0;
		if (this->row[colIndex] == 0)
			return 0;
		UOSInt len = Text::StrCharCnt(this->row[colIndex]);
		MemCopyNO(buff, this->row[colIndex], len);
		return len;
	}

	virtual Math::Vector2D *GetVector(UOSInt colIndex)
	{
		return 0;
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->row == 0)
			return true;
		if (colIndex >= this->data->colList->GetCount())
			return true;
		if (this->row[colIndex] == 0)
			return true;
		return false;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		const UTF8Char *name = this->data->colList->GetItem(colIndex);
		if (name)
		{
			return Text::StrConcat(buff, name);
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		if (colIndex >= this->data->colList->GetCount())
			return DB::DBUtil::CT_Unknown;
		if (colSize)
		{
			*colSize = 256;
		}
		return DB::DBUtil::CT_VarChar;
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		if (colIndex >= this->data->colList->GetCount())
		{
			colDef->SetColType(DB::DBUtil::CT_Unknown);
			return false;
		}
		colDef->SetColName(this->data->colList->GetItem(colIndex));
		colDef->SetColSize(256);
		colDef->SetColType(DB::DBUtil::CT_VarChar);
		colDef->SetAttr(0);
		colDef->SetColDP(0);
		colDef->SetDefVal(0);
		colDef->SetIsAutoInc(false);
		colDef->SetIsNotNull(false);
		colDef->SetIsPK(false);
		return true;
	}

	virtual void DelNewStr(const UTF8Char *s)
	{
		Text::StrDelNew(s);
	}
};

DB::TextDB::TextDB(const UTF8Char *sourceName) : DB::ReadingDB(sourceName)
{
	NEW_CLASS(this->dbMap, Data::StringUTF8Map<DBData*>());
	this->currDB = 0;
}

DB::TextDB::~TextDB()
{
	OSInt i;
	OSInt j;
	OSInt k;
	Data::ArrayList<DBData*> *dbList = this->dbMap->GetValues();
	DBData *data;
	const UTF8Char **vals;
	k = dbList->GetCount();
	while (k-- > 0)
	{
		data = dbList->GetItem(k);
		i = data->valList->GetCount();
		while (i-- > 0)
		{
			vals = data->valList->GetItem(i);
			j = data->colList->GetCount();
			while (j-- > 0)
			{
				SDEL_TEXT(vals[j]);
			}
			MemFree(vals);
		}
		i = data->colList->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(data->colList->GetItem(i));
		}
		DEL_CLASS(data->colList);
		DEL_CLASS(data->valList);
		Text::StrDelNew(data->name);
		MemFree(data);
	}
	DEL_CLASS(this->dbMap);
}

UOSInt DB::TextDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	UOSInt initCnt = names->GetCount();
	names->AddRange(this->dbMap->GetKeys());
	return names->GetCount() - initCnt;
}

DB::DBReader *DB::TextDB::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	DBData *data;
	if (name == 0)
	{
		if (this->dbMap->GetCount() == 1)
		{
			data = this->dbMap->GetValues()->GetItem(0);
		}
		else
		{
			data = 0;
		}
	}
	else
	{
		data = this->dbMap->Get(name);
	}
	if (data == 0)
	{
		return 0;
	}
	DB::DBReader *reader;
	NEW_CLASS(reader, TextDBReader(data));
	return reader;
}

void DB::TextDB::CloseReader(DBReader *r)
{
	TextDBReader *reader = (TextDBReader*)r;
	DEL_CLASS(reader);
}

void DB::TextDB::GetErrorMsg(Text::StringBuilderUTF *str)
{

}

void DB::TextDB::Reconnect()
{

}

Bool DB::TextDB::AddTable(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *colList)
{
	DBData *data = this->dbMap->Get(tableName);
	if (data)
		return false;
	OSInt i;
	OSInt j;
	data = MemAlloc(DBData, 1);
	data->name = Text::StrCopyNew(tableName);
	NEW_CLASS(data->colList, Data::ArrayList<const UTF8Char*>());
	i = 0;
	j = colList->GetCount();
	while (i < j)
	{
		data->colList->Add(Text::StrCopyNew(colList->GetItem(i)));
		i++;
	}
	NEW_CLASS(data->valList, Data::ArrayList<const UTF8Char**>());
	this->dbMap->Put(tableName, data);
	this->currDB = data;
	return true;
}

Bool DB::TextDB::AddTableData(Data::ArrayList<const UTF8Char*> *valList)
{
	if (this->currDB == 0)
		return false;
	if (this->currDB->colList->GetCount() != valList->GetCount())
	{
		return false;
	}
	OSInt i = 0;
	OSInt j = valList->GetCount();
	const UTF8Char *csptr;
	const UTF8Char **vals = MemAlloc(const UTF8Char*, j);
	while (i < j)
	{
		csptr = valList->GetItem(i);
		if (csptr)
		{
			vals[i] = Text::StrCopyNew(csptr);
		}
		else
		{
			vals[i] = 0;
		}
		
		i++;
	}
	this->currDB->valList->Add(vals);
	return true;
}
