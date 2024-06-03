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
			UnsafeArrayOpt<const UTF8Char> browserVer;
			UOSInt browserVerLen;
			Manage::OSInfo::OSType os;
			UnsafeArrayOpt<const UTF8Char> osVer;
			UOSInt osVerLen;
			UnsafeArrayOpt<const UTF8Char> devName;
			UOSInt devNameLen;
			UnsafeArray<const UTF8Char> userAgent;
			UOSInt userAgentLen;
		} UAEntry;
		
	private:
		static UAEntry uaList[];
	public:
		static Optional<const UAEntry> GetUserAgentInfo(UnsafeArray<const UTF8Char> userAgent);
		static UAEntry *GetUAEntryList(UOSInt *cnt);
		static void ParseUserAgent(NN<UAEntry> ent, Text::CStringNN userAgent);
		static Text::CStringNN FindUserAgent(Manage::OSInfo::OSType os, Net::BrowserInfo::BrowserType browser);
	};
}
#endif
