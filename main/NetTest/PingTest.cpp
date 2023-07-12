#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Net/OSSocketFactory.h"
#include "Sync/SimpleThread.h"
#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Net::SocketFactory *sockf;
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	UInt32 respTime;
	UInt32 ttl;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1"), &addr);
	while (true)
	{
		if (sockf->IcmpSendEcho2(&addr, &respTime, &ttl))
		{
//			printf("Received reply: %d, %d\r\n", respTime, ttl);
		}
		else
		{
//			printf("Error in receiving reply\r\n");
		}
		Sync::SimpleThread::Sleep(60000);
	}
	DEL_CLASS(sockf);
	return 0;
}
