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

			virtual void SetValuePtr(const Char *name, void *val) = 0;
			virtual void SetValueDbl(const Char *name, Double val) = 0;
			virtual void SetValueInt64(const Char *name, Int64 val) = 0;
			virtual void SetValueInt32(const Char *name, Int32 val) = 0;

			virtual void *GetValuePtr(const Char *name) = 0;
			virtual Double GetValueDbl(const Char *name) = 0;
			virtual Int64 GetValueInt64(const Char *name) = 0;
			virtual Int32 GetValueInt32(const Char *name) = 0;
		};
	};
};
#endif
