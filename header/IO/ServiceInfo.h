#ifndef _SM_IO_SERVICEINFO
#define _SM_IO_SERVICEINFO
#include "Text/CString.h"

namespace IO
{
	class ServiceInfo
	{
	public:
		enum class ServiceState
		{
			Unknown,
			Active,
			ManualStart,
			Inactive,
			Static,
			Alias,
			Generated,
			Indirect,
			EnabledRuntime,
			Masked,
			Transient
		};

		enum class RunStatus
		{
			Unknown,
			Running,
			Stopped,
			Starting,
			Stopping
		};

		static Text::CStringNN ServiceStateGetName(ServiceState state);
		static Text::CStringNN RunStatusGetName(RunStatus status);
	};
}
#endif
