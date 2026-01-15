#include "Stdafx.h"
#include "Net/WiFiUtil.h"

UnsafeArrayOpt<UTF8Char> Net::WiFiUtil::GuessDeviceType(UnsafeArray<UTF8Char> sbuff, NN<Net::WirelessLAN::BSSInfo> bss)
{
	Text::CStringNN manu = CSTR("");
	NN<Net::WirelessLANIE> ie;
	UnsafeArray<const UInt8> ieBuff;
	UnsafeArray<const UInt8> buffPtr;
	UnsafeArray<const UInt8> buffEnd;
	UOSInt i = 0;
	UOSInt j = bss->GetIECount();
	while (i < j)
	{
		if (bss->GetIE(i).SetTo(ie))
		{
			ieBuff = ie->GetIEBuff();
			if (ieBuff[0] == 0xdd)
			{
				if (ReadMUInt32(&ieBuff[2]) == 0x0050F204) //WPS
				{
					buffPtr = ieBuff + 6;
					buffEnd = ieBuff + 2 + ieBuff[1];
					UInt16 itemId;
					UInt16 itemSize;
					while (buffEnd - buffPtr >= 4)
					{
						itemId = ReadMUInt16(buffPtr);
						itemSize = ReadMUInt16(&buffPtr[2]);
						if (buffPtr + itemSize + 4 > buffEnd)
						{
							break;
						}
						switch (itemId)
						{
						case 0x1021: //Manu
							manu = Text::CStringNN(&buffPtr[4], itemSize);
							break;
						case 0x1023: //Model
							if (manu.StartsWith(CSTR("Linksys")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("Linksys ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWithICase(CSTR("TP-LINK")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("TP-Link ")), &buffPtr[4], itemSize);
							}
							if (manu.Equals(CSTR("HP")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("HP ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWith(CSTR("NETGEAR")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("NETGEAR ")), &buffPtr[4], itemSize);
							}
							break;
						case 0x1024: //Model Number
							if (manu.StartsWith(CSTR("Linksys")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("Linksys ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWith(CSTR("ASUSTeK")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("Asus ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWith(CSTR("D-Link")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("D-Link ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWith(CSTR("NETGEAR")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("NETGEAR ")), &buffPtr[4], itemSize);
							}
							break;
						case 0x1042: //Serial
							break;
						case 0x1011: //Device Name
							if (manu.StartsWith(CSTR("Huawei")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("Huawei ")), &buffPtr[4], itemSize);
							}
							if (manu.StartsWith(CSTR("ZOWEE")))
							{
								return Text::StrConcatC(Text::StrConcatC(sbuff, UTF8STRC("ZOWEE ")), &buffPtr[4], itemSize);
							}
							break;
						}
						buffPtr += itemSize + 4;
					}
					break;
				}
			}
		}
		i++;
	}
	return nullptr;
}
