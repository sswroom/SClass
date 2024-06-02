#ifndef _SM_IO_SERVICEMANAGER
#define _SM_IO_SERVICEMANAGER
#include "Data/ArrayListNN.h"
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
			NN<Text::String> name;
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

		class ServiceComparator : public Data::Comparator<NN<ServiceItem>>
		{
			virtual OSInt Compare(NN<ServiceItem> a, NN<ServiceItem> b) const;
		};
	private:
		struct ClassData;
		ClassData *clsData;

	public:
		ServiceManager();
		~ServiceManager();

		Bool ServiceCreate(Text::CStringNN svcName, Text::CString svcDesc, Text::CStringNN cmdLine, IO::ServiceInfo::ServiceState stype);
		Bool ServiceDelete(Text::CStringNN svcName);
		Bool ServiceSetDesc(Text::CStringNN svcName, Text::CStringNN svcDesc);
		Bool ServiceStart(Text::CStringNN svcName);
		Bool ServiceStop(Text::CStringNN svcName);
		Bool ServiceEnable(Text::CStringNN svcName);
		Bool ServiceDisable(Text::CStringNN svcName);
		Bool ServiceGetDetail(Text::CStringNN svcName, ServiceDetail *svcDetail);

		UOSInt QueryServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList);
		void FreeServiceList(NN<Data::ArrayListNN<ServiceItem>> svcList);
	};
}
#endif
