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
		NN<IO::LogTool> log;
		NN<Net::SocketFactory> sockf;
		Optional<Parser::ParserList> parsers;
		Optional<Text::String> connStr;
		Optional<DB::ReadingDB> db;
		ConnStatus status;

		DBManagerCtrl(NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
	public:
		~DBManagerCtrl();

		Bool Connect();
		void Disconnect();
		ConnStatus GetStatus();
		Optional<Text::String> GetConnStr();
		Optional<DB::ReadingDB> GetDB();
		void GetConnName(NN<Text::StringBuilderUTF8> sb);

		static NN<DBManagerCtrl> Create(NN<Text::String> connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NN<DBManagerCtrl> Create(Text::CStringNN connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NN<DBManagerCtrl> Create(NN<DB::DBTool> db, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers);
		static NN<DBManagerCtrl> CreateFromFile(NN<DB::ReadingDB> db, NN<Text::String> filePath, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, NN<Parser::ParserList> parsers);
	};
}
#endif
