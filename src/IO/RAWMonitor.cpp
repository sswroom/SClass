#include "Stdafx.h"
#include "IO/RAWMonitor.h"

const UTF8Char *IO::RAWMonitor::LinkTypeGetName(UInt32 linkType)
{
	switch (linkType)
	{
	case 0:
		return (const UTF8Char*)"Null";
	case 1:
		return (const UTF8Char*)"Ethernet";
	case 3:
		return (const UTF8Char*)"AX 25";
	case 6:
		return (const UTF8Char*)"IEEE802.5";
	case 7:
		return (const UTF8Char*)"ARCNET";
	case 8:
		return (const UTF8Char*)"SLIP";
	case 9:
		return (const UTF8Char*)"PPP";
	case 101:
		return (const UTF8Char*)"IPv4";
	case 113:
		return (const UTF8Char*)"Linux";
	case 201:
		return (const UTF8Char*)"Bluetooth";
	}
	return 0;
}
