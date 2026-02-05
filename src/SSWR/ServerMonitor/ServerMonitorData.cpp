#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorData.h"

Text::CStringNN SSWR::ServerMonitor::ServerTypeGetName(ServerType serverType)
{
	switch (serverType)
	{
	case ServerType::URL:
		return CSTR("URL");
	case ServerType::TCP:
		return CSTR("TCP");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN SSWR::ServerMonitor::AlertTypeGetName(AlertType alertType)
{
	switch (alertType)
	{
	case AlertType::Email:
		return CSTR("Email");
	case AlertType::WebPush:
		return CSTR("WebPush");
	default:
		return CSTR("Unknown");
	}
}