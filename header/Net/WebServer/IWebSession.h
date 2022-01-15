#ifndef _SM_NET_WEBSERVER_IWEBSESSION
#define _SM_NET_WEBSERVER_IWEBSESSION
#include "Net/WebServer/IWebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class IWebSession
		{
		public:
			virtual Bool RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os) = 0;
			virtual void EndUse() = 0;
			virtual Int64 GetSessId() = 0;

			virtual void SetValuePtr(const UTF8Char *name, UOSInt nameLen, void *val) = 0;
			virtual void SetValueDbl(const UTF8Char *name, UOSInt nameLen, Double val) = 0;
			virtual void SetValueInt64(const UTF8Char *name, UOSInt nameLen, Int64 val) = 0;
			virtual void SetValueInt32(const UTF8Char *name, UOSInt nameLen, Int32 val) = 0;

			virtual void *GetValuePtr(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual Double GetValueDbl(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual Int64 GetValueInt64(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual Int32 GetValueInt32(const UTF8Char *name, UOSInt nameLen) = 0;
		};
	}
}
#endif
