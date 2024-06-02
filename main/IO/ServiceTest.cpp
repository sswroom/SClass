#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ServiceManager.h"

#include <stdio.h>

void PrintSvcDetail(IO::ServiceManager *svcMgr, Text::CStringNN svcName)
{
	IO::ServiceManager::ServiceDetail svc;
	if (svcMgr->ServiceGetDetail(svcName, &svc))
	{
		Data::DateTime dt;
		UTF8Char sbuff[64];
		printf("Status = %s\r\n", IO::ServiceInfo::RunStatusGetName(svc.status).v.Ptr());
		printf("PID = %d\r\n", svc.procId);
		printf("Memory = %lld\r\n", svc.memoryUsage);
		svc.startTime.ToLocalTime().ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		printf("Start Time = %s\r\n", sbuff);
	}
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CStringNN svcName = CSTR("mariadb");
	IO::ServiceManager svcMgr;
	Data::ArrayListNN<IO::ServiceManager::ServiceItem> svcList;
	svcMgr.QueryServiceList(svcList);
	svcMgr.FreeServiceList(svcList);

	PrintSvcDetail(&svcMgr, svcName);
	svcMgr.ServiceStop(svcName);
	PrintSvcDetail(&svcMgr, svcName);
	svcMgr.ServiceStart(svcName);
	return 0;
}