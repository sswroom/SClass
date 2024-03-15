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

			typedef WChar *(__stdcall *MailHandler)(WChar *queryId, AnyType userObj, Net::TCPClient *cli, MailStatus *mail);
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Bool sslConn;
			Net::TCPServer *svr;
			Net::TCPClientMgr cliMgr;
			NotNullPtr<IO::LogTool> log;
			NotNullPtr<Text::String> greeting;

			NotNullPtr<Net::Email::MailController> mailCtrl;
			IO::FileStream *rawLog;

			static void __stdcall ConnReady(NotNullPtr<Net::TCPClient> cli, AnyType userObj);
			static void __stdcall ConnHdlr(Socket *s, AnyType userObj);
			static void __stdcall ClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType);
			static void __stdcall ClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff);
			static void __stdcall ClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData);
			UOSInt WriteMessage(NotNullPtr<Net::TCPClient> cli, Bool success, Text::CString msg);
			UOSInt WriteRAW(NotNullPtr<Net::TCPClient> cli, const UTF8Char *msg, UOSInt msgLen);
			//static OSInt WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg);
			void ParseCmd(NotNullPtr<Net::TCPClient> cli, NotNullPtr<MailStatus> cliStatus, const UTF8Char *cmd, UOSInt cmdLen);
		public:
			POP3Server(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Bool sslConn, UInt16 port, NotNullPtr<IO::LogTool> log, Text::CString greeting, NotNullPtr<Net::Email::MailController> mailCtrl, Bool autoStart);
			~POP3Server();

			Bool Start();
			Bool IsError();
			static UInt16 GetDefaultPort(Bool ssl);
		};
	}
}
#endif