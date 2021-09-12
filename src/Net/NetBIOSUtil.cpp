#include "Stdafx.h"
#include "Net/NetBIOSUtil.h"

UTF8Char *Net::NetBIOSUtil::GetName(UTF8Char *sbuff, const UTF8Char *nbName)
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
			return 0;
		}
		*sbuff++ = (UTF8Char)(((c1 - 65) << 4) | (c2 - 65));
	}
	*sbuff = 0;
	return sbuff;
}

UTF8Char *Net::NetBIOSUtil::SetName(UTF8Char *nbBuff, const UTF8Char *name)
{
	UOSInt i = 16;
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

const UTF8Char *Net::NetBIOSUtil::NameTypeGetName(UInt8 nameType)
{
	switch (nameType)
	{
	case 0:
		return (const UTF8Char*)"Workstation Service";
	case 1:
		return (const UTF8Char*)"Browser";
	case 3:
		return (const UTF8Char*)"Windows Messenger Service";
	case 6:
		return (const UTF8Char*)"Remote Access Service";
	case 27:
		return (const UTF8Char*)"Domain Master Browser";
	case 28:
		return (const UTF8Char*)"Domain Controllers";
	case 29:
		return (const UTF8Char*)"Master Browser";
	case 30:
		return (const UTF8Char*)"Browser Election Service";
	case 32:
		return (const UTF8Char*)"File Service";
	case 33:
		return (const UTF8Char*)"Remote Access Service Client";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
