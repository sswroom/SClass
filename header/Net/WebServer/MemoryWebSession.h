#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSION
#define _SM_NET_WEBSERVER_MEMORYWEBSESSION
#include "Data/FastStringMap.h"
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
			Data::FastStringMap<Int64> items;
			Sync::Mutex mut;
			Int64 sessId;

		public:
			MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			virtual ~MemoryWebSession();

			virtual Bool RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os);
			void BeginUse();
			virtual void EndUse();
			virtual Int64 GetSessId();

			virtual void SetValuePtr(Text::CStringNN name, void *val);
			virtual void SetValueDbl(Text::CStringNN name, Double val);
			virtual void SetValueInt64(Text::CStringNN name, Int64 val);
			virtual void SetValueInt32(Text::CStringNN name, Int32 val);

			virtual void *GetValuePtr(Text::CStringNN name);
			virtual Double GetValueDbl(Text::CStringNN name);
			virtual Int64 GetValueInt64(Text::CStringNN name);
			virtual Int32 GetValueInt32(Text::CStringNN name);
		};
	}
}
#endif
