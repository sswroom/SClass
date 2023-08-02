#include "Stdafx.h"
#include "Data/ArrayListString.h"
#include "Data/ByteBuffer.h"
#include "Data/FastMap.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "Net/SNS/SNS7gogo.h"
#include "Net/SNS/SNSInstagram.h"
#include "Net/SNS/SNSManager.h"
#include "Net/SNS/SNSRSS.h"
#include "Net/SNS/SNSTwitter.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include <stdio.h>

Net::SNS::SNSControl *Net::SNS::SNSManager::CreateControl(Net::SNS::SNSControl::SNSType type, Text::CString channelId)
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
	Net::SNS::SNSManager::ChannelData *channel;
	NEW_CLASS(channel, Net::SNS::SNSManager::ChannelData());
	channel->ctrl = ctrl;
	channel->lastLoadTime = Data::DateTimeUtil::GetCurrTimeMillis();

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = this->dataPath->ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("curritem.txt"));
	Text::StringBuilderUTF8 sb;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Reader reader(fs);
	while (reader.ReadLine(sb, 1024))
	{
		if (sb.GetLength() > 0)
		{
			channel->currItems.SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
		}
		sb.ClearStr();
	}
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
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dt.ToString(sptr, "yyyyMM");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));

	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	NotNullPtr<Text::String> s;
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
		s = Text::String::NewCSVRec(item->id->v);
		sb.Append(s);
		s->Release();
		sb.AppendUTF8Char(',');
		sb.AppendUTF8Char('"');
		sb.AppendI64(item->msgTime);
		sb.AppendUTF8Char('"');
		sb.AppendUTF8Char(',');
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
		sb.AppendUTF8Char(',');
		s = Text::String::NewCSVRec(item->message->v);
		sb.Append(s);
		s->Release();
		sb.AppendUTF8Char(',');
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
		sb.AppendUTF8Char(',');
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
		writer.WriteLineC(sb.ToString(), sb.GetLength());
	}

	if (item->imgURL && item->imgURL->v[0])
	{
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt retryCnt;
		UInt64 leng;
		Text::PString sarr[2];
		NotNullPtr<Net::HTTPClient> cli;
		Data::ByteBuffer tmpBuff(65536);

		sptr = this->dataPath->ConcatTo(sbuff);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = channel->ctrl->GetDirName(sptr);
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMM");
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = channel->ctrl->GetItemShortId(sptr, item);
		*sptr++ = '_';

		sb.ClearStr();
		sb.Append(item->imgURL);
		sarr[1] = sb;
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
					{
						IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						while (true)
						{
							k = cli->Read(tmpBuff);
							if (k <= 0)
							{
								break;
							}
							leng += fs.Write(tmpBuff.Ptr(), k);
						}
					}
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
				cli.Delete();
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
			sarr[1] = sb;
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
					{
						IO::FileStream fs(CSTRP(sbuff, sptr2), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
						while (true)
						{
							k = cli->Read(tmpBuff);
							if (k <= 0)
							{
								break;
							}
							leng += fs.Write(tmpBuff.Ptr(), k);
						}
					}
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
				cli.Delete();

				if (j != 2)
				{
					break;
				}
			}
		}
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
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("curritem.txt"));
	Text::StringBuilderUTF8 sb;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);
	UOSInt i = 0;
	UOSInt j = channel->currItems.GetCount();
	while (i < j)
	{
		Text::String *s = channel->currItems.GetItem(i);
		writer.WriteLineC(s->v, s->leng);
		i++;
	}
}

void Net::SNS::SNSManager::ChannelUpdate(Net::SNS::SNSManager::ChannelData *channel)
{
	Data::ArrayList<Net::SNS::SNSControl::SNSItem*> itemList;
	Net::SNS::SNSControl::SNSItem *item;
	Bool updated = false;
	channel->ctrl->GetCurrItems(itemList);
	Data::ArrayListStringNN oldItems;
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
			channel->currItems.SortedInsert(item->id->Clone());
			this->ChannelAddMessage(channel, item);
			updated = true;
		}
		
		i++;
	}
	i = oldItems.GetCount();
	while (i-- > 0)
	{
		si = channel->currItems.SortedIndexOf(Text::String::OrEmpty(oldItems.GetItem(i)));
		if (si >= 0)
		{
			channel->currItems.RemoveAt((UOSInt)si)->Release();
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
	Int64 t;
	UOSInt i;
	Int32 cnt;
	Net::SNS::SNSManager::ChannelData *channel;
	me->threadRunning = true;
	{
		Data::FastMap<Int32, Int32> cntMap;
		while (!me->threadToStop)
		{
			t = Data::DateTimeUtil::GetCurrTimeMillis();
			Sync::MutexUsage mutUsage(me->mut);
			cntMap.Clear();
			i = me->channelList.GetCount();
			while (i-- > 0)
			{
				channel = me->channelList.GetItem(i);
				if (t - channel->lastLoadTime >= channel->ctrl->GetMinIntevalMS())
				{
					cnt = cntMap.Get((Int32)channel->ctrl->GetSNSType());
					if (cnt < 5)
					{
						channel->lastLoadTime = Data::DateTimeUtil::GetCurrTimeMillis();
						me->ChannelReload(channel);
						cntMap.Put((Int32)channel->ctrl->GetSNSType(), cnt + 1);
					}
				}
			}
			mutUsage.EndUse();
			me->threadEvt.Wait(60000);
		}
	}
	me->threadRunning = false;
	return 0;
}

Net::SNS::SNSManager::SNSManager(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString userAgent, Text::CString dataPath)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->userAgent = Text::String::NewOrNull(userAgent);

	this->threadRunning = false;
	this->threadToStop = false;

	if (dataPath.leng > 0)
	{
		this->dataPath = Text::String::New(dataPath);
		sptr = this->dataPath->ConcatTo(sbuff);
	}
	else
	{
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SNS"));
		this->dataPath = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
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
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				Text::UTF8Reader reader(fs);
				sb.ClearStr();
				if (reader.ReadLine(sb, 1024))
				{
					type = Net::SNS::SNSControl::SNSTypeFromName(sb.ToCString());
				}
				sb.ClearStr();
				if (reader.ReadLine(sb, 1024))
				{
					if (sb.GetLength() > 0)
					{
						Net::SNS::SNSControl *ctrl = this->CreateControl(type, sb.ToCString());
						if (ctrl)
						{
							Net::SNS::SNSManager::ChannelData *channel = this->ChannelInit(ctrl);
							this->channelList.Add(channel);
							this->ChannelUpdate(channel);
						}
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	Sync::ThreadUtil::Create(ThreadProc, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Net::SNS::SNSManager::~SNSManager()
{
	UOSInt i;
	UOSInt j;
	Net::SNS::SNSManager::ChannelData *channel;
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}

	SDEL_STRING(this->userAgent);
	this->dataPath->Release();
	i = this->channelList.GetCount();
	while (i-- > 0)
	{
		channel = this->channelList.GetItem(i);
		DEL_CLASS(channel->ctrl);
		j = channel->currItems.GetCount();
		while (j-- > 0)
		{
			channel->currItems.GetItem(j)->Release();
		}
		DEL_CLASS(channel);
	}
}

Net::SNS::SNSControl *Net::SNS::SNSManager::AddChannel(Net::SNS::SNSControl::SNSType type, Text::CString channelId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::SNS::SNSControl *ctrl;
	UOSInt i = 0;
	UOSInt j = this->channelList.GetCount();
	while (i < j)
	{
		ctrl = this->channelList.GetItem(i)->ctrl;
		if (ctrl->GetSNSType() == type && ctrl->GetChannelId()->Equals(channelId.v, channelId.leng))
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
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("channel.txt"));
		{
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Writer writer(fs);
			writer.WriteLine(Net::SNS::SNSControl::SNSTypeGetName(ctrl->GetSNSType()));
			NotNullPtr<Text::String> s = ctrl->GetChannelId();
			writer.WriteLineC(s->v, s->leng);
		}
		Net::SNS::SNSManager::ChannelData *channel = this->ChannelInit(ctrl);
		Sync::MutexUsage mutUsage(this->mut);
		this->channelList.Add(channel);
		this->ChannelUpdate(channel);
		mutUsage.EndUse();
	}
	return ctrl;
}

void Net::SNS::SNSManager::Use(NotNullPtr<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}

UOSInt Net::SNS::SNSManager::GetCount() const
{
	return this->channelList.GetCount();
}

Net::SNS::SNSControl *Net::SNS::SNSManager::GetItem(UOSInt index) const
{
	return this->channelList.GetItem(index)->ctrl;
}
