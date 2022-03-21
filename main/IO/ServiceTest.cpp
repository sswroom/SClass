#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "IO/ServiceManager.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ServiceManager svcMgr;
	Data::ArrayList<IO::ServiceManager::ServiceItem*> svcList;
	svcMgr.QueryServiceList(&svcList);
	svcMgr.FreeServiceList(&svcList);

	IO::ServiceManager::ServiceDetail svc;
	if (svcMgr.GetServiceDetail(CSTR("mariadb"), &svc))
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
	return 0;
}