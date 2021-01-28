#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "IO/Device/IBuddy.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

OSInt IO::Device::IBuddy::GetNumDevice()
{
	Data::ArrayList<IO::HIDInfo *> hidList;
	IO::HIDInfo::GetHIDList(&hidList);
	IO::HIDInfo *hid;
	OSInt ret = 0;
	OSInt i = 0;
	OSInt j = hidList.GetCount();
	while (i < j)
	{
		hid = hidList.GetItem(i);
		if (hid->GetBusType() == IO::HIDInfo::BT_USB && hid->GetVendorId() == 0x1130)
		{
			ret++;
		}
		DEL_CLASS(hid);
		i++;
	}
	return ret;
}

IO::Device::IBuddy::IBuddy(OSInt devNo)
{
	this->stm = 0;
	Data::ArrayList<IO::HIDInfo *> hidList;
	IO::HIDInfo::GetHIDList(&hidList);
	IO::HIDInfo *hid;
	OSInt ret = 0;
	OSInt i = 0;
	OSInt j = hidList.GetCount();
	while (i < j)
	{
		hid = hidList.GetItem(i);
		if (hid->GetBusType() == IO::HIDInfo::BT_USB && hid->GetVendorId() == 0x1130)
		{
			if (ret == devNo)
			{
				this->stm = hid->OpenHID();
			}
			ret++;
		}
		DEL_CLASS(hid);
		i++;
	}
}

IO::Device::IBuddy::~IBuddy()
{
	if (this->stm)
	{
		if (lastEffect)
		{
			PlayEffect(IO::Device::IBuddy::IBBE_OFF, IO::Device::IBuddy::IBHDE_OFF, IO::Device::IBuddy::IBHRE_OFF, IO::Device::IBuddy::IBWE_OFF);
		}
		DEL_CLASS(this->stm);
		this->stm = 0;
	}
}

Bool IO::Device::IBuddy::IsError()
{
	return this->stm == 0;
}

void IO::Device::IBuddy::PlayEffect(IBuddyBodyEffect be, IBuddyHeadEffect hde, IBuddyHeartEffect hre, IBuddyWingEffect we)
{
	UInt8 effects = (UInt8)(be | hde | hre | we);
	UInt8 buff[9];
	buff[0] = 0x00;
	buff[1] = 0x55;
	buff[2] = 0x53;
	buff[3] = 0x42;
	buff[4] = 0x43;
	buff[5] = 0x00;
	buff[6] = 0x40;
	buff[7] = 0x02;
	buff[8] = 255 - effects;
	if (this->stm)
	{
		OSInt retryCnt;
		retryCnt = 1;
		while (retryCnt-- > 0)
		{
			OSInt writeSize = this->stm->Write(buff, 9);
			if (writeSize == 9)
				break;
		}
		this->lastEffect = effects;
	}
}

