#ifndef _SM_DB_MYSQLMAINTANCE
#define _SM_DB_MYSQLMAINTANCE
#include "Net/MySQLTCPClient.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class MySQLMaintance
	{
	private:
		Net::MySQLTCPClient *cli;
		Bool needRelease;

	public:
		MySQLMaintance(Net::MySQLTCPClient *cli, Bool needRelease);
		~MySQLMaintance();

		void RepairSchema(const UTF8Char *schema, Text::StringBuilderUTF *sb);
		void RepairTable(const UTF8Char *tableName, Text::StringBuilderUTF *sb);
	};
}
#endif