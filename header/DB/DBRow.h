#ifndef _SM_DB_DBROW
#define _SM_DB_DBROW
#include "Data/StringUTF8Map.h"
#include "Data/Timestamp.h"
#include "DB/DBReader.h"
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
			Data::Timestamp ts;
			const UTF8Char *str;
			Double dVal;
			Math::Geometry::Vector2D *vec;
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
		Data::StringUTF8Map<Field*> dataMap;

		void FreeField(Field *field);
		DataType GetDataType(Field *field) const;

		Bool SetFieldNull(Field *field);
		Bool SetFieldStr(Field *field, const UTF8Char *strValue);
		Bool SetFieldInt64(Field *field, Int64 intValue);
		Bool SetFieldDouble(Field *field, Double dblValue);
		Bool SetFieldDate(Field *field, const Data::Timestamp &ts);
		Bool SetFieldVector(Field *field, Math::Geometry::Vector2D *vec);
		Bool SetFieldBinary(Field *field, const UInt8 *buff, UOSInt buffSize);

		Bool IsFieldNull(Field *field) const;
		const UTF8Char *GetFieldStr(Field *field) const;
		Int64 GetFieldInt64(Field *field) const;
		Double GetFieldDouble(Field *field) const;
		Data::Timestamp GetFieldDate(Field *field) const;
		Math::Geometry::Vector2D *GetFieldVector(Field *field) const;
		const UInt8 *GetFieldBinary(Field *field, UOSInt *buffSize) const;

	public:
		DBRow(TableDef *table);
		~DBRow();

		Bool SetByReader(DB::DBReader *r, Bool commit);

		DB::ColDef *GetFieldType(const UTF8Char *fieldName) const;
		DataType GetFieldDataType(const UTF8Char *fieldName) const;
		Bool SetValueNull(const UTF8Char *fieldName);
		Bool SetValueStr(const UTF8Char *fieldName, const UTF8Char *strValue);
		Bool SetValueInt64(const UTF8Char *fieldName, Int64 intValue);
		Bool SetValueDouble(const UTF8Char *fieldName, Double dblValue);
		Bool SetValueDate(const UTF8Char *fieldName, const Data::Timestamp &ts);
		Bool SetValueVector(const UTF8Char *fieldName, Math::Geometry::Vector2D *vec);
		Bool SetValueBinary(const UTF8Char *fieldName, const UInt8 *buff, UOSInt buffSize);

		Bool IsNull(const UTF8Char *fieldName) const;
		const UTF8Char *GetValueStr(const UTF8Char *fieldName) const;
		Int64 GetValueInt64(const UTF8Char *fieldName) const;
		Double GetValueDouble(const UTF8Char *fieldName) const;
		Data::Timestamp GetValueDate(const UTF8Char *fieldName) const;
		Math::Geometry::Vector2D *GetValueVector(const UTF8Char *fieldName) const;
		const UInt8 *GetValueBinary(const UTF8Char *fieldName, UOSInt *buffSize) const;

		void Commit();
		void Rollback();

		Bool GetSinglePKI64(Int64 *key) const;
		void ToString(Text::StringBuilderUTF8 *sb) const;
		void AppendTableName(Text::StringBuilderUTF8 *sb) const;
		void AppendVarNameForm(Text::StringBuilderUTF8 *sb, const UTF8Char *colName) const;

		TableDef *GetTableDef();
	};
}
#endif
