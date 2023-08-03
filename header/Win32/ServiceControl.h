#ifndef _SM_WIN32_SERVICECONTROL
#define _SM_WIN32_SERVICECONTROL
#include "Core/Core.h"
#include "Sync/Event.h"

namespace Win32
{
	struct ServiceControl : public Core::IProgControl
	{
		Sync::Event *evt;
		Bool exited;
		Bool toRestart;
		UTF8Char *argv;
	};

	void ServiceControl_Create(NotNullPtr<Core::IProgControl> progCtrl);
	void ServiceControl_Destroy(NotNullPtr<Core::IProgControl> progCtrl);
		
	void ServiceControl_SignalExit(NotNullPtr<Core::IProgControl> progCtrl);
	void ServiceControl_SignalRestart(NotNullPtr<Core::IProgControl> progCtrl);
};
#endif
