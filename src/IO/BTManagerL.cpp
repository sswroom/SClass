#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BTManager.h"
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

IO::BTManager::BTManager()
{

}

IO::BTManager::~BTManager()
{
	
}

UOSInt IO::BTManager::CreateControllers(NotNullPtr<Data::ArrayListNN<IO::BTController>> ctrlList)
{
	UOSInt ret = 0;
	NotNullPtr<IO::BTController> btCtrl;
	int i = 0;
	int res;
	hci_dev_info di;
	while (true)
	{
		res = hci_devinfo(i, &di);
		if (res < 0)
			break;
		NEW_CLASSNN(btCtrl, IO::BTController(0, (void*)(OSInt)i));
		ctrlList->Add(btCtrl);
		ret++;
		i++;
	}
	return ret;
}
