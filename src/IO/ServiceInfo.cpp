#include "Stdafx.h"
#include "IO/ServiceInfo.h"

Text::CStringNN IO::ServiceInfo::ServiceStateGetName(ServiceState state)
{
	switch (state)
	{
	case ServiceState::Active:
		return CSTR("Active");
	case ServiceState::ManualStart:
		return CSTR("ManualStart");
	case ServiceState::Inactive:
		return CSTR("Inactive");
	case ServiceState::Static:
		return CSTR("Static");
	case ServiceState::Alias:
		return CSTR("Alias");
	case ServiceState::Generated:
		return CSTR("Generated");
	case ServiceState::Indirect:
		return CSTR("Indirect");
	case ServiceState::EnabledRuntime:
		return CSTR("EnabledRuntime");
	case ServiceState::Masked:
		return CSTR("Masked");
	case ServiceState::Transient:
		return CSTR("Transient");
	case ServiceState::Unknown:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::ServiceInfo::RunStatusGetName(RunStatus status)
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
