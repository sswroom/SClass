#ifndef _SM_NET_EMAIL_POP3CONN
#define _SM_NET_EMAIL_POP3CONN
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
		class POP3Conn
		{
		public:
			enum ConnType
			{
				CT_PLAIN,
				CT_STARTTLS,
				CT_SSL
			};

			enum ResultStatus
			{
				Success,
				Failed,
				TimedOut
			};
		private:
			NN<Net::TCPClient> cli;
			Text::UTF8Writer *writer;
			Bool threadToStop;
			Bool threadRunning;
			Bool threadStarted;
			Bool statusChg;
			ResultStatus lastStatus;
			UnsafeArrayOpt<UTF8Char> msgRet;
			Text::StringBuilderUTF8 *msgData;
			Bool msgToDataMode;
			Sync::Event evt;
			IO::Writer *logWriter;
			ResultStatus welcomeMsg;
			UOSInt maxSize;
			Bool authPlain;
			Bool authLogin;

			static UInt32 __stdcall RecvThread(AnyType userObj);
			ResultStatus WaitForResult(OptOut<UnsafeArrayOpt<UTF8Char>> msgRetEnd);
		public:
			POP3Conn(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, ConnType connType, IO::Writer *logWriter, Data::Duration timeout);
			~POP3Conn();

			Bool IsError();

			ResultStatus SendUser(Text::CStringNN username);
			ResultStatus SendPass(Text::CStringNN password);
			ResultStatus SendNoop();
			ResultStatus SendStat(UOSInt *msgCount, UOSInt *msgSize);
			ResultStatus SendRetr(UOSInt msgIndex, NN<Text::StringBuilderUTF8> msgBuff);
			ResultStatus SendDele(UOSInt msgIndex);
			ResultStatus SendQuit();

			static UInt16 GetDefaultPort(ConnType connType);
		};
	}
}
#endif
