#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/ICMPScanner.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Net::SocketFactory *sockf;
IO::ConsoleWriter *console;
Net::ICMPScanner *scanner;

void PingScan(UInt32 ip)
{
	UTF8Char sbuff[32];
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Scanning with interface ip "));
	Net::SocketUtil::GetIPv4Name(sbuff, ip);
	sb.Append(sbuff);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	
	if (scanner->Scan(ip))
	{
		Data::ArrayList<Net::ICMPScanner::ScanResult*> *results = scanner->GetResults();
		Net::ICMPScanner::ScanResult *result;
		UOSInt i = 0;
		UOSInt j = results->GetCount();
		while (i < j)
		{
			result = results->GetItem(i);
			sb.ClearStr();
			Net::SocketUtil::GetIPv4Name(sbuff, result->ip);
			sb.Append(sbuff);
			sb.AppendChar('\t', 1);
			Text::SBAppendF64(&sb, result->respTime);
			sb.AppendChar('\t', 1);
			sb.AppendHexBuff(result->mac, 6, ':', Text::LineBreakType::None);
			sb.AppendChar('\t', 1);
			sb.Append((const UTF8Char*)Net::MACInfo::GetMACInfoBuff(result->mac)->name);
			console->WriteLineC(sb.ToString(), sb.GetLength());
			i++;
		}
		console->WriteLine();
	}
	else
	{
		console->WriteLineC(UTF8STRC("Scan failed"));
	}
	///////////////////////////
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(scanner, Net::ICMPScanner(sockf));

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
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
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				Net::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
				if (ipType == Net::IPType::Private)
				{
					PingScan(ip);
				}
				k++;
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}

	DEL_CLASS(scanner);
	DEL_CLASS(console);
	DEL_CLASS(sockf);
	return 0;
}
