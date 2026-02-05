#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORSMTPALERTER
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORSMTPALERTER
#include "Net/SSLEngine.h"
#include "Net/Email/SMTPClient.h"
#include "SSWR/ServerMonitor/ServerMonitorAlerter.h"
#include "Text/StringBuilderUTF8.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorSMTPAlerter : public ServerMonitorAlerter
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Net::Email::SMTPClient> cli;
			NN<Text::String> fromEmail;
			NN<Text::String> toEmails;

		public:
			ServerMonitorSMTPAlerter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log, Text::CStringNN setting, Text::CStringNN target);
			ServerMonitorSMTPAlerter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log, Text::CStringNN smtpHost, UInt16 smtpPort, Net::Email::SMTPConn::ConnType connType, Text::CStringNN smtpUser, Text::CStringNN smtpPassword, Text::CStringNN fromEmail, Text::CStringNN toEmails);
			virtual ~ServerMonitorSMTPAlerter();

			virtual Bool HasError() const;
			virtual Bool Send(Text::CStringNN serverName);
			virtual Bool BuildSetting(NN<Text::StringBuilderUTF8> setting);
			virtual Bool BuildTarget(NN<Text::StringBuilderUTF8> target);
		};
	}
}
#endif