#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTMonitor.h"
#include "IO/RAWBTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/Thread.h"

//#include <stdio.h>

struct IO::RAWBTScanner::ClassData
{
	UOSInt devCnt;
	IO::RAWBTMonitor *btMon;
	RecordHandler hdlr;
	void *hdlrObj;
	Bool noCtrl;
	Bool threadRunning;
	Bool threadToStop;
	IO::ProgCtrl::BluetoothCtlProgCtrl *btCtrl;
};

UInt32 __stdcall IO::RAWBTScanner::RecvThread(void *userObj)
{
	IO::RAWBTScanner *me = (IO::RAWBTScanner*)userObj;
	UInt8 *buff;
	UOSInt packetSize;
	Int64 timeTicks;
	me->clsData->threadRunning = true;
	buff = MemAlloc(UInt8, me->clsData->btMon->GetMTU());
	while (!me->clsData->threadToStop)
	{
		packetSize = me->clsData->btMon->NextPacket(buff, &timeTicks);
		if (packetSize > 0)
		{
			me->OnPacket(timeTicks, buff, packetSize);
		}
	}
	MemFree(buff);
	me->clsData->threadRunning = false;
	return 0;
}

void IO::RAWBTScanner::FreeRec(IO::BTScanLog::ScanRecord3* rec)
{
	SDEL_STRING(rec->name);
	MemFree(rec);
}

IO::RAWBTScanner::RAWBTScanner(Bool noCtrl)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->noCtrl = noCtrl;
	this->clsData->btMon = 0;
	this->clsData->hdlr = 0;
	this->clsData->hdlrObj = 0;
	this->clsData->threadRunning = false;
	this->clsData->threadToStop = false;
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
	MemFree(this->clsData);
	const Data::ArrayList<IO::BTScanLog::ScanRecord3*> *recList;
	recList = this->pubRecMap.GetValues();
	LIST_CALL_FUNC(recList, this->FreeRec);
	recList = this->randRecMap.GetValues();
	LIST_CALL_FUNC(recList, this->FreeRec);
}

Bool IO::RAWBTScanner::IsError()
{
	return this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl == 0);
}

void IO::RAWBTScanner::HandleRecordUpdate(RecordHandler hdlr, void *userObj)
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
	if (this->clsData->devCnt == 0 || (!this->clsData->noCtrl && this->clsData->btCtrl == 0) || this->clsData->threadRunning)
	{
		return;
	}
	this->clsData->threadToStop = false;
	NEW_CLASS(this->clsData->btMon, IO::RAWBTMonitor(0));
	if (this->clsData->btMon->IsError())
	{
		DEL_CLASS(this->clsData->btMon);
		this->clsData->btMon = 0;
		return;
	}
	Sync::Thread::Create(RecvThread, this);
	if (this->clsData->btCtrl)
		this->clsData->btCtrl->ScanOn();
	while (!this->clsData->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
}

void IO::RAWBTScanner::ScanOff()
{
	if (this->clsData->threadRunning)
	{
		this->clsData->threadToStop = true;
		//printf("RAWBT scan off\r\n");
		this->clsData->btMon->Close();
		//printf("RAWBT btMon closed\r\n");
		if (this->clsData->btCtrl)
			this->clsData->btCtrl->ScanOff();
		//printf("RAWBT ctrl off\r\n");
		while (this->clsData->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
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

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *IO::RAWBTScanner::GetPublicMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->recMut);
	return &this->pubRecMap;	
}

Data::UInt64Map<IO::BTScanLog::ScanRecord3*> *IO::RAWBTScanner::GetRandomMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->recMut);
	return &this->randRecMap;	
}

void IO::RAWBTScanner::OnPacket(Int64 timeTicks, const UInt8 *packet, UOSInt packetSize)
{
	IO::BTScanLog::ScanRecord3 rec;
	if (IO::BTScanLog::ParseBTRAWPacket(&rec, timeTicks, packet, packetSize))
	{
		IO::BTScanLog::ScanRecord3 *dev;
		Sync::MutexUsage mutUsage(&this->recMut);
		if (rec.addrType == IO::BTScanLog::AT_RANDOM)
		{
			dev = this->randRecMap.Get(rec.macInt);
		}
		else
		{
			dev = this->pubRecMap.Get(rec.macInt);
		}
		if (dev == 0)
		{
			dev = MemAlloc(IO::BTScanLog::ScanRecord3, 1);
			MemCopyNO(dev, &rec, sizeof(IO::BTScanLog::ScanRecord3));
			dev->name = SCOPY_STRING(rec.name);
			if (rec.addrType == IO::BTScanLog::AT_RANDOM)
			{
				this->randRecMap.Put(dev->macInt, dev);
			}
			else
			{
				this->pubRecMap.Put(dev->macInt, dev);
			}
		}
		dev->lastSeenTime = rec.lastSeenTime;
		if (dev->company == 0 && rec.company != 0)
		{
			dev->company = rec.company;
		}
		if (dev->name == 0 && rec.name != 0)
		{
			dev->name = rec.name->Clone();
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
			this->clsData->hdlr(&rec, IO::BTScanner::UT_RSSI, this->clsData->hdlrObj);
		SDEL_STRING(rec.name);
	}
}