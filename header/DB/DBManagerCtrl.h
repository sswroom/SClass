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
		IO::LogTool *log;
		Net::SocketFactory *sockf;
		Parser::ParserList *parsers;
		Text::String *connStr;
		DB::DBTool *db;
		ConnStatus status;

		DBManagerCtrl(IO::LogTool *log, Net::SocketFactory *sockf, Parser::ParserList *parsers);
	public:
		~DBManagerCtrl();

		Bool Connect();
		void Disconnect();
		ConnStatus GetStatus();
		Text::String *GetConnStr();
		DB::DBTool *GetDB();
		void GetConnName(Text::StringBuilderUTF8 *sb);

		static DBManagerCtrl *Create(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf, Parser::ParserList *parsers);
		static DBManagerCtrl *Create(Text::CString connStr, IO::LogTool *log, Net::SocketFactory *sockf, Parser::ParserList *parsers);
		static DBManagerCtrl *Create(DB::DBTool *db, IO::LogTool *log, Net::SocketFactory *sockf, Parser::ParserList *parsers);
	};
}
#endif
