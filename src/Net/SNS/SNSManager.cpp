#include "Stdafx.h"
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
	NEW_CLASS(channel->currItems, Data::ArrayListStrUTF8());

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, this->dataPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"curritem.txt");
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(reader, Text::UTF8Reader(fs));
	while (reader->ReadLine(&sb, 1024))
	{
		if (sb.GetLength() > 0)
		{
			channel->currItems->SortedInsert(Text::StrCopyNew(sb.ToString()));
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
	sptr = Text::StrConcat(sbuff, this->dataPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = channel->ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = dt.ToString(sptr, "yyyyMM");
	Text::StrConcat(sptr, (const UTF8Char*)".csv");

	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	const UTF8Char *csptr;
	csptr = Text::StrToNewCSVRec(item->id);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.AppendChar(',', 1);
	sb.AppendChar('"', 1);
	sb.AppendI64(item->msgTime);
	sb.AppendChar('"', 1);
	sb.AppendChar(',', 1);
	if (item->title)
	{
		csptr = Text::StrToNewCSVRec(item->title);
		sb.Append(csptr);
		Text::StrDelNew(csptr);
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	sb.AppendChar(',', 1);
	csptr = Text::StrToNewCSVRec(item->message);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.AppendChar(',', 1);
	if (item->msgLink)
	{
		csptr = Text::StrToNewCSVRec(item->msgLink);
		sb.Append(csptr);
		Text::StrDelNew(csptr);
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	sb.AppendChar(',', 1);
	if (item->imgURL)
	{
		csptr = Text::StrToNewCSVRec(item->imgURL);
		sb.Append(csptr);
		Text::StrDelNew(csptr);
	}
	else
	{
		sb.AppendChar('"', 2);
	}
	writer->WriteLine(sb.ToString());
	DEL_CLASS(writer);
	DEL_CLASS(fs);

	if (item->imgURL && item->imgURL[0])
	{
		UOSInt i = 0;
		UOSInt j;
		UOSInt k;
		UOSInt retryCnt;
		UInt64 leng;
		UTF8Char *sarr[2];
		Net::HTTPClient *cli;
		UInt8 *tmpBuff = MemAlloc(UInt8, 65536);

		sptr = Text::StrConcat(sbuff, this->dataPath);
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
		sarr[1] = sb.ToString();
		while (true)
		{
			j = Text::StrSplit(sarr, 2, sarr[1], ' ');
			retryCnt = 0;
			while (retryCnt < 3)
			{
				cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, this->userAgent, true, Text::StrStartsWith(sarr[0], (const UTF8Char*)"https://"));
				if (cli->Connect(sarr[0], "GET", 0, 0, true))
				{
					if (Text::StrEndsWith(sarr[0], (const UTF8Char*)".mp4"))
					{
						Text::StrConcat(Text::StrUOSInt(sptr, i), (const UTF8Char*)".mp4");
					}
					else if (Text::StrEndsWith(sarr[0], (const UTF8Char*)".png"))
					{
						Text::StrConcat(Text::StrUOSInt(sptr, i), (const UTF8Char*)".png");
					}
					else
					{
						Text::StrConcat(Text::StrUOSInt(sptr, i), (const UTF8Char*)".jpg");
					}
					leng = 0;
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
						printf("Image download failed: %lld != %lld, url: %s\r\n", cli->GetContentLength(), leng, sarr[0]);
						IO::Path::DeleteFile(sbuff);
						retryCnt++;
					}
					else if (cli->GetContentLength() == 0 && leng == 0)
					{
						printf("Image download failed: HTTP Status = %d, url: %s\r\n", cli->GetRespStatus(), sarr[0]);
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
					printf("Image download failed: Cannot connect to server, url: %s\r\n", sarr[0]);
					retryCnt++;
				}
				DEL_CLASS(cli);
			}

			if (j != 2)
			{
				break;
			}
		}

		if (item->videoURL && item->videoURL[0])
		{
			sb.ClearStr();
			sb.Append(item->videoURL);
			sarr[1] = sb.ToString();
			while (true)
			{
				j = Text::StrSplit(sarr, 2, sarr[1], ' ');
				cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, this->userAgent, true, Text::StrStartsWith(sarr[0], (const UTF8Char*)"https://"));
				if (cli->Connect(sarr[0], "GET", 0, 0, true))
				{
					Text::StrConcat(Text::StrUOSInt(sptr, i), (const UTF8Char*)".mp4");
					leng = 0;
					NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
						printf("Video download failed: %lld != %lld, url: %s\r\n", cli->GetContentLength(), leng, sarr[0]);
						IO::Path::DeleteFile(sbuff);
					}
					else if (cli->GetContentLength() == 0 && leng == 0)
					{
						printf("Video download failed: HTTP Status = %d, url: %s\r\n", cli->GetRespStatus(), sarr[0]);
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
	sptr = Text::StrConcat(sbuff, this->dataPath);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = channel->ctrl->GetDirName(sptr);
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, (const UTF8Char*)"curritem.txt");
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(writer, Text::UTF8Writer(fs));
	UOSInt i = 0;
	UOSInt j = channel->currItems->GetCount();
	while (i < j)
	{
		writer->WriteLine(channel->currItems->GetItem(i));
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
	Data::ArrayListStrUTF8 oldItems;
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
			channel->currItems->SortedInsert(Text::StrCopyNew(item->id));
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
			Text::StrDelNew(channel->currItems->RemoveAt((UOSInt)si));
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
	printf("Reload %s\r\n", channel->ctrl->GetChannelId());
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
	this->userAgent = userAgent?Text::StrCopyNew(userAgent):0;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->channelList, Data::ArrayList<Net::SNS::SNSManager::ChannelData*>());

	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"Net.SNS.SNSManager.threadEvt"));

	if (dataPath)
	{
		this->dataPath = Text::StrCopyNew(dataPath);
		sptr = Text::StrConcat(sbuff, dataPath);
	}
	else
	{
		IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"SNS");
		this->dataPath = Text::StrCopyNew(sbuff);
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		Text::StringBuilderUTF8 sb;
		IO::Path::PathType pt;
		Net::SNS::SNSControl::SNSType type = Net::SNS::SNSControl::ST_UNKNOWN;
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PT_DIRECTORY && sptr[0] != '.')
			{
				*sptr2++ = IO::Path::PATH_SEPERATOR;
				Text::StrConcat(sptr2, (const UTF8Char*)"channel.txt");
				IO::FileStream *fs;
				Text::UTF8Reader *reader;
				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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

	SDEL_TEXT(this->userAgent);
	Text::StrDelNew(this->dataPath);
	i = this->channelList->GetCount();
	while (i-- > 0)
	{
		channel = this->channelList->GetItem(i);
		DEL_CLASS(channel->ctrl);
		j = channel->currItems->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(channel->currItems->GetItem(j));
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
	UOSInt i = 0;
	UOSInt j = this->channelList->GetCount();
	while (i < j)
	{
		ctrl = this->channelList->GetItem(i)->ctrl;
		if (ctrl->GetSNSType() == type && Text::StrEquals(ctrl->GetChannelId(), channelId))
		{
			return 0;
		}
		i++;
	}

	ctrl = CreateControl(type, channelId);
	if (ctrl)
	{
		sptr = Text::StrConcat(sbuff, this->dataPath);
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		sptr = ctrl->GetDirName(sptr);
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcat(sptr, (const UTF8Char*)"channel.txt");
		IO::FileStream *fs;
		Text::UTF8Writer *writer;
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteLine(Net::SNS::SNSControl::SNSTypeGetName(ctrl->GetSNSType()));
		writer->WriteLine(ctrl->GetChannelId());
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
