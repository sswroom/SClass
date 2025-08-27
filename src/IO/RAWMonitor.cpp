#include "Stdafx.h"
#include "IO/RAWMonitor.h"

Text::CString IO::RAWMonitor::LinkTypeGetName(UInt32 linkType)
{
	switch (linkType)
	{
	case 0:
		return CSTR("Null");
	case 1:
		return CSTR("Ethernet");
	case 3:
		return CSTR("AX 25");
	case 6:
		return CSTR("IEEE802.5");
	case 7:
		return CSTR("ARCNET");
	case 8:
		return CSTR("SLIP");
	case 9:
		return CSTR("PPP");
	case 101:
		return CSTR("IPv4");
	case 113:
		return CSTR("Linux");
	case 201:
		return CSTR("Bluetooth");
	}
	return nullptr;
}
