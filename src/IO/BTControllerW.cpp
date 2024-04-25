#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTController.h"
#include "IO/Library.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>

#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#define NTDDI_VERSION NTDDI_WINXPSP2
#include <bthsdpdef.h>
#include <bluetoothapis.h>
//#include <BluetoothLEAPIs.h>
#if !defined(BLUETOOTH_SERVICE_ENABLE)
#define BLUETOOTH_SERVICE_ENABLE 0x01
#define BLUETOOTH_SERVICE_DISABLE 0x00
#endif

//https://social.msdn.microsoft.com/Forums/en-US/bad452cb-4fc2-4a86-9b60-070b43577cc9/is-there-a-simple-example-desktop-programming-c-for-bluetooth-low-energy-devices?forum=wdk

typedef struct
{
	IO::Library *lib;
	Int32 useCnt;
} InternalData;

typedef HBLUETOOTH_DEVICE_FIND (__stdcall *BluetoothFindFirstDeviceFunc)(BLUETOOTH_DEVICE_SEARCH_PARAMS *pbtsp, BLUETOOTH_DEVICE_INFO *pbtdi);
typedef BOOL (__stdcall *BluetoothFindNextDeviceFunc)(HBLUETOOTH_DEVICE_FIND hFind, BLUETOOTH_DEVICE_INFO  *pbtdi);
typedef BOOL (__stdcall *BluetoothFindDeviceCloseFunc)(HBLUETOOTH_DEVICE_FIND hFind);
typedef DWORD (__stdcall *BluetoothGetRadioInfoFunc)(HANDLE hRadio, PBLUETOOTH_RADIO_INFO pRadioInfo);
typedef DWORD (__stdcall *BluetoothSetServiceStateFunc)(HANDLE hRadio, BLUETOOTH_DEVICE_INFO *pbtdi, GUID *pGuidService, DWORD dwServiceFlags);
typedef DWORD (__stdcall *BluetoothAuthenticateDeviceFunc)(HWND hwndParent, HANDLE hRadio, BLUETOOTH_DEVICE_INFO *pbtdi, PWCHAR pszPasskey, ULONG ulPasskeyLength);
typedef DWORD (__stdcall *BluetoothRemoveDeviceFunc)(BLUETOOTH_ADDRESS *pAddress);
typedef DWORD (__stdcall *BluetoothEnumerateInstalledServicesFunc)(HANDLE hRadio, BLUETOOTH_DEVICE_INFO *pbtdi, DWORD *pcServices, GUID *pGuidServices);

IO::BTController::BTDevice::BTDevice(void *internalData, void *hRadio, void *devInfo)
{
	InternalData *me = (InternalData*)internalData;
	Sync::Interlocked::IncrementI32(me->useCnt);
	this->internalData = internalData;
	this->hRadio = hRadio;
	this->devInfo = MemAlloc(UInt8, sizeof(BLUETOOTH_DEVICE_INFO));
	this->clsData = 0;
	MemCopyNO(this->devInfo, devInfo, sizeof(BLUETOOTH_DEVICE_INFO));
}

IO::BTController::BTDevice::~BTDevice()
{
	InternalData *me = (InternalData*)this->internalData;
	if (Sync::Interlocked::DecrementI32(me->useCnt) <= 0)
	{
		DEL_CLASS(me->lib);
		MemFree(me);
	}
	MemFree(this->devInfo);
	if (this->clsData)
	{
		((Text::String*)this->clsData)->Release();
		this->clsData = 0;
	}
}

NN<Text::String> IO::BTController::BTDevice::GetName() const
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	if (this->clsData == 0)
	{
		((IO::BTController::BTDevice*)this)->clsData = (void*)Text::String::NewNotNull(dev->szName).Ptr();
	}
	return Text::String::OrEmpty((Text::String*)this->clsData);
}

UInt8 *IO::BTController::BTDevice::GetAddress()
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return dev->Address.rgBytes;
}

UInt32 IO::BTController::BTDevice::GetDevClass()
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return dev->ulClassofDevice;
}

Bool IO::BTController::BTDevice::IsConnected()
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return dev->fConnected != 0;
}

Bool IO::BTController::BTDevice::IsRemembered()
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return dev->fRemembered != 0;
}

Bool IO::BTController::BTDevice::IsAuthenticated()
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return dev->fAuthenticated != 0;
}

void IO::BTController::BTDevice::GetLastSeen(Data::DateTime *dt)
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	dt->SetValueSYSTEMTIME(&dev->stLastSeen);
}

void IO::BTController::BTDevice::GetLastUsed(Data::DateTime *dt)
{
	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	dt->SetValueSYSTEMTIME(&dev->stLastUsed);
}

Bool IO::BTController::BTDevice::Pair(const UTF8Char *key)
{
	InternalData *me = (InternalData*)internalData;
	BluetoothAuthenticateDeviceFunc AuthDev = (BluetoothAuthenticateDeviceFunc)me->lib->GetFunc("BluetoothAuthenticateDevice");
	if (AuthDev == 0)
		return false;

	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	const WChar *wptr = Text::StrToWCharNew(key);
	Bool ret = (ERROR_SUCCESS == AuthDev(0, this->hRadio, dev, (PWSTR)wptr, (ULONG)Text::StrCharCnt(wptr)));
	Text::StrDelNew(key);
	return ret;
}

Bool IO::BTController::BTDevice::Unpair()
{
	InternalData *me = (InternalData*)internalData;
	BluetoothRemoveDeviceFunc RemoveDev = (BluetoothRemoveDeviceFunc)me->lib->GetFunc("BluetoothRemoveDevice");
	if (RemoveDev == 0)
		return false;

	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return ERROR_SUCCESS == RemoveDev(&dev->Address);
}

UOSInt IO::BTController::BTDevice::QueryServices(Data::ArrayList<void*> *guidList)
{
	InternalData *me = (InternalData*)internalData;
	BluetoothEnumerateInstalledServicesFunc EnumServices = (BluetoothEnumerateInstalledServicesFunc)me->lib->GetFunc("BluetoothEnumerateInstalledServices");
	if (EnumServices == 0)
		return false;

	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	GUID guids[16];
	void *guid;
	UInt32 i;
	DWORD nServices = 16;
	if (EnumServices((HANDLE)this->hRadio, dev, &nServices, guids) == ERROR_SUCCESS)
	{
		i = 0;
		while (i < nServices)
		{
			guid = MemAlloc(GUID, 1);
			MemCopyNO(guid, &guids[i], sizeof(GUID));
			guidList->Add(guid);
			i++;
		}
	}
	else
	{
		nServices = 0;
	}
	return nServices;
}

void IO::BTController::BTDevice::FreeServices(Data::ArrayList<void*> *guidList)
{
	UOSInt i = guidList->GetCount();
	while (i-- > 0)
	{
		MemFree(guidList->GetItem(i));
	}
	guidList->Clear();
}

Bool IO::BTController::BTDevice::EnableService(void *guid, Bool toEnable)
{
	InternalData *me = (InternalData*)internalData;
	BluetoothSetServiceStateFunc SetState = (BluetoothSetServiceStateFunc)me->lib->GetFunc("BluetoothSetServiceState");
	if (SetState == 0)
		return false;

	BLUETOOTH_DEVICE_INFO *dev = (BLUETOOTH_DEVICE_INFO*)this->devInfo;
	return ERROR_SUCCESS == SetState((HANDLE)this->hRadio, dev, (GUID*)guid, toEnable?BLUETOOTH_SERVICE_ENABLE:BLUETOOTH_SERVICE_DISABLE);
}

IO::BTController::BTController(void *internalData, void *hand)
{
	InternalData *me = (InternalData*)internalData;
	Sync::Interlocked::IncrementI32(me->useCnt);
	this->internalData = internalData;
	this->hand = hand;
	this->leScanning = false;
	this->leScanToStop = false;
	this->leHdlr = 0;
	this->leHdlrObj = 0;
	BluetoothGetRadioInfoFunc GetInfo = (BluetoothGetRadioInfoFunc)me->lib->GetFunc("BluetoothGetRadioInfo");

	BLUETOOTH_RADIO_INFO info;
	info.dwSize = sizeof(info);
	if (GetInfo != 0 && GetInfo((HANDLE)this->hand, &info) == ERROR_SUCCESS)
	{
		this->name = Text::String::NewNotNull(info.szName);
		this->addr[0] = info.address.rgBytes[0];
		this->addr[1] = info.address.rgBytes[1];
		this->addr[2] = info.address.rgBytes[2];
		this->addr[3] = info.address.rgBytes[3];
		this->addr[4] = info.address.rgBytes[4];
		this->addr[5] = info.address.rgBytes[5];
		this->devClass = info.ulClassofDevice;
		this->subversion = info.lmpSubversion;
		this->manufacturer = info.manufacturer;
	}
	else
	{
		this->name = Text::String::NewEmpty();
		this->addr[0] = 0;
		this->addr[1] = 0;
		this->addr[2] = 0;
		this->addr[3] = 0;
		this->addr[4] = 0;
		this->addr[5] = 0;
		this->devClass = 0;
		this->subversion = 0;
		this->manufacturer = 0;
	}
}

IO::BTController::~BTController()
{
	InternalData *me = (InternalData*)this->internalData;
	CloseHandle((HANDLE)this->hand);
	this->name->Release();
	if (Sync::Interlocked::DecrementI32(me->useCnt) <= 0)
	{
		DEL_CLASS(me->lib);
		MemFree(me);
	}
}

OSInt IO::BTController::CreateDevices(NN<Data::ArrayListNN<BTDevice>> devList, Bool toSearch)
{
	InternalData *me = (InternalData*)this->internalData;
	BluetoothFindFirstDeviceFunc FindFirst = (BluetoothFindFirstDeviceFunc)me->lib->GetFunc("BluetoothFindFirstDevice");
	BluetoothFindNextDeviceFunc FindNext = (BluetoothFindNextDeviceFunc)me->lib->GetFunc("BluetoothFindNextDevice");
	BluetoothFindDeviceCloseFunc FindClose = (BluetoothFindDeviceCloseFunc)me->lib->GetFunc("BluetoothFindDeviceCloseFunc");
	if (FindFirst == 0 || FindNext == 0 || FindClose == 0)
		return 0;

	OSInt ret = 0;
	BLUETOOTH_DEVICE_SEARCH_PARAMS dsp;
	BLUETOOTH_DEVICE_INFO devInfo;
	NN<IO::BTController::BTDevice> btDev;
	dsp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	dsp.fReturnAuthenticated = TRUE;
	dsp.fReturnRemembered = TRUE;
	dsp.fReturnUnknown = TRUE;
	dsp.fReturnConnected = TRUE;
	dsp.fIssueInquiry = toSearch?TRUE:FALSE;
	dsp.cTimeoutMultiplier = 10; //12.8s
	dsp.hRadio = (HANDLE)this->hand;
	devInfo.dwSize = sizeof(devInfo);

	HBLUETOOTH_DEVICE_FIND hSrch = FindFirst(&dsp, &devInfo);
	if (hSrch == 0)
	{
//		UInt32 err = GetLastError();
		return 0;
	}
	while (true)
	{
		NEW_CLASSNN(btDev, IO::BTController::BTDevice(this->internalData, this->hand, &devInfo));
		devList->Add(btDev);
		ret++;
		if (!FindNext(hSrch, &devInfo))
		{
			break;
		}
	}
	FindClose(hSrch);
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
	this->leHdlr = leHdlr;
	this->leHdlrObj = leHdlrObj;
}

Bool IO::BTController::LEScanBegin()
{
	return false;
}

Bool IO::BTController::LEScanEnd()
{
	return false;
}
