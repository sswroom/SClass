#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/RTPPayloadHandler.h"
#include "Net/RTPSvrChannel.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/URLString.h"

void __stdcall Net::RTPSvrChannel::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::RTPSvrChannel> me = userData.GetNN<Net::RTPSvrChannel>();

	if (data.GetSize() < 12)
	{
		return;
	}
	Int32 v = data[0] >> 6;
	if (v != 2)
		return;
	if (data[0] & 0x20) //padding
	{
		data = data.WithSize(data.GetSize() - data[data.GetSize() - 1]);
		if (data.GetSize() < 12)
			return;
	}
/*	Bool extension = (data[0] & 0x10) != 0;
	Int32 csrcCnt = data[0] & 15;
	Bool marker = (data[1] & 0x80) != 0;
	Int32 payloadType = data[1] & 0x7f;
	Int32 seqNum = ReadMUInt16(&data[2]);
	UInt32 timestamp = ReadMUInt32(&data[4]);*/
	me->lastSSRC = ReadMInt32(&data[8]);

/*	me->packMut->Lock();
	if (me->packCnt >= me->threadCnt)
	{
		Bool lastExist = false;
		OSInt i;
		OSInt minIndex = 0;
		Int32 minSeq = me->packBuff[0].seqNum;
		if (minSeq == 65535)
			lastExist = true;
		i = me->threadCnt;
		while (i-- > 0)
		{
			if (me->packBuff[i].seqNum == 65535)
				lastExist = true;
			if (me->packBuff[i].seqNum < minSeq)
			{
				minSeq = me->packBuff[i].seqNum;
				minIndex = i;
			}
		}
		if (lastExist)
		{
			minSeq = 65536;
			i = me->threadCnt;
			while (i-- > 0)
			{
				if (me->packBuff[i].seqNum > 32767 && me->packBuff[i].seqNum < minSeq)
				{
					minSeq = me->packBuff[i].seqNum;
					minIndex = i;
				}
			}
		}


		Net::RTPPayloadHandler *plHdlr = me->payloadMap->Get(me->packBuff[minIndex].payloadType);
		if (plHdlr)
		{
			plHdlr->MediaDataReceived(me->packBuff[minIndex].buff, me->packBuff[minIndex].dataSize, me->packBuff[minIndex].seqNum, me->packBuff[minIndex].ts);
		}
		me->packBuff[minIndex].payloadType = payloadType;
		me->packBuff[minIndex].seqNum = seqNum;
		me->packBuff[minIndex].dataSize  = dataSize - 12;
		me->packBuff[minIndex].ts = timestamp;
		MemCopy(me->packBuff[minIndex].buff, &buff[12], dataSize - 12);
	}
	else
	{
		me->packBuff[me->packCnt].payloadType = payloadType;
		me->packBuff[me->packCnt].seqNum = seqNum;
		me->packBuff[me->packCnt].dataSize  = dataSize - 12;
		me->packBuff[me->packCnt].ts = timestamp;
		MemCopy(me->packBuff[me->packCnt].buff, &buff[12], dataSize - 12);
		me->packCnt++;
	}

	me->packMut->Unlock();*/
}

void __stdcall Net::RTPSvrChannel::PacketCtrlHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::RTPSvrChannel> me = userData.GetNN<Net::RTPSvrChannel>();
	UInt32 size = 0;
	UInt32 ofst = 0;
	while (ofst < data.GetSize())
	{
		size = (UInt32)(ReadMUInt16(&data[ofst + 2]) + 1) << 2;
		if (size + ofst > data.GetSize())
		{
			break;
		}
		else if ((data[ofst + 0] & 0xc0) == 0x80)
		{
			switch (data[ofst + 1])
			{
			case 200: //SR - Sender Report
/*				
				{
					Text::StringBuilder sb;
					sb.Append(L"SR: Len=");
					sb.Append(size);
					sb.Append(L",RC=");
					sb.Append(data[ofst + 0] & 0x1f);
					sb.Append(L",SSRC=");
					sb.Append(ReadMInt32(&data[ofst + 4]));
					if (size >= 28)
					{
						NEW_CLASS(dt, Data::DateTime());
						sb.Append(L",NTP ts=");
						dt->SetNTPTime(ReadMInt32(&data[ofst + 8]), ReadMInt32(&data[ofst + 12]));
						sb.Append(dt);
						sb.Append(L",RTP ts=");
						sb.Append(ReadMInt32(&data[ofst + 16]));
						sb.Append(L",nPacket=");
						sb.Append(ReadMInt32(&data[ofst + 20]));
						sb.Append(L",nOctet=");
						sb.Append(ReadMInt32(&data[ofst + 24]));
						DEL_CLASS(dt);
					}
					sb->Append(L"\r\n");
					IO::Console::PrintStrO(sb->ToString());
				}*/

				break;
			case 201: //RR - Receiver Report
/*				{
					Text::StringBuilderUTF8 sb;
					sb.Append(L"RR: Len=");
					sb.Append(size);
					sb.Append(L",RC=");
					sb.Append(data[ofst + 0] & 0x1f);
					sb.Append(L",SSRC=");
					sb.Append(ReadMInt32(&data[ofst + 4]));
					sb.Append(L"\r\n");
					IO::Console::PrintStrO(sb.ToString());
				}*/
				me->sessCtrl->SessionKA(me->ssrc);
				break;
			case 202: //SDES - Source Description RTCP Packet
//				IO::Console::PrintStrO(L"SDES\r\n");
				break;
			case 203: //BYE
				break;
			default:
				break;
			}
		}
		ofst += size;
	}
}

Net::RTPSvrChannel::RTPSvrChannel(NN<Net::SocketFactory> sockf, UInt16 port, Int32 ssrc, NN<const Net::SocketUtil::AddressInfo> targetAddr, UInt16 targetPort, Net::RTPSessionController *sessCtrl)
{
	this->rtpUDP = 0;
	this->rtcpUDP = 0;
	this->threadCnt = 5;
	this->ssrc = ssrc;
	this->targetAddr = targetAddr.Ptr()[0];
	this->targetPort = targetPort;
	this->sessCtrl = sessCtrl;
	this->seqNum = 12442;

	if (port & 1)
	{
		port += 1;
	}
	NEW_CLASS(this->rtpUDP, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, this->log, CSTR_NULL, this->threadCnt, false));
	if (port == 0)
	{
		port = this->rtpUDP->GetPort();
		if (port & 1)
		{
			port += 1;
			DEL_CLASS(this->rtpUDP);
			NEW_CLASS(this->rtpUDP, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, this->log, CSTR_NULL, this->threadCnt, false));
		}
	}
	NEW_CLASS(this->rtcpUDP, Net::UDPServer(sockf, 0, port + 1, CSTR_NULL, PacketCtrlHdlr, this, this->log, CSTR_NULL, 1, false));
}

Net::RTPSvrChannel::~RTPSvrChannel()
{

	DEL_CLASS(this->rtpUDP);
	SDEL_CLASS(this->rtcpUDP);
}

NN<const Net::SocketUtil::AddressInfo> Net::RTPSvrChannel::GetTargetAddr()
{
	return this->targetAddr;
}

UInt16 Net::RTPSvrChannel::GetPort()
{
	return this->rtpUDP->GetPort();
}

Int32 Net::RTPSvrChannel::GetSeqNum()
{
	return seqNum;
}

Bool Net::RTPSvrChannel::SendPacket(Int32 payloadType, Int32 ts, UInt8 *buff, UOSInt dataSize, Bool marker)
{
	UOSInt sendSize = dataSize + 12;
	UInt8 sendBuff[1500];
	sendBuff[0] = 0x80;
	if (dataSize & 1)
	{
		sendBuff[0] |= 0x20;
		sendBuff[sendSize] = 0;
		sendSize++;
	}
	sendBuff[1] = payloadType & 0x7f;
	if (marker)
		sendBuff[1] |= 0x80;
	WriteMInt16(&sendBuff[2], this->seqNum);
	this->seqNum = (seqNum + 1) & 65535;
	WriteMInt32(&sendBuff[4], ts);
	WriteMInt32(&sendBuff[8], this->ssrc);
	MemCopyNO(&sendBuff[12], buff, dataSize);
	this->rtpUDP->SendTo(this->targetAddr, this->targetPort, sendBuff, sendSize);
	return true;
}

Bool Net::RTPSvrChannel::SendControl(UInt8 *buff, UOSInt dataSize)
{
	this->rtcpUDP->SendTo(this->targetAddr, this->targetPort + 1, buff, dataSize);
	return true;
}
