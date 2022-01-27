#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/ARPHandler.h"
#include "Net/ConnectionInfo.h"
#include "Net/OSSocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
Net::SocketFactory *sockf;

void __stdcall ARPHandler(const UInt8 *hwAddr, UInt32 ipv4, void *userData)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb.ClearStr();
	sb.AppendC(UTF8STRC("ARP Reply: HW Addr = "));
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC(", IP = "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
	sb.AppendP(sbuff, sptr);
	console->WriteLineC(sb.ToString(), sb.GetLength());
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::ARPHandler *arp = 0;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));

	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	Net::IPType ipType;
	UInt8 hwAddr[32];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		k = 0;
		while (arp == 0)
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
					sb.AppendUOSInt(connInfo->GetIndex());
					console->WriteLineC(sb.ToString(), sb.GetLength());
					connInfo->GetName(sbuff);
					NEW_CLASS(arp, Net::ARPHandler(sockf, sbuff, hwAddr, ip, ARPHandler, 0, 1));
				}
			}
			k++;
		}
		DEL_CLASS(connInfo);
		i++;
	}

	if (arp == 0)
	{
		console->WriteLineC(UTF8STRC("No adapter detected"));
	}
	else if (arp->IsError())
	{
		console->WriteLineC(UTF8STRC("Error in listening to arp data"));
	}
	else
	{
		console->WriteLineC(UTF8STRC("Waiting for data"));
		progCtrl->WaitForExit(progCtrl);
		DEL_CLASS(arp);
	}
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
