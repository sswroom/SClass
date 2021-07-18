#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Net::SocketFactory *sockf;
Manage::HiResClock *clk;
IO::ConsoleWriter *console;
Bool toStop;
UInt32 procRunning;
Bool recvRunning;
Bool dispRunning;
Sync::Event *mainEvt;
Sync::Event *procEvt;
Sync::Event *recvEvt;
Sync::Event *dispEvt;
Sync::Mutex *cliMut;
Net::TCPClient *cli;
UInt64 totalRecvSize;
UInt64 totalSendSize;
Double lastTime;
UInt64 lastRecvSize;
UInt64 lastSendSize;

UInt32 __stdcall DispThread(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	dispRunning = true;
	while (!toStop)
	{
		dispEvt->Wait(1000);

		Double currTime = clk->GetTimeDiff();
		UInt64 currRecvSize = totalRecvSize;
		UInt64 currSendSize = totalSendSize;
		if (currTime > lastTime)
		{
			sptr = sbuff;
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Send: ");
			sptr = Text::StrDouble(sptr, (Double)(currSendSize - lastSendSize) / (currTime - lastTime));
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Bps\tRecv: ");
			sptr = Text::StrDouble(sptr, (Double)(currRecvSize - lastRecvSize) / (currTime - lastTime));
			sptr = Text::StrConcat(sptr, (const UTF8Char*)"Bps       ");
			console->WriteLine(sbuff);
		}
		lastTime = currTime;
		lastRecvSize = currRecvSize;
		lastSendSize = currSendSize;
	}
	dispRunning = false;
	mainEvt->Set();
	return 0;
}

UInt32 __stdcall ProcThread(void *userObj)
{
	UInt8 *sendBuff;
	UOSInt sendSize;
	UOSInt sendBuffSize = 9000;
	Sync::Interlocked::Increment(&procRunning);
	mainEvt->Set();
	sendBuff = MemAlloc(UInt8, sendBuffSize);
	while (!toStop)
	{
		Sync::MutexUsage mutUsage(cliMut);
		if (cli)
		{
			sendSize = cli->Write(sendBuff, sendBuffSize);
			mutUsage.EndUse();
			if (sendSize > 0)
			{
				Sync::Interlocked::Add(&totalSendSize, sendSize);
			}
			else
			{
				procEvt->Wait(1000);
			}
		}
		else
		{
			mutUsage.EndUse();
			procEvt->Wait(1000);
		}
	}
	MemFree(sendBuff);
	Sync::Interlocked::Decrement(&procRunning);
	mainEvt->Set();
	return 0;
}

UInt32 __stdcall RecvThread(void *userObj)
{
	UInt8 *recvBuff;
	UOSInt recvSize;
	recvRunning = true;
	mainEvt->Set();
	recvBuff = MemAlloc(UInt8, 9000);
	while (!toStop)
	{
		if (cli)
		{
			recvSize = cli->Read(recvBuff, 9000);
			if (recvSize > 0)
			{
				Sync::Interlocked::Add(&totalRecvSize, recvSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(cliMut);
				DEL_CLASS(cli);
				cli = 0;
				mutUsage.EndUse();
				recvEvt->Wait(1000);
			}
		}
		else
		{
			recvEvt->Wait(1000);
		}
	}
	MemFree(recvBuff);
	recvRunning = false;
	mainEvt->Set();
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UOSInt argc;
	UTF8Char **argv;
	NEW_CLASS(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc <= 2)
	{
		console->WriteLine((const UTF8Char*)"Usage: STCPSpdCli [IP] [Port]");
	}
	else
	{
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		totalSendSize = 0;
		totalRecvSize = 0;
		lastTime = 0;
		lastSendSize = 0;
		lastRecvSize = 0;
		cli = 0;
		toStop = false;
		procRunning = 0;
		recvRunning = false;
		NEW_CLASS(cliMut, Sync::Mutex());
		NEW_CLASS(clk, Manage::HiResClock());
		NEW_CLASS(mainEvt, Sync::Event(true, (const UTF8Char*)"mainEvt"));
		NEW_CLASS(recvEvt, Sync::Event(true, (const UTF8Char*)"recvEvt"));
		NEW_CLASS(procEvt, Sync::Event(true, (const UTF8Char*)"procEvt"));
		NEW_CLASS(dispEvt, Sync::Event(true, (const UTF8Char*)"dispEvt"));

		UOSInt threadCnt = Sync::Thread::GetThreadCnt();
		UOSInt i;
		Sync::Thread::Create(DispThread, 0);
		i = 0;
		while (i < threadCnt)
		{
			Sync::Thread::Create(ProcThread, 0);
			i++;
		}
		Sync::Thread::Create(RecvThread, 0);
		while (!recvRunning || procRunning != threadCnt || !dispRunning)
		{
			mainEvt->Wait(100);
		}

		Bool valid = true;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		Net::TCPClient *c;

		if (!sockf->DNSResolveIP(argv[1], &addr))
		{
			console->WriteLine((const UTF8Char*)"Host is not valid");
			valid = false;
		}
		if (!Text::StrToUInt16(argv[2], &port))
		{
			console->WriteLine((const UTF8Char*)"Port is not a number");
			valid = false;
		}
		if (port <= 0 || port > 65535)
		{
			console->WriteLine((const UTF8Char*)"Port is out of range");
			valid = false;
		}

		if (valid)
		{
			NEW_CLASS(c, Net::TCPClient(sockf, &addr, port));
			if (c->IsConnectError())
			{
				DEL_CLASS(c);
				console->WriteLine((const UTF8Char*)"Error in connect to server");
				valid = false;
			}
			else
			{
				c->SetNoDelay(true);
				clk->Start();
				totalRecvSize = 0;
				totalSendSize = 0;
				lastRecvSize = 0;
				lastSendSize = 0;
				lastTime = 0;
				cli = c;
				procEvt->Set();
				recvEvt->Set();
				dispEvt->Set();
			}

			console->WriteLine((const UTF8Char*)"Running");
			progCtrl->WaitForExit(progCtrl);
		}

		if (cli)
		{
			cli->Close();
		}
		toStop = true;
		dispEvt->Set();
		while (recvRunning || procRunning != 0 || dispRunning)
		{
			mainEvt->Wait(100);
		}
		DEL_CLASS(mainEvt);
		DEL_CLASS(procEvt);
		DEL_CLASS(recvEvt);
		DEL_CLASS(dispEvt);
		DEL_CLASS(cliMut);
		DEL_CLASS(clk);
		DEL_CLASS(sockf);
	}

	DEL_CLASS(console);
	return 0;
}
