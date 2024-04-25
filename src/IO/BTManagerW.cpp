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

typedef struct
{
	IO::Library *lib;
	Int32 useCnt;
} InternalData;

typedef HBLUETOOTH_RADIO_FIND (__stdcall *BluetoothFindFirstRadioFunc)(BLUETOOTH_FIND_RADIO_PARAMS *pbtfrp, HANDLE *phRadio);
typedef BOOL (__stdcall *BluetoothFindNextRadioFunc)(HBLUETOOTH_RADIO_FIND hFind, HANDLE *phRadio);
typedef BOOL (__stdcall *BluetoothFindRadioCloseFunc)(HBLUETOOTH_RADIO_FIND hFind);

IO::BTManager::BTManager()
{
	InternalData *me = MemAlloc(InternalData, 1);
	this->internalData = me;
	NEW_CLASS(me->lib, IO::Library((const UTF8Char*)"Bthprops.cpl"));
	me->useCnt = 1;
}

IO::BTManager::~BTManager()
{
	InternalData *me = (InternalData*)this->internalData;
	if (Sync::Interlocked::DecrementI32(me->useCnt) <= 0)
	{
		DEL_CLASS(me->lib);
		MemFree(me);
	}
}

UOSInt IO::BTManager::CreateControllers(NN<Data::ArrayListNN<IO::BTController>> ctrlList)
{
	InternalData *me = (InternalData*)this->internalData;
	BluetoothFindFirstRadioFunc FindFirst = (BluetoothFindFirstRadioFunc)me->lib->GetFunc("BluetoothFindFirstRadio");
	BluetoothFindNextRadioFunc FindNext = (BluetoothFindNextRadioFunc)me->lib->GetFunc("BluetoothFindNextRadio");
	BluetoothFindRadioCloseFunc FindClose = (BluetoothFindRadioCloseFunc)me->lib->GetFunc("BluetoothFindRadioClose");
	if (FindFirst == 0 || FindNext == 0 || FindClose == 0)
		return 0;

	UOSInt ret = 0;
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
		NEW_CLASSNN(btCtrl, IO::BTController(me, hand));
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
