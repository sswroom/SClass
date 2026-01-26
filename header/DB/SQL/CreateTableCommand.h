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
			NN<DB::TableDef> tableDef;
		public:
			CreateTableCommand(NN<DB::TableDef> tableDef, Bool toRelease);
			virtual ~CreateTableCommand();

			virtual CommandType GetCommandType();
			NN<TableDef> GetTableDef();
		};
	}
}
#endif
