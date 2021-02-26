#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSION
#define _SM_NET_WEBSERVER_MEMORYWEBSESSION
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListStrUTF8.h"
#include "Net/WebServer/IWebSession.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class MemoryWebSession : public Net::WebServer::IWebSession
		{
		private:
			Net::BrowserInfo::BrowserType browser;
			Manage::OSInfo::OSType os;
			Data::ArrayListStrUTF8 *names;
			Data::ArrayListInt64 *vals;
			Sync::Mutex *mut;
			Int64 sessId;

		public:
			MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			virtual ~MemoryWebSession();

			virtual Bool RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			void BeginUse();
			virtual void EndUse();
			virtual Int64 GetSessId();

			virtual void SetValuePtr(const Char *name, void *val);
			virtual void SetValueDbl(const Char *name, Double val);
			virtual void SetValueInt64(const Char *name, Int64 val);
			virtual void SetValueInt32(const Char *name, Int32 val);

			virtual void *GetValuePtr(const Char *name);
			virtual Double GetValueDbl(const Char *name);
			virtual Int64 GetValueInt64(const Char *name);
			virtual Int32 GetValueInt32(const Char *name);
		};
	}
}
#endif
