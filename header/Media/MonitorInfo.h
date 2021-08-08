#ifndef _SM_MEDIA_MONITORINFO
#define _SM_MEDIA_MONITORINFO
#include "Handles.h"

namespace Media
{
	class MonitorInfo
	{
	private:
		Int32 left;
		Int32 top;
		Int32 right;
		Int32 bottom;
		Bool isPrimary;
		const UTF8Char *name;
		const UTF8Char *desc;
		const UTF8Char *monId;

	public:
		MonitorInfo(MonitorHandle *hMonitor);
		~MonitorInfo();

		const UTF8Char *GetName();
		const UTF8Char *GetDesc();
		const UTF8Char *GetMonitorID();
		Int32 GetLeft();
		Int32 GetTop();
		Int32 GetPixelWidth();
		Int32 GetPixelHeight();
		Bool IsPrimary();
	};
}
#endif
