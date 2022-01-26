#include "Stdafx.h"
#include "IO/RAWMonitor.h"

Text::CString IO::RAWMonitor::LinkTypeGetName(UInt32 linkType)
{
	switch (linkType)
	{
	case 0:
		return {UTF8STRC("Null")};
	case 1:
		return {UTF8STRC("Ethernet")};
	case 3:
		return {UTF8STRC("AX 25")};
	case 6:
		return {UTF8STRC("IEEE802.5")};
	case 7:
		return {UTF8STRC("ARCNET")};
	case 8:
		return {UTF8STRC("SLIP")};
	case 9:
		return {UTF8STRC("PPP")};
	case 101:
		return {UTF8STRC("IPv4")};
	case 113:
		return {UTF8STRC("Linux")};
	case 201:
		return {UTF8STRC("Bluetooth")};
	}
	return {0, 0};
}
