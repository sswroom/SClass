#ifndef _SM_MEDIA_MONITORMGR
#define _SM_MEDIA_MONITORMGR
#include "Handles.h"
#include "Data/FastStringMap.h"
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
		Sync::Mutex *monMut;
		Data::FastStringMap<MonitorSetting*> *monMap;

	public:
		MonitorMgr();
		~MonitorMgr();

		Double GetMonitorHDPI(MonitorHandle *hMonitor);
		void SetMonitorHDPI(MonitorHandle *hMonitor, Double monitorHDPI);
		Double GetMonitorDDPI(MonitorHandle *hMonitor);
		void SetMonitorDDPI(MonitorHandle *hMonitor, Double monitorDDPI);
	};
}
#endif
