#include "Stdafx.h"
#include "Net/RSSReader.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Net::RSSReader::RSSThread(AnyType userObj)
{
	NN<Net::RSSReader> me = userObj.GetNN<Net::RSSReader>();
	Net::RSS *rss;
	NN<RSSStatus> status;
	Data::DateTime *dt;
	UOSInt i;
	UOSInt cnt;
	NN<Text::String> id;

	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	while (!me->threadToStop)
	{
		dt->SetCurrTimeUTC();
		if (dt->CompareTo(me->nextDT) > 0)
		{
			NEW_CLASS(rss, Net::RSS(me->url->ToCString(), 0, me->clif, me->ssl, me->timeout, me->log));
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

				i = me->currRSSMaps.GetCount();
				while (i-- > 0)
				{
					me->currRSSMaps.GetItemNoCheck(i)->exist = false;
				}
				i = 0;
				cnt = rss->GetCount();
				while (i < cnt)
				{
					NN<Net::RSSItem> item = rss->GetItemNoCheck(i);
					id = Text::String::OrEmpty(item->GetId());

					if (me->currRSSMaps.Get(id).SetTo(status))
					{
						status->exist = true;
						status->item = item;
					}
					else
					{
						status = MemAllocNN(RSSStatus);
						status->exist = true;
						status->item = item;
						me->currRSSMaps.Put(id, status);
						me->hdlr->ItemAdded(item);
					}
					
					i++;
				}
				i = me->currRSSMaps.GetCount();
				while (i-- > 0)
				{
					status = me->currRSSMaps.GetItemNoCheck(i);
					if (!status->exist)
					{
						me->currRSSMaps.Remove(status->item->GetId());
						me->hdlr->ItemRemoved(status->item);
						MemFreeNN(status);
					}
				}

				me->lastRSS.Delete();
				me->lastRSS = rss;
			}
		}

		me->threadEvt.Wait(1000);
	}
	DEL_CLASS(dt);
	me->threadRunning = false;
	return false;
}

Net::RSSReader::RSSReader(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt32 refreshSecond, NN<Net::RSSHandler> hdlr, Data::Duration timeout, NN<IO::LogTool> log)
{
	this->url = Text::String::New(url);
	this->timeout = timeout;
	this->clif = clif;
	this->ssl = ssl;
	this->refreshSecond = refreshSecond;
	this->hdlr = hdlr;
	this->log = log;
	this->lastRSS = 0;
	this->threadRunning = false;
	this->threadToStop = false;
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
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->lastRSS.Delete();
	this->url->Release();

	UOSInt i;
	NN<RSSStatus> status;
	i = this->currRSSMaps.GetCount();
	while (i-- > 0)
	{
		status = this->currRSSMaps.GetItemNoCheck(i);
		MemFreeNN(status);
	}
}

Bool Net::RSSReader::IsError()
{
	return !threadRunning;
}
