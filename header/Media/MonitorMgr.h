#ifndef _SM_MEDIA_MONITORMGR
#define _SM_MEDIA_MONITORMGR
#include "Handles.h"
#include "Data/FastStringMapNN.h"
#include "Sync/Mutex.h"

namespace Media
{
	class MonitorMgr
	{
	private:
		typedef struct
		{
			Double hdpi;
			Double ddpi;
		} MonitorSetting;
	private:
		Sync::Mutex monMut;
		Data::FastStringMapNN<MonitorSetting> monMap;

	public:
		MonitorMgr();
		~MonitorMgr();

		Double GetMonitorHDPI(Optional<MonitorHandle> hMonitor);
		void SetMonitorHDPI(Optional<MonitorHandle> hMonitor, Double monitorHDPI);
		Double GetMonitorDDPI(Optional<MonitorHandle> hMonitor);
		void SetMonitorDDPI(Optional<MonitorHandle> hMonitor, Double monitorDDPI);
	};
}
#endif
