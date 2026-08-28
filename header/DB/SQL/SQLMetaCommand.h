#ifndef _SM_DB_SQLMETACOMMAND
#define _SM_DB_SQLMETACOMMAND
#include "DB/SQL/SQLCommand.h"
#include "Text/String.h"

namespace DB
{
	namespace SQL
	{
		class SQLMetaCommand : public SQLCommand
		{
		private:
			NN<Text::String> commandText;
		public:
			SQLMetaCommand(Text::CStringNN commandText);
			virtual ~SQLMetaCommand();

			virtual CommandType GetCommandType() const;
			NN<Text::String> GetCommandText() const;
		};
	}
}
#endif
