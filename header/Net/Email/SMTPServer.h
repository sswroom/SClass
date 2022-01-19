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
				UInt8 *buff;
				UOSInt buffSize;
				const UTF8Char *cliName;
				const UTF8Char *mailFrom;
				Data::ArrayList<const UTF8Char *> *rcptTo;
				Bool dataMode;
				Int32 loginMode;
				IO::MemoryStream *dataStm;
				Text::LineBreakType lastLBT;
				Bool login;
				const UTF8Char *userName;
			} MailStatus;

			typedef UTF8Char *(__stdcall *MailHandler)(UTF8Char *queryId, void *userObj, Net::TCPClient *cli, MailStatus *mail);
			typedef Bool (__stdcall *LoginHandler)(void *userObj, const UTF8Char *userName, const UTF8Char *pwd);
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::Email::SMTPConn::ConnType connType;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::LogTool *log;
			Text::String *domain;
			const UTF8Char *serverName;

			MailHandler mailHdlr;
			LoginHandler loginHdlr;
			void *mailObj;
			UInt32 maxMailSize;
			IO::FileStream *rawLog;

			static void __stdcall ClientReady(Net::TCPClient *cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
			UOSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const UTF8Char *msg);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(Net::TCPClient *cli, MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen, Text::LineBreakType lbt);
		public:
			SMTPServer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::LogTool *log, const UTF8Char *domain, const UTF8Char *serverName, MailHandler mailHdlr, LoginHandler loginHdlr, void *userObj);
			~SMTPServer();

			Bool IsError();
			static UInt16 GetDefaultPort(Net::Email::SMTPConn::ConnType connType);
		};
	}
}
#endif