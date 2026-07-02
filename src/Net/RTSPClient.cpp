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
	UIntOS buffSize;
	UIntOS thisSize;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	Bool content;
	
	cliData->threadRunning = true;
	{
		NN<Net::TCPClient> cli;
		IO::MemoryStream mstm;
		buffSize = 0;
		content = false;
		while (!cliData->threadToStop)
		{
			if (!cliData->cli.SetTo(cli))
			{
				Sync::MutexUsage mutUsage(cliData->cliMut);
				cliData->cli = cli = cliData->clif->Create(cliData->host->ToCString(), cliData->port, cliData->timeout);
			}
			if (content)
			{
				thisSize = cli->Read(BYTEARR(dataBuff));
				if (thisSize == 0)
				{
					Sync::MutexUsage mutUsage(cliData->cliMut);
					cliData->cli.Delete();
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
				thisSize = cli->Read(Data::ByteArray(&dataBuff[buffSize], 2048 - buffSize));
				if (thisSize == 0)
				{
					Sync::MutexUsage mutUsage(cliData->cliMut);
					cliData->cli.Delete();
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
								if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Content-Length: ")))
								{
									cliData->reqReplySize = Text::StrToUInt32(&sbuff[16]);
								}
								else if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Session: ")))
								{
									OPTSTR_DEL(cliData->reqStrs);
									k = Text::StrIndexOfChar(&sbuff[9], ';');
									if (k != INVALID_INDEX)
									{
										sbuff[9 + k] = 0;
										sptr = &sbuff[9 + k];
									}
									cliData->reqStrs = Text::String::New(&sbuff[9], (UIntOS)(sptr - &sbuff[9]));
								}
								else if (Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Public: ")))
								{
									OPTSTR_DEL(cliData->reqStrs);
									cliData->reqStrs = Text::String::New(&sbuff[8], (UIntOS)(sptr - &sbuff[8]));
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
		cliData->cli.Delete();
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
	OPTSTR_DEL(this->cliData->reqStrs);
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

Bool Net::RTSPClient::SendData(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	Bool succ = false;
	Sync::MutexUsage mutUsage(this->cliData->cliMut);
	NN<Net::TCPClient> cli;
	if (this->cliData->cli.SetTo(cli))
	{
		succ = cli->Write(Data::ByteArrayR(buff, buffSize)) == buffSize;
	}
	return succ;
}

Net::RTSPClient::RTSPClient(NN<const Net::RTSPClient> cli)
{
	this->cliData = cli->cliData;
	this->cliData->useCnt++;
}

Net::RTSPClient::RTSPClient(NN<Net::TCPClientFactory> clif, Text::CStringNN host, UInt16 port, Data::Duration timeout)
{
	NEW_CLASSNN(this->cliData, ClientData());
	this->cliData->useCnt = 1;
	this->cliData->timeout = timeout;
	this->cliData->clif = clif;
	this->cliData->nextSeq = 1;
	this->cliData->threadRunning = false;
	this->cliData->threadToStop = false;
	this->cliData->reqReply = 0;
	this->cliData->reqReplySize = 0;
	this->cliData->reqStrs = nullptr;
	this->cliData->cli = nullptr;
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
	NN<Net::TCPClient> cli;
	if (--this->cliData->useCnt == 0)
	{
		this->cliData->threadToStop = true;
		Sync::MutexUsage mutUsage(this->cliData->cliMut);
		if (this->cliData->cli.SetTo(cli))
		{
			cli->Close();
		}
		mutUsage.EndUse();
		while (this->cliData->threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->NextRequest();
		this->cliData->host->Release();
		this->cliData.Delete();
	}
}

Bool Net::RTSPClient::GetOptions(Text::CStringNN url, NN<Data::ArrayListArr<const UTF8Char>> options)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> buff;
	Text::PString sarr[10];
	UIntOS i;
	UIntOS buffSize;

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
	NN<Text::String> reqStrs;

	if (succ && this->cliData->reqReplyStatus == 200)
	{
		if (this->cliData->reqStrs.SetTo(reqStrs))
		{
			sptr = reqStrs->ConcatTo(sbuff);
			buffSize = Text::StrSplitTrimP(sarr, 10, {sbuff, (UIntOS)(sptr - sbuff)}, ',');
			i = 0;
			while (i < buffSize)
			{
				options->Add(Text::StrCopyNewC(sarr[i].v, sarr[i].leng));
				i++;
			}
			ret = true;
		}
	}
	mutUsage.EndUse();
	return ret;
}

Optional<Net::SDPFile> Net::RTSPClient::GetMediaInfo(Text::CStringNN url)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> buff;
	UIntOS buffSize;

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

	Optional<Net::SDPFile> sdp = nullptr;
	if (succ && this->cliData->reqReplyStatus == 200 && this->cliData->reqReplySize > 0 && this->cliData->reqReply)
	{
		NEW_CLASSOPT(sdp, Net::SDPFile(this->cliData->reqReply, this->cliData->reqReplySize));
	}
	mutUsage.EndUse();
	return sdp;
}

UnsafeArrayOpt<UTF8Char> Net::RTSPClient::SetupRTP(UnsafeArray<UTF8Char> sessIdOut, Text::CStringNN url, NN<Net::RTPCliChannel> rtpChannel)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> buff;
	UIntOS buffSize;

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

	UnsafeArrayOpt<UTF8Char> ret = nullptr;
	if (succ && this->cliData->reqReplyStatus == 200)
	{
		ret = Text::String::OrEmpty(this->cliData->reqStrs)->ConcatTo(sessIdOut);
	}
	mutUsage.EndUse();
	return ret;
}

Optional<IO::ParsedObject> Net::RTSPClient::ParseURL(NN<Net::TCPClientFactory> clif, Text::CStringNN url, Data::Duration timeout, NN<IO::LogTool> log)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Net::RTSPClient> cli;
	UIntOS i;
	UIntOS j;
	UIntOS k;

	UInt16 port = 554;
	sptr = Text::URLString::GetURLDomain(sbuff, url, port);
	if (port == 0)
	{
		port = 554;
	}

	NEW_CLASSNN(cli, Net::RTSPClient(clif, CSTRP(sbuff, sptr), port, timeout));

	NN<Net::SDPFile> sdp;
	if (cli->GetMediaInfo(url).SetTo(sdp))
	{
		NN<Media::MediaFile> mediaFile;
		NEW_CLASSNN(mediaFile, Media::MediaFile(url));
		Data::ArrayListNN<Net::RTPCliChannel> chList;
		NN<Media::VideoSource> vid;
		NN<Media::AudioSource> aud;
		i = 0;
		j = sdp->GetMediaCount();
		while (i < j)
		{
			NN<Data::ArrayListStrUTF8> mediaDesc;
			if (sdp->GetMediaDesc(i).SetTo(mediaDesc))
			{
				NN<Net::RTPCliChannel> rtp = Net::RTPCliChannel::CreateChannel(clif->GetSocketFactory(), mediaDesc, url, cli, log);
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
		sdp.Delete();
		cli.Delete();
		return mediaFile;
	}
	else
	{
		cli.Delete();
		return nullptr;
	}
}

Bool Net::RTSPClient::Play(Text::CStringNN url, Text::CStringNN sessId)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UInt8> buff;
	UIntOS buffSize;

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
	UnsafeArray<UInt8> buff;
	UIntOS buffSize;

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
	if (this->SetupRTP(sbuff, Text::String::OrEmpty(rtpChannel->GetControlURL())->ToCString(), rtpChannel).SetTo(sptr))
	{
		rtpChannel->SetUserData((void*)Text::String::New(sbuff, (UIntOS)(sptr - sbuff)).Ptr());
		return true;
	}
	return false;
}

Bool Net::RTSPClient::Play(NN<Net::RTPCliChannel> rtpChannel)
{
	return this->Play(Text::String::OrEmpty(rtpChannel->GetControlURL())->ToCString(), rtpChannel->GetUserData().GetNN<Text::String>()->ToCString());
}

Bool Net::RTSPClient::KeepAlive(NN<Net::RTPCliChannel> rtpChannel)
{
//	return this->Play(rtpChannel->GetControlURL(), (Text::String*)rtpChannel->GetUserData());
	return true;
}

Bool Net::RTSPClient::StopPlay(NN<Net::RTPCliChannel> rtpChannel)
{
	return this->Close(Text::String::OrEmpty(rtpChannel->GetControlURL())->ToCString(), rtpChannel->GetUserData().GetNN<Text::String>()->ToCString());
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

NN<Net::RTPController> Net::RTSPClient::Clone() const
{
	NN<Net::RTSPClient> cli;
	NEW_CLASSNN(cli, Net::RTSPClient(*this));
	return cli;
}
