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
	UOSInt devCnt;
	IO::RAWBTMonitor *btMon;
	RecordHandler hdlr;
	AnyType hdlrObj;
	Bool noCtrl;
	IO::ProgCtrl::BluetoothCtlProgCtrl *btCtrl;
};

void __stdcall IO::RAWBTScanner::RecvThread(NN<Sync::Thread> thread)
{
	NN<IO::RAWBTScanner> me = thread->GetUserObj().GetNN<IO::RAWBTScanner>();
	UInt8 *buff;
	UOSInt packetSize;
	Int64 timeTicks;
	buff = MemAlloc(UInt8, me->clsData->btMon->GetMTU());
	while (!thread->IsStopping())
	{
		packetSize = me->clsData->btMon->NextPacket(buff, timeTicks);
		if (packetSize > 0)
		{
			me->OnPacket(timeTicks, Data::ByteArrayR(buff, packetSize));
		}
	}
	MemFree(buff);
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
	this->clsData->btMon = 0;
	this->clsData->hdlr = 0;
	this->clsData->hdlrObj = 0;
	this->clsData->btCtrl = 0;
	this->clsData->devCnt = IO::RAWBTMonitor::GetDevCount();
	if (this->clsData->devCnt > 0 && !this->clsData->noCtrl)
	{
		NEW_CLASS(this->clsData->btCtrl, IO::ProgCtrl::BluetoothCtlProgCtrl());
		if (!this->clsData->btCtrl->WaitForCmdReady())
		{
			DEL_CLASS(this->clsData->btCtrl);
			this->clsData->btCtrl = 0;
		}
	}
}

IO::RAWBTScanner::~RAWBTScanner()
{
	this->ScanOff();
	this->Close();
	SDEL_CLASS(this->clsData->btCtrl);
	MemFreeNN(this->clsData);
	this->pubRecMap.FreeAll(FreeRec);
	this->randRecMap.FreeAll(FreeRec);
}

Bool IO::RAWBTScanner::IsError()
{
	return this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl == 0);
}

void IO::RAWBTScanner::HandleRecordUpdate(RecordHandler hdlr, AnyType userObj)
{
	this->clsData->hdlrObj = userObj;
	this->clsData->hdlr = hdlr;
}

Bool IO::RAWBTScanner::IsScanOn()
{
	return this->clsData->btCtrl != 0 && this->clsData->btCtrl->IsScanOn();
}

void IO::RAWBTScanner::ScanOn()
{
	if (this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl == 0) || this->thread.IsRunning())
	{
		return;
	}
	NEW_CLASS(this->clsData->btMon, IO::RAWBTMonitor(0));
	if (this->clsData->btMon->IsError())
	{
		DEL_CLASS(this->clsData->btMon);
		this->clsData->btMon = 0;
		return;
	}
	if (this->thread.Start())
	{
		if (this->clsData->btCtrl)
			this->clsData->btCtrl->ScanOn();
	}
	else
	{
		DEL_CLASS(this->clsData->btMon);
		this->clsData->btMon = 0;
	}
}

void IO::RAWBTScanner::ScanOff()
{
	if (this->thread.IsRunning())
	{
		this->thread.BeginStop();
		//printf("RAWBT scan off\r\n");
		this->clsData->btMon->Close();
		//printf("RAWBT btMon closed\r\n");
		if (this->clsData->btCtrl)
			this->clsData->btCtrl->ScanOff();
		//printf("RAWBT ctrl off\r\n");
		this->thread.WaitForEnd();
		//printf("RAWBT thread stopped\r\n");
		DEL_CLASS(this->clsData->btMon);
		//printf("RAWBT released\r\n");
		this->clsData->btMon = 0;
	}
}

void IO::RAWBTScanner::Close()
{
	this->ScanOff();
	if (this->clsData->btCtrl)
	{
		this->clsData->btCtrl->Close();
	}
}

Bool IO::RAWBTScanner::SetScanMode(ScanMode scanMode)
{
	if (this->clsData->btCtrl)
	{
		return this->clsData->btCtrl->SetScanMode(scanMode);
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