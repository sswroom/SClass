#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/SocketMonitor.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

void __stdcall Net::SocketMonitor::DataThread(NotNullPtr<Sync::Thread> thread)
{
	NotNullPtr<Net::SocketMonitor> me = thread->GetUserObj().GetNN<Net::SocketMonitor>();
	{
		UInt8 *buff = MemAlloc(UInt8, 65536);
		while (!thread->IsStopping())
		{
			UOSInt recvSize;
			Net::SocketUtil::AddressInfo addr;
			UInt16 port;
			Net::SocketFactory::ErrorType et;
			recvSize = me->sockf->UDPReceive(me->soc, buff, 65536, addr, port, et);
			Data::DateTime logTime;
			logTime.SetCurrTimeUTC();
			if (recvSize > 0)
			{
				if (me->hdlr.func)
				{
					me->hdlr.func(me->hdlr.userObj, buff, recvSize);
				}
			}
		}
		MemFree(buff);
	}
}

Net::SocketMonitor::SocketMonitor(NotNullPtr<Net::SocketFactory> sockf, Socket *soc, RAWDataHdlr hdlr, AnyType userData, UOSInt threadCnt)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->threadCnt = threadCnt;
	this->threads = 0;
	UOSInt i;

	this->sockf = sockf;
	this->hdlr = {hdlr, userData};

	this->soc = soc;
	if (this->soc)
	{
		this->threads = MemAlloc(Sync::Thread*, this->threadCnt);
		i = this->threadCnt;
		while (i-- > 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("SocketMonitor")), i);
			NEW_CLASS(this->threads[i], Sync::Thread(DataThread, this, CSTRP(sbuff, sptr)));
			this->threads[i]->Start();
		}
	}
}

Net::SocketMonitor::~SocketMonitor()
{
	UOSInt i;
	if (this->threads)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threads[i]->BeginStop();
		}
	}
	if (this->soc)
	{
		this->sockf->ShutdownSocket(this->soc);
		this->sockf->DestroySocket(this->soc);
	}
	if (this->threads)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threads[i]->WaitForEnd();
			DEL_CLASS(this->threads[i]);
		}
		MemFree(this->threads);
	}
	if (this->soc)
	{
		this->soc = 0;
	}
}
