#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/PacketAnalyse.h"
#include "IO/Path.h"
#include "IO/PcapngWriter.h"
#include "Net/ConnectionInfo.h"
#include "Net/OSSocketFactory.h"
#include "Net/SocketMonitor.h"


void __stdcall OnPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize)
{
	NN<IO::PacketLogWriter> writer = userData.GetNN<IO::PacketLogWriter>();
	writer->WritePacket(Data::ByteArrayR(packetData, packetSize));
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter console;
	Net::OSSocketFactory sockf(true);
	NN<Socket> soc;
	IO::PacketAnalyse::LinkType linkType;
	Optional<Socket> optSoc;
	linkType = IO::PacketAnalyse::LinkType::Null;
	optSoc = 0;
	if (sockf.CreateRAWSocket().SetTo(soc))
	{
		linkType = IO::PacketAnalyse::LinkType::Ethernet;
		optSoc = soc;
	}
	else
	{
		Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
		NN<Net::ConnectionInfo> connInfo;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UInt32 ip;
		sockf.GetConnInfoList(connInfoList);
		i = 0;
		j = connInfoList.GetCount();
		while (i < j)
		{
			connInfo = connInfoList.GetItemNoCheck(i);
			if (optSoc.IsNull())
			{
				k = 0;
				while (true)
				{
					ip = connInfo->GetIPAddress(k);
					if (ip == 0)
						break;
					if (sockf.CreateRAWIPv4Socket(ip).SetTo(soc))
					{
						linkType = IO::PacketAnalyse::LinkType::Linux;
						optSoc = soc;
						break;
					}
					k++;
				}
			}
			connInfo.Delete();
			i++;
		}		
	}

	if (optSoc.SetTo(soc))
	{
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("RAW"));
		sptr = Text::StrInt64(sptr, Data::DateTimeUtil::GetCurrTimeMillis());
		sptr = Text::StrConcatC(sptr, UTF8STRC(".pcapng"));
		IO::PcapngWriter plogWriter(CSTRP(sbuff, sptr), linkType, CSTR("NetRAWLogger"));
		if (plogWriter.IsError())
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Error in creating "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Writing to "));
			sb.AppendP(sbuff, sptr);
			console.WriteLine(sb.ToCString());
			Net::SocketMonitor socMon(sockf, soc, OnPacket, &plogWriter, 3);
			progCtrl->WaitForExit(progCtrl);
		}
	}
	else
	{
		console.WriteLine(CSTR("Error in opening RAW socket"));
	}
	return 0;
}
