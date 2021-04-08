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
			DB::TableDef *tableDef;
		public:
			CreateTableCommand(DB::TableDef *tableDef, Bool toRelease);
			virtual ~CreateTableCommand();

			virtual CommandType GetCommandType();
			TableDef *GetTableDef();
		};
	}
}
#endif
