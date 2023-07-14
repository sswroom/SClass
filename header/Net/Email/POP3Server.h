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
				UInt8 buff[2048];
				UOSInt buffSize;
				Int32 userId;
				const UTF8Char *cliName;
				Text::String *userName;
				Data::ArrayList<const UTF8Char *> rcptTo;
				Bool dataMode;
				IO::MemoryStream *dataStm;
			} MailStatus;

			typedef WChar *(__stdcall *MailHandler)(WChar *queryId, void *userObj, Net::TCPClient *cli, MailStatus *mail);
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::TCPServer *svr;
			Net::TCPClientMgr cliMgr;
			IO::LogTool *log;
			NotNullPtr<Text::String> greeting;

			Net::Email::MailController *mailCtrl;
			IO::FileStream *rawLog;

			static void __stdcall ConnReady(NotNullPtr<Net::TCPClient> cli, void *userObj);
			static void __stdcall ConnHdlr(Socket *s, void *userObj);
			static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size);
			static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData);
			UOSInt WriteMessage(NotNullPtr<Net::TCPClient> cli, Bool success, const UTF8Char *msg, UOSInt msgLen);
			UOSInt WriteRAW(NotNullPtr<Net::TCPClient> cli, const UTF8Char *msg, UOSInt msgLen);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(NotNullPtr<Net::TCPClient> cli, MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen);
		public:
			POP3Server(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, Text::CString greeting, Net::Email::MailController *mailCtrl, Bool autoStart);
			~POP3Server();

			Bool Start();
			Bool IsError();
			static UInt16 GetDefaultPort(Bool ssl);
		};
	}
}
#endif