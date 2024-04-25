#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"

Optional<IO::BTScanner> IO::BTScanner::CreateScanner()
{
	NN<IO::RAWBTScanner> rawBT;
	NEW_CLASSNN(rawBT, IO::RAWBTScanner(false));
	if (!rawBT->IsError())
	{
		return rawBT;
	}
	rawBT.Delete();

	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> bt;
	NEW_CLASSNN(bt, IO::ProgCtrl::BluetoothCtlProgCtrl());
	if (bt->WaitForCmdReady())
	{
		return bt;
	}
	bt.Delete();
	return 0;
}