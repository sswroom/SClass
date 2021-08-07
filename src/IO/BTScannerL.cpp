#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"

IO::BTScanner *IO::BTScanner::CreateScanner()
{
	IO::RAWBTScanner *rawBT;
	NEW_CLASS(rawBT, IO::RAWBTScanner());
	if (!rawBT->IsError())
	{
		return rawBT;
	}
	DEL_CLASS(rawBT);

	IO::ProgCtrl::BluetoothCtlProgCtrl *bt;
	NEW_CLASS(bt, IO::ProgCtrl::BluetoothCtlProgCtrl());
	if (bt->WaitForCmdReady())
	{
		return bt;
	}
	DEL_CLASS(bt);
	return 0;
}