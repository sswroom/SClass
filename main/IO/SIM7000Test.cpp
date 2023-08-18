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

IO::ConsoleWriter *console;

void __stdcall OnUDPData(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("UDP: "));
	sptr = Net::SocketUtil::GetAddrName(sptr, addr, port);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", Size = "));
	sptr = Text::StrUOSInt(sptr, dataSize);
	console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	UOSInt portNum = Test::TestModem::ListPorts(console);
	UInt32 baudRate = 115200;

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
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
	console->WriteLineC(sb.ToString(), sb.GetLength());

	NotNullPtr<IO::SerialPort> port;
	NEW_CLASSNN(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, true));
	if (port->IsError())
	{
		console->WriteLineC(UTF8STRC("Error in opening the port"));
	}
	else
	{
		IO::ATCommandChannel *channel;
		IO::Device::SIM7000 *modem;
		NEW_CLASS(channel, IO::ATCommandChannel(port, false));
		NEW_CLASS(modem, IO::Device::SIM7000(channel, false));
		modem->SetEcho(true);

		Test::TestModem::GSMModemTest(console, modem, true);
		Test::TestSIM7000::SIM7000Info(console, modem);

		console->WriteLineC(UTF8STRC("End Test Begin Connect"));
		modem->GSMSetFunctionalityFull();
		modem->GPRSEPSReg();
		modem->GPRSNetworkReg();
		modem->GPRSServiceSetAttached(true);
		console->WriteLineC(UTF8STRC("GPRS Service Attached"));
		modem->GPRSSetAPN(CSTR(""));
		console->WriteLineC(UTF8STRC("APN Set"));
		Int32 plmn = 0;
		OSInt i = 30;
		plmn = modem->GSMGetSIMPLMN();
		while (plmn == 0 && i-- > 0)
		{
			Sync::SimpleThread::Sleep(1000);
			plmn = modem->GSMGetSIMPLMN();
		}
		console->WriteLineC(UTF8STRC("PLMN Get"));
		if (plmn != 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Connecting to "));
			sb.AppendI32(plmn);
			sb.AppendC(UTF8STRC("..."));
			console->WriteStrC(sb.ToString(), sb.GetLength());
			if (modem->GSMConnectPLMN(plmn))
			{
				console->WriteLineC(UTF8STRC("OK"));
			}
			else
			{
				console->WriteLineC(UTF8STRC("Failed"));
			}

			if ((sptr = modem->SIMCOMGetUESysInfo(sbuff)) != 0)
			{
				sb.ClearStr();
				sb.AppendC(UTF8STRC("UE Sys Info: "));
				sb.AppendP(sbuff, sptr);
				console->WriteLineC(sb.ToString(), sb.GetLength());
			}
			else
			{
				console->WriteLineC(UTF8STRC("UE Sys Info: Error in getting the value"));
			}

			NotNullPtr<IO::Device::SIM7000SocketFactory> sockf;
			NEW_CLASSNN(sockf, IO::Device::SIM7000SocketFactory(modem, false));
			if (sockf->NetworkStart())
			{
				Net::SocketUtil::AddressInfo addr;
				Bool valid = true;
				console->WriteLineC(UTF8STRC("Network started"));

				if ((sptr = modem->NetGetIFAddr(sbuff)) != 0)
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("IF Addr: "));
					sb.AppendP(sbuff, sptr);
					console->WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					valid = false;
					console->WriteLineC(UTF8STRC("IF Addr: Error in getting value"));
				}
				
				if (valid)
				{
					console->WriteStrC(UTF8STRC("Resolving (def) www.google.com: "));
					if (sockf->DNSResolveIPDef("www.google.com", addr))
					{
						sptr = Net::SocketUtil::GetAddrName(sbuff, addr);
						console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
					}
					else
					{
						console->WriteLineC(UTF8STRC("Error in getting value"));
					}
				}

				if (valid)
				{
					Data::ArrayList<UInt32> dnsList;
					UOSInt i;
					UOSInt j;
					if (sockf->GetDNSList(&dnsList))
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
						console->WriteLineC(sb.ToString(), sb.GetLength());

					}
					else
					{
						console->WriteLineC(UTF8STRC("DNS List: Error in getting value"));
					}
				}

				if (valid)
				{
					Net::SocketUtil::SetAddrInfoV4(addr, 0x08080808);
					console->WriteLineC(UTF8STRC("Ping to 8.8.8.8..."));
					OSInt i = 4;
					UInt32 respTime;
					UInt32 ttl;
					while (i-- > 0)
					{
						if (sockf->IcmpSendEcho2(addr, &respTime, &ttl))
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Resp time = "));
							sb.AppendDouble(respTime / 1000000.0);
							sb.AppendC(UTF8STRC("s, TTL = "));
							sb.AppendU32(ttl);
							console->WriteLineC(sb.ToString(), sb.GetLength());
							Sync::SimpleThread::Sleep(1000);
						}
						else
						{
							console->WriteLineC(UTF8STRC("Cannot ping"));
							Sync::SimpleThread::Sleep(1000);
						}					
					}
				}


				if (valid)
				{
					console->WriteStrC(UTF8STRC("Resolving www.google.com: "));
					if (sockf->DNSResolveIP(CSTR("www.google.com"), addr))
					{
						sptr = Net::SocketUtil::GetAddrName(sbuff, addr);
						console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
					}
					else
					{
						console->WriteLineC(UTF8STRC("Error in getting value"));
					}
				}

				if (valid)
				{
					Net::UDPServer *udp;
					NEW_CLASS(udp, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnUDPData, 0, 0, CSTR_NULL, 1, false));
					if (udp->IsError())
					{
						console->WriteLineC(UTF8STRC("Error in listening to UDP port"));
					}
					else
					{
						console->WriteLineC(UTF8STRC("UDP Server started"));

						console->WriteStrC(UTF8STRC("Resolving (def) my server: "));
						if (sockf->DNSResolveIPDef("sswroom.no-ip.org", addr))
						{
							sptr = Net::SocketUtil::GetAddrName(sbuff, addr);
							console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
						}
						else
						{
							console->WriteLineC(UTF8STRC("Error in getting value"));
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
				console->WriteLineC(UTF8STRC("Network cannot start"));
			}

/*			if (sockf->NetworkEnd())
			{
				console.WriteLineC(UTF8STRC("Network ended");
			}
			else
			{
				console.WriteLineC(UTF8STRC("Network cannot end");
			}*/

			sockf.Delete();	
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in getting plmn"));
		}
		

		DEL_CLASS(modem);
		DEL_CLASS(channel);
	}
	port.Delete();
	DEL_CLASS(console);
	return 0;
}
