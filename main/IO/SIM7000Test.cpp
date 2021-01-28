#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/ModemController.h"
#include "IO/SerialPort.h"
#include "IO/Device/SIM7000.h"
#include "IO/Device/SIM7000SocketFactory.h"
#include "Net/UDPServer.h"
#include "Sync/Thread.h"
#include "Test/TestModem.h"
#include "Test/TestSIM7000.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

void __stdcall OnUDPData(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"UDP: ");
	sptr = Net::SocketUtil::GetAddrName(sptr, addr, port);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)", Size = ");
	sptr = Text::StrUOSInt(sptr, dataSize);
	console->WriteLine(sbuff);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[256];
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(console, IO::ConsoleWriter());
	Int32 portNum = Test::TestModem::ListPorts(console);
	Int32 baudRate = 115200;

	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToInt32(argv[1], &portNum);
	}
	if (argc >= 3)
	{
		Text::StrToInt32(argv[2], &baudRate);
	}
	console->WriteLine();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Trying Port ");
	sb.AppendI32(portNum);
	console->WriteLine(sb.ToString());

	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, true));
	if (port->IsError())
	{
		console->WriteLine((const UTF8Char*)"Error in opening the port");
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

		console->WriteLine((const UTF8Char*)"End Test Begin Connect");
		modem->GSMSetFunctionalityFull();
		modem->GPRSEPSReg();
		modem->GPRSNetworkReg();
		modem->GPRSServiceSetAttached(true);
		console->WriteLine((const UTF8Char*)"GPRS Service Attached");
		modem->GPRSSetAPN((const UTF8Char*)"");
		console->WriteLine((const UTF8Char*)"APN Set");
		Int32 plmn = 0;
		OSInt i = 30;
		plmn = modem->GSMGetSIMPLMN();
		while (plmn == 0 && i-- > 0)
		{
			Sync::Thread::Sleep(1000);
			plmn = modem->GSMGetSIMPLMN();
		}
		console->WriteLine((const UTF8Char*)"PLMN Get");
		if (plmn != 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Connecting to ");
			sb.AppendI32(plmn);
			sb.Append((const UTF8Char*)"...");
			console->Write(sb.ToString());
			if (modem->GSMConnectPLMN(plmn))
			{
				console->WriteLine((const UTF8Char*)"OK");
			}
			else
			{
				console->WriteLine((const UTF8Char*)"Failed");
			}

			if (modem->SIMCOMGetUESysInfo(sbuff))
			{
				sb.ClearStr();
				sb.Append((const UTF8Char*)"UE Sys Info: ");
				sb.Append(sbuff);
				console->WriteLine(sb.ToString());
			}
			else
			{
				console->WriteLine((const UTF8Char*)"UE Sys Info: Error in getting the value");
			}

			IO::Device::SIM7000SocketFactory *sockf;
			NEW_CLASS(sockf, IO::Device::SIM7000SocketFactory(modem, false));
			if (sockf->NetworkStart())
			{
				Net::SocketUtil::AddressInfo addr;
				Bool valid = true;
				console->WriteLine((const UTF8Char*)"Network started");

				if (modem->NetGetIFAddr(sbuff))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"IF Addr: ");
					sb.Append(sbuff);
					console->WriteLine(sb.ToString());
				}
				else
				{
					valid = false;
					console->WriteLine((const UTF8Char*)"IF Addr: Error in getting value");
				}
				
				if (valid)
				{
					console->Write((const UTF8Char*)"Resolving (def) www.google.com: ");
					if (sockf->DNSResolveIPDef("www.google.com", &addr))
					{
						Net::SocketUtil::GetAddrName(sbuff, &addr);
						console->WriteLine(sbuff);
					}
					else
					{
						console->WriteLine((const UTF8Char*)"Error in getting value");
					}
				}

				if (valid)
				{
					Data::ArrayList<UInt32> dnsList;
					OSInt i;
					OSInt j;
					if (sockf->GetDNSList(&dnsList))
					{
						sb.ClearStr();
						sb.Append((const UTF8Char*)"DNS List: ");
						i = 0;
						j = dnsList.GetCount();
						while (i < j)
						{
							if (i > 0)
							{
								sb.Append((const UTF8Char*)", ");
							}
							Net::SocketUtil::GetIPv4Name(sbuff, dnsList.GetItem(i));
							sb.Append(sbuff);
							i++;
						}
						console->WriteLine(sb.ToString());

					}
					else
					{
						console->WriteLine((const UTF8Char*)"DNS List: Error in getting value");
					}
				}

				if (valid)
				{
					Net::SocketUtil::SetAddrInfoV4(&addr, 0x08080808);
					console->WriteLine((const UTF8Char*)"Ping to 8.8.8.8...");
					OSInt i = 4;
					Int32 respTime;
					Int32 ttl;
					while (i-- > 0)
					{
						if (sockf->IcmpSendEcho2(&addr, &respTime, &ttl))
						{
							sb.ClearStr();
							sb.Append((const UTF8Char*)"Resp time = ");
							Text::SBAppendF64(&sb, respTime / 1000000.0);
							sb.Append((const UTF8Char*)"s, TTL = ");
							sb.AppendI32(ttl);
							console->WriteLine(sb.ToString());
							Sync::Thread::Sleep(1000);
						}
						else
						{
							console->WriteLine((const UTF8Char*)"Cannot ping");
							Sync::Thread::Sleep(1000);
						}					
					}
				}


				if (valid)
				{
					console->Write((const UTF8Char*)"Resolving www.google.com: ");
					if (sockf->DNSResolveIP("www.google.com", &addr))
					{
						Net::SocketUtil::GetAddrName(sbuff, &addr);
						console->WriteLine(sbuff);
					}
					else
					{
						console->WriteLine((const UTF8Char*)"Error in getting value");
					}
				}

				if (valid)
				{
					Net::UDPServer *udp;
					NEW_CLASS(udp, Net::UDPServer(sockf, 0, 0, 0, OnUDPData, 0, 0, 0, 1, false));
					if (udp->IsError())
					{
						console->WriteLine((const UTF8Char*)"Error in listening to UDP port");
					}
					else
					{
						console->WriteLine((const UTF8Char*)"UDP Server started");

						console->Write((const UTF8Char*)"Resolving (def) my server: ");
						if (sockf->DNSResolveIPDef("sswroom.no-ip.org", &addr))
						{
							Net::SocketUtil::GetAddrName(sbuff, &addr);
							console->WriteLine(sbuff);
						}
						else
						{
							console->WriteLine((const UTF8Char*)"Error in getting value");
						}
						udp->SendTo(&addr, 10107, (const UInt8*)"Testing", 7);
						Sync::Thread::Sleep(3000);
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
				console->WriteLine((const UTF8Char*)"Network cannot start");
			}

/*			if (sockf->NetworkEnd())
			{
				console.WriteLine((const UTF8Char*)"Network ended");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"Network cannot end");
			}*/

			DEL_CLASS(sockf);			
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Error in getting plmn");
		}
		

		DEL_CLASS(modem);
		DEL_CLASS(channel);
	}
	DEL_CLASS(port);
	DEL_CLASS(console);
	return 0;
}
