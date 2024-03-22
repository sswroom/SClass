#include "Stdafx.h"
#include "Net/RSSReader.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Net::RSSReader::RSSThread(AnyType userObj)
{
	NotNullPtr<Net::RSSReader> me = userObj.GetNN<Net::RSSReader>();
	Net::RSS *rss;
	RSSStatus *status;
	Data::DateTime *dt;
	UOSInt i;
	UOSInt cnt;
	Text::String *id;

	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	while (!me->threadToStop)
	{
		dt->SetCurrTimeUTC();
		if (dt->CompareTo(me->nextDT) > 0)
		{
			NEW_CLASS(rss, Net::RSS(me->url->ToCString(), 0, me->sockf, me->ssl, me->timeout, me->log));
			if (rss->IsError())
			{
				DEL_CLASS(rss);
				me->nextDT.SetCurrTimeUTC();
				me->nextDT.AddSecond(300);
			}
			else
			{
				me->nextDT.SetCurrTimeUTC();
				me->nextDT.AddSecond((OSInt)me->refreshSecond);

				i = me->currRSSMaps->GetCount();
				while (i-- > 0)
				{
					me->currRSSMaps->GetItem(i)->exist = false;
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
				i = me->currRSSMaps->GetCount();
				while (i-- > 0)
				{
					status = me->currRSSMaps->GetItem(i);
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

Net::RSSReader::RSSReader(Text::CString url, NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt32 refreshSecond, Net::RSSHandler *hdlr, Data::Duration timeout, NotNullPtr<IO::LogTool> log)
{
	this->url = Text::String::New(url);
	this->timeout = timeout;
	this->sockf = sockf;
	this->ssl = ssl;
	this->refreshSecond = refreshSecond;
	this->hdlr = hdlr;
	this->log = log;
	this->lastRSS = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->currRSSMaps, Data::FastStringMap<RSSStatus*>());
	NEW_CLASS(this->threadEvt, Sync::Event(true));
	this->nextDT.SetCurrTimeUTC();
	Sync::ThreadUtil::Create(RSSThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::RSSReader::~RSSReader()
{
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (this->lastRSS)
	{
		DEL_CLASS(this->lastRSS);
		this->lastRSS = 0;
	}
	DEL_CLASS(this->threadEvt);
	this->url->Release();

	UOSInt i;
	RSSStatus *status;
	i = this->currRSSMaps->GetCount();
	while (i-- > 0)
	{
		status = this->currRSSMaps->GetItem(i);
		MemFree(status);
	}
	DEL_CLASS(this->currRSSMaps);

}

Bool Net::RSSReader::IsError()
{
	return !threadRunning;
}
