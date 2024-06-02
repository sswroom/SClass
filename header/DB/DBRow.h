#ifndef _SM_DB_DBROW
#define _SM_DB_DBROW
#include "Data/StringMapNN.h"
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
			UnsafeArrayOpt<const UTF8Char> str;
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
		NN<TableDef> table;
		Data::StringMapNN<Field> dataMap;

		static void FreeField(NN<Field> field);
		static DataType GetDataType(NN<Field> field);

		Bool SetFieldNull(NN<Field> field);
		Bool SetFieldStr(NN<Field> field, UnsafeArrayOpt<const UTF8Char> strValue);
		Bool SetFieldInt64(NN<Field> field, Int64 intValue);
		Bool SetFieldDouble(NN<Field> field, Double dblValue);
		Bool SetFieldDate(NN<Field> field, const Data::Timestamp &ts);
		Bool SetFieldVector(NN<Field> field, Optional<Math::Geometry::Vector2D> vec);
		Bool SetFieldBinary(NN<Field> field, const UInt8 *buff, UOSInt buffSize);

		Bool IsFieldNull(NN<Field> field) const;
		UnsafeArrayOpt<const UTF8Char> GetFieldStr(NN<Field> field) const;
		Int64 GetFieldInt64(NN<Field> field) const;
		Double GetFieldDouble(NN<Field> field) const;
		Data::Timestamp GetFieldDate(NN<Field> field) const;
		Math::Geometry::Vector2D *GetFieldVector(NN<Field> field) const;
		const UInt8 *GetFieldBinary(NN<Field> field, UOSInt *buffSize) const;

	public:
		DBRow(NN<TableDef> table);
		~DBRow();

		Bool SetByReader(NN<DB::DBReader> r, Bool commit);

		DB::ColDef *GetFieldType(Text::CStringNN fieldName) const;
		DataType GetFieldDataType(Text::CStringNN fieldName) const;
		Bool SetValueNull(Text::CStringNN fieldName);
		Bool SetValueStr(Text::CStringNN fieldName, UnsafeArrayOpt<const UTF8Char> strValue);
		Bool SetValueInt64(Text::CStringNN fieldName, Int64 intValue);
		Bool SetValueDouble(Text::CStringNN fieldName, Double dblValue);
		Bool SetValueDate(Text::CStringNN fieldName, const Data::Timestamp &ts);
		Bool SetValueVector(Text::CStringNN fieldName, Math::Geometry::Vector2D *vec);
		Bool SetValueBinary(Text::CStringNN fieldName, const UInt8 *buff, UOSInt buffSize);

		Bool IsNull(Text::CStringNN fieldName) const;
		UnsafeArrayOpt<const UTF8Char> GetValueStr(Text::CStringNN fieldName) const;
		Int64 GetValueInt64(Text::CStringNN fieldName) const;
		Double GetValueDouble(Text::CStringNN fieldName) const;
		Data::Timestamp GetValueDate(Text::CStringNN fieldName) const;
		Math::Geometry::Vector2D *GetValueVector(Text::CStringNN fieldName) const;
		const UInt8 *GetValueBinary(Text::CStringNN fieldName, UOSInt *buffSize) const;

		void Commit();
		void Rollback();

		Bool GetSinglePKI64(OutParam<Int64> key) const;
		void ToString(NN<Text::StringBuilderUTF8> sb) const;
		void AppendTableName(NN<Text::StringBuilderUTF8> sb) const;
		void AppendVarNameForm(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> colName) const;

		NN<TableDef> GetTableDef();
	};
}
#endif
