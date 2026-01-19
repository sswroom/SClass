#ifndef _SM_NET_EMAIL_POP3SERVER
#define _SM_NET_EMAIL_POP3SERVER
#include "Data/ArrayListArr.hpp"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"
#include "Net/Email/MailController.h"

namespace Net
{
	namespace Email
	{
		class POP3Server
		{
		public:
			typedef struct
			{
				UInt8 buff[2048];
				UIntOS buffSize;
				Int32 userId;
				UnsafeArrayOpt<const UTF8Char> cliName;
				Optional<Text::String> userName;
				Data::ArrayListArr<const UTF8Char> rcptTo;
				Bool dataMode;
				Optional<IO::MemoryStream> dataStm;
			} MailStatus;

		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Bool sslConn;
			NN<Net::TCPServer> svr;
			Net::TCPClientMgr cliMgr;
			NN<IO::LogTool> log;
			NN<Text::String> greeting;

			NN<Net::Email::MailController> mailCtrl;
			Optional<IO::FileStream> rawLog;

			static void __stdcall ConnReady(NN<Net::TCPClient> cli, AnyType userObj);
			static void __stdcall ConnHdlr(NN<Socket> s, AnyType userObj);
			static void __stdcall ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			UIntOS WriteMessage(NN<Net::TCPClient> cli, Bool success, Text::CString msg);
			UIntOS WriteRAW(NN<Net::TCPClient> cli, UnsafeArray<const UTF8Char> msg, UIntOS msgLen);
			//static IntOS WriteMessage(NN<Net::TCPClient> cli, Int32 statusCode, UnsafeArray<const Char> msg);
			void ParseCmd(NN<Net::TCPClient> cli, NN<MailStatus> cliStatus, UnsafeArray<const UTF8Char> cmd, UIntOS cmdLen);
		public:
			POP3Server(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Bool sslConn, UInt16 port, NN<IO::LogTool> log, Text::CStringNN greeting, NN<Net::Email::MailController> mailCtrl, Bool autoStart);
			~POP3Server();

			Bool Start();
			Bool IsError();
			static UInt16 GetDefaultPort(Bool ssl);
		};
	}
}
#endif