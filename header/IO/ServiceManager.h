#ifndef _SM_IO_SERVICEMANAGER
#define _SM_IO_SERVICEMANAGER
#include "Data/ArrayList.h"
#include "Data/Comparator.h"
#include "Data/Timestamp.h"
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
			IO::ServiceInfo::RunStatus runStatus;
		};

		struct ServiceDetail
		{
			IO::ServiceInfo::RunStatus status;
			UInt32 procId;
			Data::Timestamp startTime;
			UInt64 memoryUsage;
			IO::ServiceInfo::ServiceState enabled;
		};

		class ServiceComparator : public Data::Comparator<ServiceItem*>
		{
			virtual OSInt Compare(ServiceItem* a, ServiceItem* b) const;
		};
	private:
		struct ClassData;
		ClassData *clsData;

	public:
		ServiceManager();
		~ServiceManager();

		Bool ServiceCreate(Text::CString svcName, Text::CString svcDesc, Text::CString cmdLine, IO::ServiceInfo::ServiceState stype);
		Bool ServiceDelete(Text::CString svcName);
		Bool ServiceSetDesc(Text::CString svcName, Text::CString svcDesc);
		Bool ServiceStart(Text::CString svcName);
		Bool ServiceStop(Text::CString svcName);
		Bool ServiceEnable(Text::CString svcName);
		Bool ServiceDisable(Text::CString svcName);
		Bool ServiceGetDetail(Text::CString svcName, ServiceDetail *svcDetail);

		UOSInt QueryServiceList(Data::ArrayList<ServiceItem*> *svcList);
		void FreeServiceList(Data::ArrayList<ServiceItem*> *svcList);
	};
}
#endif
