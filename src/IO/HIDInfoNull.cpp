#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

IO::HIDInfo::HIDInfo(void *clsData)
{
}

IO::HIDInfo::~HIDInfo()
{
}

IO::HIDInfo::BusType IO::HIDInfo::GetBusType()
{
	return IO::HIDInfo::BT_UNKNOWN;
}

UInt16 IO::HIDInfo::GetVendorId()
{
	return 0;
}

UInt16 IO::HIDInfo::GetProductId()
{
	return 0;
}

const UTF8Char *IO::HIDInfo::GetDevPath()
{
	return (const UTF8Char*)"";
}

IO::Stream *IO::HIDInfo::OpenHID()
{
	return 0;
}

OSInt IO::HIDInfo::GetHIDList(Data::ArrayList<HIDInfo*> *hidList)
{
	return 0;
}