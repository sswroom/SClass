#ifndef _SM_DB_DBMANAGERCTRL
#define _SM_DB_DBMANAGERCTRL
#include "DB/DBTool.h"
#include "Net/SocketFactory.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"

namespace DB
{
	class DBManagerCtrl
	{
	public:
		enum class ConnStatus
		{
			NotConnected,
			Error,
			Connected
		};
	private:
		NotNullPtr<IO::LogTool> log;
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Parser::ParserList> parsers;
		Optional<Text::String> connStr;
		Optional<DB::ReadingDB> db;
		ConnStatus status;

		DBManagerCtrl(NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
	public:
		~DBManagerCtrl();

		Bool Connect();
		void Disconnect();
		ConnStatus GetStatus();
		Optional<Text::String> GetConnStr();
		Optional<DB::ReadingDB> GetDB();
		void GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb);

		static NotNullPtr<DBManagerCtrl> Create(Text::String *connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NotNullPtr<DBManagerCtrl> Create(Text::CString connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NotNullPtr<DBManagerCtrl> Create(NotNullPtr<DB::DBTool> db, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NotNullPtr<DBManagerCtrl> CreateFromFile(NotNullPtr<DB::ReadingDB> db, NotNullPtr<Text::String> filePath, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Parser::ParserList> parsers);
	};
}
#endif
