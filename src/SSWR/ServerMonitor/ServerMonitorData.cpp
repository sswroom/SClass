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
