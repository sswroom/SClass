#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#if defined(DEBUGCON)
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Net/SocketUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

IO::Writer *console;
Int32 threadCnt;
Bool threadToStop;
Socket *rawSock;
Net::SocketFactory *sockf;
IO::LogTool *logTool;

UInt32 __stdcall RecvThread(void *userObj)
{
	UInt8 packetBuff[10240];
	UOSInt packetSize;
	UInt8 *buff;
	UOSInt buffSize;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt16 etherType;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Net::SocketFactory::ErrorType et;
	Sync::Interlocked::Increment(&threadCnt);
	{
		Text::StringBuilderUTF8 sb;
		while (!threadToStop)
		{
			packetSize = sockf->UDPReceive(rawSock, packetBuff, 10240, &addr, &port, &et);
			if (packetSize >= 14)
			{
				etherType = ReadMUInt16(&packetBuff[12]);
				switch (etherType)
				{
				case 0x0026: //Unknown
					break;
				case 0x0046: //Unknown
					break;
				case 0x0800: //IPv4
					buff = &packetBuff[14];
					buffSize = packetSize - 14;
					if ((buff[0] & 0xf0) == 0x40)
					{
						UInt8 *ipData;
						UOSInt ipDataSize;

						if ((buff[0] & 0xf) <= 5)
						{
							ipData = &buff[20];
							ipDataSize = buffSize - 20;
						}
						else
						{
							ipData = &buff[(buff[0] & 0xf) << 2];
							ipDataSize = buffSize - ((buff[0] & 0xf) << 2);
						}
						
						switch (buff[9])
						{
						case 1: //ICMP
							if (ipData[0] == 8) //Echo Request
							{
								sb.ClearStr();
								sb.AppendC(UTF8STRC("Received ping from "));
								sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&buff[12]));
								sb.AppendP(sbuff, sptr);
								sb.AppendC(UTF8STRC(", Size = "));
								sb.AppendUOSInt(ipDataSize);
								logTool->LogMessage(sb.ToCString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
	//							console->WriteLineC(sb->ToString(), sb->GetLength());
							}
							break;
						case 2: //IGMP
							break;
						case 6: //TCP
							break;
						case 17: //UDP
							break;
						default:
							break;
						}
					}
					break;
				case 0x0806: //ARP
					break;
				case 0x86DD: //IPv6
					break;
				case 0x888E: //EAP over LAN
					break;
				default:
					break;
				}
			}
			else
			{
				
			}
		}
	}
	Sync::Interlocked::Decrement(&threadCnt);
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt i;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	threadCnt = 0;
	threadToStop = false;

#if defined(DEBUGCON)
	NEW_CLASS(console, IO::DebugWriter());
#else
	NEW_CLASS(console, IO::ConsoleWriter());
#endif
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	console->WriteLineC(UTF8STRC("PingMonitor Started"));
	rawSock = sockf->CreateRAWSocket();
	if (rawSock)
	{
		NEW_CLASS(logTool, IO::LogTool());
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("PingLog"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("Log"));
		logTool->AddFileLog(CSTRP(sbuff, sptr), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
		i = 3;
		while (i-- > 0)
		{
			Sync::Thread::Create(RecvThread, 0);
		}
		progCtrl->WaitForExit(progCtrl);

		threadToStop = true;
		sockf->DestroySocket(rawSock);
		while (threadCnt > 0)
		{
			Sync::Thread::Sleep(1);
		}
		DEL_CLASS(logTool);
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in creating RAW socket"));
	}
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
