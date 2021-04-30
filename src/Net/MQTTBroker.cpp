#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MQTTBroker.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

typedef struct
{
	void *cliData;
	const UTF8Char *cliId;
	UOSInt buffSize;
	UInt8 recvBuff[4096];
	UInt16 keepAlive;
	Bool connected;
} ClientData;

void __stdcall Net::MQTTBroker::OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	Net::MQTTBroker *me = (Net::MQTTBroker*)userObj;
	ClientData *data = (ClientData*)cliData;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		if (me->log)
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Client ");
			sptr = cli->GetRemoteName(sptr);
			sptr = Text::StrConcat(sptr, (const UTF8Char*)" disconnect");
			me->log->LogMessage(sbuff, IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		me->protoHdlr->DeleteStreamData(cli, data->cliData);
		SDEL_TEXT(data->cliId);
		MemFree(data);

		UOSInt i;
		SubscribeInfo *subscribe;
		Sync::MutexUsage mutUsage(me->subscribeMut);
		i = me->subscribeList->GetCount();
		while (i-- > 0)
		{
			subscribe = me->subscribeList->GetItem(i);
			if (subscribe->cli == cli)
			{
				me->subscribeList->RemoveAt(i);
				Text::StrDelNew(subscribe->topic);
				MemFree(subscribe);
			}
		}
		mutUsage.EndUse();
		DEL_CLASS(cli);
	}
}

void __stdcall Net::MQTTBroker::OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::MQTTBroker *me = (Net::MQTTBroker*)userObj;
	ClientData *data = (ClientData*)cliData;
	Sync::Interlocked::Add(&me->infoTotalRecv, (OSInt)size);

	if (me->log)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Received ");
		sb.AppendUOSInt(size);
		sb.Append((const UTF8Char*)" bytes");
		me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
	}
	UOSInt i;
	if (data->buffSize > 0)
	{
		MemCopyNO(&data->recvBuff[data->buffSize], buff, size);
		data->buffSize += size;
		i = me->protoHdlr->ParseProtocol(cli, data, data->cliData, data->recvBuff, data->buffSize);
		if (i > 0)
		{
			MemCopyO(data->recvBuff, &data->recvBuff[data->buffSize - i], i);
			data->buffSize = i;
		}
		else
		{
			data->buffSize = 0;
		}
	}
	else
	{
		i = me->protoHdlr->ParseProtocol(cli, data, data->cliData, buff, size);
		if (i > 0)
		{
			MemCopyNO(data->recvBuff, &buff[size - i], i);
			data->buffSize = i;
		}
	}
}

void __stdcall Net::MQTTBroker::OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

void __stdcall Net::MQTTBroker::OnClientConn(UInt32 *s, void *userObj)
{
	Net::MQTTBroker *me = (Net::MQTTBroker*)userObj;
	Net::TCPClient *cli;
	ClientData *data;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	data = MemAlloc(ClientData, 1);
	data->buffSize = 0;
	data->cliData = me->protoHdlr->CreateStreamData(cli);
	data->keepAlive = 0;
	data->connected = false;
	data->cliId = 0;
	me->cliMgr->AddClient(cli, data);
	UOSInt cnt = me->cliMgr->GetClientCount();
	if (cnt > me->infoCliMax)
	{
		me->infoCliMax = cnt;
	}
}

UInt32 __stdcall Net::MQTTBroker::SysInfoThread(void *userObj)
{
	Net::MQTTBroker *me = (Net::MQTTBroker*)userObj;
	Data::DateTime *dt;
	UTF8Char sbuff[64];
	UOSInt i;
	NEW_CLASS(dt, Data::DateTime());

	dt->SetCurrTimeUTC();
	i = (UOSInt)(dt->ToString(Text::StrConcat(sbuff, (const UTF8Char*)"SMQTT "), "yyyyMMdd") - sbuff);
	me->UpdateTopic((const UTF8Char*)"$SYS/broker/version", sbuff, i, true);
	me->sysInfoRunning = true;
	while (!me->sysInfoToStop)
	{
		dt->SetCurrTimeUTC();
		i = (UOSInt)(Text::StrConcat(Text::StrInt64(sbuff, (dt->ToTicks() - me->infoStartTime) / 1000), (const UTF8Char*)" seconds") - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/uptime", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoTotalRecv) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/load/bytes/received", sbuff, i, true);

		i = (UOSInt)(Text::StrUInt64(sbuff, me->infoTotalSent) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/load/bytes/sent", sbuff, i, true);

		i = (UOSInt)(Text::StrUOSInt(sbuff, me->cliMgr->GetClientCount()) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/clients/connected", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoCliDisconn) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/clients/disconnected", sbuff, i, true);

		i = (UOSInt)(Text::StrUInt64(sbuff, me->infoCliMax) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/clients/maximum", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoMsgRecv) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/received", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoMsgSent) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/sent", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubDrop) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/publish/dropped", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubRecv) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/publish/received", sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubSent) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/publish/sent", sbuff, i, true);

		i = (UOSInt)(Text::StrUOSInt(sbuff, me->topicMap->GetCount()) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/messages/retained/count", sbuff, i, true);

		i = (UOSInt)(Text::StrUOSInt(sbuff, me->subscribeList->GetCount()) - sbuff);
		me->UpdateTopic((const UTF8Char*)"$SYS/broker/subscriptions/count", sbuff, i, true);

		me->sysInfoEvt->Wait(10000);
	}
	DEL_CLASS(dt);
	me->sysInfoRunning = false;
	return 0;
}

void Net::MQTTBroker::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	ClientData *data = (ClientData*)stmObj;
	UInt8 packet[256];
	UInt8 packet2[256];
	UOSInt i;
	UOSInt j;
	UOSInt sent;
	Sync::Interlocked::Increment(&this->infoMsgRecv);
	switch (cmdType >> 4)
	{
	case 1: //CONNECT
		{
			const UTF8Char *clientId = 0;
			const UTF8Char *userName = 0;
			const UTF8Char *password = 0;
			if (this->log)
			{
				UInt8 connFlags;
				Text::StringBuilderUTF8 sb;
				Text::StringBuilderUTF8 sb2;
				sb.Append((const UTF8Char*)"Packet Type = CONNECT, size = ");
				sb.AppendUOSInt(cmdSize);
				sb.Append((const UTF8Char*)" bytes, protoName = ");
				i = 0;
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
				{
					sb.Append((const UTF8Char*)", data = ");
					sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
					this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					break;
				}
				if (cmdSize - i < 4)
				{
					sb.Append((const UTF8Char*)", data = ");
					sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
					this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					break;
				}
				sb.Append((const UTF8Char*)", Level = ");
				sb.AppendU32(cmd[i]);
				sb.Append((const UTF8Char*)", Flags = 0x");
				sb.AppendHex8(cmd[i + 1]);
				sb.Append((const UTF8Char*)", Keep Alive = ");
				data->keepAlive = ReadMUInt16(&cmd[i + 2]);
				sb.AppendU16(data->keepAlive);
				connFlags = cmd[i + 1];
				i += 4;
				sb.Append((const UTF8Char*)", ClientId = ");
				sb2.ClearStr();
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb2))
				{
					sb.Append((const UTF8Char*)", data = ");
					sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
					this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					break;
				}
				clientId = Text::StrCopyNew(sb.ToString());
				sb.Append(sb2.ToString());
				if (connFlags & 4)
				{
					sb.Append((const UTF8Char*)", Will Topic = ");
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						SDEL_TEXT(clientId);
						break;
					}
					sb.Append((const UTF8Char*)", Will Message = ");
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						SDEL_TEXT(clientId);
						break;
					}
				}
				if (connFlags & 0x80)
				{
					sb.Append((const UTF8Char*)", User Name = ");
					sb2.ClearStr();
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb2))
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						SDEL_TEXT(clientId);
						break;
					}
					userName = Text::StrCopyNew(sb2.ToString());
					sb.Append(sb2.ToString());
				}
				if (connFlags & 0x40)
				{
					sb.Append((const UTF8Char*)", Password = ");
					if (cmdSize - i < 2)
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						SDEL_TEXT(clientId);
						SDEL_TEXT(userName);
						break;
					}
					UOSInt pwdSize = ReadMUInt16(&cmd[i]);
					if (cmdSize - i - 2 < pwdSize)
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						SDEL_TEXT(clientId);
						SDEL_TEXT(userName);
						break;
					}
					sb2.ClearStr();
					sb2.AppendC(&cmd[i + 2], pwdSize);
					password = Text::StrCopyNew(sb2.ToString());

					sb.AppendHex(&cmd[i + 2], pwdSize, ' ', Text::LBT_NONE);
					i += pwdSize + 2;
				}
				this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
			}
			else
			{
				UInt8 connFlags;
				Text::StringBuilderUTF8 sb;
				i = 0;
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
				{
					break;
				}
				if (cmdSize - i < 4)
				{
					break;
				}
				data->keepAlive = ReadMUInt16(&cmd[i + 2]);
				connFlags = cmd[i + 1];
				i += 4;
				sb.ClearStr();
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
				{
					break;
				}
				clientId = Text::StrCopyNew(sb.ToString());
				if (connFlags & 4)
				{
					sb.ClearStr();
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
					{
						SDEL_TEXT(clientId);
						break;
					}
					sb.ClearStr();
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
					{
						SDEL_TEXT(clientId);
						break;
					}
				}
				if (connFlags & 0x80)
				{
					sb.ClearStr();
					if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
					{
						SDEL_TEXT(clientId);
						break;
					}
					userName = Text::StrCopyNew(sb.ToString());
				}
				if (connFlags & 0x40)
				{
					if (cmdSize - i < 2)
					{
						SDEL_TEXT(clientId);
						SDEL_TEXT(userName);
						break;
					}
					UOSInt pwdSize = ReadMUInt16(&cmd[i]);
					if (cmdSize - i - 2 < pwdSize)
					{
						SDEL_TEXT(clientId);
						SDEL_TEXT(userName);
						break;
					}
					sb.ClearStr();
					sb.AppendC(&cmd[i], pwdSize);
					password = Text::StrCopyNew(sb.ToString());

					i += pwdSize + 2;
				}
			}
			ConnectStatus cs;
			if (this->connHdlr)
			{
				Net::SocketUtil::AddressInfo addr;
				((Net::TCPClient*)stm)->GetRemoteAddr(&addr);
				cs = this->connHdlr(this->connObj, clientId, userName, password, &addr);
			}
			else
			{
				cs = CS_ACCEPTED;
			}

			packet[0] = 0; //Session Present = 0;
			switch (cs)
			{
			case CS_ACCEPTED:
				packet[1] = 0;
				break;
			case CS_UNSUPP_MQTT_VER:
				packet[1] = 1;
				break;
			case CS_CLI_ID_NOT_VALID:
				packet[1] = 2;
				break;
			case CS_SERVER_UNAVAILABLE:
				packet[1] = 3;
				break;
			case CS_BAD_USER_NAME_OR_PWD:
				packet[1] = 4;
				break;
			case CS_NOT_AUTHORIZED:
				packet[1] = 5;
				break;
			default:
				packet[1] = 6;
				break;
			}
			if (cs == CS_ACCEPTED)
			{
				data->connected = true;
				SDEL_TEXT(data->cliId);
				data->cliId = Text::StrCopyNew(clientId);
			}

			i = this->protoHdlr->BuildPacket(packet2, 0x20, 0, packet, 2, data->cliData);
			sent = stm->Write(packet2, i);
			SDEL_TEXT(clientId);
			SDEL_TEXT(userName);
			SDEL_TEXT(password);
			Sync::Interlocked::Add(&this->infoTotalSent, sent);
		}
		break;
	case 2: //CONNACK
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = CONNACK, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)", data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 3: //PUBLISH
		{
			Text::StringBuilderUTF8 topicSb;
			UInt16 packetId = 0;
			const UInt8 *message;
			UOSInt messageSize;
			Sync::Interlocked::Increment(&this->infoPubRecv);
			if (this->log)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"Packet Type = PUBLISH, Flags = 0x");
				sb.AppendHex8(cmdType & 15);
				sb.Append((const UTF8Char*)", Topic = ");
				i = 0;
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &topicSb))
				{
					sb.Append((const UTF8Char*)", data = ");
					sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
					this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
					break;
				}
				sb.Append(topicSb.ToString());
				if (cmdType & 6)
				{
					if (cmdSize - i < 2)
					{
						sb.Append((const UTF8Char*)", data = ");
						sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
						this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
						break;
					}
					sb.Append((const UTF8Char*)", PacketId = ");					
					sb.AppendU16(packetId = ReadMUInt16(&cmd[i]));
					i += 2;
				}
				sb.Append((const UTF8Char*)", Message = ");
				message = &cmd[i];
				if (i < cmdSize)
				{
					messageSize = cmdSize - i;
					sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_NONE);
					sb.Append((const UTF8Char*)" (");
					sb.AppendC(&cmd[i], cmdSize - i);
					sb.Append((const UTF8Char*)")");
				}
				else
				{
					messageSize = 0;
				}
				this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
			}
			else
			{
				i = 0;
				if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &topicSb))
				{
					break;
				}
				if (cmdType & 6)
				{
					if (cmdSize - i < 2)
					{
						break;
					}
					packetId = ReadMUInt16(&cmd[i]);
					i += 2;
				}
				message = &cmd[i];
				if (i < cmdSize)
				{
					messageSize = cmdSize - i;
				}
				else
				{
					messageSize = 0;
				}
			}
			if (data->connected)
			{
				if (this->publishHdlr)
				{
					this->publishHdlr(this->publishObj, topicSb.ToString(), packetId, message, messageSize);
				}
				this->UpdateTopic(topicSb.ToString(), message, messageSize, false);
				if ((cmdType & 6) == 2)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0;
					packet[3] = 0;
					i = this->protoHdlr->BuildPacket(packet2, 0x40, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
				else if ((cmdType & 6) == 4)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0;
					packet[3] = 0;
					i = this->protoHdlr->BuildPacket(packet2, 0x50, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
			}
			else
			{
				if ((cmdType & 6) == 2)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0x87;
					packet[3] = 0;
					i = this->protoHdlr->BuildPacket(packet2, 0x40, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
				else if ((cmdType & 6) == 4)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0x87;
					packet[3] = 0;
					i = this->protoHdlr->BuildPacket(packet2, 0x50, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
			}
		}
		break;
	case 4: //PUBACK
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PUBACK, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 5: //PUBREC
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PUBREC, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 6: //PUBREL
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PUBREL, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 7: //PUBCOMP
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PUBCOMP, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 8: //SUBSCRIBE
		if (this->log)
		{
			UInt16 packetId = 0;
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = SUBSCRIBE, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)", Packet Id = ");
			if (cmdSize >= 2)
			{
				packetId = ReadMUInt16(&cmd[0]);
				sb.AppendU16(packetId);
			}
			i = 2;
			sb.Append((const UTF8Char*)", Topic = ");
			if (!this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sb))
			{
				sb.Append((const UTF8Char*)", data = ");
				sb.AppendHex(&cmd[i], cmdSize - i, ' ', Text::LBT_CRLF);
				this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
				break;
			}
			if (cmdSize > i)
			{
				sb.Append((const UTF8Char*)", Requested QoS = ");
				sb.AppendU16(cmd[i]);
			}
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		{
			UInt16 packetId = 0;
			Text::StringBuilderUTF8 sbTopic;
			i = 2;
			sbTopic.ClearStr();
			this->protoHdlr->ParseUTF8Str(cmd, &i, cmdSize, &sbTopic);
			if (cmdSize >= 2)
			{
				packetId = ReadMUInt16(&cmd[0]);
			}

			ConnectStatus cs = CS_NOT_AUTHORIZED;
			if (data->cliId == 0)
			{
				cs = CS_CLI_ID_NOT_VALID;
			}
			else if (!this->TopicValid(sbTopic.ToString()))
			{
				cs = CS_MALFORMED_PACKET;
			}
			else if (this->subscribeHdlr)
			{
				cs = this->subscribeHdlr(this->subscribeObj, data->cliId, sbTopic.ToString());
			}
			else
			{
				cs = CS_ACCEPTED;
			}
			
			WriteMInt16(&packet[0], packetId);
			if (cs == CS_ACCEPTED)
			{
				packet[2] = 0;
			}
			else
			{
				packet[2] = 0x80;
			}
			i = this->protoHdlr->BuildPacket(packet2, 0x90, 0, packet, 3, data->cliData);
			sent = stm->Write(packet2, i);
			Sync::Interlocked::Add(&this->infoTotalSent, sent);

			if (cs == CS_ACCEPTED)
			{
				SubscribeInfo *subscribe = MemAlloc(SubscribeInfo, 1);
				subscribe->topic = Text::StrCopyNew(sbTopic.ToString());
				subscribe->cli = (Net::TCPClient*)stm;
				subscribe->cliData = data;
				Sync::MutexUsage subscribeMutUsage(this->subscribeMut);
				this->subscribeList->Add(subscribe);
				subscribeMutUsage.EndUse();

				Data::ArrayList<TopicInfo*> *topicList;
				TopicInfo *topicInfo;
				Sync::MutexUsage topicMutUsage(this->topicMut);
				topicList = this->topicMap->GetValues();
				i = 0;
				j = topicList->GetCount();
				while (i < j)
				{
					topicInfo = topicList->GetItem(i);
					if (this->TopicMatch(topicInfo->topic, sbTopic.ToString()))
					{
						this->TopicSend(stm, data->cliData, topicInfo);
					}
					i++;
				}
				topicMutUsage.EndUse();
			}
		}
		break;
	case 9: //SUBACK
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = SUBACK, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 10: //UNSUBSCRIBE
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = UNSUBSCRIBE, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 11: //UNSUBACK
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = UNSUBACK, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 12: //PINGREQ
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PINGREQ, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		i = this->protoHdlr->BuildPacket(packet2, 0xD0, 0, packet, 0, data->cliData);
		sent = stm->Write(packet2, i);
		Sync::Interlocked::Add(&this->infoTotalSent, sent);
		break;
	case 13: //PINGRESP
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = PINGRESP, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	case 14: //DISCONNECT
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = DISCONNECT, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	default:
		if (this->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Packet Type = Reserved, size = ");
			sb.AppendUOSInt(cmdSize);
			sb.Append((const UTF8Char*)" bytes, data = ");
			sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_CRLF);
			this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
		break;
	}
}

void Net::MQTTBroker::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void Net::MQTTBroker::UpdateTopic(const UTF8Char *topic, const UInt8 *message, UOSInt msgSize, Bool suppressUnchg)
{
	TopicInfo *topicInfo;
	Bool unchanged = false;
	Sync::MutexUsage topicMutUsage(this->topicMut);
	topicInfo = this->topicMap->Get(topic);
	if (topicInfo == 0)
	{
		topicInfo = MemAlloc(TopicInfo, 1);
		topicInfo->topic = Text::StrCopyNew(topic);
		topicInfo->message = MemAlloc(UInt8, msgSize);
		MemCopyNO(topicInfo->message, message, msgSize);
		topicInfo->msgSize = msgSize;
		this->topicMap->Put(topic, topicInfo);
	}
	else
	{
		if (msgSize != topicInfo->msgSize)
		{
			MemFree(topicInfo->message);
			topicInfo->message = MemAlloc(UInt8, msgSize);
			topicInfo->msgSize = msgSize;
			MemCopyNO(topicInfo->message, message, msgSize);
		}
		else
		{
			UOSInt i = msgSize;
			unchanged = true;
			while (i-- > 0)
			{
				if (topicInfo->message[i] != message[i])
				{
					unchanged = false;
					break;
				}
			}
			if (unchanged)
			{
				if (suppressUnchg)
				{
					topicMutUsage.EndUse();
					return;
				}
			}
			else
			{
				MemCopyNO(topicInfo->message, message, msgSize);
			}		
		}
	}
	topicMutUsage.EndUse();
	
	if (this->topicUpdHdlr)
	{
		this->topicUpdHdlr(this->topicUpdObj, topic, message, msgSize);
	}

	SubscribeInfo *subscribe;
	UOSInt i;
	Sync::MutexUsage subscribeMutUsage(this->subscribeMut);
	i = this->subscribeList->GetCount();
	while (i-- > 0)
	{
		subscribe = this->subscribeList->GetItem(i);
		if (this->TopicMatch(topic, subscribe->topic))
		{
			topicMutUsage.BeginUse();
			this->TopicSend(subscribe->cli, ((ClientData*)subscribe->cliData)->cliData, topicInfo);
			topicMutUsage.EndUse();
		}
	}
	subscribeMutUsage.EndUse();
}

Bool Net::MQTTBroker::TopicValid(const UTF8Char *topic)
{
	OSInt i;
	OSInt j;
	i = Text::StrIndexOf(topic, '#');
	if (i >= 0)
	{
		if (topic[i + 1])
		{
			return false;
		}
		if (i > 0 && topic[i - 1] != '/')
		{
			return false;
		}
	}
	i = 0;
	while (true)
	{
		j = Text::StrIndexOf(&topic[i], '+');
		if (j < 0)
			break;
		if (i + j > 0 && topic[i + j - 1] != '/')
		{
			return false;
		}
		if (topic[i + j + 1] != '/' && topic[i + j + 1] != 0)
		{
			return false;
		}
		i += j + 2;
	}
	return true;
}

Bool Net::MQTTBroker::TopicMatch(const UTF8Char *topic, const UTF8Char *subscribeTopic)
{
	if (subscribeTopic[0] == '#' && subscribeTopic[1] == 0)
	{
		if (!Text::StrStartsWith(topic, (const UTF8Char*)"$SYS/"))
		{
			return true;
		}
		return false;
	}
	OSInt i;
	Text::StringBuilderUTF8 sb;
	while (true)
	{
		i = Text::StrIndexOf(subscribeTopic, '+');
		if (i < 0)
			break;
		if (i > 0)
		{
			sb.ClearStr();
			sb.AppendC(subscribeTopic, (UOSInt)i);
			if (!Text::StrStartsWith(topic, sb.ToString()))
			{
				return false;
			}
			topic += i;
			subscribeTopic += i;
		}
		i = Text::StrIndexOf(topic, '/');
		if (subscribeTopic[1] == 0)
		{
			return (i < 0);
		}
		else if (i < 0)
		{
			return false;
		}
		subscribeTopic++;
		topic += i;
	}
	i = Text::StrIndexOf(subscribeTopic, '#');
	if (i < 0)
	{
		return Text::StrEquals(topic, subscribeTopic);
	}
	else if (i == 0)
	{
		return true;
	}

	sb.ClearStr();
	sb.AppendC(subscribeTopic, (UOSInt)i);
	if (!Text::StrStartsWith(topic, sb.ToString()))
	{
		return false;
	}
	return true;
}

Bool Net::MQTTBroker::TopicSend(IO::Stream *stm, void *stmData, const TopicInfo *topic)
{
	UInt8 packet1[128];
	UInt8 packet2[128];
	UInt8 *packetBuff1;
	UInt8 *packetBuff2;
	UOSInt i;
	UOSInt sent;
	UOSInt topicLen = Text::StrCharCnt(topic->topic);
	Sync::Interlocked::Increment(&this->infoPubSent);
	Sync::Interlocked::Increment(&this->infoMsgSent);
	if (topicLen + topic->msgSize + 4 <= 128)
	{
		WriteMInt16(&packet1[0], topicLen);
		Text::StrConcat(&packet1[2], topic->topic);
		MemCopyNO(&packet1[2 + topicLen], topic->message, topic->msgSize);
		i = this->protoHdlr->BuildPacket(packet2, 0x30, 0, packet1, 2 + topicLen + topic->msgSize, stmData);
		sent = stm->Write(packet2, i);
		Sync::Interlocked::Add(&this->infoTotalSent, sent);
		return sent == i;
	}
	else
	{
		packetBuff1 = MemAlloc(UInt8, topicLen + topic->msgSize + 3);
		packetBuff2 = MemAlloc(UInt8, topicLen + topic->msgSize + 7);
		WriteMInt16(&packetBuff1[0], topicLen);
		Text::StrConcat(&packetBuff1[2], topic->topic);
		MemCopyNO(&packetBuff1[2 + topicLen], topic->message, topic->msgSize);
		i = this->protoHdlr->BuildPacket(packetBuff2, 0x30, 0, packetBuff1, 2 + topicLen + topic->msgSize, stmData);
		sent = stm->Write(packetBuff2, i);
		MemFree(packetBuff1);
		MemFree(packetBuff2);
		Sync::Interlocked::Add(&this->infoTotalSent, sent);
		return sent == i;
	}
}

Net::MQTTBroker::MQTTBroker(Net::SocketFactory *sockf, IO::LogTool *log, UInt16 port, Bool sysInfo)
{
	this->sockf = sockf;
	this->log = log;
	this->connHdlr = 0;
	this->connObj = 0;
	this->publishHdlr = 0;
	this->publishObj = 0;
	this->subscribeHdlr = 0;
	this->subscribeObj = 0;
	this->topicUpdHdlr = 0;
	this->topicUpdObj = 0;
	this->infoTotalRecv = 0;
	this->infoTotalSent = 0;
	this->infoCliDisconn = 0;
	this->infoCliMax = 0;
	this->infoMsgRecv = 0;
	this->infoMsgSent = 0;
	this->infoPubRecv = 0;
	this->infoPubSent = 0;
	this->infoPubDrop = 0;
	this->infoStartTime = 0;
	this->sysInfoRunning = false;
	this->sysInfoToStop = false;
	NEW_CLASS(this->sysInfoEvt, Sync::Event(true, (const UTF8Char*)"Net.MQTTBroker.sysInfoEvt"));
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	this->infoStartTime = dt.ToTicks();

	NEW_CLASS(this->topicMut, Sync::Mutex());
	NEW_CLASS(this->topicMap, Data::StringUTF8Map<TopicInfo*>());
	NEW_CLASS(this->subscribeMut, Sync::Mutex());
	NEW_CLASS(this->subscribeList, Data::ArrayList<SubscribeInfo*>());
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoMQTTHandler(this));
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(240, OnClientEvent, OnClientData, this, Sync::Thread::GetThreadCnt(), OnClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, this->log, OnClientConn, this, (const UTF8Char*)"MQTT: "));
	if (this->svr->IsV4Error())
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
		DEL_CLASS(this->cliMgr);
		this->cliMgr = 0;
	}
	else if (sysInfo)
	{
		Sync::Thread::Create(SysInfoThread, this);
		while (!this->sysInfoRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

Net::MQTTBroker::~MQTTBroker()
{
	if (this->sysInfoRunning)
	{
		this->sysInfoToStop = true;
		this->sysInfoEvt->Set();
		while (this->sysInfoRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		DEL_CLASS(this->cliMgr);
		this->svr = 0;
		this->cliMgr = 0;
	}
	DEL_CLASS(this->sysInfoEvt);
	DEL_CLASS(this->protoHdlr);
	Data::ArrayList<TopicInfo*> *topicList = this->topicMap->GetValues();
	TopicInfo *topic;
	UOSInt i = topicList->GetCount();
	while (i-- > 0)
	{
		topic = topicList->GetItem(i);
		Text::StrDelNew(topic->topic);
		MemFree(topic->message);
		MemFree(topic);
	}
	DEL_CLASS(this->topicMap);
	DEL_CLASS(this->topicMut);

	SubscribeInfo *subscribe;
	i = this->subscribeList->GetCount();
	while (i-- > 0)
	{
		subscribe = this->subscribeList->GetItem(i);
		Text::StrDelNew(subscribe->topic);
		MemFree(subscribe);
	}
	DEL_CLASS(this->subscribeList);
	DEL_CLASS(this->subscribeMut);
}

Bool Net::MQTTBroker::IsError()
{
	return this->svr == 0;
}

void Net::MQTTBroker::HandleConnect(ConnectHandler connHdlr, void *userObj)
{
	this->connHdlr = connHdlr;
	this->connObj = userObj;
}

void Net::MQTTBroker::HandlePublish(PublishHandler publishHdlr, void *userObj)
{
	this->publishHdlr = publishHdlr;
	this->publishObj = userObj;
}

void Net::MQTTBroker::HandleSubscribe(SubscribeHandler subscribeHdlr, void *userObj)
{
	this->subscribeHdlr = subscribeHdlr;
	this->subscribeObj = userObj;
}

void Net::MQTTBroker::HandleTopicUpdate(TopicUpdateHandler topicUpdHdlr, void *userObj)
{
	this->topicUpdHdlr = topicUpdHdlr;
	this->topicUpdObj = userObj;

	if (this->topicUpdHdlr)
	{
		Data::ArrayList<TopicInfo*> *topicList;
		TopicInfo *topic;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage(this->topicMut);
		topicList = this->topicMap->GetValues();
		i = 0;
		j = topicList->GetCount();
		while (i < j)
		{
			topic = topicList->GetItem(i);
			this->topicUpdHdlr(this->topicUpdObj, topic->topic, topic->message, topic->msgSize);
			i++;
		}
		mutUsage.EndUse();
	}
}
