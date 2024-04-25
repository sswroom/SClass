#ifndef _SM_NET_EMAIL_SMTPCONN
#define _SM_NET_EMAIL_SMTPCONN
#include "AnyType.h"
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
			enum class ConnType
			{
				Plain,
				STARTTLS,
				SSL
			};
		private:
			NN<Net::TCPClient> cli;
			NN<Text::UTF8Writer> writer;
			Bool threadToStop;
			Bool threadRunning;
			Bool threadStarted;
			Bool statusChg;
			UInt32 lastStatus;
			UTF8Char *msgRet;
			Sync::Event evt;
			Optional<IO::Writer> logWriter;
			UInt32 initCode;
			UOSInt maxSize;
			Bool authPlain;
			Bool authLogin;

			static UInt32 __stdcall SMTPThread(AnyType userObj);
			UInt32 WaitForResult(UTF8Char **msgRetEnd);
		public:
			SMTPConn(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, ConnType connType, Optional<IO::Writer> logWriter, Data::Duration timeout);
			~SMTPConn();

			Bool IsError();
			UOSInt GetMaxSize();

			Bool SendHelo(Text::CString cliName);
			Bool SendEHlo(Text::CString cliName);
			Bool SendAuth(Text::CString userName, Text::CString password);
			Bool SendMailFrom(Text::CString fromEmail);
			Bool SendRcptTo(Text::CString toEmail);
			Bool SendData(const UTF8Char *buff, UOSInt buffSize);
			Bool SendQuit();
		};
	}
}
#endif
