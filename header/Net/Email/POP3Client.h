#ifndef _SM_NET_EMAIL_POP3CLIENT
#define _SM_NET_EMAIL_POP3CLIENT
#include "IO/Writer.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/POP3Conn.h"

namespace Net
{
	namespace Email
	{
		class POP3Client
		{
		private:
			Net::Email::POP3Conn conn;
			Bool loggedIn;
			UOSInt msgCnt;
			UOSInt msgSize;

		public:
			POP3Client(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::POP3Conn::ConnType connType, Optional<IO::Writer> logWriter, Text::CStringNN username, Text::CStringNN password, Data::Duration timeout);
			~POP3Client();

			Bool IsError();
			UOSInt GetMessageCount();
			UOSInt GetMessageSize();
			Bool ReadMessageAsString(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			Bool DeleteMessage(UOSInt index);
		};
	}
}
#endif
