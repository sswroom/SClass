#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTManager.h"
#include "IO/Library.h"
#include "Sync/Interlocked.h"

#include <windows.h>
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#define NTDDI_VERSION NTDDI_WINXPSP2
#include <bthsdpdef.h>
#include <bluetoothapis.h>

struct IO::BTManager::ClassData
{
	NN<IO::Library> lib;
	Int32 useCnt;
};

typedef HBLUETOOTH_RADIO_FIND (__stdcall *BluetoothFindFirstRadioFunc)(BLUETOOTH_FIND_RADIO_PARAMS *pbtfrp, HANDLE *phRadio);
typedef BOOL (__stdcall *BluetoothFindNextRadioFunc)(HBLUETOOTH_RADIO_FIND hFind, HANDLE *phRadio);
typedef BOOL (__stdcall *BluetoothFindRadioCloseFunc)(HBLUETOOTH_RADIO_FIND hFind);

IO::BTManager::BTManager()
{
	NN<ClassData> me = MemAllocNN(ClassData);
	this->internalData = me;
	NEW_CLASSNN(me->lib, IO::Library((const UTF8Char*)"Bthprops.cpl"));
	me->useCnt = 1;
}

IO::BTManager::~BTManager()
{
	NN<ClassData> me = this->internalData;
	if (Sync::Interlocked::DecrementI32(me->useCnt) <= 0)
	{
		me->lib.Delete();
		MemFreeNN(me);
	}
}

UIntOS IO::BTManager::CreateControllers(NN<Data::ArrayListNN<IO::BTController>> ctrlList)
{
	NN<ClassData> me = this->internalData;
	BluetoothFindFirstRadioFunc FindFirst = (BluetoothFindFirstRadioFunc)me->lib->GetFunc("BluetoothFindFirstRadio");
	BluetoothFindNextRadioFunc FindNext = (BluetoothFindNextRadioFunc)me->lib->GetFunc("BluetoothFindNextRadio");
	BluetoothFindRadioCloseFunc FindClose = (BluetoothFindRadioCloseFunc)me->lib->GetFunc("BluetoothFindRadioClose");
	if (FindFirst == 0 || FindNext == 0 || FindClose == 0)
		return 0;

	UIntOS ret = 0;
	BLUETOOTH_FIND_RADIO_PARAMS frp;
	HANDLE hand;
	NN<IO::BTController> btCtrl;
	frp.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
	HBLUETOOTH_RADIO_FIND hSrch = FindFirst(&frp, &hand);
	if (hSrch == 0)
	{
		return 0;
	}
	while (true)
	{
		NEW_CLASSNN(btCtrl, IO::BTController(me.Ptr(), hand));
		ctrlList->Add(btCtrl);
		ret++;
		if (!FindNext(hSrch, &hand))
		{
			break;
		}
	}
	FindClose(hSrch);
	return ret;
}
