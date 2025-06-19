#ifndef _SM_NET_WEBSERVER_WEBSESSION
#define _SM_NET_WEBSERVER_WEBSESSION
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"

namespace Net
{
	namespace WebServer
	{
		class WebSession
		{
		public:
			virtual Bool RequestValid(Net::BrowserInfo::BrowserType browser, Manage::OSInfo::OSType os, Text::CStringNN origin) = 0;
			virtual void EndUse() = 0;
			virtual Int64 GetSessId() const = 0;
			virtual NN<Text::String> GetOrigin() const = 0;

			virtual void SetValuePtr(Text::CStringNN name, void *val) = 0;
			virtual void SetValueDbl(Text::CStringNN name, Double val) = 0;
			virtual void SetValueInt64(Text::CStringNN name, Int64 val) = 0;
			virtual void SetValueInt32(Text::CStringNN name, Int32 val) = 0;

			virtual void *GetValuePtr(Text::CStringNN name) = 0;
			virtual Double GetValueDbl(Text::CStringNN name) = 0;
			virtual Int64 GetValueInt64(Text::CStringNN name) = 0;
			virtual Int32 GetValueInt32(Text::CStringNN name) = 0;
		};
	}
}
#endif
