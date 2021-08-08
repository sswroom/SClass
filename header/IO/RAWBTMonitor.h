#ifndef _SM_IO_RAWBTMONITOR
#define _SM_IO_RAWBTMONITOR
#include "IO/RAWMonitor.h"

namespace IO
{
	class RAWBTMonitor : public IO::RAWMonitor
	{
	private:
		Int32 fd;
	public:
		RAWBTMonitor(UOSInt devNum);
		virtual ~RAWBTMonitor();

		virtual LinkType GetLinkType();
		virtual Bool IsError();
		virtual void Close();
		virtual UOSInt GetMTU();
		virtual UOSInt NextPacket(UInt8 *buff, Int64 *timeTicks);

		static UOSInt GetDevCount();
	};
}
#endif
