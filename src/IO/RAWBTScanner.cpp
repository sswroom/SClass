#include "Stdafx.h"
#include "IO/BTScanner.h"
#include "IO/RAWBTScanner.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/Thread.h"
#include <pcap/pcap.h>
#include <unistd.h>

struct IO::RAWBTScanner::ClassData
{
	pcap_t *pcap;
	RecordHandler hdlr;
	void *hdlrObj;
	Bool threadRunning;
	Bool threadToStop;
	IO::ProgCtrl::BluetoothCtlProgCtrl *btCtrl;
};

void RAWBTScanner_Packet(u_char *user, const struct pcap_pkthdr *h, const u_char *bytes)
{
	IO::RAWBTScanner *me = (IO::RAWBTScanner*)user;
	me->OnPacket(1000 * (Int64)h->ts.tv_sec + h->ts.tv_usec / 1000, bytes, h->len);
}

UInt32 __stdcall IO::RAWBTScanner::RecvThread(void *userObj)
{
	IO::RAWBTScanner *me = (IO::RAWBTScanner*)userObj;
	me->clsData->threadRunning = true;
	while (!me->clsData->threadToStop)
	{
		pcap_loop(me->clsData->pcap, 0, RAWBTScanner_Packet, (u_char*)me);
	}
	me->clsData->threadRunning = false;
	return 0;
}

IO::RAWBTScanner::RAWBTScanner()
{
	NEW_CLASS(this->recMap, Data::UInt64Map<IO::BTScanLog::ScanRecord*>());
	NEW_CLASS(this->recMut, Sync::Mutex());
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->pcap = 0;
	this->clsData->hdlr = 0;
	this->clsData->hdlrObj = 0;
	this->clsData->threadRunning = false;
	this->clsData->threadToStop = false;
	this->clsData->btCtrl = 0;
	Char errbuf[PCAP_ERRBUF_SIZE];
	if (geteuid() == 0)
	{
		this->clsData->pcap = pcap_create("bluetooth0", errbuf);
		if (this->clsData->pcap)
		{
			NEW_CLASS(this->clsData->btCtrl, IO::ProgCtrl::BluetoothCtlProgCtrl());
			if (!this->clsData->btCtrl->WaitForCmdReady())
			{
				DEL_CLASS(this->clsData->btCtrl);
				this->clsData->btCtrl = 0;
			}
		}
	}
}

IO::RAWBTScanner::~RAWBTScanner()
{
	this->ScanOff();
	this->Close();
	SDEL_CLASS(this->clsData->btCtrl);
	MemFree(this->clsData);
	DEL_CLASS(this->recMut);
	Data::ArrayList<IO::BTScanLog::ScanRecord*> *recList = this->recMap->GetValues();
	IO::BTScanLog::ScanRecord *rec;
	UOSInt i = recList->GetCount();
	while (i-- > 0)
	{
		rec = recList->GetItem(i);
		SDEL_TEXT(rec->name);
		MemFree(rec);
	}
	DEL_CLASS(this->recMap);
}

Bool IO::RAWBTScanner::IsError()
{
	return this->clsData->pcap == 0 || this->clsData->btCtrl == 0;
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
	if (this->clsData->pcap == 0 || this->clsData->btCtrl == 0 || this->clsData->threadRunning)
	{
		return;
	}
	this->clsData->threadToStop = false;
	int ret = pcap_activate(this->clsData->pcap);
	if (ret != 0)
	{
		return;
	}
	Sync::Thread::Create(RecvThread, this);
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
		pcap_breakloop(this->clsData->pcap);
		this->clsData->btCtrl->ScanOff();
		while (this->clsData->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

void IO::RAWBTScanner::Close()
{
	if (this->clsData->pcap)
	{
		pcap_close(this->clsData->pcap);
		this->clsData->pcap = 0;
	}
	if (this->clsData->btCtrl)
	{
		this->clsData->btCtrl->Close();
	}
}

Bool IO::RAWBTScanner::SetScanMode(ScanMode scanMode)
{
	return this->clsData->btCtrl->SetScanMode(scanMode);
}

Data::UInt64Map<IO::BTScanLog::ScanRecord*> *IO::RAWBTScanner::GetRecordMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->recMut);
	return this->recMap;	
}

void IO::RAWBTScanner::OnPacket(Int64 timeTicks, const UInt8 *packet, UOSInt packetSize)
{
	IO::BTScanLog::ScanRecord rec;
	if (IO::BTScanLog::ParseBTRAWPacket(&rec, timeTicks, packet, packetSize))
	{
		IO::BTScanLog::ScanRecord *dev;
		Sync::MutexUsage mutUsage(this->recMut);
		dev = this->recMap->Get(rec.macInt);
		if (dev == 0)
		{
			dev = MemAlloc(IO::BTScanLog::ScanRecord, 1);
			MemCopyNO(dev, &rec, sizeof(IO::BTScanLog::ScanRecord));
			dev->name = SCOPY_TEXT(rec.name);
			this->recMap->Put(dev->macInt, dev);
		}
		dev->lastSeenTime = rec.lastSeenTime;
		if (dev->company == 0 && rec.company != 0)
		{
			dev->company = rec.company;
		}
		if (dev->name == 0 && rec.name != 0)
		{
			dev->name = Text::StrCopyNew(rec.name);
		}
		dev->inRange = rec.inRange;
		dev->connected = rec.connected;
		dev->rssi = rec.rssi;
		dev->txPower = rec.txPower;
		mutUsage.EndUse();
		
		if (this->clsData->hdlr)
			this->clsData->hdlr(&rec, IO::BTScanner::UT_RSSI, this->clsData->hdlrObj);
		SDEL_TEXT(rec.name);
	}
}