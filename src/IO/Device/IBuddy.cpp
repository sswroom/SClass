#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/HIDInfo.h"
#include "IO/Path.h"
#include "IO/Device/IBuddy.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UIntOS IO::Device::IBuddy::GetNumDevice()
{
	Data::ArrayListNN<IO::HIDInfo> hidList;
	IO::HIDInfo::GetHIDList(hidList);
	NN<IO::HIDInfo> hid;
	UIntOS ret = 0;
	UIntOS i = 0;
	UIntOS j = hidList.GetCount();
	while (i < j)
	{
		hid = hidList.GetItemNoCheck(i);
		if (hid->GetBusType() == IO::HIDInfo::BT_USB && hid->GetVendorId() == 0x1130)
		{
			ret++;
		}
		hid.Delete();
		i++;
	}
	return ret;
}

IO::Device::IBuddy::IBuddy(UIntOS devNo)
{
	this->stm = nullptr;
	Data::ArrayListNN<IO::HIDInfo> hidList;
	IO::HIDInfo::GetHIDList(hidList);
	NN<IO::HIDInfo> hid;
	UIntOS ret = 0;
	UIntOS i = 0;
	UIntOS j = hidList.GetCount();
	while (i < j)
	{
		hid = hidList.GetItemNoCheck(i);
		if (hid->GetBusType() == IO::HIDInfo::BT_USB && hid->GetVendorId() == 0x1130)
		{
			if (ret == devNo)
			{
				this->stm = hid->OpenHID();
			}
			ret++;
		}
		hid.Delete();
		i++;
	}
}

IO::Device::IBuddy::~IBuddy()
{
	NN<IO::Stream> stm;
	if (this->stm.SetTo(stm))
	{
		if (lastEffect)
		{
			PlayEffect(IO::Device::IBuddy::IBBE_OFF, IO::Device::IBuddy::IBHDE_OFF, IO::Device::IBuddy::IBHRE_OFF, IO::Device::IBuddy::IBWE_OFF);
		}
		stm.Delete();
		this->stm = nullptr;
	}
}

Bool IO::Device::IBuddy::IsError()
{
	return this->stm.IsNull();
}

void IO::Device::IBuddy::PlayEffect(IBuddyBodyEffect be, IBuddyHeadEffect hde, IBuddyHeartEffect hre, IBuddyWingEffect we)
{
	UInt8 effects = (UInt8)(be | hde | hre | we);
	UInt8 buff[9];
	NN<IO::Stream> stm;
	buff[0] = 0x00;
	buff[1] = 0x55;
	buff[2] = 0x53;
	buff[3] = 0x42;
	buff[4] = 0x43;
	buff[5] = 0x00;
	buff[6] = 0x40;
	buff[7] = 0x02;
	buff[8] = (UInt8)(255 - effects);
	if (this->stm.SetTo(stm))
	{
		UIntOS retryCnt;
		retryCnt = 1;
		while (retryCnt-- > 0)
		{
			UIntOS writeSize = stm->Write(Data::ByteArrayR(buff, 9));
			if (writeSize == 9)
				break;
		}
		this->lastEffect = effects;
	}
}

