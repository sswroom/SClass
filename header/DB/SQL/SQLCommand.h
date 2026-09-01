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
			Use,
			Comment,
			MetaCommand,
			SetConfig,
			Select,
			Insert
		};
		class SQLCommand
		{
		private:
			static Bool IsPunctuation(UnsafeArray<const UTF8Char> s);
			static void ParseColumnName(NN<Text::StringBuilderUTF8> sb, DB::SQLType sqlType);
		public:
			virtual ~SQLCommand() {};

			virtual CommandType GetCommandType() const = 0;

			static Optional<SQLCommand> Parse(UnsafeArray<const UTF8Char> sql, DB::SQLType sqlType);
		};
	}
}
#endif
