#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/ARPHandler.h"
#include "Net/ConnectionInfo.h"
#include "Net/OSSocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

NN<IO::ConsoleWriter> console;
NN<Net::SocketFactory> sockf;

void __stdcall ARPHandler(UnsafeArray<const UInt8> hwAddr, UInt32 ipv4, AnyType userData)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("ARP Reply: HW Addr = "));
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC(", IP = "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
	sb.AppendP(sbuff, sptr);
	console->WriteLine(sb.ToCString());
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Optional<Net::ARPHandler> arp = nullptr;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));

	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	Net::IPType ipType;
	UInt8 hwAddr[32];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UInt32 ip;
	sockf->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		k = 0;
		while (arp.IsNull())
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::IPType::Private)
			{
				if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Detected adapter: HW Addr = "));
					sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC(", IP = "));
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
					sb.AppendP(sbuff, sptr);
					sb.AppendC(UTF8STRC(", Index = "));
					sb.AppendUIntOS(connInfo->GetIndex());
					console->WriteLine(sb.ToCString());
					connInfo->GetName(sbuff);
					NEW_CLASSOPT(arp, Net::ARPHandler(sockf, sbuff, hwAddr, ip, ARPHandler, 0, 1));
				}
			}
			k++;
		}
		connInfo.Delete();
		i++;
	}

	NN<Net::ARPHandler> nnarp;
	if (!arp.SetTo(nnarp))
	{
		console->WriteLine(CSTR("No adapter detected"));
	}
	else if (nnarp->IsError())
	{
		console->WriteLine(CSTR("Error in listening to arp data"));
		nnarp.Delete();
	}
	else
	{
		console->WriteLine(CSTR("Waiting for data"));
		progCtrl->WaitForExit(progCtrl);
		nnarp.Delete();
	}
	sockf.Delete();
	console.Delete();
	return 0;
}
