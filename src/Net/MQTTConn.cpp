#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "Manage/HiResClock.h"
#include "Net/MQTTConn.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

//#define DEBUG_PRINT
#if defined(DEBUG_PRINT)
#ifdef _MSC_VER
#include <windows.h>
#include <stdio.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); OutputDebugStringA(sbuff);}
#elif defined(DEBUGCON)
#include <stdio.h>
#include <syslog.h>
#define printf(fmt, ...) {Char sbuff[512]; sprintf(sbuff, fmt, __VA_ARGS__); syslog(LOG_DEBUG, sbuff);}
#else
#include <stdio.h>
#endif
#endif

void Net::MQTTConn::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
{
#if defined(DEBUG_PRINT)
	printf("On MQTT packet: type = %x, size = %d\r\n", cmdType, (UInt32)cmdSize);
#endif
	UInt8 packetType = (UInt8)(cmdType >> 4);
	if (packetType == 3 && cmdSize >= 2)
	{
		UInt8 qosLev = (cmdType & 6) >> 1;
		UOSInt i;
		UOSInt packetId = ReadMUInt16(&cmd[0]);
		UnsafeArrayOpt<UTF8Char> topic = 0;
		UnsafeArray<UTF8Char> nntopic;
		UOSInt topicLen = 0;
		if ((UOSInt)(packetId + 2) <= cmdSize)
		{
			nntopic = MemAlloc(UTF8Char, packetId + 1);
			MemCopyNO(nntopic.Ptr(), &cmd[2], packetId);
			nntopic[packetId] = 0;
			topic = nntopic;
			topicLen = packetId;
		}
		i = packetId + 2;
		if (qosLev == 1 || qosLev == 2)
		{
			if (i + 2 <= cmdSize)
			{
				packetId = ReadMUInt16(&cmd[i]);
			}
			else
			{
				packetId = 0;
			}
			i += 2;
		}
		if (topic.SetTo(nntopic) && i <= cmdSize)
		{
#if defined(DEBUG_PRINT)
			printf("MQTT data: topic = %s, size = %d\r\n", nntopic.Ptr(), (UInt32)(cmdSize - i));
#endif
			this->OnPublishMessage({nntopic, topicLen}, &cmd[i], cmdSize - i);
		}
		if (qosLev == 1)
		{
			this->SendPubAck((UInt16)packetId);
		}
		else if (qosLev == 2)
		{
			this->SendPubRec((UInt16)packetId);
		}
		if (topic.SetTo(nntopic))
		{
			MemFreeArr(nntopic);
			topic = 0;
		}
	}
	else if (packetType == 2 || packetType == 4 || packetType == 9 || packetType == 11 || packetType == 13)
	{
		PacketInfo *packet;
		packet = (PacketInfo*)MemAlloc(UInt8, sizeof(PacketInfo) + cmdSize);
		packet->packetType = (UInt8)cmdType;
		packet->size = cmdSize;
		MemCopyNO(packet->content, cmd.Ptr(), cmdSize);
		{
			Sync::MutexUsage mutUsage(this->packetMut);
			this->packetList.Add(NN<PacketInfo>::FromPtr(packet));
		}
		this->packetEvt.Set();
	}
}

void Net::MQTTConn::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
}

UInt32 __stdcall Net::MQTTConn::RecvThread(AnyType userObj)
{
	NN<Net::MQTTConn> me = userObj.GetNN<Net::MQTTConn>();
	Sync::ThreadUtil::SetName(CSTR("MQTTConnRecv"));
	UOSInt maxBuffSize = 9000;
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	NN<IO::Stream> stm;
	if (!me->stm.SetTo(stm))
	{
		return 0;
	}
	me->recvStarted = true;
	me->recvRunning = true;
	buffSize = 0;
	buff = MemAlloc(UInt8, maxBuffSize);
	while (true)
	{
		readSize = stm->Read(Data::ByteArray(&buff[buffSize], maxBuffSize - buffSize));
		if (readSize <= 0)
			break;
#ifdef DEBUG_PRINT
		printf("MQTTConn: Received %d bytes\r\n", (UInt32)readSize);
#endif
		me->totalDownload += readSize;
		buffSize += readSize;
		readSize = me->protoHdlr.ParseProtocol(stm, me, me->cliData, Data::ByteArrayR(buff, buffSize));
		if (readSize == 0)
		{
			buffSize = 0;
		}
		else if (readSize < buffSize)
		{
			MemCopyO(buff, &buff[buffSize - readSize], readSize);
			buffSize = readSize;
		}
	}
#ifdef DEBUG_PRINT
	printf("MQTTConn: Disconnected\r\n");
#endif
	MemFree(buff);
	me->recvRunning = false;
	me->packetEvt.Set();
	if (me->discHdlr.func)
	{
		me->discHdlr.func(me->discHdlr.userObj);
	}
	return 0;
}

void Net::MQTTConn::OnPublishMessage(Text::CStringNN topic, UnsafeArray<const UInt8> message, UOSInt msgSize)
{
	UOSInt i = this->hdlrList.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<PublishMessageHdlr> cb = this->hdlrList.GetItem(i);
		cb.func(cb.userObj, topic, Data::ByteArrayR(message, msgSize));
	}
}

Optional<Net::MQTTConn::PacketInfo> Net::MQTTConn::GetNextPacket(UInt8 packetType, Data::Duration timeout)
{
	Manage::HiResClock clk;
	NN<PacketInfo> packet;
	Data::Duration t;
	while (true)
	{
		while (this->packetList.GetCount() > 0)
		{
			Sync::MutexUsage mutUsage(this->packetMut);
			if (!this->packetList.RemoveAt(0).SetTo(packet))
				break;
			mutUsage.EndUse();
			if ((packet->packetType & 0xf0) == packetType)
			{
				return packet;
			}
			MemFreeNN(packet);
		}
		t = Data::Duration::FromUs(clk.GetTimeDiffus());
		if (!this->recvRunning || t >= timeout)
			return 0;
		this->packetEvt.Wait(timeout - t);
	}
}

Bool Net::MQTTConn::SendPacket(UnsafeArray<const UInt8> packet, UOSInt packetSize)
{
	NN<IO::Stream> stm;
	if (!this->stm.SetTo(stm))
	{
		return false;
	}
	Sync::MutexUsage mutUsage(this->cliMut);
	UOSInt sendSize = stm->Write(Data::ByteArrayR(packet, packetSize));
	this->totalUpload += sendSize;
	return sendSize == packetSize;
}

void Net::MQTTConn::InitStream(NN<IO::Stream> stm)
{
	this->stm = stm.Ptr();
	this->cliData = this->protoHdlr.CreateStreamData(stm);
	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->recvStarted)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

Net::MQTTConn::MQTTConn(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, DisconnectHdlr discHdlr, AnyType discHdlrObj, Data::Duration timeout) : protoHdlr(*this)
{
	this->recvRunning = false;
	this->recvStarted = false;
	this->totalDownload = 0;
	this->totalUpload = 0;
	this->discHdlr = {discHdlr, discHdlrObj};
	this->cliData = 0;
	this->stm = 0;

	NN<Net::TCPClient> cli;
	NN<Net::SSLEngine> nnssl;
	if (ssl.SetTo(nnssl))
	{
		Net::SSLEngine::ErrorType err;
		clif->GetSocketFactory()->ReloadDNS();
		if (!Optional<Net::TCPClient>(nnssl->ClientConnect(host, port, err, timeout)).SetTo(cli))
		{
			return;
		}
#ifdef DEBUG_PRINT
		printf("MQTTConn: Connect to MQTTS: err = %d\r\n", (UInt32)err);
#endif
	}
	else
	{
		cli = clif->Create(host, port, timeout);
	}
	if (cli->IsConnectError() != 0)
	{
		cli.Delete();
#ifdef DEBUG_PRINT
		printf("MQTTConn connect error, %d\r\n", 0);
#endif
	}
	else
	{
		cli->SetNoDelay(true);
		this->InitStream(cli);
	}
}

Net::MQTTConn::MQTTConn(NN<IO::Stream> stm, DisconnectHdlr discHdlr, AnyType discHdlrObj) : protoHdlr(*this)
{
	this->recvRunning = false;
	this->recvStarted = false;
	this->totalDownload = 0;
	this->totalUpload = 0;
	this->discHdlr = {discHdlr, discHdlrObj};
	this->cliData = 0;
	this->stm = 0;
	this->InitStream(stm);
}

Net::MQTTConn::~MQTTConn()
{
	NN<IO::Stream> stm;
	if (this->stm.SetTo(stm))
	{
		if (this->recvRunning)
		{
			stm->Close();
		}
		while (this->recvRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
		this->protoHdlr.DeleteStreamData(stm, this->cliData);
		stm.Delete();
		this->stm = 0;
	}
	UOSInt i;
	i = this->packetList.GetCount();
	while (i-- > 0)
	{
		MemFreeNN(this->packetList.GetItemNoCheck(i));
	}
}

void Net::MQTTConn::HandlePublishMessage(PublishMessageHdlr hdlr, AnyType userObj)
{
	this->hdlrList.Add({hdlr, userObj});
}

Bool Net::MQTTConn::IsError()
{
	return this->stm.IsNull() || !this->recvRunning;
}

Bool Net::MQTTConn::SendConnect(UInt8 protoVer, UInt16 keepAliveS, Text::CStringNN clientId, Text::CString userName, Text::CString password)
{
	UInt8 packet1[512];
	UInt8 packet2[512];

	UOSInt i;
	UOSInt j;
	packet1[0] = 0;
	packet1[1] = 4;
	packet1[2] = 'M';
	packet1[3] = 'Q';
	packet1[4] = 'T';
	packet1[5] = 'T';
	packet1[6] = protoVer;
	packet1[7] = 2; //Flags;
	WriteMInt16(&packet1[8], keepAliveS);
	i = 10;
	j = clientId.leng;
	WriteMInt16(&packet1[i], j);
	MemCopyNO(&packet1[i + 2], clientId.v.Ptr(), j);
	i += j + 2;
	if (userName.leng > 0)
	{
		packet1[7] |= 0x80;
		j = userName.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], userName.v.Ptr(), j);
		i += j + 2;
	}
	if (password.leng > 0)
	{
		packet1[7] |= 0x40;
		j = password.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], password.v.Ptr(), j);
		i += j + 2;
	}
	j = this->protoHdlr.BuildPacket(packet2, 0x10, 0, packet1, i, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPublish(Text::CStringNN topic, Text::CStringNN message, Bool dup, UInt8 qos, Bool retain)
{
	UOSInt i;
	UOSInt j;
	i = 0;

	UInt8 cmd = 0x30 | (UInt8)((qos & 3) << 1);
	if (dup) cmd = (UInt8)(cmd | 8);
	if (retain) cmd = (UInt8)(cmd | 1);
	if (topic.leng + message.leng > 507)
	{
		UInt8 *pack1 = MemAlloc(UInt8, topic.leng + message.leng + 2);
		UInt8 *pack2 = MemAlloc(UInt8, topic.leng + message.leng + 7);
		j = topic.leng;
		WriteMInt16(&pack1[i], j);
		MemCopyNO(&pack1[i + 2], topic.v.Ptr(), j);
		i += j + 2;
		j = message.leng;
		MemCopyNO(&pack1[i], message.v.Ptr(), j);
		i += j;

		j = this->protoHdlr.BuildPacket(pack2, 0x30, 0, pack1, i, this->cliData);
		i = this->SendPacket(pack2, j);
		MemFree(pack1);
		MemFree(pack2);
		return i;
	}
	else
	{
		UInt8 packet1[512];
		UInt8 packet2[512];

		j = topic.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], topic.v.Ptr(), j);
		i += j + 2;
		j = message.leng;
		MemCopyNO(&packet1[i], message.v.Ptr(), j);
		i += j;

		j = this->protoHdlr.BuildPacket(packet2, 0x30, 0, packet1, i, this->cliData);
		return this->SendPacket(packet2, j);
	}
}

Bool Net::MQTTConn::SendPubAck(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr.BuildPacket(packet2, 0x40, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPubRec(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr.BuildPacket(packet2, 0x50, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPubRel(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr.BuildPacket(packet2, 0x62, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendPubComp(UInt16 packetId)
{
	UInt8 packet1[16];
	UInt8 packet2[16];
	UOSInt j;

	WriteMInt16(&packet1[0], packetId);
	j = this->protoHdlr.BuildPacket(packet2, 0x70, 0, packet1, 2, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendSubscribe(UInt16 packetId, Text::CStringNN topic)
{

	UOSInt i;
	UOSInt j;

	if (topic.leng > 504)
	{
		UInt8 *packet1 = MemAlloc(UInt8, topic.leng + 5);
		UInt8 *packet2 = MemAlloc(UInt8, topic.leng + 10);
		WriteMInt16(&packet1[0], packetId);
		i = 2;
		j = topic.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], topic.v.Ptr(), j);
		i += j + 2;
		packet1[i] = 0;
		i++;
#if defined(DEBUG_PRINT)
		printf("Subscribing topic %s\r\n", topic.v.Ptr());
#endif
		j = this->protoHdlr.BuildPacket(packet2, 0x82, 0, packet1, i, this->cliData);
		i = this->SendPacket(packet2, j);
		MemFree(packet1);
		MemFree(packet2);
		return i;
	}
	else
	{
		UInt8 packet1[512];
		UInt8 packet2[512];
		WriteMInt16(&packet1[0], packetId);
		i = 2;
		j = topic.leng;
		WriteMInt16(&packet1[i], j);
		MemCopyNO(&packet1[i + 2], topic.v.Ptr(), j);
		i += j + 2;
		packet1[i] = 0;
		i++;
#if defined(DEBUG_PRINT)
		printf("Subscribing topic %s\r\n", topic.v.Ptr());
#endif
		j = this->protoHdlr.BuildPacket(packet2, 0x82, 0, packet1, i, this->cliData);
		return this->SendPacket(packet2, j);
	}
}

Bool Net::MQTTConn::SendPing()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr.BuildPacket(packet2, 0xc0, 0, packet2, 0, this->cliData);
	return this->SendPacket(packet2, j);
}

Bool Net::MQTTConn::SendDisconnect()
{
	UInt8 packet2[16];
	UOSInt j;
	j = this->protoHdlr.BuildPacket(packet2, 0xe0, 0, packet2, 0, this->cliData);
	return this->SendPacket(packet2, j);
}

Net::MQTTConn::ConnectStatus Net::MQTTConn::WaitConnAck(Data::Duration timeout)
{
	NN<PacketInfo> packet;
	if (!this->GetNextPacket(0x20, timeout).SetTo(packet))
		return Net::MQTTConn::CS_TIMEDOUT;

	Net::MQTTConn::ConnectStatus ret = (Net::MQTTConn::ConnectStatus)packet->content[1];
	MemFreeNN(packet);
	return ret;
}

UInt8 Net::MQTTConn::WaitSubAck(UInt16 packetId, Data::Duration timeout)
{
	NN<PacketInfo> packet;
	if (!this->GetNextPacket(0x90, timeout).SetTo(packet))
		return 0x80;

	UInt8 ret;
	if (packet->size < 3 || packetId != ReadMUInt16(&packet->content[0]))
	{
		ret = 0x80;
	}
	else
	{
		ret = packet->content[2];
	}
	MemFreeNN(packet);
	return ret;
}

void Net::MQTTConn::ClearPackets()
{
	Sync::MutexUsage mutUsage(this->packetMut);
	this->packetList.MemFreeAll();
}

UInt64 Net::MQTTConn::GetTotalUpload()
{
	return this->totalUpload;
}

UInt64 Net::MQTTConn::GetTotalDownload()
{
	return this->totalDownload;
}

Bool Net::MQTTConn::PublishMessage(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, Text::CString username, Text::CString password, Text::CStringNN topic, Text::CStringNN message, Data::Duration timeout)
{
	Net::MQTTConn *cli;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	NEW_CLASS(cli, Net::MQTTConn(clif, ssl, host, port, 0, 0, timeout));
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return false;
	}

	Bool succ = false;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(Text::StrConcatC(sbuff, UTF8STRC("sswrMQTT/")), dt.ToTicks());
	if (cli->SendConnect(4, 30, CSTRP(sbuff, sptr), username, password))
	{
		succ = (cli->WaitConnAck(timeout) == Net::MQTTConn::CS_ACCEPTED);
	}
	if (succ)
	{
		succ = cli->SendPublish(topic, message, false, 0, false);
		cli->SendDisconnect();
	}
	
	DEL_CLASS(cli);
	return succ;
}
