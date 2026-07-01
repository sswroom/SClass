#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTMonitor.h"
#include "IO/RAWBTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

//#include <stdio.h>

struct IO::RAWBTScanner::ClassData
{
	UIntOS devCnt;
	Optional<IO::RAWBTMonitor> btMon;
	RecordHandler hdlr;
	AnyType hdlrObj;
	Bool noCtrl;
	Optional<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
};

void __stdcall IO::RAWBTScanner::RecvThread(NN<Sync::Thread> thread)
{
	NN<IO::RAWBTScanner> me = thread->GetUserObj().GetNN<IO::RAWBTScanner>();
	NN<IO::RAWBTMonitor> btMon;
	UnsafeArray<UInt8> buff;
	UIntOS packetSize;
	Int64 timeTicks;
	if (me->clsData->btMon.SetTo(btMon))
	{
		buff = MemAllocArr(UInt8, btMon->GetMTU());
		while (!thread->IsStopping())
		{
			packetSize = btMon->NextPacket(buff, timeTicks);
			if (packetSize > 0)
			{
				me->OnPacket(timeTicks, Data::ByteArrayR(buff, packetSize));
			}
		}
		MemFreeArr(buff);
	}
}

void IO::RAWBTScanner::FreeRec(NN<IO::BTScanLog::ScanRecord3> rec)
{
	OPTSTR_DEL(rec->name);
	MemFreeNN(rec);
}

IO::RAWBTScanner::RAWBTScanner(Bool noCtrl) : thread(RecvThread, this, CSTR("RAWBTScanner"))
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->noCtrl = noCtrl;
	this->clsData->btMon = nullptr;
	this->clsData->hdlr = 0;
	this->clsData->hdlrObj = 0;
	this->clsData->btCtrl = nullptr;
	this->clsData->devCnt = IO::RAWBTMonitor::GetDevCount();
	if (this->clsData->devCnt > 0 && !this->clsData->noCtrl)
	{
		NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
		NEW_CLASSNN(btCtrl, IO::ProgCtrl::BluetoothCtlProgCtrl());
		if (!btCtrl->WaitForCmdReady())
		{
			btCtrl.Delete();
		}
		else
		{
			this->clsData->btCtrl = btCtrl;
		}
	}
}

IO::RAWBTScanner::~RAWBTScanner()
{
	this->ScanOff();
	this->Close();
	this->clsData->btCtrl.Delete();
	MemFreeNN(this->clsData);
	this->pubRecMap.FreeAll(FreeRec);
	this->randRecMap.FreeAll(FreeRec);
}

Bool IO::RAWBTScanner::IsError()
{
	return this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl.IsNull());
}

void IO::RAWBTScanner::HandleRecordUpdate(RecordHandler hdlr, AnyType userObj)
{
	this->clsData->hdlrObj = userObj;
	this->clsData->hdlr = hdlr;
}

Bool IO::RAWBTScanner::IsScanOn()
{
	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
	return this->clsData->btCtrl.SetTo(btCtrl) && btCtrl->IsScanOn();
}

void IO::RAWBTScanner::ScanOn()
{
	if (this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl.IsNull()) || this->thread.IsRunning())
	{
		return;
	}
	NN<IO::RAWBTMonitor> btMon;
	NEW_CLASSNN(btMon, IO::RAWBTMonitor(0));
	if (btMon->IsError())
	{
		btMon.Delete();
		return;
	}
	this->clsData->btMon = btMon;
	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
	if (this->thread.Start())
	{
		if (this->clsData->btCtrl.SetTo(btCtrl))
			btCtrl->ScanOn();
	}
	else
	{
		this->clsData->btMon.Delete();
	}
}

void IO::RAWBTScanner::ScanOff()
{
	NN<IO::RAWBTMonitor> btMon;
	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
	if (this->thread.IsRunning() && this->clsData->btMon.SetTo(btMon))
	{
		this->thread.BeginStop();
		//printf("RAWBT scan off\r\n");
		btMon->Close();
		//printf("RAWBT btMon closed\r\n");
		if (this->clsData->btCtrl.SetTo(btCtrl))
			btCtrl->ScanOff();
		//printf("RAWBT ctrl off\r\n");
		this->thread.WaitForEnd();
		//printf("RAWBT thread stopped\r\n");
		this->clsData->btMon.Delete();
		//printf("RAWBT released\r\n");
	}
}

void IO::RAWBTScanner::Close()
{
	this->ScanOff();
	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
	if (this->clsData->btCtrl.SetTo(btCtrl))
	{
		btCtrl->Close();
	}
}

Bool IO::RAWBTScanner::SetScanMode(ScanMode scanMode)
{
	NN<IO::ProgCtrl::BluetoothCtlProgCtrl> btCtrl;
	if (this->clsData->btCtrl.SetTo(btCtrl))
	{
		return btCtrl->SetScanMode(scanMode);
	}
	return false;
}

NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> IO::RAWBTScanner::GetPublicMap(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->recMut);
	return this->pubRecMap;	
}

NN<Data::FastMapNN<UInt64, IO::BTScanLog::ScanRecord3>> IO::RAWBTScanner::GetRandomMap(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->recMut);
	return this->randRecMap;	
}

void IO::RAWBTScanner::OnPacket(Int64 timeTicks, Data::ByteArrayR packet)
{
	IO::BTScanLog::ScanRecord3 rec;
	if (IO::BTScanLog::ParseBTRAWPacket(rec, timeTicks, packet))
	{
		Optional<IO::BTScanLog::ScanRecord3> optdev;
		NN<IO::BTScanLog::ScanRecord3> dev;
		Sync::MutexUsage mutUsage(this->recMut);
		if (rec.addrType == IO::BTScanLog::AT_RANDOM)
		{
			optdev = this->randRecMap.Get(rec.mac64Int);
		}
		else
		{
			optdev = this->pubRecMap.Get(rec.mac64Int);
		}
		if (!optdev.SetTo(dev))
		{
			dev = MemAllocNN(IO::BTScanLog::ScanRecord3);
			dev.CopyFrom(rec);
			dev->name = Text::String::CopyOrNull(rec.name);
			if (rec.addrType == IO::BTScanLog::AT_RANDOM)
			{
				this->randRecMap.Put(dev->mac64Int, dev);
			}
			else
			{
				this->pubRecMap.Put(dev->mac64Int, dev);
			}
		}
		dev->lastSeenTime = rec.lastSeenTime;
		if (dev->company == 0 && rec.company != 0)
		{
			dev->company = rec.company;
		}
		NN<Text::String> name;
		if (dev->name.IsNull() && rec.name.SetTo(name))
		{
			dev->name = name->Clone();
		}
		dev->inRange = rec.inRange;
		dev->connected = rec.connected;
		dev->rssi = rec.rssi;
		dev->txPower = rec.txPower;
		if (rec.measurePower != 0)
		{
			dev->measurePower = rec.measurePower;
		}
		mutUsage.EndUse();
		
		if (this->clsData->hdlr)
			this->clsData->hdlr(rec, IO::BTScanner::UT_RSSI, this->clsData->hdlrObj);
		OPTSTR_DEL(rec.name);
	}
}