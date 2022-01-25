#ifndef _SM_MANAGE_OSINFO
#define _SM_MANAGE_OSINFO
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
		static const UTF8Char *GetName(OSType osType);
		static const UTF8Char *GetDefName(OSType osType);
		static void GetCommonName(Text::StringBuilderUTF8 *sb, OSType osType, const UTF8Char *osVer);
	};
}
#endif
