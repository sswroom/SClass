#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ActiveStreamReader.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall IO::ActiveStreamReader::ReadThread(void *obj)
{
	IO::ActiveStreamReader *me = (IO::ActiveStreamReader *)obj;
	Int32 i = 0;
	BottleNeckType *bnt;
	me->running = true;
	while (!me->toStop)
	{
		while (me->buffs[i].buffSize > 0)
		{
			me->hdlr(me->buffs[i].buff, me->buffs[i].buffSize, me->userData);
			me->buffs[i].buffSize = 0;
			me->emptyEvt.Set();
			i = (i + 1) % ACTIVESTREAMREADER_BUFFCNT;
		}
		bnt = me->bnt;
		if (bnt && me->reading)
		{
			*bnt = IO::ActiveStreamReader::BottleNeckType::Read;
		}
		me->fullEvt.Wait(1000);
	}
	me->running = false;
	me->emptyEvt.Set();
	return 0;
}

IO::ActiveStreamReader::ActiveStreamReader(DataHdlr hdlr, void *userData, IO::Stream *stm, UOSInt buffSize)
{
	this->hdlr = hdlr;
	this->stm = stm;
	this->buffSize = buffSize;
	this->currIndex = 0;
	this->userData = userData;
	this->running = false;
	this->toStop = false;
	this->reading = false;

	Int32 i = ACTIVESTREAMREADER_BUFFCNT;
	while (i-- > 0)
	{
		buffs[i].buff = MemAllocA(UInt8, buffSize);
		buffs[i].buffSize = 0;
	}
	Sync::ThreadUtil::Create(ReadThread, this);
}

IO::ActiveStreamReader::~ActiveStreamReader()
{
	this->toStop = true;
	this->fullEvt.Set();
	while (this->running)
	{
		this->emptyEvt.Wait(1000);
	}
	Int32 i = ACTIVESTREAMREADER_BUFFCNT;
	while (i-- > 0)
	{
		MemFreeA(this->buffs[i].buff);
	}
}

void IO::ActiveStreamReader::ReadStream(IO::ActiveStreamReader::BottleNeckType *bnt)
{
	Int32 i = this->currIndex;
	UOSInt readSize = this->buffSize;
	UOSInt actSize;
	IO::Stream *stm = this->stm;
	this->bnt = bnt;
	this->reading = true;
	while (true)
	{
		while (this->buffs[i].buffSize)
		{
			if (bnt)
			{
				*bnt = IO::ActiveStreamReader::BottleNeckType::Write;
			}
			this->emptyEvt.Wait(1000);
		}
		actSize = stm->Read(Data::ByteArray(this->buffs[i].buff, readSize));
		if (actSize <= 0)
			break;
		this->buffs[i].buffSize = actSize;
		this->fullEvt.Set();
		i = (i + 1) % ACTIVESTREAMREADER_BUFFCNT;
	}
	this->reading = false;
	this->bnt = 0;
	
	i = (i + ACTIVESTREAMREADER_BUFFCNT - 1) % ACTIVESTREAMREADER_BUFFCNT;
	while (this->buffs[i].buffSize)
	{
		this->emptyEvt.Wait(1000);
	}
	this->currIndex = (i + 1) % ACTIVESTREAMREADER_BUFFCNT;
}
