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
		NotNullPtr<Text::String> tableName;
		Optional<Text::String> engine;
		Optional<Text::String> charset;
		const UTF8Char *attr;
		const UTF8Char *comments;
		SQLType sqlType;

		Data::ArrayListNN<ColDef> cols;

	public:
		TableDef(Text::CString schemaName, Text::CString tableName);
		~TableDef();

		Optional<Text::String> GetDatabaseName() const;
		Optional<Text::String> GetSchemaName() const;
		NotNullPtr<Text::String> GetTableName() const;
		Optional<Text::String> GetEngine() const;
		Optional<Text::String> GetCharset() const;
		const UTF8Char *GetAttr() const;
		const UTF8Char *GetComments() const;
		DB::SQLType GetSQLType() const;
		UOSInt GetColCnt() const;
		Optional<DB::ColDef> GetCol(UOSInt index) const;
		Optional<DB::ColDef> GetSinglePKCol() const;
		UOSInt CountPK() const;
		Data::ArrayIterator<NotNullPtr<DB::ColDef>> ColIterator() const;

		TableDef *AddCol(NotNullPtr<DB::ColDef> col);
		TableDef *SetDatabaseName(Text::CString databaseName);
		TableDef *SetSchemaName(Text::CString schemaName);
		TableDef *SetTableName(Text::CString tableName);
		TableDef *SetEngine(Text::CString engine);
		TableDef *SetCharset(Text::CString charset);
		TableDef *SetAttr(const UTF8Char *attr);
		TableDef *SetComments(const UTF8Char *comments);
		TableDef *SetSQLType(SQLType sqlType);
		
		void ColFromReader(NotNullPtr<DB::DBReader> r);
		NotNullPtr<TableDef> Clone() const;
		NotNullPtr<Data::Class> CreateTableClass() const;
	};
}
#endif
