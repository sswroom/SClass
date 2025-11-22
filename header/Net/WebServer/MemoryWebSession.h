#ifndef _SM_NET_WEBSERVER_MEMORYWEBSESSION
#define _SM_NET_WEBSERVER_MEMORYWEBSESSION
#include "Data/FastStringMap.hpp"
#include "Net/WebServer/WebSession.h"
#include "Sync/Mutex.h"

namespace Net
{
	namespace WebServer
	{
		class MemoryWebSession : public Net::WebServer::WebSession
		{
		private:
			Net::BrowserInfo::BrowserType browser;
			Manage::OSInfo::OSType os;
			Data::FastStringMap<Int64> items;
			NN<Text::String> origin;
			Sync::Mutex mut;
			Int64 sessId;

		public:
			MemoryWebSession(Int64 sessId, Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os, Text::CStringNN origin);
			virtual ~MemoryWebSession();

			virtual Bool RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os, Text::CStringNN origin);
			void BeginUse();
			virtual void EndUse();
			virtual Int64 GetSessId() const;
			virtual NN<Text::String> GetOrigin() const;

			virtual void SetValuePtr(Text::CStringNN name, AnyType val);
			virtual void SetValueDbl(Text::CStringNN name, Double val);
			virtual void SetValueInt64(Text::CStringNN name, Int64 val);
			virtual void SetValueInt32(Text::CStringNN name, Int32 val);

			virtual AnyType GetValuePtr(Text::CStringNN name);
			virtual Double GetValueDbl(Text::CStringNN name);
			virtual Int64 GetValueInt64(Text::CStringNN name);
			virtual Int32 GetValueInt32(Text::CStringNN name);
		};
	}
}
#endif
