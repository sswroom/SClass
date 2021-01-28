#ifndef _SM_MEDIA_MONITORMGR
#define _SM_MEDIA_MONITORMGR
#include "Data/StringUTF8Map.h"
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
		Data::StringUTF8Map<MonitorSetting*> *monMap;

	public:
		MonitorMgr();
		~MonitorMgr();

		Double GetMonitorHDPI(void *hMonitor);
		void SetMonitorHDPI(void *hMonitor, Double monitorHDPI);
		Double GetMonitorDDPI(void *hMonitor);
		void SetMonitorDDPI(void *hMonitor, Double monitorDDPI);
	};
};
#endif
