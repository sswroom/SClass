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
			const UTF8Char *host;
			UInt16 port;
			IO::Writer *logWriter;

		public:
			SMTPClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter);
			~SMTPClient();

			Bool Send(Net::Email::EmailMessage *message);
		};
	}
}
#endif
