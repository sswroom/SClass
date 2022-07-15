#ifndef _SM_NET_MYSQLSERVER
#define _SM_NET_MYSQLSERVER
#include "Crypto/Hash/SHA1.h"
#include "Data/RandomMT19937.h"
#include "Data/StringUTF8Map.h"
#include "DB/DBMS.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClientMgr.h"
#include "Net/TCPServer.h"
#include "Sync/Mutex.h"

namespace Net
{
	class MySQLServer
	{
	public:
		typedef struct
		{
			Int32 id;
			const Char *collationName;
			const Char *characterSetName;
		} CharsetInfo;

	private:
		Net::SocketFactory *sockf;
		IO::LogTool *log;
		Net::TCPServer *svr;
		DB::DBMS *dbms;
		Net::TCPClientMgr *cliMgr;
		Int32 connId;
		Sync::Mutex randMut;
		Data::RandomMT19937 rand;

		static CharsetInfo charsets[];

		static void __stdcall OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
		static void __stdcall OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
		static void __stdcall OnClientConn(Socket *s, void *userObj);
		
	public:
		MySQLServer(Net::SocketFactory *sockf, UInt16 port, DB::DBMS *dbms);
		~MySQLServer();

		Bool IsError();
	};
}
#endif
