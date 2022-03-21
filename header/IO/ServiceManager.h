#ifndef _SM_IO_SERVICEMANAGER
#define _SM_IO_SERVICEMANAGER
#include "Data/ArrayList.h"
#include "IO/ServiceInfo.h"
#include "Text/String.h"

namespace IO
{
	class ServiceManager
	{
	public:
		struct ServiceItem
		{
			Text::String *name;
			IO::ServiceInfo::ServiceState state;
		};

		struct ServiceDetail
		{
			IO::ServiceInfo::RunStatus status;
			UInt32 procId;
			Int64 startTimeTicks;
			UInt64 memoryUsage;
		};
	private:
		struct ClassData;
		ClassData *clsData;

	public:
		ServiceManager();
		~ServiceManager();

		Bool CreateService(Text::CString svcName, IO::ServiceInfo::ServiceState stype);
		Bool DeleteService(Text::CString svcName);
		Bool SetServiceDesc(Text::CString svcName, Text::CString svcDesc);

		UOSInt QueryServiceList(Data::ArrayList<ServiceItem*> *svcList);
		void FreeServiceList(Data::ArrayList<ServiceItem*> *svcList);
		Bool GetServiceDetail(Text::CString svcName, ServiceDetail *svcDetail);
	};
}
#endif
