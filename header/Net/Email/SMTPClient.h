#ifndef _SM_NET_EMAIL_SMTPCLIENT
#define _SM_NET_EMAIL_SMTPCLIENT
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/SMTPConn.h"

namespace Net
{
	namespace Email
	{
		class SMTPClient
		{
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Net::Email::SMTPConn::ConnType connType;
			NotNullPtr<Text::String> host;
			UInt16 port;
			Data::Duration timeout;
			IO::Writer *logWriter;
			Text::String *authUser;
			Text::String *authPassword;

		public:
			SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter, Data::Duration timeout);
			~SMTPClient();

			void SetPlainAuth(Text::CString userName, Text::CString password);
			Bool Send(Net::Email::EmailMessage *message);
		};
	}
}
#endif
