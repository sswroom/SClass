#ifndef _SM_DB_TABLEDEF
#define _SM_DB_TABLEDEF
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "Data/ArrayList.h"
#include "Text/String.h"

namespace DB
{
	class TableDef
	{
	private:
		Text::String *databaseName;
		Text::String *tableName;
		Text::String *engine;
		Text::String *charset;
		const UTF8Char *attr;
		const UTF8Char *comments;
		DB::DBUtil::ServerType svrType;

		Data::ArrayList<ColDef*> cols;

	public:
		TableDef(Text::CString tableName);
		~TableDef();

		Text::String *GetDatabaseName() const;
		Text::String *GetTableName() const;
		Text::String *GetEngine() const;
		Text::String *GetCharset() const;
		const UTF8Char *GetAttr() const;
		const UTF8Char *GetComments() const;
		DB::DBUtil::ServerType GetSvrType() const;
		UOSInt GetColCnt() const;
		DB::ColDef *GetCol(UOSInt index) const;
		DB::ColDef *GetSinglePKCol() const;

		TableDef *AddCol(DB::ColDef *col);
		TableDef *SetDatabaseName(Text::CString databaseName);
		TableDef *SetTableName(Text::CString tableName);
		TableDef *SetEngine(Text::CString engine);
		TableDef *SetCharset(Text::CString charset);
		TableDef *SetAttr(const UTF8Char *attr);
		TableDef *SetComments(const UTF8Char *comments);
		TableDef *SetSvrType(DB::DBUtil::ServerType svrType);
		
		TableDef *Clone() const;
		Data::Class *CreateTableClass() const;
	};
}
#endif
