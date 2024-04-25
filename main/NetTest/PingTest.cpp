#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Net/OSSocketFactory.h"
#include "Sync/SimpleThread.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Net::OSSocketFactory sockf(true);
	UInt32 respTime;
	UInt32 ttl;
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfo(addr, CSTR("127.0.0.1"));
	while (true)
	{
		if (sockf.IcmpSendEcho2(addr, &respTime, &ttl))
		{
//			printf("Received reply: %d, %d\r\n", respTime, ttl);
		}
		else
		{
//			printf("Error in receiving reply\r\n");
		}
		Sync::SimpleThread::Sleep(60000);
	}
	return 0;
}
