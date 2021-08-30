#ifndef _SM_NET_EMAIL_SMTPCONN
#define _SM_NET_EMAIL_SMTPCONN
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Text/UTF8Writer.h"

namespace Net
{
	namespace Email
	{
		class SMTPConn
		{
		public:
			enum ConnType
			{
				CT_PLAIN,
				CT_STARTTLS,
				CT_SSL
			};
		private:
			Net::TCPClient *cli;
			Text::UTF8Writer *writer;
			Bool threadToStop;
			Bool threadRunning;
			Bool threadStarted;
			Bool logged;
			Bool statusChg;
			UInt32 lastStatus;
			UTF8Char *msgRet;
			Sync::Event *evt;
			IO::Writer *logWriter;
			UInt32 initCode;

			static UInt32 __stdcall SMTPThread(void *userObj);
			UInt32 WaitForResult();
		public:
			SMTPConn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, ConnType connType, IO::Writer *logWriter);
			~SMTPConn();

			Bool IsError();

			Bool SendHelo(const UTF8Char *cliName);
			Bool SendEHlo(const UTF8Char *cliName);
			Bool SendMailFrom(const UTF8Char *fromEmail);
			Bool SendRcptTo(const UTF8Char *toEmail);
			Bool SendData(const UTF8Char *buff, UOSInt buffSize);
			Bool SendQuit();
		};
	}
}
#endif
