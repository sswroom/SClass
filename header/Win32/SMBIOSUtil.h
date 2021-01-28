#ifndef _SM_WIN32_SMBIOSUTIL
#define _SM_WIN32_SMBIOSUTIL
#include "Win32/SMBIOS.h"

namespace Win32
{
	class SMBIOSUtil
	{
	public:
		static SMBIOS *GetSMBIOS();
	};
}
#endif
