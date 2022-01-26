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

Text::CString Net::NetBIOSUtil::NameTypeGetName(UInt8 nameType)
{
	switch (nameType)
	{
	case 0:
		return {UTF8STRC("Workstation Service")};
	case 1:
		return {UTF8STRC("Browser")};
	case 3:
		return {UTF8STRC("Windows Messenger Service")};
	case 6:
		return {UTF8STRC("Remote Access Service")};
	case 27:
		return {UTF8STRC("Domain Master Browser")};
	case 28:
		return {UTF8STRC("Domain Controllers")};
	case 29:
		return {UTF8STRC("Master Browser")};
	case 30:
		return {UTF8STRC("Browser Election Service")};
	case 32:
		return {UTF8STRC("File Service")};
	case 33:
		return {UTF8STRC("Remote Access Service Client")};
	default:
		return {UTF8STRC("Unknown")};
	}
}
