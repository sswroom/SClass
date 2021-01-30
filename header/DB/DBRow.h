#ifndef _SM_DB_DBROW
#define _SM_DB_DBROW
#include "Data/DateTime.h"
#include "Data/StringUTF8Map.h"
#include "DB/TableDef.h"

namespace DB
{
	class DBRow
	{
	public:
		typedef enum
		{
			DT_UNKNOWN,
			DT_INT64,
			DT_DATETIME,
			DT_STRING,
			DT_DOUBLE,
			DT_VECTOR,
			DT_BINARY
		} DataType;

	private:
		union FieldData
		{
			Int64 iVal;
			Data::DateTime *dt;
			const UTF8Char *str;
			Double dVal;
			Math::Vector2D *vec;
			UInt8 *bin;
		};

		struct Field
		{
			ColDef *def;
			Bool committedNull;
			FieldData committedData;
			Bool currentChanged;
			Bool currentNull;
			FieldData currentData;
		};
	private:
		TableDef *table;
		Data::StringUTF8Map<Field*> *dataMap;

		void FreeField(Field *field);
		DataType GetDataType(Field *field);
	public:
		DBRow(TableDef *table);
		~DBRow();

		DB::ColDef *GetFieldType(const UTF8Char *fieldName);
		DataType GetFieldDataType(const UTF8Char *fieldName);
		Bool SetValueNull(const UTF8Char *fieldName);
		Bool SetValueStr(const UTF8Char *fieldName, const UTF8Char *strValue);
		Bool SetValueInt64(const UTF8Char *fieldName, Int64 intValue);
		Bool SetValueDouble(const UTF8Char *fieldName, Double dblValue);
		Bool SetValueDate(const UTF8Char *fieldName, Data::DateTime *dt);
		Bool SetValueVector(const UTF8Char *fieldName, Math::Vector2D *vec);
		Bool SetValueBinary(const UTF8Char *fieldName, const UInt8 *buff, UOSInt buffSize);

		Bool IsNull(const UTF8Char *fieldName);
		const UTF8Char *GetValueStr(const UTF8Char *fieldName);
		Int64 GetValueInt64(const UTF8Char *fieldName);
		Double GetValueDouble(const UTF8Char *fieldName);
		Data::DateTime *GetValueDate(const UTF8Char *fieldName);
		Math::Vector2D *GetValueVector(const UTF8Char *fieldName);
		const UInt8 *GetValueBinary(const UTF8Char *fieldName, UOSInt *buffSize);

		void Commit();
		void Rollback();
	};
}
#endif
