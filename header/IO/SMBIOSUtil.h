#ifndef _SM_IO_SMBIOSUTIL
#define _SM_IO_SMBIOSUTIL
#include "IO/SMBIOS.h"

namespace IO
{
	class SMBIOSUtil
	{
	public:
		static Optional<SMBIOS> GetSMBIOS();
	};
}
#endif
