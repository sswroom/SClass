#ifndef _SM_DB_SQLUSECOMMAND
#define _SM_DB_SQLUSECOMMAND
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class SQLUseCommand : public SQLCommand
		{
		private:
			NN<Text::String> dbName;
		public:
			SQLUseCommand(Text::CStringNN dbName);
			virtual ~SQLUseCommand();

			virtual CommandType GetCommandType() const;
			NN<Text::String> GetDBName() const { return this->dbName; }
		};
	}
}
#endif
