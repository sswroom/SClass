#ifndef _SM_DB_SQLCOMMAND
#define _SM_DB_SQLCOMMAND
#include "DB/DBUtil.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	namespace SQL
	{
		enum class CommandType
		{
			CreateTable,
			ShowDatabases,
			Use
		};
		class SQLCommand
		{
		private:
			static Bool IsPunctuation(UnsafeArray<const UTF8Char> s);
		public:
			virtual ~SQLCommand() {};

			virtual CommandType GetCommandType() = 0;

			static Optional<SQLCommand> Parse(UnsafeArray<const UTF8Char> sql, DB::SQLType sqlType);
		};
	}
}
#endif
