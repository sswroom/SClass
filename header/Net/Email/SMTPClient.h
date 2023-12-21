#ifndef _SM_NET_EMAIL_SMTPCLIENT
#define _SM_NET_EMAIL_SMTPCLIENT
#include "IO/Writer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/Email/EmailMessage.h"
#include "Net/Email/EmailSender.h"
#include "Net/Email/SMTPConn.h"

namespace Net
{
	namespace Email
	{
		class SMTPClient : public EmailSender
		{
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Net::Email::SMTPConn::ConnType connType;
			NotNullPtr<Text::String> host;
			UInt16 port;
			Data::Duration timeout;
			IO::Writer *logWriter;
			Optional<Text::String> authUser;
			Optional<Text::String> authPassword;

		public:
			SMTPClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::Writer *logWriter, Data::Duration timeout);
			virtual ~SMTPClient();

			void SetPlainAuth(Text::CString userName, Text::CString password);
			virtual Bool Send(NotNullPtr<Net::Email::EmailMessage> message);
		};
	}
}
#endif
