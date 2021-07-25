#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Console.h"
#include "Net/IRTPPLHandler.h"
#include "Net/RTPASource.h"
#include "Net/RTPCliChannel.h"
#include "Net/RTPH264Handler.h"
#include "Net/RTPAACHandler.h"
#include "Net/RTPVSource.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/URLString.h"

#include <stdio.h>

void __stdcall Net::RTPCliChannel::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	ChannelData *chData = (ChannelData*)userData;
//	WChar sbuff[32];

	if (dataSize < 12)
		return;
	Int32 v = buff[0] >> 6;
	if (v != 2)
		return;
	if (buff[0] & 0x20) //padding
	{
		dataSize -= buff[dataSize - 1];
		if (dataSize < 12)
			return;
	}
//	Bool extension = (buff[0] & 0x10) != 0;
//	Int32 csrcCnt = buff[0] & 15;
	Bool marker = (buff[1] & 0x80) != 0;
	Int32 payloadType = buff[1] & 0x7f;
	UInt32 seqNum = ReadMUInt16(&buff[2]);
	UInt32 timestamp = ReadMUInt32(&buff[4]);
	chData->lastSSRC = ReadMUInt32(&buff[8]);

	if (marker)
	{
		marker = true;
	}

	if (seqNum == 0)
	{
		chData->lastSeqNumHi++;
	}
	chData->lastSeqNumLo = seqNum;
//	Text::StrConcat(Text::StrInt32(sbuff, seqNum), L"\r\n");
//	IO::Console::PrintStrO(sbuff);

	Sync::MutexUsage mutUsage(chData->packMut);
	if (chData->packCnt >= chData->buffCnt)
	{
		Bool lastExist = false;
		UOSInt i;
		UOSInt minIndex = 0;
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


		Net::IRTPPLHandler *plHdlr = chData->payloadMap->Get(chData->packBuff[minIndex].payloadType);
		if (plHdlr)
		{
			plHdlr->MediaDataReceived(chData->packBuff[minIndex].buff, chData->packBuff[minIndex].dataSize, chData->packBuff[minIndex].seqNum, chData->packBuff[minIndex].ts);
		}
		chData->packBuff[minIndex].payloadType = payloadType;
		chData->packBuff[minIndex].seqNum = seqNum;
		chData->packBuff[minIndex].dataSize  = dataSize - 12;
		chData->packBuff[minIndex].ts = timestamp;
		MemCopyNO(chData->packBuff[minIndex].buff, &buff[12], dataSize - 12);
	}
	else
	{
		chData->packBuff[chData->packCnt].payloadType = payloadType;
		chData->packBuff[chData->packCnt].seqNum = seqNum;
		chData->packBuff[chData->packCnt].dataSize  = dataSize - 12;
		chData->packBuff[chData->packCnt].ts = timestamp;
		MemCopyNO(chData->packBuff[chData->packCnt].buff, &buff[12], dataSize - 12);
		chData->packCnt++;
	}

	mutUsage.EndUse();
}

void __stdcall Net::RTPCliChannel::PacketCtrlHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	ChannelData *chData = (ChannelData*)userData;
	UInt8 tmpBuff[100];
	UTF8Char u8buff[64];
	UTF8Char *u8ptr;
	Data::DateTime *dt;
	Text::StringBuilderUTF8 *sb;
	UOSInt size = 0;
	UOSInt ofst = 0;
	UOSInt i;
	while (ofst < dataSize)
	{
		size = (UOSInt)(ReadMUInt16(&buff[ofst + 2]) + 1) << 2;
		if (size + ofst > dataSize)
		{
			break;
		}
		else if ((buff[ofst + 0] & 0xc0) == 0x80)
		{
			switch (buff[ofst + 1])
			{
			case 200: //SR - Sender Report
				NEW_CLASS(sb, Text::StringBuilderUTF8());
				sb->Append((const UTF8Char*)"SR: Len=");
				sb->AppendUOSInt(size);
				sb->Append((const UTF8Char*)",RC=");
				sb->AppendU32(buff[ofst +0] & 0x1f);
				sb->Append((const UTF8Char*)",SSRC=");
				sb->AppendI32(ReadMInt32(&buff[ofst + 4]));
				if (size >= 28)
				{
					NEW_CLASS(dt, Data::DateTime());
					sb->Append((const UTF8Char*)",NTP ts=");
					dt->SetNTPTime(ReadMInt32(&buff[ofst + 8]), ReadMInt32(&buff[ofst + 12]));
					sb->AppendDate(dt);
					sb->Append((const UTF8Char*)",RTP ts=");
					sb->AppendI32(ReadMInt32(&buff[ofst + 16]));
					sb->Append((const UTF8Char*)",nPacket=");
					sb->AppendI32(ReadMInt32(&buff[ofst + 20]));
					sb->Append((const UTF8Char*)",nOctet=");
					sb->AppendI32(ReadMInt32(&buff[ofst + 24]));
					DEL_CLASS(dt);
				}
				sb->Append((const UTF8Char*)"\r\n");
				printf("%s", sb->ToString());
				DEL_CLASS(sb);

				tmpBuff[0] = 0x81;
				tmpBuff[1] = 201;
				tmpBuff[2] = 0;
				tmpBuff[3] = 7;
				WriteMInt32(&tmpBuff[4], ReadMInt32(&buff[ofst + 4]));
				WriteMInt32(&tmpBuff[8], ReadMInt32(&buff[ofst + 4]));
				tmpBuff[12] = 0;
				tmpBuff[13] = 0xff;
				tmpBuff[14] = 0xff;
				tmpBuff[15] = 0xff;
				WriteMUInt32(&tmpBuff[16], (chData->lastSeqNumHi << 16) | chData->lastSeqNumLo);
				WriteMInt32(&tmpBuff[20], 0);
				WriteMInt32(&tmpBuff[24], ReadMInt32(&buff[ofst + 10]));
				WriteMInt32(&tmpBuff[28], 0);

				tmpBuff[32] = 0x81;
				tmpBuff[33] = 202;
				WriteMInt32(&tmpBuff[36], ReadMInt32(&buff[ofst + 4]));
				{
					i = 40;
					tmpBuff[i] = 1;
					u8ptr = Net::SocketUtil::GetAddrName(u8buff, addr);
					Text::StrConcatC(&tmpBuff[i + 2], u8buff, (UOSInt)(u8ptr - u8buff));
					tmpBuff[i + 1] = (UInt8)(u8ptr - u8buff);
					i += (UOSInt)(tmpBuff[i + 1] + 2);
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
				NEW_CLASS(sb, Text::StringBuilderUTF8());
				sb->Append((const UTF8Char*)"SDES: Len=");
				sb->AppendUOSInt(size);
				sb->Append((const UTF8Char*)",RC=");
				sb->AppendU32(buff[ofst + 0] & 0x1f);
				sb->Append((const UTF8Char*)",SSRC=");
				sb->AppendI32(ReadMInt32(&buff[ofst + 4]));
				{
					i = 8;
					while (i < size)
					{
						switch (buff[ofst + i])
						{
						case 0:
						default:
							i = size;
							break;
						case 1:
							sb->Append((const UTF8Char*)", CNAME=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 2:
							sb->Append((const UTF8Char*)", NAME=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 3:
							sb->Append((const UTF8Char*)", EMAIL=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 4:
							sb->Append((const UTF8Char*)", PHONE=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 5:
							sb->Append((const UTF8Char*)", LOC=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 6:
							sb->Append((const UTF8Char*)", TOOL=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 7:
							sb->Append((const UTF8Char*)", NOTE=");
							sb->AppendC((const UTF8Char*)&buff[ofst + i + 2], buff[ofst + i + 1]);
							i += (UOSInt)(buff[ofst + i + 1] + 2);
							break;
						case 8:
							sb->Append((const UTF8Char*)", PRIV=");
							i = size;
							break;
						}
					}
				}
				sb->Append((const UTF8Char*)"\r\n");
				printf("%s", sb->ToString());
				DEL_CLASS(sb);
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

UInt32 __stdcall Net::RTPCliChannel::PlayThread(void *userObj)
{
	Net::RTPCliChannel *me = (Net::RTPCliChannel*)userObj;
	Data::DateTime *dt;
	Data::DateTime *lastDt;
	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(lastDt, Data::DateTime());

	me->chData->playing = true;
	if (me->chData->playCtrl->Init(me))
	{
		if (me->chData->playCtrl->Play(me))
		{
			lastDt->SetCurrTimeUTC();
			while (!me->chData->playToStop)
			{
				dt->SetCurrTimeUTC();
				if (dt->DiffMS(lastDt) > 5000)
				{
					lastDt->SetCurrTimeUTC();
					me->chData->playCtrl->KeepAlive(me);
				}
				me->chData->playEvt->Wait(5000);
			}
			me->chData->playCtrl->StopPlay(me);
		}
		me->chData->playCtrl->Deinit(me);
	}
	DEL_CLASS(dt);
	DEL_CLASS(lastDt);
	me->chData->playing = false;
	return 0;
}

void Net::RTPCliChannel::SetControlURL(const UTF8Char *url)
{
	SDEL_TEXT(this->chData->controlURL);
	this->chData->controlURL = Text::StrCopyNew(url);
}

void Net::RTPCliChannel::SetPlayControl(Net::IRTPController *playCtrl)
{
	this->chData->playCtrl = playCtrl;
}

Net::RTPCliChannel::RTPCliChannel(Net::SocketFactory *sockf, UInt16 port)
{
	this->chData = MemAlloc(ChannelData, 1);
	this->chData->useCnt = 1;

	UOSInt i;
	this->chData->sockf = sockf;
	this->chData->rtpUDP = 0;
	this->chData->rtcpUDP = 0;
	this->chData->userData = 0;
	this->chData->controlURL = 0;
	this->chData->playing = false;
	this->chData->lastSeqNumHi = 0;
	this->chData->lastSeqNumLo = 0;
	NEW_CLASS(this->chData->playEvt, Sync::Event(true, (const UTF8Char*)"Net.RTPCliChannel.playEvt"));
	this->chData->mediaType = Media::MEDIA_TYPE_UNKNOWN;
	this->chData->threadCnt = 5;
	this->chData->buffCnt = 32;
	NEW_CLASS(this->chData->packMut, Sync::Mutex());
	this->chData->packCnt = 0;
	this->chData->packBuff = MemAlloc(PacketBuff, this->chData->buffCnt);
	i = this->chData->buffCnt;
	while (i-- > 0)
	{
		this->chData->packBuff[i].buff = MemAlloc(UInt8, 2048);
	}

	NEW_CLASS(this->chData->payloadMap, Data::Int32Map<Net::IRTPPLHandler*>());

	if (port & 1)
	{
		port = (UInt16)(port + 1);
	}
	NEW_CLASS(this->chData->rtpUDP, Net::UDPServer(sockf, 0, port, 0, PacketHdlr, this->chData, 0, 0, this->chData->threadCnt, false));
	if (port == 0)
	{
		port = this->chData->rtpUDP->GetPort();
		if (port & 1)
		{
			port = (UInt16)(port + 1);
			DEL_CLASS(this->chData->rtpUDP);
			NEW_CLASS(this->chData->rtpUDP, Net::UDPServer(sockf, 0, port, 0, PacketHdlr, this->chData, 0, 0, this->chData->threadCnt, false));
		}
	}
	this->chData->rtpUDP->SetBuffSize(1048576);
	NEW_CLASS(this->chData->rtcpUDP, Net::UDPServer(sockf, 0, (UInt16)(port + 1), 0, PacketCtrlHdlr, this->chData, 0, 0, 1, false));
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
		UOSInt i;
		Data::ArrayList<Net::IRTPPLHandler*> *plHdlrs;
		Net::IRTPPLHandler *plHdlr;
		this->StopPlay();
		DEL_CLASS(this->chData->playEvt);

		DEL_CLASS(this->chData->rtpUDP);
		SDEL_CLASS(this->chData->rtcpUDP);
		SDEL_TEXT(this->chData->controlURL);

		plHdlrs = this->chData->payloadMap->GetValues();
		i = plHdlrs->GetCount();
		while (i-- > 0)
		{
			plHdlr = plHdlrs->GetItem(i);
			DEL_CLASS(plHdlr);
		}
		DEL_CLASS(this->chData->payloadMap);
		DEL_CLASS(this->chData->packMut);
		i = this->chData->buffCnt;
		while (i-- > 0)
		{
			MemFree(this->chData->packBuff[i].buff);
		}
		MemFree(this->chData->packBuff);

		DEL_CLASS(this->chData->playCtrl);
		MemFree(this->chData);
	}
}

UInt16 Net::RTPCliChannel::GetPort()
{
	return this->chData->rtpUDP->GetPort();
}

UTF8Char *Net::RTPCliChannel::GetTransportDesc(UTF8Char *sbuff)
{
	UInt16 port = GetPort();
	sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"RTP/AVP;unicast;client_port=");
	sbuff = Text::StrInt32(sbuff, port);
	sbuff = Text::StrConcat(sbuff, (const UTF8Char*)"-");
	sbuff = Text::StrInt32(sbuff, port + 1);
	return sbuff;
}

const UTF8Char *Net::RTPCliChannel::GetControlURL()
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

Media::IVideoSource *Net::RTPCliChannel::GetVideo(UOSInt index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_VIDEO)
		return 0;
	return (Net::RTPVPLHandler*)this->chData->payloadMap->GetValues()->GetItem(index);
}

Media::IAudioSource *Net::RTPCliChannel::GetAudio(UOSInt index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_AUDIO)
		return 0;
	return 0;//(Net::RTPVPLHandler*)this->payloadMap->GetValues()->GetItem(index);
}

Media::IVideoSource *Net::RTPCliChannel::CreateShadowVideo(UOSInt index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_VIDEO)
		return 0;
	Net::RTPVPLHandler *hdlr = (Net::RTPVPLHandler*)this->chData->payloadMap->GetValues()->GetItem(index);
	if (hdlr == 0)
	{
		return 0;
	}
	Net::RTPVSource *vSrc;
	Net::RTPCliChannel *ch;
	NEW_CLASS(ch, Net::RTPCliChannel(this));
	NEW_CLASS(vSrc, Net::RTPVSource(ch, hdlr));
	return vSrc;
}

Media::IAudioSource *Net::RTPCliChannel::CreateShadowAudio(UOSInt index)
{
	if (this->chData->mediaType != Media::MEDIA_TYPE_AUDIO)
		return 0;
	Net::RTPAPLHandler *hdlr = (Net::RTPAPLHandler*)this->chData->payloadMap->GetValues()->GetItem(index);
	if (hdlr == 0)
	{
		return 0;
	}
	Net::RTPASource *aSrc;
	Net::RTPCliChannel *ch;
	NEW_CLASS(ch, Net::RTPCliChannel(this));
	NEW_CLASS(aSrc, Net::RTPASource(ch, hdlr));
	return aSrc;
}

void *Net::RTPCliChannel::GetUserData()
{
	return this->chData->userData;
}

void Net::RTPCliChannel::SetUserData(void *userData)
{
	this->chData->userData = userData;
}

Bool Net::RTPCliChannel::StartPlay()
{
	if (this->chData->playing)
		return false;
	this->chData->playToStop = false;
	Sync::Thread::Create(PlayThread, this);
	while (!this->chData->playing)
	{
		Sync::Thread::Sleep(10);
	}
	return true;
}

Bool Net::RTPCliChannel::StopPlay()
{
	if (this->chData->playing)
	{
		this->chData->playToStop = true;
		this->chData->playEvt->Set();
		while (this->chData->playing)
		{
			Sync::Thread::Sleep(10);
		}
	}
	return true;
}

Bool Net::RTPCliChannel::IsRunning()
{
	return this->chData->playing;
}

Bool Net::RTPCliChannel::MapPayloadType(Int32 payloadType, const UTF8Char *typ, UInt32 freq, UInt32 nChannel)
{
	Net::IRTPPLHandler *hdlr;
	if (this->chData->payloadMap->Get(payloadType))
	{
		return false;
	}
	if (this->chData->mediaType == Media::MEDIA_TYPE_AUDIO)
	{
		if (Text::StrCompare(typ, (const UTF8Char*)"PCMU") == 0)
		{
			//////////////////////////
			return false;
		}
		else if (Text::StrCompare(typ, (const UTF8Char*)"telephone-event") == 0)
		{
			//////////////////////////
			return false;
		}
		else if (Text::StrCompare(typ, (const UTF8Char*)"mpeg4-generic") == 0)
		{
			NEW_CLASS(hdlr, Net::RTPAACHandler(payloadType, freq, nChannel));
			this->chData->payloadMap->Put(payloadType, hdlr);
			return true;
		}
	}
	else if (this->chData->mediaType == Media::MEDIA_TYPE_VIDEO)
	{
		if (Text::StrCompare(typ, (const UTF8Char*)"H264") == 0)
		{
			NEW_CLASS(hdlr, Net::RTPH264Handler(payloadType));
			this->chData->payloadMap->Put(payloadType, hdlr);
			return true;
		}
	}
	return false;
}

Bool Net::RTPCliChannel::SetPayloadFormat(Int32 payloadType, const UTF8Char *format)
{
	Net::IRTPPLHandler *plHdlr = this->chData->payloadMap->Get(payloadType);
	if (plHdlr)
	{
		plHdlr->SetFormat(format);
		return true;
	}
	return false;
}

Net::RTPCliChannel *Net::RTPCliChannel::CreateChannel(Net::SocketFactory *sockf, Data::ArrayList<const UTF8Char *> *sdpDesc, const UTF8Char *ctrlURL, Net::IRTPController *playCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sarr[5];
	Net::RTPCliChannel *ch;
	const UTF8Char *desc;
	NEW_CLASS(ch, Net::RTPCliChannel(sockf, 0));
	ch->SetPlayControl(playCtrl->Clone());
	Bool ctrlFound = false;
	Int32 payloadType;
	UOSInt k;

	UOSInt i = 0;
	UOSInt j = sdpDesc->GetCount();
	while (i < j)
	{
		desc = sdpDesc->GetItem(i);
		if (Text::StrStartsWith(desc, (const UTF8Char*)"m="))
		{
			if (Text::StrStartsWith(desc, (const UTF8Char*)"m=video "))
			{
				ch->SetMediaType(Media::MEDIA_TYPE_VIDEO);
			}
			else if (Text::StrStartsWith(desc, (const UTF8Char*)"m=audio "))
			{
				ch->SetMediaType(Media::MEDIA_TYPE_AUDIO);
			}
		}
		else if (Text::StrStartsWith(desc, (const UTF8Char*)"a=control:"))
		{
			ctrlFound = true;
			if (Text::StrIndexOf(&desc[10], (const UTF8Char*)"://") != INVALID_INDEX)
			{
				ch->SetControlURL(&desc[10]);
			}
			else
			{
				Text::StrConcat(Text::StrConcat(sbuff, ctrlURL), (const UTF8Char*)"/");
				Text::URLString::AppendURLPath(sbuff, &desc[10]);
				ch->SetControlURL(sbuff);
			}
		}
		else if (Text::StrStartsWith(desc, (const UTF8Char*)"a=rtpmap:"))
		{
			Text::StrConcat(sbuff, &desc[9]);
			if (Text::StrSplit(sarr, 3, sbuff, ' ') == 2)
			{
				payloadType = Text::StrToInt32(sarr[0]);
				k = Text::StrSplit(sarr, 4, sarr[1], '/');
				if (k == 2 && ch->GetMediaType() == Media::MEDIA_TYPE_VIDEO)
				{
					ch->MapPayloadType(payloadType, sarr[0], Text::StrToUInt32(sarr[1]), 0);
				}
				else if (k == 3 && ch->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					ch->MapPayloadType(payloadType, sarr[0], Text::StrToUInt32(sarr[1]), Text::StrToUInt32(sarr[2]));
				}
				else if (k == 2 && ch->GetMediaType() == Media::MEDIA_TYPE_AUDIO)
				{
					ch->MapPayloadType(payloadType, sarr[0], Text::StrToUInt32(sarr[1]), 1);
				}
			}
		}
		else if (Text::StrStartsWith(desc, (const UTF8Char*)"a=fmtp:"))
		{
			Text::StrConcat(sbuff, &desc[7]);
			if (Text::StrSplit(sarr, 2, sbuff, ' ') == 2)
			{
				payloadType = Text::StrToInt32(sarr[0]);
				ch->SetPayloadFormat(payloadType, sarr[1]);
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
