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
	sb.ClearStr();
	sb.Append((const UTF8Char*)"ARP Reply: HW Addr = ");
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LBT_NONE);
	sb.Append((const UTF8Char*)", IP = ");
	Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
	sb.Append(sbuff);
	console->WriteLine(sb.ToString());
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::ARPHandler *arp = 0;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));

	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	Net::SocketUtil::IPType ipType;
	UInt8 hwAddr[32];
	UTF8Char sbuff[64];
	OSInt i;
	OSInt j;
	OSInt k;
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
			if (ipType == Net::SocketUtil::IT_PRIVATE)
			{
				if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Detected adapter: HW Addr = ");
					sb.AppendHexBuff(hwAddr, 6, ':', Text::LBT_NONE);
					sb.Append((const UTF8Char*)", IP = ");
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)", Index = ");
					sb.AppendOSInt(connInfo->GetIndex());
					console->WriteLine(sb.ToString());
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
		console->WriteLine((const UTF8Char*)"No adapter detected");
	}
	else if (arp->IsError())
	{
		console->WriteLine((const UTF8Char*)"Error in listening to arp data");
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Waiting for data");
		progCtrl->WaitForExit(progCtrl);
		DEL_CLASS(arp);
	}
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
