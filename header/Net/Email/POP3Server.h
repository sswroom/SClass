#ifndef _SM_NET_EMAIL_POP3SERVER
#define _SM_NET_EMAIL_POP3SERVER
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
				UInt8 *buff;
				UOSInt buffSize;
				Int32 userId;
				const UTF8Char *cliName;
				const UTF8Char *userName;
				Data::ArrayList<const UTF8Char *> *rcptTo;
				Bool dataMode;
				IO::MemoryStream *dataStm;
			} MailStatus;

			typedef WChar *(__stdcall *MailHandler)(WChar *queryId, void *userObj, Net::TCPClient *cli, MailStatus *mail);
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::TCPServer *svr;
			Net::TCPClientMgr *cliMgr;
			IO::LogTool *log;
			Text::String *greeting;

			Net::Email::MailController *mailCtrl;
			IO::FileStream *rawLog;

			static void __stdcall ConnReady(Net::TCPClient *cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData);
			UOSInt WriteMessage(Net::TCPClient *cli, Bool success, const UTF8Char *msg, UOSInt msgLen);
			UOSInt WriteRAW(Net::TCPClient *cli, const UTF8Char *msg, UOSInt msgLen);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(Net::TCPClient *cli, MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen);
		public:
			POP3Server(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, const UTF8Char *greeting, Net::Email::MailController *mailCtrl);
			~POP3Server();

			Bool IsError();
			static UInt16 GetDefaultPort(Bool ssl);
		};
	}
}
#endif