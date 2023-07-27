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
				Data::ArrayList<Text::String *> rcptTo;
				Bool dataMode;
				Int32 loginMode;
				IO::MemoryStream *dataStm;
				Text::LineBreakType lastLBT;
				Bool login;
				Text::String *userName;
			} MailStatus;

			typedef UTF8Char *(__stdcall *MailHandler)(UTF8Char *queryId, void *userObj, NotNullPtr<Net::TCPClient> cli, MailStatus *mail);
			typedef Bool (__stdcall *LoginHandler)(void *userObj, Text::CString userName, Text::CString pwd);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			Net::Email::SMTPConn::ConnType connType;
			Net::TCPServer *svr;
			Net::TCPClientMgr cliMgr;
			IO::LogTool *log;
			NotNullPtr<Text::String> domain;
			NotNullPtr<Text::String> serverName;

			MailHandler mailHdlr;
			LoginHandler loginHdlr;
			void *mailObj;
			UInt32 maxMailSize;
			IO::FileStream *rawLog;

			static void __stdcall ClientReady(NotNullPtr<Net::TCPClient> cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff);
			static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);
			UOSInt WriteMessage(NotNullPtr<Net::TCPClient> cli, Int32 statusCode, Text::CString msg);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(NotNullPtr<Net::TCPClient> cli, MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen, Text::LineBreakType lbt);
		public:
			SMTPServer(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::LogTool *log, Text::CString domain, Text::CString serverName, MailHandler mailHdlr, LoginHandler loginHdlr, void *userObj, Bool autoStart);
			~SMTPServer();

			Bool Start();
			Bool IsError();
			static UInt16 GetDefaultPort(Net::Email::SMTPConn::ConnType connType);
		};
	}
}
#endif