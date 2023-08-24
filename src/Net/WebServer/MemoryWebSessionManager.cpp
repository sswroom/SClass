#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::WebServer::MemoryWebSessionManager::CheckThread(void *userObj)
{
	Net::WebServer::MemoryWebSessionManager *me = (Net::WebServer::MemoryWebSessionManager *)userObj;
	Net::WebServer::MemoryWebSession *sess;
	Int64 sessId;
	UOSInt i;
	OSInt j;

	{
		Data::DateTime lastChkTime;
		Data::DateTime currTime;
		lastChkTime.SetCurrTimeUTC();

		me->chkRunning = true;
		while (!me->chkToStop)
		{
			currTime.SetCurrTimeUTC();
			if (currTime.DiffMS(lastChkTime) >= me->chkInterval)
			{
				lastChkTime.SetCurrTimeUTC();

				i = me->sesses.GetCount();
				while (i-- > 0)
				{
					Sync::MutexUsage mutUsage(me->mut);
					sess = me->sesses.GetItem(i);
					sessId = me->sessIds.GetItem(i);
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
							j = me->sessIds.SortedIndexOf(sessId);
							me->sessIds.RemoveAt((UOSInt)j);
							sess = me->sesses.RemoveAt((UOSInt)j);
							mutUsage.EndUse();

							sess->BeginUse();
							me->delHdlr(sess, me->delHdlrObj);
							sess->EndUse();
							DEL_CLASS(sess);
						}
					}
				}
			}
			me->chkEvt.Wait(1000);
		}
	}
	me->chkRunning = false;
	return 0;
}

Int64 Net::WebServer::MemoryWebSessionManager::GetSessId(Net::WebServer::IWebRequest *req)
{
	UTF8Char *sbuff;
	Text::PString strs[2];
	UOSInt strCnt = 2;
	Int64 sessId = 0;

	Text::String *cookie = req->GetSHeader(CSTR("Cookie"));
	if (cookie == 0)
	{
		return 0;
	}
	sbuff = MemAlloc(UTF8Char, cookie->leng + 1);
	cookie->ConcatTo(sbuff);

	strs[1].v = sbuff;
	strs[1].leng = cookie->leng;
	while (strCnt >= 2)
	{
		strCnt = Text::StrSplitTrimP(strs, 2, strs[1], ';');
		if (Text::StrStartsWithC(strs[0].v, strs[0].leng, this->cookieName->v, this->cookieName->leng) && strs[0].v[this->cookieName->leng] == '=')
		{
			sessId = Text::StrToInt64(&strs[0].v[this->cookieName->leng + 1]);
			break;
		}
	}

	MemFree(sbuff);
	return sessId;
}

Net::WebServer::MemoryWebSessionManager::MemoryWebSessionManager(Text::CString path, SessionHandler delHdlr, void *delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, void *chkHdlrObj, Text::CString cookieName) : Net::WebServer::IWebSessionManager(delHdlr, delHdlrObj)
{
	this->path = Text::String::New(path);
	if (cookieName.leng == 0)
		this->cookieName = Text::String::New(UTF8STRC("WebSessId"));
	else
		this->cookieName = Text::String::New(cookieName);
	this->chkInterval = chkInterval;
	this->chkHdlr = chkHdlr;
	this->chkHdlrObj = chkHdlrObj;
	this->chkToStop = false;
	this->chkRunning = false;
	Sync::ThreadUtil::Create(CheckThread, this);
	while (!this->chkRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::WebServer::MemoryWebSessionManager::~MemoryWebSessionManager()
{
	Net::WebServer::MemoryWebSession *sess;

	this->chkToStop = true;
	this->chkEvt.Set();
	while (this->chkRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}

	UOSInt i = this->sesses.GetCount();
	while (i-- > 0)
	{
		sess = this->sesses.GetItem(i);
		this->delHdlr(sess, this->delHdlrObj);
		DEL_CLASS(sess);
	}
	this->path->Release();
	this->cookieName->Release();
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
	sptr = Text::StrInt64(sbuff, sessId);
	resp->AddSetCookie(this->cookieName->ToCString(), CSTRP(sbuff, sptr), this->path->ToCString(), true, req->IsSecure(), Net::WebServer::SameSiteType::Strict, 0);
	UOSInt i;
	NEW_CLASS(sess, Net::WebServer::MemoryWebSession(sessId, req->GetBrowser(), req->GetOS()));
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds.SortedInsert(sessId);
	this->sesses.Insert(i, (Net::WebServer::MemoryWebSession*)sess);
	mutUsage.EndUse();
	((Net::WebServer::MemoryWebSession*)sess)->BeginUse();
	return sess;
}

void Net::WebServer::MemoryWebSessionManager::DeleteSession(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	Int64 sessId = GetSessId(req);
	OSInt i;
	Net::WebServer::MemoryWebSession *sess;
	if (sessId != 0)
	{
		sess = 0;
		Sync::MutexUsage mutUsage(this->mut);
		i = this->sessIds.SortedIndexOf(sessId);
		if (i >= 0)
		{
			sess = this->sesses.RemoveAt((UOSInt)i);
			this->sessIds.RemoveAt((UOSInt)i);
		}
		mutUsage.EndUse();
		if (sess)
		{
			sess->BeginUse();
			this->delHdlr(sess, this->delHdlrObj);
			sess->EndUse();
			DEL_CLASS(sess);
		}
		resp->AddSetCookie(this->cookieName->ToCString(), CSTR(""), this->path->ToCString(), true, req->IsSecure(), Net::WebServer::SameSiteType::Strict, Data::Timestamp::UtcNow().AddMonth(-12));
	}
}

Int64 Net::WebServer::MemoryWebSessionManager::GenSessId(Net::WebServer::IWebRequest *req)
{
	Data::DateTime dt;
	UInt8 buff[8];
	buff[0] = 0;
	buff[1] = 0;
	*(UInt16*)&buff[2] = req->GetClientPort();
	NotNullPtr<const Net::SocketUtil::AddressInfo> addr = req->GetClientAddr();
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
	si = this->sessIds.SortedIndexOf(sessId);
	if (si < 0)
	{
		NEW_CLASS(sess, Net::WebServer::MemoryWebSession(sessId, Net::BrowserInfo::BT_UNKNOWN, Manage::OSInfo::OT_UNKNOWN));
		UOSInt i = this->sessIds.SortedInsert(sessId);
		this->sesses.Insert(i, (Net::WebServer::MemoryWebSession*)sess);
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
	i = this->sessIds.SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses.GetItem((UOSInt)i);
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
	i = this->sessIds.SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses.RemoveAt((UOSInt)i);
		this->sessIds.RemoveAt((UOSInt)i);
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
