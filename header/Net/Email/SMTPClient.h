#ifndef _SM_NET_EMAIL_SMTPCLIENT
#define _SM_NET_EMAIL_SMTPCLIENT
#include "IO/LogTool.h"
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
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Net::Email::SMTPConn::ConnType connType;
			NN<Text::String> host;
			UInt16 port;
			Data::Duration timeout;
			Optional<IO::Writer> logWriter;
			Bool logRelease;
			Optional<Text::String> authUser;
			Optional<Text::String> authPassword;

		public:
			SMTPClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, Optional<IO::Writer> logWriter, Data::Duration timeout);
			SMTPClient(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, Optional<IO::LogTool> log, Data::Duration timeout);
			virtual ~SMTPClient();

			void SetPlainAuth(Text::CString userName, Text::CString password);
			virtual Bool Send(NN<Net::Email::EmailMessage> message);
		};
	}
}
#endif
