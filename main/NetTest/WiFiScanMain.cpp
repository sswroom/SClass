#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Net/WirelessLAN.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Net::WirelessLAN *wlan;
	NEW_CLASS(wlan, Net::WirelessLAN());
	if (wlan->IsError())
	{
		console.WriteLine(CSTR("Error in initializing WiFi"));
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
		NN<Text::String> s;
		Data::ArrayListNN<Net::WirelessLAN::Interface> interfaces;
		Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
		NN<Net::WirelessLAN::BSSInfo> bss;
		NN<Net::WirelessLAN::Interface> interface;
		wlan->GetInterfaces(interfaces);
		i = interfaces.GetCount();
		if (i <= 0)
		{
			console.WriteLine(CSTR("No WiFi interface found"));
		}
		while (i-- > 0)
		{
			interface = interfaces.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Using interface "));
			sb.Append(interface->GetName());
			console.WriteLine(sb.ToCString());

			if (interface->Scan())
			{
				console.WriteLine(CSTR("Scan requested, waiting for 5 seconds"));
				Sync::SimpleThread::Sleep(5000);
				interface->GetBSSList(bssList);
				console.WriteLine(CSTR("Scan result"));
				maxSSIDLen = 0;
				maxManuLen = 0;
				maxModelLen = 0;
				maxSNLen = 0;
				j = bssList.GetCount();
				while (j-- > 0)
				{
					bss = bssList.GetItemNoCheck(j);
					thisLen = bss->GetSSID()->leng;
					if (thisLen > maxSSIDLen)
						maxSSIDLen = thisLen;

					if (s.Set(bss->GetManuf()))
					{
						thisLen = s->leng;
						if (thisLen > maxManuLen)
							maxManuLen = thisLen;
					}
						
					if (s.Set(bss->GetModel()))
					{
						thisLen = s->leng;
						if (thisLen > maxModelLen)
							maxModelLen = thisLen;
					}

					if (s.Set(bss->GetSN()))
					{
						thisLen = s->leng;
						if (thisLen > maxSNLen)
							maxSNLen = thisLen;
					}
				}
				j = 0;
				k = bssList.GetCount();
				while (j < k)
				{
					bss = bssList.GetItemNoCheck(j);
					sb.ClearStr();
					macPtr = bss->GetMAC();
					sb.AppendHexBuff(macPtr, 6, ':', Text::LineBreakType::None);
					sb.AppendUTF8Char('\t');
					s = bss->GetSSID();
					sb.Append(s);
					thisLen = s->leng;
					if (maxSSIDLen > thisLen)
					{
						sb.AppendChar(' ', maxSSIDLen - thisLen);
					}
					sb.AppendUTF8Char('\t');
					sb.AppendDouble(bss->GetRSSI());
					sb.AppendUTF8Char('\t');
					sb.AppendDouble(bss->GetFreq());
					sb.AppendUTF8Char('\t');
					if (s.Set(bss->GetManuf()))
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
					if (s.Set(bss->GetModel()))
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
					if (s.Set(bss->GetSN()))
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
					console.WriteLine(sb.ToCString());
					bss.Delete();
					j++;
				}
				bssList.Clear();
			}
			else
			{
				console.WriteLine(CSTR("Error in scanning"));
			}			
			interface.Delete();
		}
	}
	
	DEL_CLASS(wlan);
	return 0;
}
