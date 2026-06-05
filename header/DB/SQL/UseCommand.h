#ifndef _SM_DB_USECOMMAND
#define _SM_DB_USECOMMAND
#include "DB/SQL/SQLCommand.h"

namespace DB
{
	namespace SQL
	{
		class UseCommand : public SQLCommand
		{
		private:
			NN<Text::String> dbName;
		public:
			UseCommand(Text::CStringNN dbName);
			virtual ~UseCommand();

			virtual CommandType GetCommandType();
			NN<Text::String> GetDBName() const { return this->dbName; }
		};
	}
}
#endif
