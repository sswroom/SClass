#ifndef _SM_DB_DBREADER
#define _SM_DB_DBREADER
#include "Data/Class.h"
#include "Data/NamedClass.h"
#include "Data/StringMap.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Math/Vector2D.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class ColDef;
	class TableDef;
	class DBReader
	{
	public:
		typedef enum
		{
			DET_OK = 0,
			DET_NULL = 1,
			DET_ERROR = 2
		} DateErrType;

	protected:
		DBReader(){};
		virtual ~DBReader(){};

	public:
		virtual Bool ReadNext() = 0;
		virtual UOSInt ColCount() = 0;
		virtual OSInt GetRowChanged() = 0; //-1 = error
		virtual Int32 GetInt32(UOSInt colIndex) = 0;
		virtual Int64 GetInt64(UOSInt colIndex) = 0;
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff) = 0;
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb) = 0;
		Bool GetStrN(UOSInt colIndex, Text::StringBuilderUTF8 *sb) { sb->ClearStr(); return GetStr(colIndex, sb); }
		virtual Text::String *GetNewStr(UOSInt colIndex) = 0;
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize) = 0;
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal) = 0;
		virtual Double GetDbl(UOSInt colIndex) = 0;
		virtual Bool GetBool(UOSInt colIndex) = 0;
		virtual UOSInt GetBinarySize(UOSInt colIndex) = 0;
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff) = 0;
		virtual Math::Vector2D *GetVector(UOSInt colIndex) = 0;
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid) = 0;
		virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item);

		virtual Bool IsNull(UOSInt colIndex) = 0;
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff) = 0;
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize) = 0;
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef) = 0;

		TableDef *GenTableDef(const UTF8Char *tableName);
		Data::VariObject *CreateVariObject();
		Data::Class *CreateClass();
		template <class T> Bool ReadAll(Data::ArrayList<T*> *outList, Data::NamedClass<T> *cls);
	};
}

template <class T> Bool DB::DBReader::ReadAll(Data::ArrayList<T*> *outList, Data::NamedClass<T> *cls)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	Data::StringMap<UOSInt> colMap2;
	Bool clsValid = true;
	UOSInt i = 0;
	UOSInt j = this->ColCount();
	while (i < j)
	{
		this->GetName(i, sbuff);
		DB::DBUtil::DB2FieldName(sbuff2, sbuff);
		colMap2.Put(sbuff2, i);

		i++;
	}

	i = 0;
	j = cls->GetFieldCount();
	while (i < j)
	{
		if (!colMap2.ContainsKey(cls->GetFieldName(i)))
		{
			clsValid = false;
		}
		i++;
	}

	if (!clsValid)
	{
		return false;
	}

	UOSInt *colIndex = MemAlloc(UOSInt, j);
	i = 0;
	while (i < j)
	{
		colIndex[i] = colMap2.Get(cls->GetFieldName(i));
		i++;
	}
	while (this->ReadNext())
	{
		T *listObj = cls->CreateObject();
		Data::VariItem item;
		i = 0;
		j = cls->GetFieldCount();
		while (i < j)
		{
			this->GetVariItem(colIndex[i], &item);
			cls->SetFieldClearItem(listObj, i, &item);
			i++;
		}
		outList->Add(listObj);
	}
	MemFree(colIndex);
	return true;
}
#endif
