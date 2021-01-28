#include "Stdafx.h"
#include "Net/RSSReader.h"
#include "Sync/Thread.h"

UInt32 __stdcall Net::RSSReader::RSSThread(void *userObj)
{
	Net::RSSReader *me = (Net::RSSReader*)userObj;
	Net::RSS *rss;
	Data::ArrayList<RSSStatus *> *vals;
	RSSStatus *status;
	Data::DateTime *dt;
	OSInt i;
	OSInt cnt;
	const UTF8Char *id;

	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	while (!me->threadToStop)
	{
		dt->SetCurrTimeUTC();
		if (dt->CompareTo(me->nextDT) > 0)
		{
			NEW_CLASS(rss, Net::RSS(me->url, 0, me->sockf));
			if (rss->IsError())
			{
				DEL_CLASS(rss);
				me->nextDT->SetCurrTimeUTC();
				me->nextDT->AddSecond(300);
			}
			else
			{
				me->nextDT->SetCurrTimeUTC();
				me->nextDT->AddSecond(me->refreshSecond);

				vals = me->currRSSMaps->GetValues();
				i = vals->GetCount();
				while (i-- > 0)
				{
					vals->GetItem(i)->exist = false;
				}
				i = 0;
				cnt = rss->GetCount();
				while (i < cnt)
				{
					Net::RSSItem *item = rss->GetItem(i);
					id = item->GetId();

					status = me->currRSSMaps->Get(id);
					if (status)
					{
						status->exist = true;
						status->item = item;
					}
					else
					{
						status = MemAlloc(RSSStatus, 1);
						status->exist = true;
						status->item = item;
						me->currRSSMaps->Put(id, status);
						me->hdlr->ItemAdded(item);
					}
					
					i++;
				}
				vals = me->currRSSMaps->GetValues();
				i = vals->GetCount();
				while (i-- > 0)
				{
					status = vals->GetItem(i);
					if (!status->exist)
					{
						me->currRSSMaps->Remove(status->item->GetId());
						me->hdlr->ItemRemoved(status->item);
						MemFree(status);
					}
				}

				if (me->lastRSS)
				{
					DEL_CLASS(me->lastRSS);
				}
				me->lastRSS = rss;
			}
		}

		me->threadEvt->Wait(1000);
	}
	DEL_CLASS(dt);
	me->threadRunning = false;
	return false;
}

Net::RSSReader::RSSReader(const UTF8Char *url, Net::SocketFactory *sockf, Int32 refreshSecond, Net::RSSHandler *hdlr)
{
	this->url = Text::StrCopyNew(url);
	this->sockf = sockf;
	this->refreshSecond = refreshSecond;
	this->hdlr = hdlr;
	this->lastRSS = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->currRSSMaps, Data::StringUTF8Map<RSSStatus*>());
	NEW_CLASS(this->nextDT, Data::DateTime());
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Net.RSSReader.threadEvt"));
	this->nextDT->SetCurrTimeUTC();
	Sync::Thread::Create(RSSThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::RSSReader::~RSSReader()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	if (this->lastRSS)
	{
		DEL_CLASS(this->lastRSS);
		this->lastRSS = 0;
	}
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->nextDT);
	Text::StrDelNew(this->url);

	Data::ArrayList<RSSStatus*> *vals;
	OSInt i;
	RSSStatus *status;
	vals = this->currRSSMaps->GetValues();
	i = vals->GetCount();
	while (i-- > 0)
	{
		status = vals->GetItem(i);
		MemFree(status);
	}
	DEL_CLASS(this->currRSSMaps);

}

Bool Net::RSSReader::IsError()
{
	return !threadRunning;
}
