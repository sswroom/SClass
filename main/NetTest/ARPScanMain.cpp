#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Net/ARPHandler.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
Net::SocketFactory *sockf;
Data::ArrayListUInt32 *ipList;

void __stdcall ARPHandler(const UInt8 *hwAddr, UInt32 ipv4, void *userData)
{
	if (ipList->SortedIndexOf(ipv4) < 0)
	{
		const Net::MACInfo::MACEntry *macEntry;
		UTF8Char sbuff[64];
		Text::StringBuilderUTF8 sb;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IP = "));
		Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
		sb.Append(sbuff);
		sb.AppendChar(' ', 18 - sb.GetLength());
		sb.Append((const UTF8Char*)", HW Addr = ");
		sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
		sb.Append((const UTF8Char*)", Vendor = ");
		macEntry = Net::MACInfo::GetMACInfoBuff(hwAddr);
		if (macEntry)
		{
			sb.Append((const UTF8Char*)macEntry->name);
		}
		console->WriteLine(sb.ToString());
		ipList->SortedInsert(ipv4);
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(ipList, Data::ArrayListUInt32());

	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::ARPInfo *> arpList;

	UInt8 hwAddr[32];
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	const Net::MACInfo::MACEntry *macEntry;
	Net::ARPInfo::ARPType arpType;
	Net::ARPInfo *arp;
	Net::ARPInfo::GetARPInfoList(&arpList);
	console->WriteLine((const UTF8Char*)"OS ARP:");
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItem(i);
		arpType = arp->GetARPType();
		if (arpType == Net::ARPInfo::ARPT_STATIC || arpType == Net::ARPInfo::ARPT_DYNAMIC)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("IP = ");
			Net::SocketUtil::GetIPv4Name(sbuff, arp->GetIPAddress());
			sb.Append(sbuff);
			sb.AppendChar(' ', 18 - sb.GetLength());
			sb.Append((const UTF8Char*)", HW Addr = ");
			arp->GetPhysicalAddr(hwAddr);
			sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
			sb.Append((const UTF8Char*)", Vendor = ");
			macEntry = Net::MACInfo::GetMACInfoBuff(hwAddr);
			if (macEntry)
			{
				sb.Append((const UTF8Char*)macEntry->name);
			}
			console->WriteLine(sb.ToString());
			ipList->SortedInsert(arp->GetIPAddress());
		}

		DEL_CLASS(arp);
		i++;
	}


	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	Net::IPType ipType;
	UOSInt k;
	UInt32 ip;
	sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::IPType::Private)
			{
				if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
				{
					UInt8 buff[4];
					Net::ARPHandler *arpHdlr;
					console->WriteLine();
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Adapter: HW Addr = ");
					sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC(", IP = ");
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					sb.Append(sbuff);
					connInfo->GetName(sbuff);
					sb.AppendC(UTF8STRC(", Name = ");
					sb.Append(sbuff);
					console->WriteLine(sb.ToString());
					connInfo->GetName(sbuff);
					WriteNUInt32(buff, ip);
					if (buff[0] == 192 && buff[1] == 168)
					{
						NEW_CLASS(arpHdlr, Net::ARPHandler(sockf, sbuff, hwAddr, ip, ARPHandler, 0, 1));
						if (arpHdlr->IsError())
						{
							console->WriteLine((const UTF8Char*)"Error in listening arp data");
						}
						else
						{
							buff[3] = 1;
							while (buff[3] < 255)
							{
								if (ipList->SortedIndexOf(ReadNUInt32(buff)) < 0)
								{
									arpHdlr->MakeRequest(ReadNUInt32(buff));
								}
								buff[3]++;
							}
							Sync::Thread::Sleep(3000);
							
						}
						DEL_CLASS(arpHdlr);
					}
					else
					{
						console->WriteLine((const UTF8Char*)"To many ip address");
					}
				}
			}
			k++;
		}
		DEL_CLASS(connInfo);
		i++;
	}

	DEL_CLASS(ipList);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
