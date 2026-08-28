#ifndef _SM_DB_SQLCREATETABLECOMMAND
#define _SM_DB_SQLCREATETABLECOMMAND
#include "DB/TableDef.h"
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLCreateTableCommand : public SQLCommand
		{
		private:
			NN<DB::TableDef> tableDef;
		public:
			SQLCreateTableCommand(NN<DB::TableDef> tableDef, Bool toRelease);
			virtual ~SQLCreateTableCommand();

			virtual CommandType GetCommandType() const;
			NN<TableDef> GetTableDef();
		};
	}
}
#endif
