#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/ModemController.h"
#include "IO/SerialPort.h"
#include "IO/Device/SIM7000.h"
#include "IO/Device/SIM7000SocketFactory.h"
#include "Net/UDPServer.h"
#include "Sync/SimpleThread.h"
#include "Test/TestModem.h"
#include "Test/TestSIM7000.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

NN<IO::ConsoleWriter> console;

void __stdcall OnUDPData(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("UDP: "));
	sptr = Net::SocketUtil::GetAddrName(sptr, addr, port).Or(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", Size = "));
	sptr = Text::StrUOSInt(sptr, data.GetSize());
	console->WriteLine(CSTRP(sbuff, sptr));
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	UOSInt portNum = Test::TestModem::ListPorts(console);
	UInt32 baudRate = 115200;

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUOSInt(argv[1], portNum);
	}
	if (argc >= 3)
	{
		Text::StrToUInt32(argv[2], baudRate);
	}
	console->WriteLine();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Trying Port "));
	sb.AppendUOSInt(portNum);
	console->WriteLine(sb.ToCString());

	NN<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, true));
	if (port->IsError())
	{
		console->WriteLine(CSTR("Error in opening the port"));
	}
	else
	{
		NN<IO::ATCommandChannel> channel;
		NN<IO::Device::SIM7000> modem;
		NEW_CLASSNN(channel, IO::ATCommandChannel(port, false));
		NEW_CLASSNN(modem, IO::Device::SIM7000(channel, false));
		modem->SetEcho(true);

		Test::TestModem::GSMModemTest(console, modem, true);
		Test::TestSIM7000::SIM7000Info(console, modem);

		console->WriteLine(CSTR("End Test Begin Connect"));
		modem->GSMSetFunctionalityFull();
		modem->GPRSEPSReg();
		modem->GPRSNetworkReg();
		modem->GPRSServiceSetAttached(true);
		console->WriteLine(CSTR("GPRS Service Attached"));
		modem->GPRSSetAPN(CSTR(""));
		console->WriteLine(CSTR("APN Set"));
		Int32 plmn = 0;
		OSInt i = 30;
		plmn = modem->GSMGetSIMPLMN();
		while (plmn == 0 && i-- > 0)
		{
			Sync::SimpleThread::Sleep(1000);
			plmn = modem->GSMGetSIMPLMN();
		}
		console->WriteLine(CSTR("PLMN Get"));
		if (plmn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Connecting to "));
			sb.AppendI32(plmn);
			sb.AppendC(UTF8STRC("..."));
			console->Write(sb.ToCString());
			if (modem->GSMConnectPLMN(plmn))
			{
				console->WriteLine(CSTR("OK"));
			}
			else
			{
				console->WriteLine(CSTR("Failed"));
			}

			if (modem->SIMCOMGetUESysInfo(sbuff).SetTo(sptr))
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("UE Sys Info: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLine(sb.ToCString());
			}
			else
			{
				console->WriteLine(CSTR("UE Sys Info: Error in getting the value"));
			}

			NN<IO::Device::SIM7000SocketFactory> sockf;
			NEW_CLASSNN(sockf, IO::Device::SIM7000SocketFactory(modem, false));
			if (sockf->NetworkStart())
			{
				Net::SocketUtil::AddressInfo addr;
				Bool valid = true;
				console->WriteLine(CSTR("Network started"));

				if (modem->NetGetIFAddr(sbuff).SetTo(sptr))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("IF Addr: "));
					sb.AppendP(sbuff, sptr);
					console->WriteLine(sb.ToCString());
				}
				else
				{
					valid = false;
					console->WriteLine(CSTR("IF Addr: Error in getting value"));
				}
				
				if (valid)
				{
					console->Write(CSTR("Resolving (def) www.google.com: "));
					if (sockf->DNSResolveIPDef("www.google.com", addr))
					{
						sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
						console->WriteLine(CSTRP(sbuff, sptr));
					}
					else
					{
						console->WriteLine(CSTR("Error in getting value"));
					}
				}

				if (valid)
				{
					Data::ArrayList<UInt32> dnsList;
					UOSInt i;
					UOSInt j;
					if (sockf->GetDNSList(dnsList))
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("DNS List: "));
						i = 0;
						j = dnsList.GetCount();
						while (i < j)
						{
							if (i > 0)
							{
								sb.AppendC(UTF8STRC(", "));
							}
							sptr = Net::SocketUtil::GetIPv4Name(sbuff, dnsList.GetItem(i));
							sb.AppendP(sbuff, sptr);
							i++;
						}
						console->WriteLine(sb.ToCString());

					}
					else
					{
						console->WriteLine(CSTR("DNS List: Error in getting value"));
					}
				}

				if (valid)
				{
					Net::SocketUtil::SetAddrInfoV4(addr, 0x08080808);
					console->WriteLine(CSTR("Ping to 8.8.8.8..."));
					OSInt i = 4;
					UInt32 respTime;
					UInt32 ttl;
					while (i-- > 0)
					{
						if (sockf->IcmpSendEcho2(addr, respTime, ttl))
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Resp time = "));
							sb.AppendDouble(respTime / 1000000.0);
							sb.AppendC(UTF8STRC("s, TTL = "));
							sb.AppendU32(ttl);
							console->WriteLine(sb.ToCString());
							Sync::SimpleThread::Sleep(1000);
						}
						else
						{
							console->WriteLine(CSTR("Cannot ping"));
							Sync::SimpleThread::Sleep(1000);
						}					
					}
				}


				if (valid)
				{
					console->Write(CSTR("Resolving www.google.com: "));
					if (sockf->DNSResolveIP(CSTR("www.google.com"), addr))
					{
						sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
						console->WriteLine(CSTRP(sbuff, sptr));
					}
					else
					{
						console->WriteLine(CSTR("Error in getting value"));
					}
				}

				if (valid)
				{
					IO::LogTool log;
					Net::UDPServer *udp;
					NEW_CLASS(udp, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnUDPData, 0, log, CSTR_NULL, 1, false));
					if (udp->IsError())
					{
						console->WriteLine(CSTR("Error in listening to UDP port"));
					}
					else
					{
						console->WriteLine(CSTR("UDP Server started"));

						console->Write(CSTR("Resolving (def) my server: "));
						if (sockf->DNSResolveIPDef("sswroom.no-ip.org", addr))
						{
							sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
							console->WriteLine(CSTRP(sbuff, sptr));
						}
						else
						{
							console->WriteLine(CSTR("Error in getting value"));
						}
						udp->SendTo(addr, 10107, (const UInt8*)"Testing", 7);
						Sync::SimpleThread::Sleep(3000);
					}				
					DEL_CLASS(udp);
				}

				if (!valid)
				{
//					modem->ResetModem();
//					modem->GSMSetFunctionalityMin();
				}
			}
			else
			{
				console->WriteLine(CSTR("Network cannot start"));
			}

/*			if (sockf->NetworkEnd())
			{
				console.WriteLine(CSTR("Network ended");
			}
			else
			{
				console.WriteLine(CSTR("Network cannot end");
			}*/

			sockf.Delete();	
		}
		else
		{
			console->WriteLine(CSTR("Error in getting plmn"));
		}
		
		modem.Delete();
		channel.Delete();
	}
	port.Delete();
	console.Delete();
	return 0;
}
