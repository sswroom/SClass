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
			POP3Client(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Net::Email::POP3Conn::ConnType connType, IO::Writer *logWriter, Text::CString username, Text::CString password);
			~POP3Client();

			Bool IsError();
			UOSInt GetMessageCount();
			UOSInt GetMessageSize();
			Bool ReadMessageAsString(UOSInt index, Text::StringBuilderUTF8 *sb);
			Bool DeleteMessage(UOSInt index);
		};
	}
}
#endif