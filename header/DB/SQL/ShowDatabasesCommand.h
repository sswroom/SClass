#ifndef _SM_DB_SHOWDATABASESCOMMAND
#define _SM_DB_SHOWDATABASESCOMMAND
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class ShowDatabasesCommand : public SQLCommand
		{
		public:
			ShowDatabasesCommand();
			virtual ~ShowDatabasesCommand();

			virtual CommandType GetCommandType();
		};
	}
}
#endif
