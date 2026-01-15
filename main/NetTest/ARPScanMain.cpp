#include "Stdafx.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "IO/ConsoleWriter.h"
#include "Net/ARPHandler.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
NN<Net::SocketFactory> sockf;
Data::ArrayListUInt32 *ipList;

void __stdcall ARPHandler(UnsafeArray<const UInt8> hwAddr, UInt32 ipv4, AnyType userData)
{
	if (ipList->SortedIndexOf(ipv4) < 0)
	{
		NN<const Net::MACInfo::MACEntry> macEntry;
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		Text::StringBuilderUTF8 sb;
		sb.ClearStr();
		sb.AppendC(UTF8STRC("IP = "));
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipv4);
		sb.AppendP(sbuff, sptr);
		sb.AppendChar(' ', 18 - sb.GetLength());
		sb.AppendC(UTF8STRC(", HW Addr = "));
		sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
		sb.AppendC(UTF8STRC(", Vendor = "));
		macEntry = Net::MACInfo::GetMACInfoBuff(hwAddr);
		sb.AppendC(macEntry->name, macEntry->nameLen);
		console->WriteLine(sb.ToCString());
		ipList->SortedInsert(ipv4);
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(ipList, Data::ArrayListUInt32());

	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<Net::ARPInfo> arpList;

	UInt8 hwAddr[32];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	UIntOS j;
	NN<const Net::MACInfo::MACEntry> macEntry;
	Net::ARPInfo::ARPType arpType;
	NN<Net::ARPInfo> arp;
	Net::ARPInfo::GetARPInfoList(arpList);
	console->WriteLine(CSTR("OS ARP:"));
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItemNoCheck(i);
		arpType = arp->GetARPType();
		if (arpType == Net::ARPInfo::ARPT_STATIC || arpType == Net::ARPInfo::ARPT_DYNAMIC)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("IP = "));
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, arp->GetIPAddress());
			sb.AppendC(sbuff, (UIntOS)(sptr - sbuff));
			sb.AppendChar(' ', 18 - sb.GetLength());
			sb.AppendC(UTF8STRC(", HW Addr = "));
			arp->GetPhysicalAddr(hwAddr);
			sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC(", Vendor = "));
			macEntry = Net::MACInfo::GetMACInfoBuff(hwAddr);
			sb.AppendC(macEntry->name, macEntry->nameLen);
			console->WriteLine(sb.ToCString());
			ipList->SortedInsert(arp->GetIPAddress());
		}
		arp.Delete();
		i++;
	}


	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	Net::IPType ipType;
	UIntOS k;
	UInt32 ip;
	sockf->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
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
					sb.AppendC(UTF8STRC("Adapter: HW Addr = "));
					sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC(", IP = "));
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
					sb.AppendP(sbuff, sptr);
					sptr = connInfo->GetName(sbuff).Or(sbuff);
					sb.AppendC(UTF8STRC(", Name = "));
					sb.AppendP(sbuff, sptr);
					console->WriteLine(sb.ToCString());
					connInfo->GetName(sbuff);
					WriteNUInt32(buff, ip);
					if (buff[0] == 192 && buff[1] == 168)
					{
						NEW_CLASS(arpHdlr, Net::ARPHandler(sockf, sbuff, hwAddr, ip, ARPHandler, 0, 1));
						if (arpHdlr->IsError())
						{
							console->WriteLine(CSTR("Error in listening arp data"));
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
							Sync::SimpleThread::Sleep(3000);
							
						}
						DEL_CLASS(arpHdlr);
					}
					else
					{
						console->WriteLine(CSTR("To many ip address"));
					}
				}
			}
			k++;
		}
		connInfo.Delete();
		i++;
	}

	DEL_CLASS(ipList);
	sockf.Delete();
	DEL_CLASS(console);
	return 0;
}
