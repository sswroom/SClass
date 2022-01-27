#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Net/WirelessLAN.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Net::WirelessLAN *wlan;
	NEW_CLASS(wlan, Net::WirelessLAN());
	if (wlan->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in initializing WiFi"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt thisLen;
		UOSInt maxSSIDLen;
		UOSInt maxManuLen;
		UOSInt maxModelLen;
		UOSInt maxSNLen;
		UInt8 buff[8];
		const UInt8 *macPtr;
		const UTF8Char *csptr;
		Data::ArrayList<Net::WirelessLAN::Interface*> interfaces;
		Data::ArrayList<Net::WirelessLAN::BSSInfo *> bssList;
		Net::WirelessLAN::BSSInfo *bss;
		Net::WirelessLAN::Interface *interface;
		wlan->GetInterfaces(&interfaces);
		i = interfaces.GetCount();
		if (i <= 0)
		{
			console.WriteLineC(UTF8STRC("No WiFi interface found"));
		}
		while (i-- > 0)
		{
			interface = interfaces.GetItem(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Using interface "));
			sb.Append(interface->GetName());
			console.WriteLineC(sb.ToString(), sb.GetLength());

			if (interface->Scan())
			{
				console.WriteLineC(UTF8STRC("Scan requested, waiting for 5 seconds"));
				Sync::Thread::Sleep(5000);
				interface->GetBSSList(&bssList);
				console.WriteLineC(UTF8STRC("Scan result"));
				maxSSIDLen = 0;
				maxManuLen = 0;
				maxModelLen = 0;
				maxSNLen = 0;
				j = bssList.GetCount();
				while (j-- > 0)
				{
					bss = bssList.GetItem(j);
					if (bss->GetSSID())
					{
						thisLen = Text::StrCharCnt(bss->GetSSID());
						if (thisLen > maxSSIDLen)
							maxSSIDLen = thisLen;

						if ((csptr = bss->GetManuf()) != 0)
						{
							thisLen = Text::StrCharCnt(csptr);
							if (thisLen > maxManuLen)
								maxManuLen = thisLen;
						}
						 
						if ((csptr = bss->GetModel()) != 0)
						{
							thisLen = Text::StrCharCnt(csptr);
							if (thisLen > maxModelLen)
								maxModelLen = thisLen;
						}

						if ((csptr = bss->GetSN()) != 0)
						{
							thisLen = Text::StrCharCnt(csptr);
							if (thisLen > maxSNLen)
								maxSNLen = thisLen;
						}
					}
				}
				j = 0;
				k = bssList.GetCount();
				while (j < k)
				{
					bss = bssList.GetItem(j);
					sb.ClearStr();
					macPtr = bss->GetMAC();
					sb.AppendHexBuff(macPtr, 6, ':', Text::LineBreakType::None);
					sb.AppendChar('\t', 1);
					if (bss->GetSSID())
					{
						sb.AppendSlow(bss->GetSSID());
						thisLen = Text::StrCharCnt(bss->GetSSID());
					}
					else
					{
						thisLen = 0;
					}
					if (maxSSIDLen > thisLen)
					{
						sb.AppendChar(' ', maxSSIDLen - thisLen);
					}
					sb.AppendChar('\t', 1);
					Text::SBAppendF64(&sb, bss->GetRSSI());
					sb.AppendChar('\t', 1);
					Text::SBAppendF64(&sb, bss->GetFreq());
					sb.AppendChar('\t', 1);
					if ((csptr = bss->GetManuf()) != 0)
					{
						sb.AppendSlow(csptr);
						thisLen = Text::StrCharCnt(csptr);
					}
					else
					{
						thisLen = 0;
					}
					if (maxManuLen > thisLen)
					{
						sb.AppendChar(' ', maxManuLen - thisLen);
					}
					sb.AppendChar('\t', 1);
					if ((csptr = bss->GetModel()) != 0)
					{
						sb.AppendSlow(csptr);
						thisLen = Text::StrCharCnt(csptr);
					}
					else
					{
						thisLen = 0;
					}
					if (maxModelLen > thisLen)
					{
						sb.AppendChar(' ', maxModelLen - thisLen);
					}
					sb.AppendChar('\t', 1);
					if ((csptr = bss->GetSN()) != 0)
					{
						sb.AppendSlow(csptr);
						thisLen = Text::StrCharCnt(csptr);
					}
					else
					{
						thisLen = 0;
					}
					if (maxSNLen > thisLen)
					{
						sb.AppendChar(' ', maxSNLen - thisLen);
					}
					sb.AppendChar('\t', 1);
					buff[0] = 0;
					buff[1] = 0;
					buff[2] = macPtr[0];
					buff[3] = macPtr[1];
					buff[4] = macPtr[2];
					buff[5] = macPtr[3];
					buff[6] = macPtr[4];
					buff[7] = macPtr[5];
					sb.AppendSlow((const UTF8Char*)Net::MACInfo::GetMACInfo(ReadMUInt64(buff))->name);					
					console.WriteLineC(sb.ToString(), sb.GetLength());
					DEL_CLASS(bss);
					j++;
				}
				bssList.Clear();
			}
			else
			{
				console.WriteLineC(UTF8STRC("Error in scanning"));
			}			
			DEL_CLASS(interface);
		}
	}
	
	DEL_CLASS(wlan);
	return 0;
}
