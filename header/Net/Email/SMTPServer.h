#ifndef _SM_NET_EMAIL_SMTPSERVER
#define _SM_NET_EMAIL_SMTPSERVER
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPServer.h"
#include "Net/TCPClientMgr.h"
#include "Net/Email/SMTPConn.h"
#include "Text/MyString.h"
#include "Text/String.h"

namespace Net
{
	namespace Email
	{
		class SMTPServer
		{
		public:
			typedef struct
			{
				UInt8 buff[4096];
				UOSInt buffSize;
				Text::String *cliName;
				Text::String *mailFrom;
				Data::ArrayListObj<Text::String *> rcptTo;
				Bool dataMode;
				Int32 loginMode;
				IO::MemoryStream *dataStm;
				Text::LineBreakType lastLBT;
				Bool login;
				Text::String *userName;
			} MailStatus;

			typedef UnsafeArrayOpt<UTF8Char> (CALLBACKFUNC MailHandler)(UnsafeArray<UTF8Char> queryId, AnyType userObj, NN<Net::TCPClient> cli, NN<const MailStatus> mail);
			typedef Bool (CALLBACKFUNC LoginHandler)(AnyType userObj, Text::CStringNN userName, Text::CStringNN pwd);
		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Net::Email::SMTPConn::ConnType connType;
			Net::TCPServer *svr;
			Net::TCPClientMgr cliMgr;
			NN<IO::LogTool> log;
			NN<Text::String> domain;
			NN<Text::String> serverName;

			MailHandler mailHdlr;
			LoginHandler loginHdlr;
			AnyType mailObj;
			UInt32 maxMailSize;
			IO::FileStream *rawLog;

			static void __stdcall ClientReady(NN<Net::TCPClient> cli, AnyType userObj);
			static void __stdcall ConnHdlr(NN<Socket> s, AnyType userObj);
			static void __stdcall ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			UOSInt WriteMessage(NN<Net::TCPClient> cli, Int32 statusCode, Text::CStringNN msg);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(NN<Net::TCPClient> cli, NN<MailStatus> cliStatus, UnsafeArray<const UTF8Char> cmd, UOSInt cmdLen, Text::LineBreakType lbt);
		public:
			SMTPServer(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt16 port, Net::Email::SMTPConn::ConnType connType, NN<IO::LogTool> log, Text::CStringNN domain, Text::CStringNN serverName, MailHandler mailHdlr, LoginHandler loginHdlr, AnyType userObj, Bool autoStart);
			~SMTPServer();

			Bool Start();
			Bool IsError();
			static UInt16 GetDefaultPort(Net::Email::SMTPConn::ConnType connType);
		};
	}
}
#endif