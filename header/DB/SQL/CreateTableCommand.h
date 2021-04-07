#ifndef _SM_DB_CREATETABLECOMMAND
#define _SM_DB_CREATETABLECOMMAND
#include "DB/TableDef.h"
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class CreateTableCommand : public SQLCommand
		{
		private:
			const UTF8Char *tableName;
			DB::TableDef *tableDef;
		public:
			CreateTableCommand(const UTF8Char *tableName, DB::TableDef *tableDef, Bool toRelease);
			virtual ~CreateTableCommand();

			virtual CommandType GetCommandType();
			const UTF8Char *GetTableName();
			TableDef *GetTableDef();
		};
	}
}
#endif
