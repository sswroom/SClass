#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/ICMPScanner.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

NotNullPtr<Net::SocketFactory> sockf;
IO::ConsoleWriter *console;
Net::ICMPScanner *scanner;

void PingScan(UInt32 ip)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Scanning with interface ip "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
	sb.AppendP(sbuff, sptr);
	console->WriteLineC(sb.ToString(), sb.GetLength());
	
	if (scanner->Scan(ip))
	{
		const Data::ReadingList<Net::ICMPScanner::ScanResult*> *results = scanner->GetResults();
		Net::ICMPScanner::ScanResult *result;
		UOSInt i = 0;
		UOSInt j = results->GetCount();
		while (i < j)
		{
			result = results->GetItem(i);
			sb.ClearStr();
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, result->ip);
			sb.AppendP(sbuff, sptr);
			sb.AppendUTF8Char('\t');
			sb.AppendDouble(result->respTime);
			sb.AppendUTF8Char('\t');
			sb.AppendHexBuff(result->mac, 6, ':', Text::LineBreakType::None);
			sb.AppendUTF8Char('\t');
			const Net::MACInfo::MACEntry *macEntry = Net::MACInfo::GetMACInfoBuff(result->mac);
			sb.AppendC(macEntry->name, macEntry->nameLen);
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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(scanner, Net::ICMPScanner(sockf));

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NotNullPtr<Net::ConnectionInfo> connInfo;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	sockf->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
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
		connInfo.Delete();
		i++;
	}

	DEL_CLASS(scanner);
	DEL_CLASS(console);
	sockf.Delete();
	return 0;
}
