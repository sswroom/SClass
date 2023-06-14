#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Net/WirelessLAN.h"
#include "Sync/SimpleThread.h"
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
		Text::String *s;
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
				Sync::SimpleThread::Sleep(5000);
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
						thisLen = bss->GetSSID()->leng;
						if (thisLen > maxSSIDLen)
							maxSSIDLen = thisLen;

						if ((s = bss->GetManuf()) != 0)
						{
							thisLen = s->leng;
							if (thisLen > maxManuLen)
								maxManuLen = thisLen;
						}
						 
						if ((s = bss->GetModel()) != 0)
						{
							thisLen = s->leng;
							if (thisLen > maxModelLen)
								maxModelLen = thisLen;
						}

						if ((s = bss->GetSN()) != 0)
						{
							thisLen = s->leng;
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
					sb.AppendUTF8Char('\t');
					if (bss->GetSSID())
					{
						s = bss->GetSSID();
						sb.Append(s);
						thisLen = s->leng;
					}
					else
					{
						thisLen = 0;
					}
					if (maxSSIDLen > thisLen)
					{
						sb.AppendChar(' ', maxSSIDLen - thisLen);
					}
					sb.AppendUTF8Char('\t');
					Text::SBAppendF64(&sb, bss->GetRSSI());
					sb.AppendUTF8Char('\t');
					Text::SBAppendF64(&sb, bss->GetFreq());
					sb.AppendUTF8Char('\t');
					if ((s = bss->GetManuf()) != 0)
					{
						sb.Append(s);
						thisLen = s->leng;
					}
					else
					{
						thisLen = 0;
					}
					if (maxManuLen > thisLen)
					{
						sb.AppendChar(' ', maxManuLen - thisLen);
					}
					sb.AppendUTF8Char('\t');
					if ((s = bss->GetModel()) != 0)
					{
						sb.Append(s);
						thisLen = s->leng;
					}
					else
					{
						thisLen = 0;
					}
					if (maxModelLen > thisLen)
					{
						sb.AppendChar(' ', maxModelLen - thisLen);
					}
					sb.AppendUTF8Char('\t');
					if ((s = bss->GetSN()) != 0)
					{
						sb.Append(s);
						thisLen = s->leng;
					}
					else
					{
						thisLen = 0;
					}
					if (maxSNLen > thisLen)
					{
						sb.AppendChar(' ', maxSNLen - thisLen);
					}
					sb.AppendUTF8Char('\t');
					buff[0] = 0;
					buff[1] = 0;
					buff[2] = macPtr[0];
					buff[3] = macPtr[1];
					buff[4] = macPtr[2];
					buff[5] = macPtr[3];
					buff[6] = macPtr[4];
					buff[7] = macPtr[5];
					const Net::MACInfo::MACEntry *macEntry = Net::MACInfo::GetMACInfo(ReadMUInt64(buff));
					sb.AppendC(macEntry->name, macEntry->nameLen);
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
