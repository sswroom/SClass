#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Data/Int32Map.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "Net/SNS/SNS7gogo.h"
#include "Net/SNS/SNSInstagram.h"
#include "Net/SNS/SNSManager.h"
#include "Net/SNS/SNSRSS.h"
#include "Net/SNS/SNSTwitter.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include <stdio.h>

Net::SNS::SNSControl *Net::SNS::SNSManager::CreateControl(Net::SNS::SNSControl::SNSType type, const UTF8Char *channelId)
{
	Net::SNS::SNSControl *ctrl = 0;
	if (type == Net::SNS::SNSControl::ST_TWITTER)
	{
		NEW_CLASS(ctrl, Net::SNS::SNSTwitter(this->sockf, this->ssl, this->encFact, this->userAgent, channelId));
	}
	else if (type == Net::SNS::SNSControl::ST_RSS)
	{
		NEW_CLASS(ctrl, Net::SNS::SNSRSS(this->sockf, this->ssl, this->encFact, this->userAgent, channelId));
	}
	else if (type == Net::SNS::SNSControl::ST_7GOGO)
	{
		NEW_CLASS(ctrl, Net::SNS::SNS7gogo(this->sockf, this->ssl, this->encFact, this->userAgent, channelId));
	}
	else if (type == Net::SNS::SNSControl::ST_INSTAGRAM)
	{
		NEW_CLASS(ctrl, Net::SNS::SNSInstagram(this->sockf, this->ssl, this->encFact, this->userAgent, channelId));
	}
	if (ctrl && ctrl->IsError())
	{
		DEL_CLASS(ctrl);
		return 0;
	}
	return ctrl;
}

Net::SNS::SNSManager::ChannelData *Net::SNS::SNSManager::ChannelInit(Net::SNS::SNSControl *ctrl)
{
	Net::SNS::SNSManager::ChannelData *channel = MemAlloc(Net::SNS::SNSManager::ChannelData, 1);
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	channel->ctrl = ctrl;
	channel->lastLoadTime = dt.ToTicks();
	NEW_CLASS(channel->currItems, Data::ArrayListString());

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->dataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("curritem.txt"));
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (reader->ReadLine(&sb, 1024))
	{
		if (sb.GetLength() > 0)
		{
			channel->currItems->SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
		}
		sb.ClearStr();
	}
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return channel;
}

void Net::SNS::SNSManager::ChannelAddMessage(Net::SNS::SNSManager::ChannelData *channel, Net::SNS::SNSControl::SNSItem *item)
{
	Data::DateTime dt;
	dt.SetTicks(item->msgTime);
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	sptr = this->dataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = channel->ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dt.ToString(sptr, "yyyyMM");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));

	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	Text::String *s;
	s = Text::String::NewCSVRec(item->id->v);
	sb.Append(s);
	s->Release();
	sb.AppendChar(',', 1);
	sb.AppendChar('"', 1);
	sb.AppendI64(item->msgTime);
	sb.AppendChar('"', 1);
	sb.AppendChar(',', 1);
	if (item->title)
	{
		s = Text::String::NewCSVRec(item->title->v);
		sb.Append(s);
		s->Release();
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	sb.AppendChar(',', 1);
	s = Text::String::NewCSVRec(item->message->v);
	sb.Append(s);
	s->Release();
	sb.AppendChar(',', 1);
	if (item->msgLink)
	{
		s = Text::String::NewCSVRec(item->msgLink->v);
		sb.Append(s);
		s->Release();
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	sb.AppendChar(',', 1);
	if (item->imgURL)
	{
		s = Text::String::NewCSVRec(item->imgURL->v);
		sb.Append(s);
		s->Release();
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	DEL_CLASS(writer);
	DEL_CLASS(fs);

	if (item->imgURL && item->imgURL->v[0])
	{
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt retryCnt;
		UInt64 leng;
		Text::PString sarr[2];
		Net::HTTPClient *cli;
		UInt8 *tmpBuff = MemAlloc(UInt8, 65536);

		sptr = this->dataPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = channel->ctrl->GetDirName(sptr);
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = channel->ctrl->GetItemShortId(sptr, item);
		*sptr++ = '_';

		sb.ClearStr();
		sb.Append(item->imgURL);
		sarr[1].v = sb.ToString();
		sarr[1].leng = sb.GetLength();
		while (true)
		{
			j = Text::StrSplitP(sarr, 2, sarr[1], ' ');
			retryCnt = 0;
			while (retryCnt < 3)
			{
				cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, {STR_PTRC(this->userAgent)}, true, Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("https://")));
				if (cli->Connect(sarr[0].ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true))
				{
					if (Text::StrEndsWithC(sarr[0].v, sarr[0].leng, UTF8STRC(".mp4")))
					{
						sptr2 = Text::StrConcatC(Text::StrUOSInt(sptr, i), UTF8STRC(".mp4"));
					}
					else if (Text::StrEndsWithC(sarr[0].v, sarr[0].leng, UTF8STRC(".png")))
					{
						sptr2 = Text::StrConcatC(Text::StrUOSInt(sptr, i), UTF8STRC(".png"));
					}
					else
					{
						sptr2 = Text::StrConcatC(Text::StrUOSInt(sptr, i), UTF8STRC(".jpg"));
					}
					leng = 0;
					NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					while (true)
					{
						k = cli->Read(tmpBuff, 65536);
						if (k <= 0)
						{
							break;
						}
						leng += fs->Write(tmpBuff, k);
					}
					DEL_CLASS(fs);
					if (cli->GetContentLength() > 0 && cli->GetContentLength() != leng)
					{
						printf("Image download failed: %lld != %lld, url: %s\r\n", cli->GetContentLength(), leng, sarr[0].v);
						IO::Path::DeleteFile(sbuff);
						retryCnt++;
					}
					else if (cli->GetContentLength() == 0 && leng == 0)
					{
						printf("Image download failed: HTTP Status = %d, url: %s\r\n", cli->GetRespStatus(), sarr[0].v);
						IO::Path::DeleteFile(sbuff);
						retryCnt = 5;
					}
					else
					{
						retryCnt = 5;
						i++;
					}
				}
				else
				{
					printf("Image download failed: Cannot connect to server, url: %s\r\n", sarr[0].v);
					retryCnt++;
				}
				DEL_CLASS(cli);
			}

			if (j != 2)
			{
				break;
			}
		}

		if (item->videoURL && item->videoURL->v[0])
		{
			sb.ClearStr();
			sb.Append(item->videoURL);
			sarr[1].v = sb.ToString();
			sarr[1].leng = sb.GetLength();
			while (true)
			{
				j = Text::StrSplitP(sarr, 2, sarr[1], ' ');
				if (this->userAgent)
				{
					cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, this->userAgent->ToCString(), true, Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("https://")));
				}
				else
				{
					cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR_NULL, true, Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("https://")));
				}
				if (cli->Connect(sarr[0].ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true))
				{
					sptr2 = Text::StrConcatC(Text::StrUOSInt(sptr, i), UTF8STRC(".mp4"));
					leng = 0;
					NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					while (true)
					{
						k = cli->Read(tmpBuff, 65536);
						if (k <= 0)
						{
							break;
						}
						leng += fs->Write(tmpBuff, k);
					}
					DEL_CLASS(fs);
					if (cli->GetContentLength() > 0 && cli->GetContentLength() != leng)
					{
						printf("Video download failed: %lld != %lld, url: %s\r\n", cli->GetContentLength(), leng, sarr[0].v);
						IO::Path::DeleteFile(sbuff);
					}
					else if (cli->GetContentLength() == 0 && leng == 0)
					{
						printf("Video download failed: HTTP Status = %d, url: %s\r\n", cli->GetRespStatus(), sarr[0].v);
						IO::Path::DeleteFile(sbuff);
					}
					else
					{
						i++;
					}
				}
				DEL_CLASS(cli);

				if (j != 2)
				{
					break;
				}
			}
		}

		MemFree(tmpBuff);
	}
}

void Net::SNS::SNSManager::ChannelStoreCurr(Net::SNS::SNSManager::ChannelData *channel)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->dataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = channel->ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("curritem.txt"));
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	UOSInt i = 0;
	UOSInt j = channel->currItems->GetCount();
	while (i < j)
	{
		Text::String *s = channel->currItems->GetItem(i);
		writer->WriteLineC(s->v, s->leng);
		i++;
	}
	DEL_CLASS(writer);
	DEL_CLASS(fs);

}

void Net::SNS::SNSManager::ChannelUpdate(Net::SNS::SNSManager::ChannelData *channel)
{
	Data::ArrayList<Net::SNS::SNSControl::SNSItem*> itemList;
	Net::SNS::SNSControl::SNSItem *item;
	Bool updated = false;
	channel->ctrl->GetCurrItems(&itemList);
	Data::ArrayListString oldItems;
	oldItems.AddAll(channel->currItems);
	UOSInt i;
	UOSInt j;
	OSInt si;
	i = 0;
	j = itemList.GetCount();
	while (i < j)
	{
		item = itemList.GetItem(i);
		si = oldItems.SortedIndexOf(item->id);
		if (si >= 0)
		{
			oldItems.RemoveAt((UOSInt)si);
		}
		else
		{
			channel->currItems->SortedInsert(item->id->Clone());
			this->ChannelAddMessage(channel, item);
			updated = true;
		}
		
		i++;
	}
	i = oldItems.GetCount();
	while (i-- > 0)
	{
		si = channel->currItems->SortedIndexOf(oldItems.GetItem(i));
		if (si >= 0)
		{
			channel->currItems->RemoveAt((UOSInt)si)->Release();
			updated = true;
		}
	}
	if (updated)
	{
		this->ChannelStoreCurr(channel);
	}
}

void Net::SNS::SNSManager::ChannelReload(Net::SNS::SNSManager::ChannelData *channel)
{
	printf("Reload %s\r\n", channel->ctrl->GetChannelId()->v);
	if (channel->ctrl->Reload())
	{
		this->ChannelUpdate(channel);
	}
}

UInt32 __stdcall Net::SNS::SNSManager::ThreadProc(void *userObj)
{
	Net::SNS::SNSManager *me = (Net::SNS::SNSManager*)userObj;
	Data::DateTime *dt;
	Int64 t;
	UOSInt i;
	Int32 cnt;
	Net::SNS::SNSManager::ChannelData *channel;
	Data::Int32Map<Int32> *cntMap;
	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(cntMap, Data::Int32Map<Int32>());
	while (!me->threadToStop)
	{
		dt->SetCurrTimeUTC();
		t = dt->ToTicks();
		Sync::MutexUsage mutUsage(me->mut);
		cntMap->Clear();
		i = me->channelList->GetCount();
		while (i-- > 0)
		{
			channel = me->channelList->GetItem(i);
			if (t - channel->lastLoadTime >= channel->ctrl->GetMinIntevalMS())
			{
				cnt = cntMap->Get((Int32)channel->ctrl->GetSNSType());
				if (cnt < 5)
				{
					dt->SetCurrTimeUTC();
					channel->lastLoadTime = dt->ToTicks();
					me->ChannelReload(channel);
					cntMap->Put((Int32)channel->ctrl->GetSNSType(), cnt + 1);
				}
			}
		}
		mutUsage.EndUse();
		me->threadEvt->Wait(60000);
	}
	DEL_CLASS(cntMap);
	DEL_CLASS(dt);
	me->threadRunning = false;
	return 0;
}

Net::SNS::SNSManager::SNSManager(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent, const UTF8Char *dataPath)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = Text::String::NewOrNull(userAgent);
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->channelList, Data::ArrayList<Net::SNS::SNSManager::ChannelData*>());

	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Net.SNS.SNSManager.threadEvt"));

	if (dataPath)
	{
		this->dataPath = Text::String::NewNotNull(dataPath);
		sptr = this->dataPath->ConcatTo(sbuff);
	}
	else
	{
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("SNS"));
		this->dataPath = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
	if (sess)
	{
		Text::StringBuilderUTF8 sb;
		IO::Path::PathType pt;
		Net::SNS::SNSControl::SNSType type = Net::SNS::SNSControl::ST_UNKNOWN;
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::Directory && sptr[0] != '.')
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				sptr2 = Text::StrConcatC(sptr2, UTF8STRC("channel.txt"));
				IO::FileStream *fs;
				Text::UTF8Reader *reader;
				NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				NEW_CLASS(reader, Text::UTF8Reader(fs));
				sb.ClearStr();
				if (reader->ReadLine(&sb, 1024))
				{
					type = Net::SNS::SNSControl::SNSTypeFromName(sb.ToString());
				}
				sb.ClearStr();
				if (reader->ReadLine(&sb, 1024))
				{
					if (sb.GetLength() > 0)
					{
						Net::SNS::SNSControl *ctrl = this->CreateControl(type, sb.ToString());
						if (ctrl)
						{
							Net::SNS::SNSManager::ChannelData *channel = this->ChannelInit(ctrl);
							this->channelList->Add(channel);
							this->ChannelUpdate(channel);
						}
					}
				}
				DEL_CLASS(reader);
				DEL_CLASS(fs);
			}
		}
		IO::Path::FindFileClose(sess);
	}

	Sync::Thread::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::SNS::SNSManager::~SNSManager()
{
	UOSInt i;
	UOSInt j;
	Net::SNS::SNSManager::ChannelData *channel;
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}

	SDEL_STRING(this->userAgent);
	this->dataPath->Release();
	i = this->channelList->GetCount();
	while (i-- > 0)
	{
		channel = this->channelList->GetItem(i);
		DEL_CLASS(channel->ctrl);
		j = channel->currItems->GetCount();
		while (j-- > 0)
		{
			channel->currItems->GetItem(j)->Release();
		}
		DEL_CLASS(channel->currItems);
		MemFree(channel);
	}
	DEL_CLASS(this->channelList);
	DEL_CLASS(this->mut);
}

Net::SNS::SNSControl *Net::SNS::SNSManager::AddChannel(Net::SNS::SNSControl::SNSType type, const UTF8Char *channelId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::SNS::SNSControl *ctrl;
	UOSInt channelIdLen = Text::StrCharCnt(channelId);
	UOSInt i = 0;
	UOSInt j = this->channelList->GetCount();
	while (i < j)
	{
		ctrl = this->channelList->GetItem(i)->ctrl;
		if (ctrl->GetSNSType() == type && ctrl->GetChannelId()->Equals(channelId, channelIdLen))
		{
			return 0;
		}
		i++;
	}

	ctrl = CreateControl(type, channelId);
	if (ctrl)
	{
		sptr = this->dataPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = ctrl->GetDirName(sptr);
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("channel.txt"));
		IO::FileStream *fs;
		Text::UTF8Writer *writer;
		NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteLine(Net::SNS::SNSControl::SNSTypeGetName(ctrl->GetSNSType()));
		Text::String *s = ctrl->GetChannelId();
		writer->WriteLineC(s->v, s->leng);
		DEL_CLASS(writer);
		DEL_CLASS(fs);
		Net::SNS::SNSManager::ChannelData *channel = this->ChannelInit(ctrl);
		Sync::MutexUsage mutUsage(mut);
		this->channelList->Add(channel);
		this->ChannelUpdate(channel);
		mutUsage.EndUse();
	}
	return ctrl;
}

void Net::SNS::SNSManager::Use(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}

UOSInt Net::SNS::SNSManager::GetCount()
{
	return this->channelList->GetCount();
}

Net::SNS::SNSControl *Net::SNS::SNSManager::GetItem(UOSInt index)
{
	return this->channelList->GetItem(index)->ctrl;
}
