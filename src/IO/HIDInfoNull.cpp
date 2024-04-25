#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

struct IO::HIDInfo::ClassData
{

};

IO::HIDInfo::HIDInfo(ClassData *clsData)
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

Text::String *IO::HIDInfo::GetDevPath()
{
	return Text::String::NewEmpty().Ptr();
}

IO::Stream *IO::HIDInfo::OpenHID()
{
	return 0;
}

OSInt IO::HIDInfo::GetHIDList(NN<Data::ArrayList<HIDInfo*>> hidList)
{
	return 0;
}