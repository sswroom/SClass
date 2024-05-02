#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/BTController.h"
#include "IO/BTUtil.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include <errno.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

typedef struct
{
	int devId;
	int dd;
	UInt8 hciVer;
	UInt16 hciRev;
	UInt8 lmpVer;
} BTControllerInfo;

typedef struct
{
	BTControllerInfo *ctrlInfo;
	UInt8 addr[6];
	NN<Text::String> name;
} BTDeviceInfo;

IO::BTController::BTDevice::BTDevice(void *internalData, void *hRadio, void *devInfo)
{
	Char name[256];
	BTDeviceInfo *dev = MemAlloc(BTDeviceInfo, 1);
	MemCopyNO(dev, devInfo, sizeof(BTDeviceInfo));
	if (hci_read_remote_name(dev->ctrlInfo->dd, (bdaddr_t *)dev->addr, 256, name, 2000) >= 0)
	{
		dev->name = Text::String::NewNotNullSlow((const UTF8Char*)name);
	}
	else
	{
		dev->name = Text::String::NewEmpty();
	}
	this->devInfo = (UInt8*)dev;
}

IO::BTController::BTDevice::~BTDevice()
{
	BTDeviceInfo *dev = (BTDeviceInfo*)this->devInfo;
	dev->name->Release();
	MemFree(this->devInfo);
}

NN<Text::String> IO::BTController::BTDevice::GetName() const
{
	BTDeviceInfo *dev = (BTDeviceInfo*)this->devInfo;
	return dev->name;
}

UInt8 *IO::BTController::BTDevice::GetAddress()
{
	BTDeviceInfo *dev = (BTDeviceInfo*)this->devInfo;
	return dev->addr;
}

UInt32 IO::BTController::BTDevice::GetDevClass()
{
	return 0; 
}

Bool IO::BTController::BTDevice::IsConnected()
{
	return false;
}

Bool IO::BTController::BTDevice::IsRemembered()
{
	return false;
}

Bool IO::BTController::BTDevice::IsAuthenticated()
{
	return false;
}

void IO::BTController::BTDevice::GetLastSeen(Data::DateTime *dt)
{
	dt->SetTicks(0);
}

void IO::BTController::BTDevice::GetLastUsed(Data::DateTime *dt)
{
	dt->SetTicks(0);
}

Bool IO::BTController::BTDevice::Pair(const UTF8Char *key)
{
	///////////////////////////////
	return false;
}

Bool IO::BTController::BTDevice::Unpair()
{
	///////////////////////////////
	return false;
}

UOSInt IO::BTController::BTDevice::QueryServices(NN<Data::ArrayListNN<Data::UUID>> guidList)
{
	///////////////////////////////
	return 0;
}

void IO::BTController::BTDevice::FreeServices(NN<Data::ArrayListNN<Data::UUID>> guidList)
{
	///////////////////////////////
}

Bool IO::BTController::BTDevice::EnableService(NN<Data::UUID> guid, Bool toEnable)
{
	///////////////////////////////
	return false;
}

UInt32 __stdcall IO::BTController::LEScanThread(AnyType userObj)
{
	NN<IO::BTController> me = userObj.GetNN<IO::BTController>();
	BTControllerInfo *info = (BTControllerInfo*)me->hand;
	UInt8 buf[HCI_MAX_EVENT_SIZE];
	UInt8 *ptr;
	struct hci_filter nf;
	struct hci_filter of;
	socklen_t olen;
//	hci_event_hdr *hdr;
	ssize_t len;

	me->leScanning = true;

	olen = sizeof(of);
	if (getsockopt(info->dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0)
	{
		me->leScanToStop = true;
	}
	else
	{
		hci_filter_clear(&nf);
		hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
		hci_filter_set_event(EVT_LE_META_EVENT, &nf);
		if (setsockopt(info->dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
		{
			me->leScanToStop = true;
		}

		while (!me->leScanToStop)
		{
			evt_le_meta_event *meta;
			le_advertising_info *ainfo;
			while ((len = read(info->dd, buf, sizeof(buf))) < 0)
			{
				if (errno == EAGAIN || errno == EINTR)
				{

				}
				else
				{
					me->leScanToStop = true;
					break;
				}
			}
			if (!me->leScanToStop)
			{
//				hdr = (hci_event_hdr *) (buf + 1);
				ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
				len -= (1 + HCI_EVENT_HDR_SIZE);
				meta = (evt_le_meta_event *) ptr;
				if (meta->subevent != 0x02)
				{
					me->leScanToStop = true;
				}
				else
				{
					ainfo = (le_advertising_info *) (meta->data + 1);
					if (me->leHdlr)
					{
						UTF8Char sbuff[64];
						Int32 rssi = 0;
						Text::CString name = CSTR_NULL;
						UInt8 len = ainfo->length;
						UInt8 eirSize;
						OSInt ofst = 0;
						while (ofst < len)
						{
							eirSize = ainfo->data[ofst];
							if (eirSize <= 1 || eirSize + ofst + 1 > len)
							{
								break;
							}
							switch (ainfo->data[ofst + 1])
							{
							case 8: //EIR_NAME_SHORT
								if (name.leng == 0)
								{
									name = CSTRP(sbuff, Text::StrConcatC(sbuff, (const UTF8Char*)&ainfo->data[ofst + 2], (UOSInt)eirSize - 1));
								}
								break;
							case 9: //EIR_NAME_COMPLETE
								name = CSTRP(sbuff, Text::StrConcatC(sbuff, (const UTF8Char*)&ainfo->data[ofst + 2], (UOSInt)eirSize - 1));
								break;
							case 10: //EIR_TX_POWER
								break;
							}
							ofst += eirSize;
						}
						rssi = (Int8)ainfo->data[len];
						me->leHdlr(me->leHdlrObj, IO::BTUtil::GetAddrMAC(ainfo->bdaddr.b), rssi, name);
					}
				}
			}
		}

		setsockopt(info->dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));
	}

	hci_le_set_scan_enable(info->dd, 0x00, 0x00, 1000);
	me->leScanning = false;
	return 0;
}

IO::BTController::BTController(void *internalData, void *hand)
{
	char name[256];
	BTControllerInfo *info;
	info = MemAlloc(BTControllerInfo, 1);
	info->devId = (int)(OSInt)hand;
	info->dd = hci_open_dev(info->devId);
	info->hciVer = 0;
	info->hciRev = 0;
	info->lmpVer = 0;
	this->hand = info;
	this->leScanning = false;
	this->leScanToStop = false;
	this->leHdlr = 0;
	this->leHdlrObj = 0;

	UInt8 cls[3];
	bdaddr_t addr;
	hci_version ver;
	hci_devba(info->devId, &addr);

	this->addr[0] = addr.b[0];
	this->addr[1] = addr.b[1];
	this->addr[2] = addr.b[2];
	this->addr[3] = addr.b[3];
	this->addr[4] = addr.b[4];
	this->addr[5] = addr.b[5];
	this->devClass = 0;
	this->subversion = 0;
	this->manufacturer = 0;
	if (info->dd)
	{
		if (hci_read_local_name(info->dd, 256, name, 1000) >= 0)
		{
			this->name = Text::String::NewNotNullSlow((const UTF8Char*)name);
		}
		else
		{
			this->name = Text::String::NewEmpty();
		}
		if (hci_read_local_version(info->dd, &ver, 1000) >= 0)
		{
			this->manufacturer = ver.manufacturer;
			this->subversion = ver.lmp_subver;
			info->hciVer = ver.hci_ver;
			info->hciRev = ver.hci_rev;
			info->lmpVer = ver.lmp_ver;
		}
		if (hci_read_class_of_dev(info->dd, cls, 1000) >= 0)
		{
			this->devClass = ReadUInt24(cls);
		}
	}
	else
	{
		this->name = Text::String::NewEmpty();
	}
}

IO::BTController::~BTController()
{
	if (this->leScanning)
	{
		this->LEScanEnd();
	}

	BTControllerInfo *info = (BTControllerInfo*)this->hand;
	if (info->dd)
	{
		hci_close_dev(info->dd);
	}
	MemFree(info);
	this->name->Release();
}

OSInt IO::BTController::CreateDevices(NN<Data::ArrayListNN<BTDevice>> devList, Bool toSearch)
{
	BTControllerInfo *info = (BTControllerInfo*)this->hand;
	OSInt ret = 0;
	OSInt i;
	inquiry_info *ii;
	BTDeviceInfo devInfo;
	NN<BTDevice> dev;
	if (toSearch)
	{
		devInfo.ctrlInfo = info;
		ii = MemAlloc(inquiry_info, 255);
		ret = hci_inquiry(info->devId, 8, 255, 0, &ii, 0);
		i = 0;
		while (i < ret)
		{
			devInfo.addr[0] = ii[i].bdaddr.b[0];
			devInfo.addr[1] = ii[i].bdaddr.b[1];
			devInfo.addr[2] = ii[i].bdaddr.b[2];
			devInfo.addr[3] = ii[i].bdaddr.b[3];
			devInfo.addr[4] = ii[i].bdaddr.b[4];
			devInfo.addr[5] = ii[i].bdaddr.b[5];
			NEW_CLASSNN(dev, BTDevice(0, 0, &devInfo));
			devList->Add(dev);
			i++;
		}
		MemFree(ii);
	}
	return ret;
}

UInt8 *IO::BTController::GetAddress()
{
	return this->addr;
}

NN<Text::String> IO::BTController::GetName() const
{
	return this->name;
}

UInt32 IO::BTController::GetDevClass()
{
	return this->devClass;
}

UInt16 IO::BTController::GetManufacturer()
{
	return this->manufacturer;
}

UInt16 IO::BTController::GetSubversion()
{
	return this->subversion;
}

void IO::BTController::LEScanHandleResult(LEScanHandler leHdlr, AnyType leHdlrObj)
{
	this->leHdlrObj = leHdlrObj;
	this->leHdlr = leHdlr;
}

Bool IO::BTController::LEScanBegin()
{
	if (this->leScanning)
	{
		return true;
	}
	this->leScanToStop = false;
	BTControllerInfo *info = (BTControllerInfo*)this->hand;
	int err;
	err = hci_le_set_scan_parameters(info->dd, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 1000);
	if (err < 0)
	{
		printf("Error in setting params\r\n");
		return false;
	}
	err = hci_le_set_scan_enable(info->dd, 0x01, 0x01, 1000);
	if (err < 0)
	{
		printf("Error in enabling scanning\r\n");
		return false;
	}
	Sync::ThreadUtil::Create(LEScanThread, this);
	while (!this->leScanning && !this->leScanToStop)
	{
		Sync::SimpleThread::Sleep(1);
	}
	return true;
}

Bool IO::BTController::LEScanEnd()
{
	if (this->leScanning)
	{
		this->leScanToStop = true;
		while (this->leScanning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	return true;
}
