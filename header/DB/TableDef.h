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
		Text::String *databaseName;
		Text::String *schemaName;
		NotNullPtr<Text::String> tableName;
		Text::String *engine;
		Text::String *charset;
		const UTF8Char *attr;
		const UTF8Char *comments;
		SQLType sqlType;

		Data::ArrayListNN<ColDef> cols;

	public:
		TableDef(Text::CString schemaName, Text::CString tableName);
		~TableDef();

		Text::String *GetDatabaseName() const;
		Text::String *GetSchemaName() const;
		Text::String *GetTableName() const;
		Text::String *GetEngine() const;
		Text::String *GetCharset() const;
		const UTF8Char *GetAttr() const;
		const UTF8Char *GetComments() const;
		DB::SQLType GetSQLType() const;
		UOSInt GetColCnt() const;
		DB::ColDef *GetCol(UOSInt index) const;
		DB::ColDef *GetSinglePKCol() const;

		TableDef *AddCol(NotNullPtr<DB::ColDef> col);
		TableDef *SetDatabaseName(Text::CString databaseName);
		TableDef *SetSchemaName(Text::CString schemaName);
		TableDef *SetTableName(Text::CString tableName);
		TableDef *SetEngine(Text::CString engine);
		TableDef *SetCharset(Text::CString charset);
		TableDef *SetAttr(const UTF8Char *attr);
		TableDef *SetComments(const UTF8Char *comments);
		TableDef *SetSQLType(SQLType sqlType);
		
		void ColFromReader(DB::DBReader *r);
		TableDef *Clone() const;
		Data::Class *CreateTableClass() const;
	};
}
#endif
