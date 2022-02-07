#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StreamReader.h"
#include "IO/StreamWriter.h"
#include "Manage/HiResClock.h"
#include "Media/MediaFile.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Writer.h"

UInt32 __stdcall Net::RTSPClient::ControlThread(void *userObj)
{
	ClientData *cliData = (ClientData*)userObj;
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UTF8Char *sarr[3];
	UInt8 dataBuff[2048];
	UOSInt buffSize;
	UOSInt thisSize;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Bool content;
	IO::MemoryStream *mstm;
	IO::StreamReader *reader;
	
	cliData->threadRunning = true;
	NEW_CLASS(mstm, IO::MemoryStream(UTF8STRC("Net.RTSPCLient.ControlThread")));
	buffSize = 0;
	content = false;
	while (!cliData->threadToStop)
	{
		if (cliData->cli == 0)
		{
			Sync::MutexUsage mutUsage(cliData->cliMut);
			NEW_CLASS(cliData->cli, Net::TCPClient(cliData->sockf, cliData->host, cliData->port));
			mutUsage.EndUse();
		}
		if (content)
		{
			thisSize = cliData->cli->Read(dataBuff, 2048);
			if (thisSize == 0)
			{
				Sync::MutexUsage mutUsage(cliData->cliMut);
				DEL_CLASS(cliData->cli);
				cliData->cli = 0;
				mutUsage.EndUse();
				continue;
			}
			if (cliData->reqReplySize > buffSize + thisSize)
			{
				MemCopyNO(&cliData->reqReply[buffSize], dataBuff, thisSize);
				buffSize += thisSize;
				continue;
			}
			else
			{
				MemCopyNO(&cliData->reqReply[buffSize], dataBuff, cliData->reqReplySize - buffSize);
				cliData->reqSuccess = true;
				cliData->reqEvt->Set();
				if (cliData->reqReplySize < buffSize + thisSize)
				{
					MemCopyO(dataBuff, &dataBuff[cliData->reqReplySize - buffSize], thisSize - cliData->reqReplySize + buffSize);
				}
				buffSize = thisSize - cliData->reqReplySize + buffSize;
				content = false;
			}
			
		}
		else
		{
			thisSize = cliData->cli->Read(&dataBuff[buffSize], 2048 - buffSize);
			if (thisSize == 0)
			{
				Sync::MutexUsage mutUsage(cliData->cliMut);
				DEL_CLASS(cliData->cli);
				cliData->cli = 0;
				mutUsage.EndUse();
				continue;
			}
			buffSize += thisSize;
		}
		Int32 val = *(Int32*)"\r\n\r\n";
		i = 0;
		j = buffSize - 4;
		while (i <= j)
		{
			if (*(Int32*)&dataBuff[i] == val)
			{
				mstm->Clear();
				mstm->Write(dataBuff, i + 4);
				mstm->SeekFromBeginning(0);
				NEW_CLASS(reader, IO::StreamReader(mstm, 65001));
				sptr = reader->ReadLine(sbuff, 1021);
				if (Text::StrSplit(sarr, 3, sbuff, ' ') != 3)
				{
					cliData->reqReplyStatus = 0;
					cliData->reqSuccess = true;
					cliData->reqEvt->Set();
				}
				else if (!Text::StrEquals(sarr[0], (const UTF8Char*)"RTSP/1.0"))
				{
					cliData->reqReplyStatus = 0;
					cliData->reqSuccess = true;
					cliData->reqEvt->Set();
				}
				else
				{
					cliData->reqReplyStatus = Text::StrToInt32(sarr[1]);
					cliData->reqReplySize = 0;
					while (true)
					{
						sptr = reader->ReadLine(sbuff, 1021);
						if (sptr == 0 || sptr == sbuff)
							break;
						if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Content-Length: ")))
						{
							cliData->reqReplySize = Text::StrToUInt32(&sbuff[16]);
						}
						else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Session: ")))
						{
							SDEL_TEXT(cliData->reqStrs);
							k = Text::StrIndexOfChar(&sbuff[9], ';');
							if (k != INVALID_INDEX)
							{
								sbuff[9 + k] = 0;
								sptr = &sbuff[9 + k];
							}
							cliData->reqStrs = Text::StrCopyNewC(&sbuff[9], (UOSInt)(sptr - &sbuff[9]));
						}
						else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Public: ")))
						{
							SDEL_TEXT(cliData->reqStrs);
							cliData->reqStrs = Text::StrCopyNewC(&sbuff[8], (UOSInt)(sptr - &sbuff[8]));
						}
					}
					if (cliData->reqReplySize == 0)
					{
						cliData->reqSuccess = true;
						cliData->reqEvt->Set();
					}
					else
					{
						cliData->reqReply = MemAlloc(UInt8, cliData->reqReplySize);
						content = true;
					}
				}
				DEL_CLASS(reader);
				if (i + 4 < buffSize)
				{
					MemCopyO(dataBuff, &dataBuff[i + 4], buffSize - i - 4);
				}
				buffSize -= i + 4;
				if (cliData->reqReplySize > 0 && buffSize > 0)
				{
					if (cliData->reqReplySize > buffSize)
					{
						MemCopyNO(cliData->reqReply, dataBuff, buffSize);
						break;
					}
					else
					{
						MemCopyNO(cliData->reqReply, dataBuff, cliData->reqReplySize);
						cliData->reqSuccess = true;
						cliData->reqEvt->Set();
						if (cliData->reqReplySize < buffSize)
						{
							MemCopyO(dataBuff, &dataBuff[cliData->reqReplySize], buffSize - cliData->reqReplySize);

						}
						buffSize -= cliData->reqReplySize;
						content = false;
					}
				}
				i = 0;
				j = buffSize - 4;
			}
			else
			{
				i++;
			}
		}
	}
	if (cliData->cli)
	{
		DEL_CLASS(cliData->cli);
		cliData->cli = 0;
	}
	DEL_CLASS(mstm);
	cliData->threadRunning = false;
	return 0;
}

Int32 Net::RTSPClient::NextRequest()
{
	if (this->cliData->reqReply)
	{
		MemFree(this->cliData->reqReply);
		this->cliData->reqReply = 0;
	}
	this->cliData->reqReplySize = 0;
	this->cliData->reqSuccess = false;
	SDEL_TEXT(this->cliData->reqStrs);
	Int32 reply = this->cliData->nextSeq++;
	return reply;
}

Bool Net::RTSPClient::WaitForReply()
{
	Manage::HiResClock clk;
	if (!this->cliData->threadRunning)
		return false;
	while (!this->cliData->reqSuccess && clk.GetTimeDiff() < 10)
	{
		this->cliData->reqEvt->Wait(1000);
	}
	return this->cliData->reqSuccess;
}

Bool Net::RTSPClient::SendData(UInt8 *buff, UOSInt buffSize)
{
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->cliData->cliMut);
	if (this->cliData->cli)
	{
		succ = this->cliData->cli->Write(buff, buffSize) == buffSize;
	}
	mutUsage.EndUse();
	return succ;
}

Net::RTSPClient::RTSPClient(Net::RTSPClient *cli)
{
	this->cliData = cli->cliData;
	this->cliData->useCnt++;
}

Net::RTSPClient::RTSPClient(Net::SocketFactory *sockf, const UTF8Char *host, UInt16 port)
{
	this->cliData = MemAlloc(ClientData, 1);
	this->cliData->useCnt = 1;
	this->cliData->sockf = sockf;
	this->cliData->nextSeq = 1;
	this->cliData->threadRunning = false;
	this->cliData->threadToStop = false;
	this->cliData->reqReply = 0;
	this->cliData->reqReplySize = 0;
	this->cliData->reqStrs = 0;
	this->cliData->cli = 0;
	this->cliData->host = Text::StrCopyNew(host);
	this->cliData->port = port;
	NEW_CLASS(this->cliData->cliMut, Sync::Mutex());
	NEW_CLASS(this->cliData->reqEvt, Sync::Event(true));
	NEW_CLASS(this->cliData->reqMut, Sync::Mutex());

	Sync::Thread::Create(ControlThread, this->cliData);
	while (!this->cliData->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

Net::RTSPClient::~RTSPClient()
{
	if (--this->cliData->useCnt == 0)
	{
		this->cliData->threadToStop = true;
		Sync::MutexUsage mutUsage(this->cliData->cliMut);
		if (this->cliData->cli)
		{
			this->cliData->cli->Close();
		}
		mutUsage.EndUse();
		while (this->cliData->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->NextRequest();
		Text::StrDelNew(this->cliData->host);
		DEL_CLASS(this->cliData->cliMut);
		DEL_CLASS(this->cliData->reqMut);
		DEL_CLASS(this->cliData->reqEvt);
		MemFree(this->cliData);
	}
}

Bool Net::RTSPClient::GetOptions(const UTF8Char *url, Data::ArrayList<const UTF8Char *> *options)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff;
	UTF8Char *sarr[10];
	UOSInt i;
	UOSInt buffSize;
	IO::StreamWriter *writer;
	IO::MemoryStream *stm;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	NEW_CLASS(stm, IO::MemoryStream(UTF8STRC("Net.RTSPClient.GetOptions")));
	NEW_CLASS(writer, IO::StreamWriter(stm, 65001));
	writer->WriteStrC(UTF8STRC("OPTIONS "));
	writer->WriteStr(url);
	writer->WriteLineC(UTF8STRC(" RTSP/1.0"));
	writer->WriteStrC(UTF8STRC("CSeq: "));
	sptr = Text::StrInt32(sbuff, reqId);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("User-Agent: RTSPClient"));
	writer->WriteLine();
	DEL_CLASS(writer);
	buff = stm->GetBuff(&buffSize);
	this->SendData(buff, buffSize);
	DEL_CLASS(stm);

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		if (this->cliData->reqStrs)
		{
			Text::StrConcat(sbuff, this->cliData->reqStrs);
			buffSize = Text::StrSplitTrim(sarr, 10, sbuff, ',');
			i = 0;
			while (i < buffSize)
			{
				options->Add(Text::StrCopyNew(sarr[i]));
				i++;
			}
			ret = true;
		}
	}
	mutUsage.EndUse();
	return ret;
}

Net::SDPFile *Net::RTSPClient::GetMediaInfo(const UTF8Char *url)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	UInt8 *buff;
	UOSInt buffSize;
	Text::UTF8Writer *writer;
	IO::MemoryStream *stm;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	NEW_CLASS(stm, IO::MemoryStream(UTF8STRC("Net.RTSPClient.GetMediaInfo")));
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	writer->WriteStrC(UTF8STRC("DESCRIBE "));
	writer->WriteStr(url);
	writer->WriteLineC(UTF8STRC(" RTSP/1.0"));
	writer->WriteStrC(UTF8STRC("CSeq: "));
	sptr = Text::StrInt32(sbuff, reqId);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLineC(UTF8STRC("Content-Type: application/sdp"));
	writer->WriteLine();
	DEL_CLASS(writer);
	buff = stm->GetBuff(&buffSize);
	this->SendData(buff, buffSize);
	DEL_CLASS(stm);

	Bool succ = this->WaitForReply();

	Net::SDPFile *sdp = 0;
	if (succ && this->cliData->reqReplyStatus == 200 && this->cliData->reqReplySize > 0 && this->cliData->reqReply)
	{
		NEW_CLASS(sdp, Net::SDPFile(this->cliData->reqReply, this->cliData->reqReplySize));
	}
	mutUsage.EndUse();
	return sdp;
}

UTF8Char *Net::RTSPClient::SetupRTP(UTF8Char *sessIdOut, const UTF8Char *url, Net::RTPCliChannel *rtpChannel)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff;
	UOSInt buffSize;
	Text::UTF8Writer *writer;
	IO::MemoryStream *stm;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	NEW_CLASS(stm, IO::MemoryStream(UTF8STRC("Net.RTSPClient.SetupRTP")));
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	writer->WriteStrC(UTF8STRC("SETUP "));
	writer->WriteStr(url);
	writer->WriteLineC(UTF8STRC(" RTSP/1.0"));
	writer->WriteStrC(UTF8STRC("CSeq: "));
	sptr = Text::StrInt32(sbuff, reqId);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteStrC(UTF8STRC("Transport: "));
	sptr = rtpChannel->GetTransportDesc(sbuff);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteLine();
	DEL_CLASS(writer);
	buff = stm->GetBuff(&buffSize);
	this->SendData(buff, buffSize);
	DEL_CLASS(stm);

	Bool succ = this->WaitForReply();

	UTF8Char *ret = 0;
	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = Text::StrConcat(sessIdOut, this->cliData->reqStrs);
	}
	mutUsage.EndUse();
	return ret;
}

IO::ParsedObject *Net::RTSPClient::ParseURL(Net::SocketFactory *sockf, const UTF8Char *url, UOSInt urlLen)
{
	UTF8Char sbuff[512];
	Net::RTSPClient *cli;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	UInt16 port = 554;
	Text::URLString::GetURLDomain(sbuff, url, urlLen, &port);
	if (port == 0)
	{
		port = 554;
	}

	NEW_CLASS(cli, Net::RTSPClient(sockf, sbuff, port));

	Net::SDPFile *sdp = cli->GetMediaInfo(url);
	if (sdp)
	{
		Media::MediaFile *mediaFile;
		NEW_CLASS(mediaFile, Media::MediaFile(url));
		Data::ArrayList<Net::RTPCliChannel *> *chList;
		Media::IVideoSource *vid;
		Media::IAudioSource *aud;
		NEW_CLASS(chList, Data::ArrayList<Net::RTPCliChannel*>());
		i = 0;
		j = sdp->GetMediaCount();
		while (i < j)
		{
			Data::ArrayList<const UTF8Char *> *mediaDesc = sdp->GetMediaDesc(i);
			if (mediaDesc)
			{
				Net::RTPCliChannel *rtp = Net::RTPCliChannel::CreateChannel(sockf, mediaDesc, url, cli);
				if (rtp)
				{
					chList->Add(rtp);
				}
			}
			i++;
		}

		i = 0;
		j = chList->GetCount();
		while (i < j)
		{
			Net::RTPCliChannel *rtp = chList->GetItem(i);
			if (rtp->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
			{
				k = 0;
				while ((vid = rtp->CreateShadowVideo(k++)) != 0)
				{
					mediaFile->AddSource(vid, 0);
				}
			}
			else if (rtp->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
			{
				k = 0;
				while ((aud = rtp->CreateShadowAudio(k++)) != 0)
				{
					mediaFile->AddSource(aud, 0);
				}
			}

			i++;
		}

		i = chList->GetCount();
		while (i-- > 0)
		{
			Net::RTPCliChannel *rtp = chList->RemoveAt(i);
			DEL_CLASS(rtp);
		}
		DEL_CLASS(chList);
		SDEL_CLASS(sdp);
		DEL_CLASS(cli);

		return mediaFile;
	}
	else
	{
		DEL_CLASS(cli);
		return 0;
	}
}

Bool Net::RTSPClient::Play(const UTF8Char *url, const UTF8Char *sessId)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff;
	UOSInt buffSize;
	Text::UTF8Writer *writer;
	IO::MemoryStream *stm;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	NEW_CLASS(stm, IO::MemoryStream(UTF8STRC("Net.RTSPClient.Play")));
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	writer->WriteStrC(UTF8STRC("PLAY "));
	writer->WriteStr(url);
	writer->WriteLineC(UTF8STRC(" RTSP/1.0"));
	writer->WriteStrC(UTF8STRC("CSeq: "));
	sptr = Text::StrInt32(sbuff, reqId);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteStrC(UTF8STRC("Session: "));
	writer->WriteLine(sessId);
	writer->WriteLine();
	DEL_CLASS(writer);
	buff = stm->GetBuff(&buffSize);
	this->SendData(buff, buffSize);
	DEL_CLASS(stm);

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = true;
	}
	mutUsage.EndUse();
	return ret;
}

Bool Net::RTSPClient::Close(const UTF8Char *url, const UTF8Char *sessId)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 *buff;
	UOSInt buffSize;
	Text::UTF8Writer *writer;
	IO::MemoryStream *stm;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	NEW_CLASS(stm, IO::MemoryStream(UTF8STRC("Net.RTSPClient.Close")));
	NEW_CLASS(writer, Text::UTF8Writer(stm));
	writer->WriteStrC(UTF8STRC("TEARDOWN "));
	writer->WriteStr(url);
	writer->WriteLineC(UTF8STRC(" RTSP/1.0"));
	writer->WriteStrC(UTF8STRC("CSeq: "));
	sptr = Text::StrInt32(sbuff, reqId);
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	writer->WriteStrC(UTF8STRC("Session: "));
	writer->WriteLine(sessId);
	writer->WriteLine();
	DEL_CLASS(writer);
	buff = stm->GetBuff(&buffSize);
	this->SendData(buff, buffSize);
	DEL_CLASS(stm);

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = true;
	}
	mutUsage.EndUse();
	return ret;
}

Bool Net::RTSPClient::Init(Net::RTPCliChannel *rtpChannel)
{
	UTF8Char sbuff[64];
	if (this->SetupRTP(sbuff, rtpChannel->GetControlURL(), rtpChannel))
	{
		rtpChannel->SetUserData((void*)Text::StrCopyNew(sbuff));
		return true;
	}
	return false;
}

Bool Net::RTSPClient::Play(Net::RTPCliChannel *rtpChannel)
{
	return this->Play(rtpChannel->GetControlURL(), (const UTF8Char*)rtpChannel->GetUserData());
}

Bool Net::RTSPClient::KeepAlive(Net::RTPCliChannel *rtpChannel)
{
//	return this->Play(rtpChannel->GetControlURL(), (const UTF8Char*)rtpChannel->GetUserData());
	return true;
}

Bool Net::RTSPClient::StopPlay(Net::RTPCliChannel *rtpChannel)
{
	return this->Close(rtpChannel->GetControlURL(), (const UTF8Char*)rtpChannel->GetUserData());
}

Bool Net::RTSPClient::Deinit(Net::RTPCliChannel *rtpChannel)
{
	const UTF8Char *sessId = (const UTF8Char*)rtpChannel->GetUserData();
	if (sessId)
	{
		Text::StrDelNew(sessId);
		rtpChannel->SetUserData(0);
	}
	return true;
}

Net::IRTPController *Net::RTSPClient::Clone()
{
	Net::RTSPClient *cli;
	NEW_CLASS(cli, Net::RTSPClient(this));
	return cli;
}
