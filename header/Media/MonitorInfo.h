#ifndef _SM_MEDIA_MONITORINFO
#define _SM_MEDIA_MONITORINFO
#include "Handles.h"
#include "Text/String.h"

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
		Text::String *name;
		Text::String *desc;
		Text::String *monId;

	public:
		MonitorInfo(MonitorHandle *hMonitor);
		~MonitorInfo();

		Text::String *GetName();
		Text::String *GetDesc();
		Text::String *GetMonitorID();
		Int32 GetLeft();
		Int32 GetTop();
		Int32 GetPixelWidth();
		Int32 GetPixelHeight();
		Bool IsPrimary();
	};
}
#endif
