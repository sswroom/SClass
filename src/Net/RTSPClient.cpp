#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "Manage/HiResClock.h"
#include "Media/MediaFile.h"
#include "Net/RTSPClient.h"
#include "Net/URL.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

UInt32 __stdcall Net::RTSPClient::ControlThread(AnyType userObj)
{
	NN<ClientData> cliData = userObj.GetNN<ClientData>();
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sarr[3];
	UInt8 dataBuff[2048];
	UOSInt buffSize;
	UOSInt thisSize;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Bool content;
	
	cliData->threadRunning = true;
	{
		IO::MemoryStream mstm;
		buffSize = 0;
		content = false;
		while (!cliData->threadToStop)
		{
			if (cliData->cli == 0)
			{
				Sync::MutexUsage mutUsage(cliData->cliMut);
				NEW_CLASS(cliData->cli, Net::TCPClient(cliData->sockf, cliData->host->ToCString(), cliData->port, cliData->timeout));
			}
			if (content)
			{
				thisSize = cliData->cli->Read(BYTEARR(dataBuff));
				if (thisSize == 0)
				{
					Sync::MutexUsage mutUsage(cliData->cliMut);
					DEL_CLASS(cliData->cli);
					cliData->cli = 0;
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
					cliData->reqEvt.Set();
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
				thisSize = cliData->cli->Read(Data::ByteArray(&dataBuff[buffSize], 2048 - buffSize));
				if (thisSize == 0)
				{
					Sync::MutexUsage mutUsage(cliData->cliMut);
					DEL_CLASS(cliData->cli);
					cliData->cli = 0;
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
					mstm.Clear();
					mstm.Write(Data::ByteArrayR(dataBuff, i + 4));
					mstm.SeekFromBeginning(0);
					{
						Text::UTF8Reader reader(mstm);
						sbuff[0] = 0;
						sptr = reader.ReadLine(sbuff, 1021).Or(sbuff);
						if (Text::StrSplit(sarr, 3, sbuff, ' ') != 3)
						{
							cliData->reqReplyStatus = 0;
							cliData->reqSuccess = true;
							cliData->reqEvt.Set();
						}
						else if (!Text::StrEquals(sarr[0], (const UTF8Char*)"RTSP/1.0"))
						{
							cliData->reqReplyStatus = 0;
							cliData->reqSuccess = true;
							cliData->reqEvt.Set();
						}
						else
						{
							cliData->reqReplyStatus = Text::StrToInt32(sarr[1]);
							cliData->reqReplySize = 0;
							while (true)
							{
								if (!reader.ReadLine(sbuff, 1021).SetTo(sptr) || sptr == sbuff)
									break;
								if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Content-Length: ")))
								{
									cliData->reqReplySize = Text::StrToUInt32(&sbuff[16]);
								}
								else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Session: ")))
								{
									SDEL_STRING(cliData->reqStrs);
									k = Text::StrIndexOfChar(&sbuff[9], ';');
									if (k != INVALID_INDEX)
									{
										sbuff[9 + k] = 0;
										sptr = &sbuff[9 + k];
									}
									cliData->reqStrs = Text::String::New(&sbuff[9], (UOSInt)(sptr - &sbuff[9])).Ptr();
								}
								else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Public: ")))
								{
									SDEL_STRING(cliData->reqStrs);
									cliData->reqStrs = Text::String::New(&sbuff[8], (UOSInt)(sptr - &sbuff[8])).Ptr();
								}
							}
							if (cliData->reqReplySize == 0)
							{
								cliData->reqSuccess = true;
								cliData->reqEvt.Set();
							}
							else
							{
								cliData->reqReply = MemAlloc(UInt8, cliData->reqReplySize);
								content = true;
							}
						}
					}

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
							cliData->reqEvt.Set();
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
	}
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
	SDEL_STRING(this->cliData->reqStrs);
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
		this->cliData->reqEvt.Wait(1000);
	}
	return this->cliData->reqSuccess;
}

Bool Net::RTSPClient::SendData(UInt8 *buff, UOSInt buffSize)
{
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->cliData->cliMut);
	if (this->cliData->cli)
	{
		succ = this->cliData->cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize;
	}
	return succ;
}

Net::RTSPClient::RTSPClient(const Net::RTSPClient *cli)
{
	this->cliData = cli->cliData;
	this->cliData->useCnt++;
}

Net::RTSPClient::RTSPClient(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	NEW_CLASS(this->cliData, ClientData());
	this->cliData->useCnt = 1;
	this->cliData->timeout = timeout;
	this->cliData->sockf = sockf;
	this->cliData->nextSeq = 1;
	this->cliData->threadRunning = false;
	this->cliData->threadToStop = false;
	this->cliData->reqReply = 0;
	this->cliData->reqReplySize = 0;
	this->cliData->reqStrs = 0;
	this->cliData->cli = 0;
	this->cliData->host = Text::String::New(host);
	this->cliData->port = port;

	Sync::ThreadUtil::Create(ControlThread, this->cliData);
	while (!this->cliData->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
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
			Sync::SimpleThread::Sleep(10);
		}
		this->NextRequest();
		this->cliData->host->Release();
		DEL_CLASS(this->cliData);
	}
}

Bool Net::RTSPClient::GetOptions(Text::CStringNN url, Data::ArrayList<const UTF8Char *> *options)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *buff;
	Text::PString sarr[10];
	UOSInt i;
	UOSInt buffSize;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	{
		IO::MemoryStream stm;
		{
			Text::UTF8Writer writer(stm);
			writer.Write(CSTR("OPTIONS "));
			writer.Write(url);
			writer.WriteLine(CSTR(" RTSP/1.0"));
			writer.Write(CSTR("CSeq: "));
			sptr = Text::StrInt32(sbuff, reqId);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.WriteLine(CSTR("User-Agent: RTSPClient"));
			writer.WriteLine();
		}
		buff = stm.GetBuff(buffSize);
		this->SendData(buff, buffSize);
	}

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		if (this->cliData->reqStrs)
		{
			sptr = this->cliData->reqStrs->ConcatTo(sbuff);
			buffSize = Text::StrSplitTrimP(sarr, 10, {sbuff, (UOSInt)(sptr - sbuff)}, ',');
			i = 0;
			while (i < buffSize)
			{
				options->Add(Text::StrCopyNewC(sarr[i].v, sarr[i].leng).Ptr());
				i++;
			}
			ret = true;
		}
	}
	mutUsage.EndUse();
	return ret;
}

Net::SDPFile *Net::RTSPClient::GetMediaInfo(Text::CStringNN url)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *buff;
	UOSInt buffSize;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	{
		IO::MemoryStream stm;
		{
			Text::UTF8Writer writer(stm);
			writer.Write(CSTR("DESCRIBE "));
			writer.Write(url);
			writer.WriteLine(CSTR(" RTSP/1.0"));
			writer.Write(CSTR("CSeq: "));
			sptr = Text::StrInt32(sbuff, reqId);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.WriteLine(CSTR("Content-Type: application/sdp"));
			writer.WriteLine();
		}
		buff = stm.GetBuff(buffSize);
		this->SendData(buff, buffSize);
	}

	Bool succ = this->WaitForReply();

	Net::SDPFile *sdp = 0;
	if (succ && this->cliData->reqReplyStatus == 200 && this->cliData->reqReplySize > 0 && this->cliData->reqReply)
	{
		NEW_CLASS(sdp, Net::SDPFile(this->cliData->reqReply, this->cliData->reqReplySize));
	}
	mutUsage.EndUse();
	return sdp;
}

UnsafeArrayOpt<UTF8Char> Net::RTSPClient::SetupRTP(UnsafeArray<UTF8Char> sessIdOut, Text::CStringNN url, NN<Net::RTPCliChannel> rtpChannel)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *buff;
	UOSInt buffSize;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	{
		IO::MemoryStream stm;
		{
			Text::UTF8Writer writer(stm);
			writer.Write(CSTR("SETUP "));
			writer.Write(url);
			writer.WriteLine(CSTR(" RTSP/1.0"));
			writer.Write(CSTR("CSeq: "));
			sptr = Text::StrInt32(sbuff, reqId);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.Write(CSTR("Transport: "));
			sptr = rtpChannel->GetTransportDesc(sbuff);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.WriteLine();
		}
		buff = stm.GetBuff(buffSize);
		this->SendData(buff, buffSize);
	}

	Bool succ = this->WaitForReply();

	UnsafeArrayOpt<UTF8Char> ret = 0;
	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = this->cliData->reqStrs->ConcatTo(sessIdOut);
	}
	mutUsage.EndUse();
	return ret;
}

IO::ParsedObject *Net::RTSPClient::ParseURL(NN<Net::SocketFactory> sockf, Text::CStringNN url, Data::Duration timeout, NN<IO::LogTool> log)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Net::RTSPClient *cli;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	UInt16 port = 554;
	sptr = Text::URLString::GetURLDomain(sbuff, url, port);
	if (port == 0)
	{
		port = 554;
	}

	NEW_CLASS(cli, Net::RTSPClient(sockf, CSTRP(sbuff, sptr), port, timeout));

	Net::SDPFile *sdp = cli->GetMediaInfo(url);
	if (sdp)
	{
		Media::MediaFile *mediaFile;
		NEW_CLASS(mediaFile, Media::MediaFile(url));
		Data::ArrayListNN<Net::RTPCliChannel> chList;
		NN<Media::IVideoSource> vid;
		NN<Media::IAudioSource> aud;
		i = 0;
		j = sdp->GetMediaCount();
		while (i < j)
		{
			NN<Data::ArrayListStrUTF8> mediaDesc;
			if (sdp->GetMediaDesc(i).SetTo(mediaDesc))
			{
				NN<Net::RTPCliChannel> rtp = Net::RTPCliChannel::CreateChannel(sockf, mediaDesc, url, cli, log);
				chList.Add(rtp);
			}
			i++;
		}

		i = 0;
		j = chList.GetCount();
		while (i < j)
		{
			NN<Net::RTPCliChannel> rtp = chList.GetItemNoCheck(i);
			if (rtp->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
			{
				k = 0;
				while (rtp->CreateShadowVideo(k++).SetTo(vid))
				{
					mediaFile->AddSource(vid, 0);
				}
			}
			else if (rtp->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
			{
				k = 0;
				while (rtp->CreateShadowAudio(k++).SetTo(aud))
				{
					mediaFile->AddSource(aud, 0);
				}
			}

			i++;
		}

		chList.DeleteAll();
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

Bool Net::RTSPClient::Play(Text::CStringNN url, Text::CStringNN sessId)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *buff;
	UOSInt buffSize;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	{
		IO::MemoryStream stm;
		{
			Text::UTF8Writer writer(stm);
			writer.Write(CSTR("PLAY "));
			writer.Write(url);
			writer.WriteLine(CSTR(" RTSP/1.0"));
			writer.Write(CSTR("CSeq: "));
			sptr = Text::StrInt32(sbuff, reqId);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.Write(CSTR("Session: "));
			writer.WriteLine(sessId);
			writer.WriteLine();
		}
		buff = stm.GetBuff(buffSize);
		this->SendData(buff, buffSize);
	}

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = true;
	}
	mutUsage.EndUse();
	return ret;
}

Bool Net::RTSPClient::Close(Text::CStringNN url, Text::CStringNN sessId)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *buff;
	UOSInt buffSize;

	Sync::MutexUsage mutUsage(this->cliData->reqMut);
	Int32 reqId = this->NextRequest();

	{
		IO::MemoryStream stm;
		{
			Text::UTF8Writer writer(stm);
			writer.Write(CSTR("TEARDOWN "));
			writer.Write(url);
			writer.WriteLine(CSTR(" RTSP/1.0"));
			writer.Write(CSTR("CSeq: "));
			sptr = Text::StrInt32(sbuff, reqId);
			writer.WriteLine(CSTRP(sbuff, sptr));
			writer.Write(CSTR("Session: "));
			writer.WriteLine(sessId);
			writer.WriteLine();
		}
		buff = stm.GetBuff(buffSize);
		this->SendData(buff, buffSize);
	}

	Bool succ = this->WaitForReply();
	Bool ret = false;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = true;
	}
	mutUsage.EndUse();
	return ret;
}

Bool Net::RTSPClient::Init(NN<Net::RTPCliChannel> rtpChannel)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (this->SetupRTP(sbuff, rtpChannel->GetControlURL()->ToCString(), rtpChannel).SetTo(sptr))
	{
		rtpChannel->SetUserData((void*)Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr());
		return true;
	}
	return false;
}

Bool Net::RTSPClient::Play(NN<Net::RTPCliChannel> rtpChannel)
{
	return this->Play(rtpChannel->GetControlURL()->ToCString(), rtpChannel->GetUserData().GetNN<Text::String>()->ToCString());
}

Bool Net::RTSPClient::KeepAlive(NN<Net::RTPCliChannel> rtpChannel)
{
//	return this->Play(rtpChannel->GetControlURL(), (Text::String*)rtpChannel->GetUserData());
	return true;
}

Bool Net::RTSPClient::StopPlay(NN<Net::RTPCliChannel> rtpChannel)
{
	return this->Close(rtpChannel->GetControlURL()->ToCString(), rtpChannel->GetUserData().GetNN<Text::String>()->ToCString());
}

Bool Net::RTSPClient::Deinit(NN<Net::RTPCliChannel> rtpChannel)
{
	NN<Text::String> sessId;
	if (rtpChannel->GetUserData().GetOpt<Text::String>().SetTo(sessId))
	{
		sessId->Release();
		rtpChannel->SetUserData(0);
	}
	return true;
}

Net::IRTPController *Net::RTSPClient::Clone() const
{
	Net::RTSPClient *cli;
	NEW_CLASS(cli, Net::RTSPClient(this));
	return cli;
}
