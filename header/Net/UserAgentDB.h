#ifndef _SM_NET_USERAGENTDB
#define _SM_NET_USERAGENTDB
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"
#include "Text/CString.h"
namespace Net
{
	class UserAgentDB
	{
	public:
		typedef struct
		{
			Net::BrowserInfo::BrowserType browser;
			const Char *browserVer;
			Manage::OSInfo::OSType os;
			const Char *osVer;
			const Char *devName;
			const Char *userAgent;
		} UAEntry;
		
	private:
		static UAEntry uaList[];
	public:
		static const UAEntry *GetUserAgentInfo(const UTF8Char *userAgent);
		static UAEntry *GetUAEntryList(UOSInt *cnt);
		static void ParseUserAgent(UAEntry *ent, Text::CString userAgent);
		static const UTF8Char *FindUserAgent(Manage::OSInfo::OSType os, Net::BrowserInfo::BrowserType browser);
	};
}
#endif
