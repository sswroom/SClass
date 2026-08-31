#ifndef _SM_DB_SQLFILE
#define _SM_DB_SQLFILE
#include "DB/SQL/SQLCommand.h"
#include "IO/ParsedObject.h"

namespace DB
{
	namespace SQL
	{
		class SQLFile : public IO::ParsedObject
		{
		private:
			Data::ArrayListNN<SQLCommand> sqlList;
			SQLType sqlType;
		public:
			SQLFile(NN<Text::String> sourceName);
			SQLFile(const Text::CStringNN &sourceName);
			virtual ~SQLFile();
			
			virtual IO::ParserType GetParserType() const;
			void SetSQLType(SQLType sqlType);
			void AddSQL(NN<SQLCommand> sql);
			SQLType GetSQLType() const;
		};
	}
}
#endif
