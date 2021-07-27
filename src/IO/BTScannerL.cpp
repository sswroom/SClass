#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"

IO::BTScanner *IO::BTScanner::CreateScanner()
{
	IO::ProgCtrl::BluetoothCtlProgCtrl *bt;
	NEW_CLASS(bt, IO::ProgCtrl::BluetoothCtlProgCtrl());
	if (bt->WaitForCmdReady())
	{
		return bt;
	}
	DEL_CLASS(bt);
	return 0;
}