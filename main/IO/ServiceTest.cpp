#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ServiceManager.h"

#include <stdio.h>

void PrintSvcDetail(IO::ServiceManager *svcMgr, Text::CString svcName)
{
	IO::ServiceManager::ServiceDetail svc;
	if (svcMgr->ServiceGetDetail(svcName, &svc))
	{
		Data::DateTime dt;
		UTF8Char sbuff[64];
		printf("Status = %s\r\n", IO::ServiceInfo::RunStatusGetName(svc.status).v);
		printf("PID = %d\r\n", svc.procId);
		printf("Memory = %lld\r\n", svc.memoryUsage);
		dt.SetTicks(svc.startTimeTicks);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		printf("Start Time = %s\r\n", sbuff);
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString svcName = CSTR("mariadb");
	IO::ServiceManager svcMgr;
	Data::ArrayList<IO::ServiceManager::ServiceItem*> svcList;
	svcMgr.QueryServiceList(&svcList);
	svcMgr.FreeServiceList(&svcList);

	PrintSvcDetail(&svcMgr, svcName);
	svcMgr.ServiceStop(svcName);
	PrintSvcDetail(&svcMgr, svcName);
	svcMgr.ServiceStart(svcName);
	return 0;
}