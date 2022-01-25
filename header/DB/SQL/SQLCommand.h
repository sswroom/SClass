#ifndef _SM_DB_SQLCOMMAND
#define _SM_DB_SQLCOMMAND
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	namespace SQL
	{
		class SQLCommand
		{
		public:
			typedef enum
			{
				CT_CREATE_TABLE
			} CommandType;
		private:
			static const UTF8Char *ParseNextWord(const UTF8Char *sql, Text::StringBuilderUTF8 *sb, DB::DBUtil::ServerType svrType);
			static Bool IsPunctuation(const UTF8Char *s);
		public:
			virtual ~SQLCommand() {};

			virtual CommandType GetCommandType() = 0;

			static SQLCommand *Parse(const UTF8Char *sql, DB::DBUtil::ServerType svrType);
		};
	}
}
#endif
