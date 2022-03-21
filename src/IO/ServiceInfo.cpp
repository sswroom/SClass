#include "Stdafx.h"
#include "IO/ServiceInfo.h"

Text::CString IO::ServiceInfo::RunStatusGetName(RunStatus status)
{
	switch (status)
	{
	case RunStatus::Running:
		return CSTR("Running");
	case RunStatus::Starting:
		return CSTR("Starting");
	case RunStatus::Stopping:
		return CSTR("Stopping");
	case RunStatus::Stopped:
		return CSTR("Stopped");
	case RunStatus::Unknown:
	default:
		return CSTR("Unknown");
	}
}
