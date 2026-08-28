#ifndef _SM_DB_SQLSHOWDATABASESCOMMAND
#define _SM_DB_SQLSHOWDATABASESCOMMAND
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLShowDatabasesCommand : public SQLCommand
		{
		public:
			SQLShowDatabasesCommand();
			virtual ~SQLShowDatabasesCommand();

			virtual CommandType GetCommandType() const;
		};
	}
}
#endif
