#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Net/WebServer/MemoryWebSessionManager.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::WebServer::MemoryWebSessionManager::CheckThread(AnyType userObj)
{
	NN<Net::WebServer::MemoryWebSessionManager> me = userObj.GetNN<Net::WebServer::MemoryWebSessionManager>();
	Optional<Net::WebServer::MemoryWebSession> sess;
	NN<MemoryWebSession> nnsess;
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
					if (sess.SetTo(nnsess))
					{
						Bool toDel;
						nnsess->BeginUse();
						toDel = me->chkHdlr(nnsess, me->chkHdlrObj);
						nnsess->EndUse();
						if (toDel)
						{
							mutUsage.BeginUse();
							j = me->sessIds.SortedIndexOf(sessId);
							me->sessIds.RemoveAt((UOSInt)j);
							sess = me->sesses.RemoveAt((UOSInt)j);
							mutUsage.EndUse();

							nnsess->BeginUse();
							me->delHdlr(nnsess, me->delHdlrObj);
							nnsess->EndUse();
							nnsess.Delete();
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

Int64 Net::WebServer::MemoryWebSessionManager::GetSessId(NN<Net::WebServer::IWebRequest> req)
{
	UnsafeArray<UTF8Char> sbuff;
	Text::PString strs[2];
	UOSInt strCnt = 2;
	Int64 sessId = 0;

	NN<Text::String> cookie;
	if (!req->GetSHeader(CSTR("Cookie")).SetTo(cookie))
	{
		return 0;
	}
	sbuff = MemAllocArr(UTF8Char, cookie->leng + 1);
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

	MemFreeArr(sbuff);
	return sessId;
}

Net::WebServer::MemoryWebSessionManager::MemoryWebSessionManager(Text::CStringNN path, SessionHandler delHdlr, AnyType delHdlrObj, Int32 chkInterval, SessionHandler chkHdlr, AnyType chkHdlrObj, Text::CString cookieName) : Net::WebServer::IWebSessionManager(delHdlr, delHdlrObj)
{
	this->path = Text::String::New(path);
	Text::CStringNN nncookieName;
	if (!cookieName.SetTo(nncookieName) || nncookieName.leng == 0)
		this->cookieName = Text::String::New(UTF8STRC("WebSessId"));
	else
		this->cookieName = Text::String::New(nncookieName);
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
	NN<Net::WebServer::MemoryWebSession> sess;

	this->chkToStop = true;
	this->chkEvt.Set();
	while (this->chkRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}

	Data::ArrayIterator<NN<MemoryWebSession>> it = this->sesses.Iterator();
	while (it.HasNext())
	{
		sess = it.Next();
		this->delHdlr(sess, this->delHdlrObj);
		sess.Delete();
	}
	this->path->Release();
	this->cookieName->Release();
}

Optional<Net::WebServer::IWebSession> Net::WebServer::MemoryWebSessionManager::GetSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp)
{
	Int64 sessId = this->GetSessId(req);
	if (sessId == 0)
		return 0;
	NN<Net::WebServer::MemoryWebSession> sess;
	if (Optional<Net::WebServer::MemoryWebSession>::ConvertFrom(this->GetSession(sessId)).SetTo(sess))
	{
		if (!sess->RequestValid(req->GetBrowser(), req->GetOS()))
		{
			sess->EndUse();
			return 0;
		}
		return NN<Net::WebServer::IWebSession>(sess);
	}
	return 0;
}

NN<Net::WebServer::IWebSession> Net::WebServer::MemoryWebSessionManager::CreateSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::WebServer::IWebSession> sess;
	if (this->GetSession(req, resp).SetTo(sess))
		return sess;
	Int64 sessId = this->GenSessId(req);
	sptr = Text::StrInt64(sbuff, sessId);
	resp->AddSetCookie(this->cookieName->ToCString(), CSTRP(sbuff, sptr), this->path->ToCString(), true, req->IsSecure(), Net::WebServer::SameSiteType::Strict, 0);
	UOSInt i;
	NEW_CLASSNN(sess, Net::WebServer::MemoryWebSession(sessId, req->GetBrowser(), req->GetOS()));
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds.SortedInsert(sessId);
	this->sesses.Insert(i, NN<Net::WebServer::MemoryWebSession>::ConvertFrom(sess));
	mutUsage.EndUse();
	NN<Net::WebServer::MemoryWebSession>::ConvertFrom(sess)->BeginUse();
	return sess;
}

void Net::WebServer::MemoryWebSessionManager::DeleteSession(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp)
{
	Int64 sessId = GetSessId(req);
	OSInt i;
	Optional<Net::WebServer::MemoryWebSession> sess;
	NN<MemoryWebSession> nnsess;
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
		if (sess.SetTo(nnsess))
		{
			nnsess->BeginUse();
			this->delHdlr(nnsess, this->delHdlrObj);
			nnsess->EndUse();
			nnsess.Delete();
		}
		resp->AddSetCookie(this->cookieName->ToCString(), CSTR(""), this->path->ToCString(), true, req->IsSecure(), Net::WebServer::SameSiteType::Strict, Data::Timestamp::UtcNow().AddMonth(-12));
	}
}

Int64 Net::WebServer::MemoryWebSessionManager::GenSessId(NN<Net::WebServer::IWebRequest> req)
{
	Data::DateTime dt;
	UInt8 buff[8];
	buff[0] = 0;
	buff[1] = 0;
	*(UInt16*)&buff[2] = req->GetClientPort();
	NN<const Net::SocketUtil::AddressInfo> addr = req->GetClientAddr();
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
	else
	{
		*(UInt32*)&buff[4] = 0;
	}
	dt.SetCurrTimeUTC();
	return dt.ToTicks() + *(Int64*)buff;
}

NN<Net::WebServer::IWebSession> Net::WebServer::MemoryWebSessionManager::CreateSession(Int64 sessId)
{
	OSInt si;
	NN<Net::WebServer::MemoryWebSession> sess;
	Sync::MutexUsage mutUsage(this->mut);
	si = this->sessIds.SortedIndexOf(sessId);
	if (si >= 0 && this->sesses.GetItem((UOSInt)si).SetTo(sess))
	{

	}
	else
	{
		NEW_CLASSNN(sess, Net::WebServer::MemoryWebSession(sessId, Net::BrowserInfo::BT_UNKNOWN, Manage::OSInfo::OT_UNKNOWN));
		UOSInt i = this->sessIds.SortedInsert(sessId);
		this->sesses.Insert(i, NN<Net::WebServer::MemoryWebSession>::ConvertFrom(sess));
	}
	mutUsage.EndUse();
	NN<Net::WebServer::MemoryWebSession>::ConvertFrom(sess)->BeginUse();
	return sess;
}

Optional<Net::WebServer::IWebSession> Net::WebServer::MemoryWebSessionManager::GetSession(Int64 sessId)
{
	Optional<Net::WebServer::IWebSession> sess;
	OSInt i;
	sess = 0;
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds.SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses.GetItem((UOSInt)i);
	}
	mutUsage.EndUse();
	if (!sess.IsNull())
	{
		((Net::WebServer::MemoryWebSession*)sess.OrNull())->BeginUse();
	}
	return sess;
}

void Net::WebServer::MemoryWebSessionManager::DeleteSession(Int64 sessId)
{
	OSInt i;
	Optional<Net::WebServer::MemoryWebSession> sess;
	NN<MemoryWebSession> nnsess;
	sess = 0;
	Sync::MutexUsage mutUsage(this->mut);
	i = this->sessIds.SortedIndexOf(sessId);
	if (i >= 0)
	{
		sess = this->sesses.RemoveAt((UOSInt)i);
		this->sessIds.RemoveAt((UOSInt)i);
	}
	mutUsage.EndUse();
	if (sess.SetTo(nnsess))
	{
		nnsess->BeginUse();
		this->delHdlr(nnsess, this->delHdlrObj);
		nnsess->EndUse();
		nnsess.Delete();
	}
}

void Net::WebServer::MemoryWebSessionManager::GetSessionIds(NN<Data::ArrayList<Int64>> sessIds)
{
	Sync::MutexUsage mutUsage(this->mut);
	sessIds->AddAll(this->sessIds);
	mutUsage.EndUse();
}
