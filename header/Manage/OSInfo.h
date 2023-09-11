#ifndef _SM_MANAGE_OSINFO
#define _SM_MANAGE_OSINFO
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class OSInfo
	{
	public:
		typedef enum
		{
			OT_UNKNOWN,
			OT_WINDOWS_NT,
			OT_WINDOWS_CE,
			OT_WINDOWS_SVR,
			OT_LINUX_X86_64,
			OT_ANDROID,
			OT_IPAD,
			OT_IPHONE,
			OT_DARWIN,
			OT_MACOS,
			OT_WINDOWS_NT64,
			OT_CHROMEOS,
			OT_LINUX_I686,
			OT_NETCAST
		} OSType;

	public:
		static Text::CStringNN GetName(OSType osType);
		static Text::CStringNN GetDefName(OSType osType);
		static void GetCommonName(NotNullPtr<Text::StringBuilderUTF8> sb, OSType osType, Text::CString osVer);
	};
}
#endif
