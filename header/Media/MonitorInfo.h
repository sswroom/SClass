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
		NN<Text::String> name;
		Optional<Text::String> desc;
		Optional<Text::String> monId;

	public:
		MonitorInfo(MonitorHandle *hMonitor);
		~MonitorInfo();

		NN<Text::String> GetName() const;
		Optional<Text::String> GetDesc() const;
		Optional<Text::String> GetMonitorID() const;
		Int32 GetLeft() const;
		Int32 GetTop() const;
		Int32 GetPixelWidth() const;
		Int32 GetPixelHeight() const;
		Bool IsPrimary();
	};
}
#endif
