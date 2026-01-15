#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Console.h"
#include "Net/RTPPayloadHandler.h"
#include "Net/RTPASource.h"
#include "Net/RTPCliChannel.h"
#include "Net/RTPH264Handler.h"
#include "Net/RTPAACHandler.h"
#include "Net/RTPVSource.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

#include <stdio.h>

void __stdcall Net::RTPCliChannel::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<ChannelData> chData = userData.GetNN<ChannelData>();
//	WChar wbuff[32];

	if (data.GetSize() < 12)
		return;
	Int32 v = data[0] >> 6;
	if (v != 2)
		return;
	if (data[0] & 0x20) //padding
	{
		data = data.WithSize(data.GetSize() - data[data.GetSize() - 1]);
		if (data.GetSize() < 12)
			return;
	}
//	Bool extension = (buff[0] & 0x10) != 0;
//	Int32 csrcCnt = buff[0] & 15;
	Bool marker = (data[1] & 0x80) != 0;
	Int32 payloadType = data[1] & 0x7f;
	UInt32 seqNum = ReadMUInt16(&data[2]);
	UInt32 timestamp = ReadMUInt32(&data[4]);
	chData->lastSSRC = ReadMUInt32(&data[8]);

	if (marker)
	{
		marker = true;
	}

	if (seqNum == 0)
	{
		chData->lastSeqNumHi++;
	}
	chData->lastSeqNumLo = seqNum;
//	Text::StrConcatC(Text::StrInt32(wbuff, seqNum), UTF8STRC("\r\n"));
//	IO::Console::PrintStrO(wbuff);

	Sync::MutexUsage mutUsage(chData->packMut);
	if (chData->packCnt >= chData->buffCnt)
	{
		Bool lastExist = false;
		UIntOS i;
		UIntOS minIndex = 0;
		UInt32 minSeq = chData->packBuff[0].seqNum;
		if (minSeq == 65535)
			lastExist = true;
		i = chData->buffCnt;
		while (i-- > 0)
		{
			if (chData->packBuff[i].seqNum == 65535)
				lastExist = true;
			if (chData->packBuff[i].seqNum < minSeq)
			{
				minSeq = chData->packBuff[i].seqNum;
				minIndex = i;
			}
		}
		if (lastExist)
		{
			minSeq = 65536;
			i = chData->buffCnt;
			while (i-- > 0)
			{
				if (chData->packBuff[i].seqNum > 32767 && chData->packBuff[i].seqNum < minSeq)
				{
					minSeq = chData->packBuff[i].seqNum;
					minIndex = i;
				}
			}
		}


		Net::RTPPayloadHandler *plHdlr = chData->payloadMap.Get(chData->packBuff[minIndex].payloadType);
		if (plHdlr)
		{
			plHdlr->MediaDataReceived(chData->packBuff[minIndex].buff, chData->packBuff[minIndex].dataSize, chData->packBuff[minIndex].seqNum, chData->packBuff[minIndex].ts);
		}
		chData->packBuff[minIndex].payloadType = payloadType;
		chData->packBuff[minIndex].seqNum = seqNum;
		chData->packBuff[minIndex].dataSize  = data.GetSize() - 12;
		chData->packBuff[minIndex].ts = timestamp;
		MemCopyNO(chData->packBuff[minIndex].buff, &data[12], data.GetSize() - 12);
	}
	else
	{
		chData->packBuff[chData->packCnt].payloadType = payloadType;
		chData->packBuff[chData->packCnt].seqNum = seqNum;
		chData->packBuff[chData->packCnt].dataSize  = data.GetSize() - 12;
		chData->packBuff[chData->packCnt].ts = timestamp;
		MemCopyNO(chData->packBuff[chData->packCnt].buff, &data[12], data.GetSize() - 12);
		chData->packCnt++;
	}

	mutUsage.EndUse();
}

void __stdcall Net::RTPCliChannel::PacketCtrlHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<ChannelData> chData = userData.GetNN<ChannelData>();
	UInt8 tmpBuff[100];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UIntOS size = 0;
	UIntOS ofst = 0;
	UIntOS i;
	while (ofst < data.GetSize())
	{
		size = (UIntOS)(ReadMUInt16(&data[ofst + 2]) + 1) << 2;
		if (size + ofst > data.GetSize())
		{
			break;
		}
		else if ((data[ofst + 0] & 0xc0) == 0x80)
		{
			switch (data[ofst + 1])
			{
			case 200: //SR - Sender Report
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("SR: Len="));
					sb.AppendUIntOS(size);
					sb.AppendC(UTF8STRC(",RC="));
					sb.AppendU32(data[ofst +0] & 0x1f);
					sb.AppendC(UTF8STRC(",SSRC="));
					sb.AppendI32(ReadMInt32(&data[ofst + 4]));
					if (size >= 28)
					{
						sb.AppendC(UTF8STRC(",NTP ts="));
						sb.AppendTSNoZone(Data::Timestamp::FromNTPTime(ReadMUInt32(&data[ofst + 8]), ReadMUInt32(&data[ofst + 12]), 0));
						sb.AppendC(UTF8STRC(",RTP ts="));
						sb.AppendI32(ReadMInt32(&data[ofst + 16]));
						sb.AppendC(UTF8STRC(",nPacket="));
						sb.AppendI32(ReadMInt32(&data[ofst + 20]));
						sb.AppendC(UTF8STRC(",nOctet="));
						sb.AppendI32(ReadMInt32(&data[ofst + 24]));
					}
					sb.AppendC(UTF8STRC("\r\n"));
					printf("%s", sb.ToPtr());
				}

				tmpBuff[0] = 0x81;
				tmpBuff[1] = 201;
				tmpBuff[2] = 0;
				tmpBuff[3] = 7;
				WriteMInt32(&tmpBuff[4], ReadMInt32(&data[ofst + 4]));
				WriteMInt32(&tmpBuff[8], ReadMInt32(&data[ofst + 4]));
				tmpBuff[12] = 0;
				tmpBuff[13] = 0xff;
				tmpBuff[14] = 0xff;
				tmpBuff[15] = 0xff;
				WriteMUInt32(&tmpBuff[16], (chData->lastSeqNumHi << 16) | chData->lastSeqNumLo);
				WriteMInt32(&tmpBuff[20], 0);
				WriteMInt32(&tmpBuff[24], ReadMInt32(&data[ofst + 10]));
				WriteMInt32(&tmpBuff[28], 0);

				tmpBuff[32] = 0x81;
				tmpBuff[33] = 202;
				WriteMInt32(&tmpBuff[36], ReadMInt32(&data[ofst + 4]));
				{
					i = 40;
					tmpBuff[i] = 1;
					sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
					Text::StrConcatC(&tmpBuff[i + 2], sbuff, (UIntOS)(sptr - sbuff));
					tmpBuff[i + 1] = (UInt8)(sptr - sbuff);
					i += (UIntOS)(tmpBuff[i + 1] + 2);
					tmpBuff[i] = 0;
					i++;
					if (i & 3)
					{
						i += 4 - (i & 3);
					}
					WriteMInt16(&tmpBuff[34], (i - 12) >> 2);
				}
				chData->rtcpUDP->SendTo(addr, port, tmpBuff, i);
				break;
			case 201: //RR - Receiver Report
				printf("RR\r\n");
				break;
			case 202: //SDES - Source Description RTCP Packet
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("SDES: Len="));
					sb.AppendUIntOS(size);
					sb.AppendC(UTF8STRC(",RC="));
					sb.AppendU32(data[ofst + 0] & 0x1f);
					sb.AppendC(UTF8STRC(",SSRC="));
					sb.AppendI32(ReadMInt32(&data[ofst + 4]));
					{
						i = 8;
						while (i < size)
						{
							switch (data[ofst + i])
							{
							case 0:
							default:
								i = size;
								break;
							case 1:
								sb.AppendC(UTF8STRC(", CNAME="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 2:
								sb.AppendC(UTF8STRC(", NAME="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 3:
								sb.AppendC(UTF8STRC(", EMAIL="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 4:
								sb.AppendC(UTF8STRC(", PHONE="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 5:
								sb.AppendC(UTF8STRC(", LOC="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 6:
								sb.AppendC(UTF8STRC(", TOOL="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 7:
								sb.AppendC(UTF8STRC(", NOTE="));
								sb.AppendC((const UTF8Char*)&data[ofst + i + 2], data[ofst + i + 1]);
								i += (UIntOS)(data[ofst + i + 1] + 2);
								break;
							case 8:
								sb.AppendC(UTF8STRC(", PRIV="));
								i = size;
								break;
							}
						}
					}
					sb.AppendC(UTF8STRC("\r\n"));
					printf("%s", sb.ToPtr());
				}
				break;
			case 203: //BYE
				break;
			default:
				i = 1;
				break;
			}
		}
		ofst += size;
	}
}

UInt32 __stdcall Net::RTPCliChannel::PlayThread(AnyType userObj)
{
	NN<Net::RTPCliChannel> me = userObj.GetNN<Net::RTPCliChannel>();
	{
		Data::DateTime dt;
		Data::DateTime lastDt;

		me->chData->playing = true;
		if (me->chData->playCtrl->Init(me))
		{
			if (me->chData->playCtrl->Play(me))
			{
				lastDt.SetCurrTimeUTC();
				while (!me->chData->playToStop)
				{
					dt.SetCurrTimeUTC();
					if (dt.DiffMS(lastDt) > 5000)
					{
						lastDt.SetCurrTimeUTC();
						me->chData->playCtrl->KeepAlive(me);
					}
					me->chData->playEvt.Wait(5000);
				}
				me->chData->playCtrl->StopPlay(me);
			}
			me->chData->playCtrl->Deinit(me);
		}
	}
	me->chData->playing = false;
	return 0;
}

void Net::RTPCliChannel::SetControlURL(Text::CStringNN url)
{
	SDEL_STRING(this->chData->controlURL);
	this->chData->controlURL = Text::String::New(url).Ptr();
}

void Net::RTPCliChannel::SetPlayControl(Net::RTPController *playCtrl)
{
	this->chData->playCtrl = playCtrl;
}

Net::RTPCliChannel::RTPCliChannel(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log)
{
	NEW_CLASS(this->chData, ChannelData());
	this->chData->useCnt = 1;

	UIntOS i;
	this->chData->sockf = sockf;
	this->chData->rtpUDP = 0;
	this->chData->rtcpUDP = 0;
	this->chData->userData = 0;
	this->chData->controlURL = 0;
	this->chData->playing = false;
	this->chData->lastSeqNumHi = 0;
	this->chData->lastSeqNumLo = 0;
	this->chData->mediaType = Media::MEDIA_TYPE_UNKNOWN;
	this->chData->threadCnt = 5;
	this->chData->buffCnt = 32;
	this->chData->packCnt = 0;
	this->chData->packBuff = MemAlloc(PacketBuff, this->chData->buffCnt);
	i = this->chData->buffCnt;
	while (i-- > 0)
	{
		this->chData->packBuff[i].buff = MemAlloc(UInt8, 2048);
	}

	if (port & 1)
	{
		port = (UInt16)(port + 1);
	}
	NEW_CLASS(this->chData->rtpUDP, Net::UDPServer(sockf, nullptr, port, nullptr, PacketHdlr, this->chData, log, nullptr, this->chData->threadCnt, false));
	if (port == 0)
	{
		port = this->chData->rtpUDP->GetPort();
		if (port & 1)
		{
			port = (UInt16)(port + 1);
			DEL_CLASS(this->chData->rtpUDP);
			NEW_CLASS(this->chData->rtpUDP, Net::UDPServer(sockf, nullptr, port, nullptr, PacketHdlr, this->chData, log, nullptr, this->chData->threadCnt, false));
		}
	}
	this->chData->rtpUDP->SetBuffSize(1048576);
	NEW_CLASS(this->chData->rtcpUDP, Net::UDPServer(sockf, nullptr, (UInt16)(port + 1), nullptr, PacketCtrlHdlr, this->chData, log, nullptr, 1, false));
}

Net::RTPCliChannel::RTPCliChannel(Net::RTPCliChannel *ch)
{
	this->chData = ch->chData;
	this->chData->useCnt++;
}

Net::RTPCliChannel::~RTPCliChannel()
{
	if (--this->chData->useCnt == 0)
	{
		UIntOS i;
		Net::RTPPayloadHandler *plHdlr;
		this->StopPlay();

		DEL_CLASS(this->chData->rtpUDP);
		SDEL_CLASS(this->chData->rtcpUDP);
		SDEL_STRING(this->chData->controlURL);

		i = this->chData->payloadMap.GetCount();
		while (i-- > 0)
		{
			plHdlr = this->chData->payloadMap.GetItem(i);
			DEL_CLASS(plHdlr);
		}
		i = this->chData->buffCnt;
		while (i-- > 0)
		{
			MemFree(this->chData->packBuff[i].buff);
		}
		MemFree(this->chData->packBuff);

		DEL_CLASS(this->chData->playCtrl);
		DEL_CLASS(this->chData);
	}
}

UInt16 Net::RTPCliChannel::GetPort()
{
	return this->chData->rtpUDP->GetPort();
}

UnsafeArray<UTF8Char> Net::RTPCliChannel::GetTransportDesc(UnsafeArray<UTF8Char> sbuff)
{
	UInt16 port = GetPort();
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("RTP/AVP;unicast;client_port="));
	sbuff = Text::StrInt32(sbuff, port);
	sbuff = Text::StrConcatC(sbuff, UTF8STRC("-"));
	sbuff = Text::StrInt32(sbuff, port + 1);
	return sbuff;
}

Text::String *Net::RTPCliChannel::GetControlURL()
{
	return this->chData->controlURL;
}

Media::MediaType Net::RTPCliChannel::GetMediaType()
{
	return this->chData->mediaType;
}

void Net::RTPCliChannel::SetMediaType(Media::MediaType mediaType)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_UNKNOWN)
		return;
	this->chData->mediaType = mediaType;
}

Optional<Media::VideoSource> Net::RTPCliChannel::GetVideo(UIntOS index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_VIDEO)
		return nullptr;
	return (Net::RTPVPLHandler*)this->chData->payloadMap.GetItem(index);
}

Optional<Media::AudioSource> Net::RTPCliChannel::GetAudio(UIntOS index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_AUDIO)
		return nullptr;
	return nullptr;//(Net::RTPVPLHandler*)this->payloadMap->GetValues()->GetItem(index);
}

Optional<Media::VideoSource> Net::RTPCliChannel::CreateShadowVideo(UIntOS index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_VIDEO)
		return nullptr;
	NN<Net::RTPVPLHandler> hdlr;
	if (!hdlr.Set((Net::RTPVPLHandler*)this->chData->payloadMap.GetItem(index)))
	{
		return nullptr;
	}
	Net::RTPVSource *vSrc;
	NN<Net::RTPCliChannel> ch;
	NEW_CLASSNN(ch, Net::RTPCliChannel(this));
	NEW_CLASS(vSrc, Net::RTPVSource(ch, hdlr));
	return vSrc;
}

Optional<Media::AudioSource> Net::RTPCliChannel::CreateShadowAudio(UIntOS index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_AUDIO)
		return nullptr;
	NN<Net::RTPAPLHandler> hdlr;
	if (!hdlr.Set((Net::RTPAPLHandler*)this->chData->payloadMap.GetItem(index)))
	{
		return nullptr;
	}
	Net::RTPASource *aSrc;
	NN<Net::RTPCliChannel> ch;
	NEW_CLASSNN(ch, Net::RTPCliChannel(*this));
	NEW_CLASS(aSrc, Net::RTPASource(ch, hdlr));
	return aSrc;
}

AnyType Net::RTPCliChannel::GetUserData()
{
	return this->chData->userData;
}

void Net::RTPCliChannel::SetUserData(AnyType userData)
{
	this->chData->userData = userData;
}

Bool Net::RTPCliChannel::StartPlay()
{
	if (this->chData->playing)
		return false;
	this->chData->playToStop = false;
	Sync::ThreadUtil::Create(PlayThread, this);
	while (!this->chData->playing)
	{
		Sync::SimpleThread::Sleep(10);
	}
	return true;
}

Bool Net::RTPCliChannel::StopPlay()
{
	if (this->chData->playing)
	{
		this->chData->playToStop = true;
		this->chData->playEvt.Set();
		while (this->chData->playing)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	return true;
}

Bool Net::RTPCliChannel::IsRunning()
{
	return this->chData->playing;
}

Bool Net::RTPCliChannel::MapPayloadType(Int32 payloadType, Text::CStringNN typ, UInt32 freq, UInt32 nChannel)
{
	Net::RTPPayloadHandler *hdlr;
	if (this->chData->payloadMap.Get(payloadType))
	{
		return false;
	}
	if (this->chData->mediaType == Media::MEDIA_TYPE_AUDIO)
	{
		if (typ.Equals(UTF8STRC("PCMU")))
		{
			//////////////////////////
			return false;
		}
		else if (typ.Equals(UTF8STRC("telephone-event")))
		{
			//////////////////////////
			return false;
		}
		else if (typ.Equals(UTF8STRC("mpeg4-generic")))
		{
			NEW_CLASS(hdlr, Net::RTPAACHandler(payloadType, freq, nChannel));
			this->chData->payloadMap.Put(payloadType, hdlr);
			return true;
		}
	}
	else if (this->chData->mediaType == Media::MEDIA_TYPE_VIDEO)
	{
		if (typ.Equals(UTF8STRC("H264")))
		{
			NEW_CLASS(hdlr, Net::RTPH264Handler(payloadType));
			this->chData->payloadMap.Put(payloadType, hdlr);
			return true;
		}
	}
	return false;
}

Bool Net::RTPCliChannel::SetPayloadFormat(Int32 payloadType, UnsafeArray<const UTF8Char> format)
{
	Net::RTPPayloadHandler *plHdlr = this->chData->payloadMap.Get(payloadType);
	if (plHdlr)
	{
		plHdlr->SetFormat(format);
		return true;
	}
	return false;
}

NN<Net::RTPCliChannel> Net::RTPCliChannel::CreateChannel(NN<Net::SocketFactory> sockf, NN<Data::ArrayListStrUTF8> sdpDesc, Text::CStringNN ctrlURL, Net::RTPController *playCtrl, NN<IO::LogTool> log)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[5];
	NN<Net::RTPCliChannel> ch;
	UnsafeArray<const UTF8Char> desc;
	UIntOS descLen;
	NEW_CLASSNN(ch, Net::RTPCliChannel(sockf, 0, log));
	ch->SetPlayControl(playCtrl->Clone());
	Bool ctrlFound = false;
	Int32 payloadType;
	UIntOS k;

	UIntOS i = 0;
	UIntOS j = sdpDesc->GetCount();
	while (i < j)
	{
		desc = sdpDesc->GetItem(i).Or(U8STR(""));
		descLen = Text::StrCharCnt(desc);
		if (Text::StrStartsWithC(desc, descLen, UTF8STRC("m=")))
		{
			if (Text::StrStartsWithC(desc, descLen, UTF8STRC("m=video ")))
			{
				ch->SetMediaType(Media::MEDIA_TYPE_VIDEO);
			}
			else if (Text::StrStartsWithC(desc, descLen, UTF8STRC("m=audio ")))
			{
				ch->SetMediaType(Media::MEDIA_TYPE_AUDIO);
			}
		}
		else if (Text::StrStartsWithC(desc, descLen, UTF8STRC("a=control:")))
		{
			ctrlFound = true;
			if (Text::StrIndexOfC(&desc[10], descLen - 10, UTF8STRC("://")) != INVALID_INDEX)
			{
				ch->SetControlURL({&desc[10], descLen - 10});
			}
			else
			{
				sptr = Text::StrConcatC(ctrlURL.ConcatTo(sbuff), UTF8STRC("/"));
				sptr = Text::URLString::AppendURLPath(sbuff, sptr, Text::CStringNN(&desc[10], descLen - 10)).Or(sbuff);
				ch->SetControlURL(CSTRP(sbuff, sptr));
			}
		}
		else if (Text::StrStartsWithC(desc, descLen, UTF8STRC("a=rtpmap:")))
		{
			sptr = Text::StrConcatC(sbuff, &desc[9], descLen - 9);
			if (Text::StrSplitP(sarr, 3, {sbuff, (UIntOS)(sptr - sbuff)}, ' ') == 2)
			{
				payloadType = Text::StrToInt32(sarr[0].v);
				k = Text::StrSplitP(sarr, 4, sarr[1], '/');
				if (k == 2 && ch->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
				{
					ch->MapPayloadType(payloadType, sarr[0].ToCString(), Text::StrToUInt32(sarr[1].v), 0);
				}
				else if (k == 3 && ch->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					ch->MapPayloadType(payloadType, sarr[0].ToCString(), Text::StrToUInt32(sarr[1].v), Text::StrToUInt32(sarr[2].v));
				}
				else if (k == 2 && ch->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					ch->MapPayloadType(payloadType, sarr[0].ToCString(), Text::StrToUInt32(sarr[1].v), 1);
				}
			}
		}
		else if (Text::StrStartsWithC(desc, descLen, UTF8STRC("a=fmtp:")))
		{
			sptr = Text::StrConcatC(sbuff, &desc[7], descLen - 7);
			if (Text::StrSplitP(sarr, 2, {sbuff, (UIntOS)(sptr - sbuff)}, ' ') == 2)
			{
				payloadType = Text::StrToInt32(sarr[0].v);
				ch->SetPayloadFormat(payloadType, sarr[1].v);
			}
		}
		i++;
	}
	if (!ctrlFound)
	{
		ch->SetControlURL(ctrlURL);
	}

	return ch;
}
