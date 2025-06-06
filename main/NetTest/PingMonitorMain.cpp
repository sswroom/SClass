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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/UTF8Writer.h"
#include "Text/XML.h"

IO::Writer *console;
Int32 threadCnt;
Bool threadToStop;
NN<Socket> rawSock;
NN<Net::SocketFactory> sockf;
IO::LogTool *logTool;

UInt32 __stdcall RecvThread(AnyType userObj)
{
	UInt8 packetBuff[10240];
	UOSInt packetSize;
	UInt8 *buff;
	UOSInt buffSize;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt16 etherType;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Net::SocketFactory::ErrorType et;
	Sync::Interlocked::IncrementI32(threadCnt);
	{
		Text::StringBuilderUTF8 sb;
		while (!threadToStop)
		{
			packetSize = sockf->UDPReceive(rawSock, packetBuff, 10240, addr, port, et);
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
								logTool->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
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
	Sync::Interlocked::DecrementI32(threadCnt);
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	OSInt i;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	threadCnt = 0;
	threadToStop = false;

#if defined(DEBUGCON)
	NEW_CLASS(console, IO::DebugWriter());
#else
	NEW_CLASS(console, IO::ConsoleWriter());
#endif
	NEW_CLASSNN(sockf, Net::OSSocketFactory(true));
	console->WriteLine(CSTR("PingMonitor Started"));
	if (sockf->CreateRAWSocket().SetTo(rawSock))
	{
		NEW_CLASS(logTool, IO::LogTool());
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("PingLog"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("Log"));
		logTool->AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		i = 3;
		while (i-- > 0)
		{
			Sync::ThreadUtil::Create(RecvThread, 0);
		}
		progCtrl->WaitForExit(progCtrl);

		threadToStop = true;
		sockf->DestroySocket(rawSock);
		while (threadCnt > 0)
		{
			Sync::SimpleThread::Sleep(1);
		}
		DEL_CLASS(logTool);
	}
	else
	{
		console->WriteLine(CSTR("Error in creating RAW socket"));
	}
	sockf.Delete();
	DEL_CLASS(console);
	return 0;
}
