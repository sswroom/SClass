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
			Text::String *host;
			UInt16 port;
			IO::Writer *logWriter;
			const UTF8Char *authUser;
			const UTF8Char *authPassword;

		public:
			SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter);
			~SMTPClient();

			void SetPlainAuth(const UTF8Char *userName, const UTF8Char *password);
			Bool Send(Net::Email::EmailMessage *message);
		};
	}
}
#endif
