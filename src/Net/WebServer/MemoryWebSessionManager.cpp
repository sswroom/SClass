#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::WebServer::MemoryWebSessionManager::CheckThread(void *userObj)
{
	Net::WebServer::MemoryWebSessionManager *me = (Net::WebServer::MemoryWebSessionManager *)userObj;
	Data::DateTime *lastChkTime;
	Data::DateTime *currTime;
	Net::WebServer::MemoryWebSession *sess;
	Int64 sessId;
	UOSInt i;
	OSInt j;

	NEW_CLASS(lastChkTime, Data::DateTime());
	NEW_CLASS(currTime, Data::DateTime());
	lastChkTime->SetCurrTimeUTC();

	me->chkRunning = true;
	while (!me->chkToStop)
	{
		currTime->SetCurrTimeUTC();
		if (currTime->DiffMS(lastChkTime) >= me->chkInterval)
		{
			lastChkTime->SetCurrTimeUTC();

			i = me->sesses->GetCount();
			while (i-- > 0)
			{
				Sync::MutexUsage mutUsage(me->mut);
				sess = me->sesses->GetItem(i);
				sessId = me->sessIds->GetItem(i);
				mutUsage.EndUse();
				if (sess)
				{
					Bool toDel;
					sess->BeginUse();
					toDel = me->chkHdlr(sess, me->chkHdlrObj);
					sess->EndUse();
					if (toDel)
					{
						mutUsage.BeginUse();
						j = me->sessIds->SortedIndexOf(sessId);
						me->sessIds->RemoveAt((UOSInt)j);
						sess = me->sesses->RemoveAt((UOSInt)j);
						mutUsage.EndUse();

						sess->BeginUse();
						me->delHdlr(sess, me->delHdlrObj);
						sess->EndUse();
						DEL_CLASS(sess);
					}
				}
			}
		}
		me->chkEvt->Wait(1000);
	}
	DEL_CLASS(currTime);
	DEL_CLASS(lastChkTime);
	me->chkRunning = false;
	return 0;
}

Int64 Net::WebServer::MemoryWebSessionManager::GetSessId(Net::WebServer::IWebRequest *req)
{
	UTF8Char *sbuff;
	UTF8Char *strs[2];
	UOSInt strCnt = 2;
	Int64 sessId = 0;

	Text::String *cookie = req->GetSHeader((const UTF8Char*)"Cookie");
	if (cookie == 0)
	{
		return 0;
	}
	sbuff = MemAlloc(UTF8Char, cookie->leng + 1);
	cookie->ConcatTo(sbuff);

	strs[1] = sbuff;
	while (strCnt >= 2)
	{
		strCnt = Text::StrSplitTrim(strs, 2, strs[1], ';');
		if (Text::StrStartsWith(strs[0], (const UTF8Char*)"WebSessId="))
		{
			sessId = Text::StrToInt64(&strs[0][10]);
			break;
		}
	}

	MemFree(sbuff);
	return sessId;
}

Net::WebServer::MemoryWebSessionManager::MemoryWebSessionManager(const UTF8Char *path, SessionHandler delHdlr, void *delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, void *chkHdlrObj) : Net::WebServer::IWebSessionManager(delHdlr, delHdlrObj)
{
	this->path = Text::StrCopyNew(path);
	this->chkInterval = chkInterval;
	this->chkHdlr = chkHdlr;
	this->chkHdlrObj = chkHdlrObj;
	NEW_CLASS(this->sessIds, Data::ArrayListInt64());
	NEW_CLASS(this->sesses, Data::ArrayList<Net::WebServer::MemoryWebSession*>());
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->chkEvt, Sync::Event(true, (const UTF8Char*)"Net.WebServer.MemoryWebSessionManager.chkEvt"));
	this->chkToStop = false;
	this->chkRunning = false;
	Sync::Thread::Create(CheckThread, this);
	while (!this->chkRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::WebServer::MemoryWebSessionManager::~MemoryWebSessionManager()
{
	Net::WebServer::MemoryWebSession *sess;

	this->chkToStop = true;
	this->chkEvt->Set();
	while (this->chkRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->chkEvt);

	UOSInt i = this->sesses->GetCount();
	while (i-- > 0)
	{
		sess = this->sesses->GetItem(i);
		this->delHdlr(sess, this->delHdlrObj);
		DEL_CLASS(sess);
	}
	DEL_CLASS(mut);
	DEL_CLASS(sesses);
	DEL_CLASS(sessIds);
	Text::StrDelNew(this->path);
}

Net::WebServer::IWebSession *Net::WebServer::MemoryWebSessionManager::GetSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Int64 sessId = this->GetSessId(req);
	if (sessId == 0)
		return 0;
	Net::WebServer::MemoryWebSession *sess = (Net::WebServer::MemoryWebSession*)this->GetSession(sessId);
	if (sess)
	{
		if (!sess->RequestValid(req->GetBrowser(), req->GetOS()))
		{
			sess->EndUse();
			sess = 0;
		}
	}
	return sess;
}

Net::WebServer::IWebSession *Net::WebServer::MemoryWebSessionManager::CreateSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::WebServer::IWebSession *sess;
	sess = this->GetSession(req, resp);
	if (sess)
		return sess;
	Int64 sessId = this->GenSessId(req);
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"WebSessId=");
	sptr = Text::StrInt64(sptr, sessId);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"; Path=");
	sptr = Text::StrConcat(sptr, this->path);
	resp->AddHeader((const UTF8Char*)"Set-Cookie", sbuff);
	UOSInt i;
	NEW_CLASS(sess, Net::WebServer::MemoryWebSession(sessId, req->GetBrowser(), req->GetOS()));
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds->SortedInsert(sessId);
	this->sesses->Insert(i, (Net::WebServer::MemoryWebSession*)sess);
	mutUsage.EndUse();
	((Net::WebServer::MemoryWebSession*)sess)->BeginUse();
	return sess;
}

void Net::WebServer::MemoryWebSessionManager::DeleteSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	Int64 sessId = GetSessId(req);
	OSInt i;
	Net::WebServer::MemoryWebSession *sess;
	if (sessId != 0)
	{
		sess = 0;
		Sync::MutexUsage mutUsage(this->mut);
		i = this->sessIds->SortedIndexOf(sessId);
		if (i >= 0)
		{
			sess = this->sesses->RemoveAt((UOSInt)i);
			this->sessIds->RemoveAt((UOSInt)i);
		}
		mutUsage.EndUse();
		if (sess)
		{
			sess->BeginUse();
			this->delHdlr(sess, this->delHdlrObj);
			sess->EndUse();
			DEL_CLASS(sess);
		}

		sptr = Text::StrConcat(sbuff, (const UTF8Char*)"WebSessId=");
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"; Path=");
		sptr = Text::StrConcat(sptr, this->path);
		sptr = Text::StrConcat(sptr, (const UTF8Char*)"; Expires=");
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		dt.AddMonth(-12);
		sptr = resp->ToTimeString(sptr, &dt);
		resp->AddHeader((const UTF8Char*)"Set-Cookie", sbuff);
	}
}

Int64 Net::WebServer::MemoryWebSessionManager::GenSessId(Net::WebServer::IWebRequest *req)
{
	Data::DateTime dt;
	UInt8 buff[8];
	buff[0] = 0;
	buff[1] = 0;
	*(UInt16*)&buff[2] = req->GetClientPort();
	const Net::SocketUtil::AddressInfo *addr = req->GetClientAddr();
	if (addr->addrType == Net::AddrType::IPv4)
	{
		*(UInt32*)&buff[4] = *(UInt32*)addr->addr;
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		Crypto::Hash::CRC32R crc;
		crc.Calc(addr->addr, 16);
		crc.GetValue(&buff[4]);
	}
	dt.SetCurrTimeUTC();
	return dt.ToTicks() + *(Int64*)buff;
}

Net::WebServer::IWebSession *Net::WebServer::MemoryWebSessionManager::CreateSession(Int64 sessId)
{
	OSInt si;
	Net::WebServer::MemoryWebSession *sess = 0;
	Sync::MutexUsage mutUsage(this->mut);
	si = this->sessIds->SortedIndexOf(sessId);
	if (si < 0)
	{
		NEW_CLASS(sess, Net::WebServer::MemoryWebSession(sessId, Net::BrowserInfo::BT_UNKNOWN, Manage::OSInfo::OT_UNKNOWN));
		UOSInt i = this->sessIds->SortedInsert(sessId);
		this->sesses->Insert(i, (Net::WebServer::MemoryWebSession*)sess);
	}
	mutUsage.EndUse();
	((Net::WebServer::MemoryWebSession*)sess)->BeginUse();
	return sess;
}

Net::WebServer::IWebSession *Net::WebServer::MemoryWebSessionManager::GetSession(Int64 sessId)
{
	Net::WebServer::IWebSession *sess;
	OSInt i;
	sess = 0;
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds->SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses->GetItem((UOSInt)i);
	}
	mutUsage.EndUse();
	if (sess)
	{
		((Net::WebServer::MemoryWebSession*)sess)->BeginUse();
	}
	return sess;
}

void Net::WebServer::MemoryWebSessionManager::DeleteSession(Int64 sessId)
{
	OSInt i;
	Net::WebServer::MemoryWebSession *sess;
	sess = 0;
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds->SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses->RemoveAt((UOSInt)i);
		this->sessIds->RemoveAt((UOSInt)i);
	}
	mutUsage.EndUse();
	if (sess)
	{
		sess->BeginUse();
		this->delHdlr(sess, this->delHdlrObj);
		sess->EndUse();
		DEL_CLASS(sess);
	}
}

void Net::WebServer::MemoryWebSessionManager::GetSessionIds(Data::ArrayList<Int64> *sessIds)
{
	Sync::MutexUsage mutUsage(this->mut);
	sessIds->AddAll(this->sessIds);
	mutUsage.EndUse();
}
