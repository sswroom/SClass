#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteBuffer.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Net/OSSocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

NN<Net::SocketFactory> sockf;
NN<Manage::HiResClock> clk;
NN<IO::ConsoleWriter> console;
Bool toStop;
UInt32 procRunning;
Bool recvRunning;
Bool dispRunning;
NN<Sync::Event> mainEvt;
NN<Sync::Event> procEvt;
NN<Sync::Event> recvEvt;
NN<Sync::Event> dispEvt;
NN<Sync::Mutex> cliMut;
Optional<Net::TCPClient> cli;
UInt64 totalRecvSize;
UInt64 totalSendSize;
Double lastTime;
UInt64 lastRecvSize;
UInt64 lastSendSize;

UInt32 __stdcall DispThread(AnyType userObj)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
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
			sptr = Text::StrConcatC(sptr, UTF8STRC("Send: "));
			sptr = Text::StrDouble(sptr, (Double)(currSendSize - lastSendSize) / (currTime - lastTime));
			sptr = Text::StrConcatC(sptr, UTF8STRC("Bps\tRecv: "));
			sptr = Text::StrDouble(sptr, (Double)(currRecvSize - lastRecvSize) / (currTime - lastTime));
			sptr = Text::StrConcatC(sptr, UTF8STRC("Bps       "));
			console->WriteLine(CSTRP(sbuff, sptr));
		}
		lastTime = currTime;
		lastRecvSize = currRecvSize;
		lastSendSize = currSendSize;
	}
	dispRunning = false;
	mainEvt->Set();
	return 0;
}

UInt32 __stdcall ProcThread(AnyType userObj)
{
	UnsafeArray<UInt8> sendBuff;
	UIntOS sendSize;
	UIntOS sendBuffSize = 9000;
	NN<Net::TCPClient> c;
	Sync::Interlocked::IncrementU32(procRunning);
	mainEvt->Set();
	sendBuff = MemAllocArr(UInt8, sendBuffSize);
	while (!toStop)
	{
		Sync::MutexUsage mutUsage(cliMut);
		if (cli.SetTo(c))
		{
			sendSize = c->Write(Data::ByteArrayR(sendBuff, sendBuffSize));
			mutUsage.EndUse();
			if (sendSize > 0)
			{
				Sync::Interlocked::AddU64(totalSendSize, sendSize);
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
	MemFreeArr(sendBuff);
	Sync::Interlocked::DecrementU32(procRunning);
	mainEvt->Set();
	return 0;
}

UInt32 __stdcall RecvThread(AnyType userObj)
{
	UIntOS recvSize;
	NN<Net::TCPClient> c;
	recvRunning = true;
	mainEvt->Set();
	{
		Data::ByteBuffer recvBuff(9000);
		while (!toStop)
		{
			if (cli.SetTo(c))
			{
				recvSize = c->Read(recvBuff);
				if (recvSize > 0)
				{
					Sync::Interlocked::AddU64(totalRecvSize, recvSize);
				}
				else
				{
					Sync::MutexUsage mutUsage(cliMut);
					cli.Delete();
					mutUsage.EndUse();
					recvEvt->Wait(1000);
				}
			}
			else
			{
				recvEvt->Wait(1000);
			}
		}
	}
	recvRunning = false;
	mainEvt->Set();
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc <= 2)
	{
		console->WriteLine(CSTR("Usage: STCPSpdCli [IP] [Port]"));
	}
	else
	{
		NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
		totalSendSize = 0;
		totalRecvSize = 0;
		lastTime = 0;
		lastSendSize = 0;
		lastRecvSize = 0;
		cli = nullptr;
		toStop = false;
		procRunning = 0;
		recvRunning = false;
		NEW_CLASSNN(cliMut, Sync::Mutex());
		NEW_CLASSNN(clk, Manage::HiResClock());
		NEW_CLASSNN(mainEvt, Sync::Event(true));
		NEW_CLASSNN(recvEvt, Sync::Event(true));
		NEW_CLASSNN(procEvt, Sync::Event(true));
		NEW_CLASSNN(dispEvt, Sync::Event(true));

		UIntOS threadCnt = Sync::ThreadUtil::GetThreadCnt();
		UIntOS i;
		Sync::ThreadUtil::Create(DispThread, 0);
		i = 0;
		while (i < threadCnt)
		{
			Sync::ThreadUtil::Create(ProcThread, 0);
			i++;
		}
		Sync::ThreadUtil::Create(RecvThread, 0);
		while (!recvRunning || procRunning != threadCnt || !dispRunning)
		{
			mainEvt->Wait(100);
		}

		Bool valid = true;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port;
		NN<Net::TCPClient> c;

		if (!sockf->DNSResolveIP(Text::CStringNN::FromPtr(argv[1]), addr))
		{
			console->WriteLine(CSTR("Host is not valid"));
			valid = false;
		}
		if (!Text::StrToUInt16(argv[2], port))
		{
			console->WriteLine(CSTR("Port is not a number"));
			valid = false;
		}
		if (port <= 0 || port > 65535)
		{
			console->WriteLine(CSTR("Port is out of range"));
			valid = false;
		}

		if (valid)
		{
			NEW_CLASSNN(c, Net::TCPClient(sockf, addr, port, 10000));
			if (c->IsConnectError())
			{
				c.Delete();
				console->WriteLine(CSTR("Error in connect to server"));
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

			console->WriteLine(CSTR("Running"));
			progCtrl->WaitForExit(progCtrl);
		}

		if (cli.SetTo(c))
		{
			c->Close();
		}
		toStop = true;
		dispEvt->Set();
		while (recvRunning || procRunning != 0 || dispRunning)
		{
			mainEvt->Wait(100);
		}
		mainEvt.Delete();
		procEvt.Delete();
		recvEvt.Delete();
		dispEvt.Delete();
		cliMut.Delete();
		clk.Delete();
		sockf.Delete();
	}

	console.Delete();
	return 0;
}
