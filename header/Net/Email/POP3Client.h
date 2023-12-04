#ifndef _SM_NET_EMAIL_POP3CLIENT
#define _SM_NET_EMAIL_POP3CLIENT
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
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
			POP3Client(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::POP3Conn::ConnType connType, IO::Writer *logWriter, Text::CString username, Text::CString password, Data::Duration timeout);
			~POP3Client();

			Bool IsError();
			UOSInt GetMessageCount();
			UOSInt GetMessageSize();
			Bool ReadMessageAsString(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			Bool DeleteMessage(UOSInt index);
		};
	}
}
#endif
