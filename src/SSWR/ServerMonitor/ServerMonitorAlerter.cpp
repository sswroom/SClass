#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorAlerter.h"
#include "SSWR/ServerMonitor/ServerMonitorSMTPAlerter.h"

Optional<SSWR::ServerMonitor::ServerMonitorAlerter> SSWR::ServerMonitor::ServerMonitorAlerter::CreateAlerter(AlertType alertType, Text::CStringNN setting, Text::CStringNN target, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log)
{
	switch (alertType)
	{
	case AlertType::Email:
		{
			NN<ServerMonitorSMTPAlerter> alerter;
			NEW_CLASSNN(alerter, ServerMonitorSMTPAlerter(clif, ssl, log, setting, target));
			if (alerter->HasError())
			{
				alerter.Delete();
				return nullptr;
			}
			else
			{
				return alerter;
			}
		}
	case AlertType::WebPush:
	default:
		log->LogMessage(CSTR("Unsupported alert type"), IO::LogHandler::LogLevel::Error);
		return nullptr;
	}
}