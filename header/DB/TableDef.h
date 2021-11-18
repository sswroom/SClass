#ifndef _SM_DB_TABLEDEF
#define _SM_DB_TABLEDEF
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "Data/ArrayList.h"

namespace DB
{
	class TableDef
	{
	private:
		const UTF8Char *databaseName;
		const UTF8Char *tableName;
		const UTF8Char *engine;
		const UTF8Char *charset;
		const UTF8Char *attr;
		const UTF8Char *comments;
		DB::DBUtil::ServerType svrType;

		Data::ArrayList<ColDef*> *cols;

	public:
		TableDef(const UTF8Char *tableName);
		~TableDef();

		const UTF8Char *GetDatabaseName();
		const UTF8Char *GetTableName();
		const UTF8Char *GetEngine();
		const UTF8Char *GetCharset();
		const UTF8Char *GetAttr();
		const UTF8Char *GetComments();
		DB::DBUtil::ServerType GetSvrType();
		UOSInt GetColCnt();
		DB::ColDef *GetCol(UOSInt index);
		DB::ColDef *GetSinglePKCol();

		TableDef *AddCol(DB::ColDef *col);
		TableDef *SetDatabaseName(const UTF8Char *databaseName);
		TableDef *SetTableName(const UTF8Char *tableName);
		TableDef *SetEngine(const UTF8Char *engine);
		TableDef *SetCharset(const UTF8Char *charset);
		TableDef *SetAttr(const UTF8Char *attr);
		TableDef *SetComments(const UTF8Char *comments);
		TableDef *SetSvrType(DB::DBUtil::ServerType svrType);
		
		TableDef *Clone();
		Data::Class *CreateTableClass();
	};
}
#endif
