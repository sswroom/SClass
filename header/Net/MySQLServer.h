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
		NotNullPtr<Net::SocketFactory> sockf;
		NotNullPtr<IO::LogTool> log;
		Net::TCPServer *svr;
		DB::DBMS *dbms;
		Net::TCPClientMgr *cliMgr;
		Int32 connId;
		Sync::Mutex randMut;
		Data::RandomMT19937 rand;

		static CharsetInfo charsets[];

		static void __stdcall OnClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
		static void __stdcall OnClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
		static void __stdcall OnClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
		static void __stdcall OnClientConn(Socket *s, AnyType userObj);
		
	public:
		MySQLServer(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SocketUtil::AddressInfo> bindAddr, UInt16 port, DB::DBMS *dbms, Bool autoStart);
		~MySQLServer();

		Bool Start();
		Bool IsError();
	};
}
#endif
