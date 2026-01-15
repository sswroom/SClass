#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/NTPClient.h"
#include "Net/OSSocketFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UIntOS cmdCnt;
	UnsafeArray<UnsafeArray<UTF8Char>> cmds = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	IO::ConsoleWriter console;
	if (cmdCnt < 2)
	{
		console.WriteLine(CSTR("Usage: NTPTime [host]"));
		return 5;
	}
	Net::OSSocketFactory sockf(false);
	Net::SocketUtil::AddressInfo addr;
	if (!sockf.DNSResolveIP(Text::CStringNN::FromPtr(cmds[1]), addr))
	{
		console.WriteLine(CSTR("Error in resolving IP"));
		return 1;
	}
	IO::LogTool log;
	Net::NTPClient ntp(sockf, 0, log);
	Data::Timestamp svrTime;
	if (!ntp.GetServerTime(addr, Net::NTPClient::GetDefaultPort(), svrTime))
	{
		console.WriteLine(CSTR("Error in getting time from server"));
		return 2;
	}
/*	IO::PrivilegeManager priv;
	if (!priv.EnableSystemTime())
	{
		console.WriteLine(CSTR("Error in aquiring privilege"));
		return 3;
	}*/
	if (!svrTime.SetAsComputerTime())
	{
		console.WriteLine(CSTR("Error in setting as computer time"));
		return 4;
	}
	console.WriteLine(CSTR("Time sync success"));
	return 0;
}