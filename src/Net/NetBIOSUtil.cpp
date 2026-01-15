#include "Stdafx.h"
#include "Net/NetBIOSUtil.h"

UnsafeArrayOpt<UTF8Char> Net::NetBIOSUtil::GetName(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> nbName)
{
	UTF8Char c1;
	UTF8Char c2;
	while (true)
	{
		c1 = *nbName++;
		if (c1 == 0)
			break;
		c2 = *nbName++;
		if (c1 < 65 || c1 >= 81 || c2 < 65 || c2 >= 81)
		{
			return nullptr;
		}
		*sbuff++ = (UTF8Char)(((c1 - 65) << 4) | (c2 - 65));
	}
	*sbuff = 0;
	return sbuff;
}

UnsafeArray<UTF8Char> Net::NetBIOSUtil::SetName(UnsafeArray<UTF8Char> nbBuff, UnsafeArray<const UTF8Char> name)
{
	UIntOS i = 16;
	UTF8Char c;
	while (i-- > 0)
	{
		c = *name;
		nbBuff[0] = (UTF8Char)(65 + (c >> 4));
		nbBuff[1] = (UTF8Char)(65 + (c & 15));
		nbBuff += 2;
		if (c) name++;
	}
	*nbBuff = 0;
	return nbBuff;
}

Text::CStringNN Net::NetBIOSUtil::NameTypeGetName(UInt8 nameType)
{
	switch (nameType)
	{
	case 0:
		return CSTR("Workstation Service");
	case 1:
		return CSTR("Browser");
	case 3:
		return CSTR("Windows Messenger Service");
	case 6:
		return CSTR("Remote Access Service");
	case 27:
		return CSTR("Domain Master Browser");
	case 28:
		return CSTR("Domain Controllers");
	case 29:
		return CSTR("Master Browser");
	case 30:
		return CSTR("Browser Election Service");
	case 32:
		return CSTR("File Service");
	case 33:
		return CSTR("Remote Access Service Client");
	default:
		return CSTR("Unknown");
	}
}
