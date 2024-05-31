#ifndef _SM_DB_TABLEDEF
#define _SM_DB_TABLEDEF
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace DB
{
	class DBReader;

	class TableDef
	{
	private:
		Optional<Text::String> databaseName;
		Optional<Text::String> schemaName;
		NN<Text::String> tableName;
		Optional<Text::String> engine;
		Optional<Text::String> charset;
		UnsafeArrayOpt<const UTF8Char> attr;
		UnsafeArrayOpt<const UTF8Char> comments;
		SQLType sqlType;

		Data::ArrayListNN<ColDef> cols;

	public:
		TableDef(Text::CString schemaName, Text::CStringNN tableName);
		~TableDef();

		Optional<Text::String> GetDatabaseName() const;
		Optional<Text::String> GetSchemaName() const;
		NN<Text::String> GetTableName() const;
		Optional<Text::String> GetEngine() const;
		Optional<Text::String> GetCharset() const;
		UnsafeArrayOpt<const UTF8Char> GetAttr() const;
		UnsafeArrayOpt<const UTF8Char> GetComments() const;
		DB::SQLType GetSQLType() const;
		UOSInt GetColCnt() const;
		Optional<DB::ColDef> GetCol(UOSInt index) const;
		Optional<DB::ColDef> GetSinglePKCol() const;
		UOSInt CountPK() const;
		Data::ArrayIterator<NN<DB::ColDef>> ColIterator() const;

		TableDef *AddCol(NN<DB::ColDef> col);
		TableDef *SetDatabaseName(Text::CString databaseName);
		TableDef *SetSchemaName(Text::CString schemaName);
		TableDef *SetTableName(Text::CString tableName);
		TableDef *SetEngine(Text::CString engine);
		TableDef *SetCharset(Text::CString charset);
		TableDef *SetAttr(UnsafeArrayOpt<const UTF8Char> attr);
		TableDef *SetComments(UnsafeArrayOpt<const UTF8Char> comments);
		TableDef *SetSQLType(SQLType sqlType);
		
		void ColFromReader(NN<DB::DBReader> r);
		NN<TableDef> Clone() const;
		NN<Data::Class> CreateTableClass() const;
	};
}
#endif
