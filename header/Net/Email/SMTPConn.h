#ifndef _SM_NET_EMAIL_SMTPCONN
#define _SM_NET_EMAIL_SMTPCONN
#include "AnyType.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "IO/Writer.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
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
			UnsafeArrayOpt<UTF8Char> msgRet;
			Sync::Event evt;
			Optional<IO::Writer> logWriter;
			UInt32 initCode;
			UIntOS maxSize;
			Bool authPlain;
			Bool authLogin;

			static UInt32 __stdcall SMTPThread(AnyType userObj);
			UInt32 WaitForResult(OptOut<UnsafeArrayOpt<UTF8Char>> msgRetEnd);
		public:
			SMTPConn(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, ConnType connType, Optional<IO::Writer> logWriter, Data::Duration timeout);
			~SMTPConn();

			Bool IsError();
			UIntOS GetMaxSize();

			Bool SendHelo(Text::CStringNN cliName);
			Bool SendEHlo(Text::CStringNN cliName);
			Bool SendAuth(Text::CStringNN userName, Text::CStringNN password);
			Bool SendMailFrom(Text::CStringNN fromEmail);
			Bool SendRcptTo(Text::CStringNN toEmail);
			Bool SendData(UnsafeArray<const UTF8Char> buff, UIntOS buffSize);
			Bool SendQuit();
		};
	}
}
#endif
