#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MQTTBroker.h"
#include "Net/MQTTUtil.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

typedef struct
{
	void *cliData;
	Text::String *cliId;
	UOSInt buffSize;
	UInt8 recvBuff[20000];
	UInt16 keepAlive;
	Bool connected;
} ClientData;

void __stdcall Net::MQTTBroker::OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	Listener *listener = (Listener*)userObj;
	ClientData *data = (ClientData*)cliData;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		if (listener->me->log->HasHandler())
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Client "));
			sptr = cli->GetRemoteName(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" disconnect"));
			listener->me->log->LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);
		}
		listener->me->StreamClosed(cli, data);
		cli.Delete();
	}
}

void __stdcall Net::MQTTBroker::OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff)
{
	Listener *listener = (Listener*)userObj;
	listener->me->StreamData(cli, cliData, buff);
}

void __stdcall Net::MQTTBroker::OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{
}

void __stdcall Net::MQTTBroker::OnClientReady(NotNullPtr<Net::TCPClient> cli, void *userObj)
{
	Listener *listener = (Listener*)userObj;
	listener->cliMgr->AddClient(cli, listener->me->StreamCreated(cli));
	UOSInt cnt = listener->cliMgr->GetClientCount();
	if (cnt > listener->me->infoCliMax)
	{
		listener->me->infoCliMax = cnt;
	}
}

void __stdcall Net::MQTTBroker::OnClientConn(Socket *s, void *userObj)
{
	Listener *listener = (Listener*)userObj;
	NotNullPtr<Net::SSLEngine> ssl;
	if (listener->ssl.SetTo(ssl))
	{
		ssl->ServerInit(s, OnClientReady, listener);
	}
	else
	{
		NotNullPtr<Net::TCPClient> cli;
		NEW_CLASSNN(cli, Net::TCPClient(listener->me->sockf, s));
		OnClientReady(cli, listener);
	}
}

UInt32 __stdcall Net::MQTTBroker::SysInfoThread(void *userObj)
{
	Net::MQTTBroker *me = (Net::MQTTBroker*)userObj;
	Data::DateTime *dt;
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt totalCnt;
	Listener *listener;
	NEW_CLASS(dt, Data::DateTime());

	dt->SetCurrTimeUTC();
	i = (UOSInt)(dt->ToString(Text::StrConcatC(sbuff, UTF8STRC("SMQTT ")), "yyyyMMdd") - sbuff);
	me->UpdateTopic(CSTR("$SYS/broker/version"), sbuff, i, true);
	me->sysInfoRunning = true;
	while (!me->sysInfoToStop)
	{
		dt->SetCurrTimeUTC();
		i = (UOSInt)(Text::StrConcatC(Text::StrInt64(sbuff, (dt->ToTicks() - me->infoStartTime) / 1000), UTF8STRC(" seconds")) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/uptime"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoTotalRecv) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/load/bytes/received"), sbuff, i, true);

		i = (UOSInt)(Text::StrUInt64(sbuff, me->infoTotalSent) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/load/bytes/sent"), sbuff, i, true);

		totalCnt = 0;
		j = me->listeners.GetCount();
		while (j-- > 0)
		{
			listener = me->listeners.GetItem(j);
			if (listener->cliMgr)
			{
				totalCnt += listener->cliMgr->GetClientCount();
			}
			else if (listener->listener)
			{
				totalCnt += listener->listener->GetClientCount();
			}
		}
		i = (UOSInt)(Text::StrUOSInt(sbuff, totalCnt) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/clients/connected"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoCliDisconn) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/clients/disconnected"), sbuff, i, true);

		i = (UOSInt)(Text::StrUInt64(sbuff, me->infoCliMax) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/clients/maximum"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoMsgRecv) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/received"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoMsgSent) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/sent"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubDrop) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/publish/dropped"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubRecv) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/publish/received"), sbuff, i, true);

		i = (UOSInt)(Text::StrInt64(sbuff, me->infoPubSent) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/publish/sent"), sbuff, i, true);

		i = (UOSInt)(Text::StrUOSInt(sbuff, me->topicMap.GetCount()) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/messages/retained/count"), sbuff, i, true);

		i = (UOSInt)(Text::StrUOSInt(sbuff, me->subscribeList.GetCount()) - sbuff);
		me->UpdateTopic(CSTR("$SYS/broker/subscriptions/count"), sbuff, i, true);

		me->sysInfoEvt.Wait(10000);
	}
	DEL_CLASS(dt);
	me->sysInfoRunning = false;
	return 0;
}

void Net::MQTTBroker::DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	ClientData *data = (ClientData*)stmObj;
	UInt8 packet[256];
	UInt8 packet2[256];
	UOSInt i;
	UOSInt j;
	UOSInt sent;
	Sync::Interlocked::IncrementI64(this->infoMsgRecv);
	switch (cmdType >> 4)
	{
	case 1: //CONNECT
		{
			Text::String *clientId = 0;
			Text::String *userName = 0;
			Text::String *password = 0;
			if (this->log->HasHandler())
			{
				UInt8 connFlags;
				Text::StringBuilderUTF8 sb;
				Text::StringBuilderUTF8 sb2;
				sb.AppendC(UTF8STRC("Packet Type = CONNECT, size = "));
				sb.AppendUOSInt(cmdSize);
				sb.AppendC(UTF8STRC(" bytes, protoName = "));
				i = 0;
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
				{
					sb.AppendC(UTF8STRC(", data = "));
					sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					break;
				}
				if (cmdSize - i < 4)
				{
					sb.AppendC(UTF8STRC(", data = "));
					sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					break;
				}
				sb.AppendC(UTF8STRC(", Level = "));
				sb.AppendU32(cmd[i]);
				sb.AppendC(UTF8STRC(", Flags = 0x"));
				sb.AppendHex8(cmd[i + 1]);
				sb.AppendC(UTF8STRC(", Keep Alive = "));
				data->keepAlive = ReadMUInt16(&cmd[i + 2]);
				sb.AppendU16(data->keepAlive);
				connFlags = cmd[i + 1];
				i += 4;
				sb.AppendC(UTF8STRC(", ClientId = "));
				sb2.ClearStr();
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb2))
				{
					sb.AppendC(UTF8STRC(", data = "));
					sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					break;
				}
				clientId = Text::String::New(sb.ToCString()).Ptr();
				sb.AppendC(sb2.ToString(), sb2.GetLength());
				if (connFlags & 4)
				{
					sb.AppendC(UTF8STRC(", Will Topic = "));
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						SDEL_STRING(clientId);
						break;
					}
					sb.AppendC(UTF8STRC(", Will Message = "));
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						SDEL_STRING(clientId);
						break;
					}
				}
				if (connFlags & 0x80)
				{
					sb.AppendC(UTF8STRC(", User Name = "));
					sb2.ClearStr();
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb2))
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						SDEL_STRING(clientId);
						break;
					}
					userName = Text::String::New(sb2.ToCString()).Ptr();
					sb.AppendC(sb2.ToString(), sb2.GetLength());
				}
				if (connFlags & 0x40)
				{
					sb.AppendC(UTF8STRC(", Password = "));
					if (cmdSize - i < 2)
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						SDEL_STRING(clientId);
						SDEL_STRING(userName);
						break;
					}
					UOSInt pwdSize = ReadMUInt16(&cmd[i]);
					if (cmdSize - i - 2 < pwdSize)
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						SDEL_STRING(clientId);
						SDEL_STRING(userName);
						break;
					}
					sb2.ClearStr();
					sb2.AppendC(&cmd[i + 2], pwdSize);
					password = Text::String::New(sb2.ToCString()).Ptr();

					sb.AppendHexBuff(&cmd[i + 2], pwdSize, ' ', Text::LineBreakType::None);
					i += pwdSize + 2;
				}
				this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
			}
			else
			{
				UInt8 connFlags;
				Text::StringBuilderUTF8 sb;
				i = 0;
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
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
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
				{
					break;
				}
				clientId = Text::String::New(sb.ToCString()).Ptr();
				if (connFlags & 4)
				{
					sb.ClearStr();
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
					{
						SDEL_STRING(clientId);
						break;
					}
					sb.ClearStr();
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
					{
						SDEL_STRING(clientId);
						break;
					}
				}
				if (connFlags & 0x80)
				{
					sb.ClearStr();
					if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
					{
						SDEL_STRING(clientId);
						break;
					}
					userName = Text::String::New(sb.ToCString()).Ptr();
				}
				if (connFlags & 0x40)
				{
					if (cmdSize - i < 2)
					{
						SDEL_STRING(clientId);
						SDEL_STRING(userName);
						break;
					}
					UOSInt pwdSize = ReadMUInt16(&cmd[i]);
					if (cmdSize - i - 2 < pwdSize)
					{
						SDEL_STRING(clientId);
						SDEL_STRING(userName);
						break;
					}
					sb.ClearStr();
					sb.AppendC(&cmd[i], pwdSize);
					password = Text::String::New(sb.ToCString()).Ptr();

					i += pwdSize + 2;
				}
			}
			ConnectStatus cs;
			if (this->connHdlr)
			{
				Net::SocketUtil::AddressInfo addr;
				((Net::TCPClient*)stm.Ptr())->GetRemoteAddr(addr);
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
			case CS_UNSPECIFIED_ERROR:
			case CS_MALFORMED_PACKET:
			case CS_PROTOCOL_ERROR:
			case CS_IMPL_ERROR:
			case CS_SERVER_BUSY:
			default:
				packet[1] = 6;
				break;
			}
			if (cs == CS_ACCEPTED)
			{
				data->connected = true;
				SDEL_STRING(data->cliId);
				data->cliId = clientId->Clone().Ptr();
			}

			i = this->protoHdlr.BuildPacket(packet2, 0x20, 0, packet, 2, data->cliData);
			sent = stm->Write(packet2, i);
			SDEL_STRING(clientId);
			SDEL_STRING(userName);
			SDEL_STRING(password);
			Sync::Interlocked::AddU64(this->infoTotalSent, sent);
		}
		break;
	case 2: //CONNACK
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = CONNACK, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(", data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 3: //PUBLISH
		{
			Text::StringBuilderUTF8 topicSb;
			UInt16 packetId = 0;
			const UInt8 *message;
			UOSInt messageSize;
			Sync::Interlocked::IncrementI64(this->infoPubRecv);
			if (this->log->HasHandler())
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Packet Type = PUBLISH, Flags = 0x"));
				sb.AppendHex8(cmdType & 15);
				sb.AppendC(UTF8STRC(", Topic = "));
				i = 0;
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, topicSb))
				{
					sb.AppendC(UTF8STRC(", data = "));
					sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					break;
				}
				sb.Append(topicSb.ToCString());
				if (cmdType & 6)
				{
					if (cmdSize - i < 2)
					{
						sb.AppendC(UTF8STRC(", data = "));
						sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
						this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
						break;
					}
					sb.AppendC(UTF8STRC(", PacketId = "));					
					sb.AppendU16(packetId = ReadMUInt16(&cmd[i]));
					i += 2;
				}
				sb.AppendC(UTF8STRC(", Message = "));
				message = &cmd[i];
				if (i < cmdSize)
				{
					messageSize = cmdSize - i;
					sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::None);
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendC(&cmd[i], cmdSize - i);
					sb.AppendC(UTF8STRC(")"));
				}
				else
				{
					messageSize = 0;
				}
				this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
			}
			else
			{
				i = 0;
				if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, topicSb))
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
					this->publishHdlr(this->publishObj, topicSb.ToCString(), packetId, message, messageSize);
				}
				this->UpdateTopic(topicSb.ToCString(), message, messageSize, false);
				if ((cmdType & 6) == 2)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0;
					packet[3] = 0;
					i = this->protoHdlr.BuildPacket(packet2, 0x40, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
				else if ((cmdType & 6) == 4)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0;
					packet[3] = 0;
					i = this->protoHdlr.BuildPacket(packet2, 0x50, 0, packet, 4, data->cliData);
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
					i = this->protoHdlr.BuildPacket(packet2, 0x40, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
				else if ((cmdType & 6) == 4)
				{
					WriteMInt16(packet, packetId);
					packet[2] = 0x87;
					packet[3] = 0;
					i = this->protoHdlr.BuildPacket(packet2, 0x50, 0, packet, 4, data->cliData);
					sent = stm->Write(packet2, i);
				}
			}
		}
		break;
	case 4: //PUBACK
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PUBACK, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 5: //PUBREC
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PUBREC, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 6: //PUBREL
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PUBREL, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 7: //PUBCOMP
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PUBCOMP, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 8: //SUBSCRIBE
		if (this->log->HasHandler())
		{
			UInt16 packetId = 0;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = SUBSCRIBE, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(", Packet Id = "));
			if (cmdSize >= 2)
			{
				packetId = ReadMUInt16(&cmd[0]);
				sb.AppendU16(packetId);
			}
			i = 2;
			sb.AppendC(UTF8STRC(", Topic = "));
			if (!this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sb))
			{
				sb.AppendC(UTF8STRC(", data = "));
				sb.AppendHexBuff(&cmd[i], cmdSize - i, ' ', Text::LineBreakType::CRLF);
				this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
				break;
			}
			if (cmdSize > i)
			{
				sb.AppendC(UTF8STRC(", Requested QoS = "));
				sb.AppendU16(cmd[i]);
			}
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		{
			UInt16 packetId = 0;
			Text::StringBuilderUTF8 sbTopic;
			i = 2;
			sbTopic.ClearStr();
			this->protoHdlr.ParseUTF8Str(cmd, &i, cmdSize, sbTopic);
			if (cmdSize >= 2)
			{
				packetId = ReadMUInt16(&cmd[0]);
			}

			ConnectStatus cs = CS_NOT_AUTHORIZED;
			if (data->cliId == 0)
			{
				cs = CS_CLI_ID_NOT_VALID;
			}
			else if (!Net::MQTTUtil::TopicValid(sbTopic.ToString()))
			{
				cs = CS_MALFORMED_PACKET;
			}
			else if (this->subscribeHdlr)
			{
				cs = this->subscribeHdlr(this->subscribeObj, data->cliId, sbTopic.ToCString());
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
			i = this->protoHdlr.BuildPacket(packet2, 0x90, 0, packet, 3, data->cliData);
			sent = stm->Write(packet2, i);
			Sync::Interlocked::AddU64(this->infoTotalSent, sent);

			if (cs == CS_ACCEPTED)
			{
				SubscribeInfo *subscribe = MemAlloc(SubscribeInfo, 1);
				subscribe->topic = Text::String::New(sbTopic.ToCString());
				subscribe->stm = stm;
				subscribe->cliData = data;
				Sync::MutexUsage subscribeMutUsage(this->subscribeMut);
				this->subscribeList.Add(subscribe);
				subscribeMutUsage.EndUse();

				TopicInfo *topicInfo;
				Sync::MutexUsage topicMutUsage(this->topicMut);
				i = 0;
				j = this->topicMap.GetCount();
				while (i < j)
				{
					topicInfo = this->topicMap.GetItem(i);
					if (Net::MQTTUtil::TopicMatch(topicInfo->topic->v, topicInfo->topic->leng, sbTopic.ToString(), sbTopic.GetLength()))
					{
						this->TopicSend(stm, data->cliData, topicInfo);
					}
					i++;
				}
			}
		}
		break;
	case 9: //SUBACK
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = SUBACK, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 10: //UNSUBSCRIBE
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = UNSUBSCRIBE, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 11: //UNSUBACK
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = UNSUBACK, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 12: //PINGREQ
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PINGREQ, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		i = this->protoHdlr.BuildPacket(packet2, 0xD0, 0, packet, 0, data->cliData);
		sent = stm->Write(packet2, i);
		Sync::Interlocked::AddU64(this->infoTotalSent, sent);
		break;
	case 13: //PINGRESP
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = PINGRESP, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	case 14: //DISCONNECT
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = DISCONNECT, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	default:
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Packet Type = Reserved, size = "));
			sb.AppendUOSInt(cmdSize);
			sb.AppendC(UTF8STRC(" bytes, data = "));
			sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::CRLF);
			this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
		break;
	}
}

void Net::MQTTBroker::DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void Net::MQTTBroker::UpdateTopic(Text::CStringNN topic, const UInt8 *message, UOSInt msgSize, Bool suppressUnchg)
{
	TopicInfo *topicInfo;
	Bool unchanged = false;
	Sync::MutexUsage topicMutUsage(this->topicMut);
	topicInfo = this->topicMap.GetC(topic);
	if (topicInfo == 0)
	{
		topicInfo = MemAlloc(TopicInfo, 1);
		topicInfo->topic = Text::String::New(topic);
		topicInfo->message = MemAlloc(UInt8, msgSize);
		MemCopyNO(topicInfo->message, message, msgSize);
		topicInfo->msgSize = msgSize;
		this->topicMap.PutNN(topicInfo->topic, topicInfo);
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
	i = this->subscribeList.GetCount();
	while (i-- > 0)
	{
		subscribe = this->subscribeList.GetItem(i);
		if (Net::MQTTUtil::TopicMatch(topic.v, topic.leng, subscribe->topic->v, subscribe->topic->leng))
		{
			topicMutUsage.BeginUse();
			this->TopicSend(subscribe->stm, ((ClientData*)subscribe->cliData)->cliData, topicInfo);
			topicMutUsage.EndUse();
		}
	}
}

Bool Net::MQTTBroker::TopicSend(NotNullPtr<IO::Stream> stm, void *stmData, const TopicInfo *topic)
{
	UInt8 packet1[128];
	UInt8 packet2[128];
	UInt8 *packetBuff1;
	UInt8 *packetBuff2;
	UOSInt i;
	UOSInt sent;
	UOSInt topicLen = topic->topic->leng;
	Sync::Interlocked::IncrementI64(this->infoPubSent);
	Sync::Interlocked::IncrementI64(this->infoMsgSent);
	if (topicLen + topic->msgSize + 4 <= 128)
	{
		WriteMInt16(&packet1[0], topicLen);
		topic->topic->ConcatTo(&packet1[2]);
		MemCopyNO(&packet1[2 + topicLen], topic->message, topic->msgSize);
		i = this->protoHdlr.BuildPacket(packet2, 0x30, 0, packet1, 2 + topicLen + topic->msgSize, stmData);
		sent = stm->Write(packet2, i);
		Sync::Interlocked::AddU64(this->infoTotalSent, sent);
		return sent == i;
	}
	else
	{
		packetBuff1 = MemAlloc(UInt8, topicLen + topic->msgSize + 3);
		packetBuff2 = MemAlloc(UInt8, topicLen + topic->msgSize + 7);
		WriteMInt16(&packetBuff1[0], topicLen);
		topic->topic->ConcatTo(&packetBuff1[2]);
		MemCopyNO(&packetBuff1[2 + topicLen], topic->message, topic->msgSize);
		i = this->protoHdlr.BuildPacket(packetBuff2, 0x30, 0, packetBuff1, 2 + topicLen + topic->msgSize, stmData);
		sent = stm->Write(packetBuff2, i);
		MemFree(packetBuff1);
		MemFree(packetBuff2);
		Sync::Interlocked::AddU64(this->infoTotalSent, sent);
		return sent == i;
	}
}

void *Net::MQTTBroker::StreamCreated(NotNullPtr<IO::Stream> stm)
{
	ClientData *data;
	data = MemAlloc(ClientData, 1);
	data->buffSize = 0;
	data->cliData = this->protoHdlr.CreateStreamData(stm);
	data->keepAlive = 0;
	data->connected = false;
	data->cliId = 0;
	return data;
}

void Net::MQTTBroker::StreamData(NotNullPtr<IO::Stream> stm, void *stmData, const Data::ByteArrayR &buff)
{
	ClientData *data = (ClientData*)stmData;
	Sync::Interlocked::AddI64(this->infoTotalRecv, (OSInt)buff.GetSize());

	if (this->log->HasHandler())
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Received "));
		sb.AppendUOSInt(buff.GetSize());
		sb.AppendC(UTF8STRC(" bytes"));
		this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	UOSInt i;
	if (data->buffSize > 0)
	{
		MemCopyNO(&data->recvBuff[data->buffSize], buff.Ptr(), buff.GetSize());
		data->buffSize += buff.GetSize();
		i = this->protoHdlr.ParseProtocol(stm, data, data->cliData, Data::ByteArrayR(data->recvBuff, data->buffSize));
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
		i = this->protoHdlr.ParseProtocol(stm, data, data->cliData, buff);
		if (i > 0)
		{
			MemCopyNO(data->recvBuff, &buff[buff.GetSize() - i], i);
			data->buffSize = i;
		}
	}
}

void Net::MQTTBroker::StreamClosed(NotNullPtr<IO::Stream> stm, void *stmData)
{
	ClientData *data = (ClientData*)stmData;
	this->protoHdlr.DeleteStreamData(stm, data->cliData);
	SDEL_STRING(data->cliId);
	MemFree(data);

	UOSInt i;
	SubscribeInfo *subscribe;
	Sync::MutexUsage mutUsage(this->subscribeMut);
	i = this->subscribeList.GetCount();
	while (i-- > 0)
	{
		subscribe = this->subscribeList.GetItem(i);
		if (subscribe->stm == stm)
		{
			this->subscribeList.RemoveAt(i);
			subscribe->topic->Release();
			MemFree(subscribe);
		}
	}
}

Net::MQTTBroker::MQTTBroker(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt16 port, NotNullPtr<IO::LogTool> log, Bool sysInfo, Bool autoStart) : protoHdlr(*this), wsHdlr(this)
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
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	this->infoStartTime = dt.ToTicks();

	if (this->AddListener(ssl, port, autoStart) && sysInfo)
	{
		Sync::ThreadUtil::Create(SysInfoThread, this);
		while (!this->sysInfoRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
}

Net::MQTTBroker::~MQTTBroker()
{
	if (this->sysInfoRunning)
	{
		this->sysInfoToStop = true;
		this->sysInfoEvt.Set();
		while (this->sysInfoRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
	UOSInt i = this->listeners.GetCount();
	Listener *listener;
	while (i-- > 0)
	{
		listener = this->listeners.GetItem(i);
		if (listener->svr)
		{
			DEL_CLASS(listener->svr);
			DEL_CLASS(listener->cliMgr);
		}
		else if (listener->listener)
		{
			DEL_CLASS(listener->listener);
		}
		MemFree(listener);
	}
	TopicInfo *topic;
	i = this->topicMap.GetCount();
	while (i-- > 0)
	{
		topic = this->topicMap.GetItem(i);
		topic->topic->Release();
		MemFree(topic->message);
		MemFree(topic);
	}

	SubscribeInfo *subscribe;
	i = this->subscribeList.GetCount();
	while (i-- > 0)
	{
		subscribe = this->subscribeList.GetItem(i);
		subscribe->topic->Release();
		MemFree(subscribe);
	}
}

Bool Net::MQTTBroker::AddListener(Optional<Net::SSLEngine> ssl, UInt16 port, Bool autoStart)
{
	Listener *listener = MemAlloc(Listener, 1);
	listener->me = this;
	listener->ssl = ssl;
	listener->listener = 0;
	NEW_CLASS(listener->cliMgr, Net::TCPClientMgr(240, OnClientEvent, OnClientData, listener, Sync::ThreadUtil::GetThreadCnt(), OnClientTimeout));
	NEW_CLASS(listener->svr, Net::TCPServer(this->sockf, 0, port, this->log, OnClientConn, listener, CSTR("MQTT: "), autoStart));
	if (listener->svr->IsV4Error())
	{
		DEL_CLASS(listener->svr);
		listener->svr = 0;
		DEL_CLASS(listener->cliMgr);
		listener->cliMgr = 0;
		MemFree(listener);
		return false;
	}
	else
	{
		this->listeners.Add(listener);
		return true;
	}
}

Bool Net::MQTTBroker::AddWSListener(Optional<Net::SSLEngine> ssl, UInt16 port, Bool autoStart)
{
	Listener *listener = MemAlloc(Listener, 1);
	listener->me = this;
	listener->ssl = 0;
	listener->cliMgr = 0;
	listener->svr = 0;
	NEW_CLASS(listener->listener, Net::WebServer::WebListener(this->sockf, ssl, this->wsHdlr, port, 60, 2, Sync::ThreadUtil::GetThreadCnt(), CSTR("SSWRMQTT/1.0"), false, Net::WebServer::KeepAlive::No, autoStart));
	if (listener->listener->IsError())
	{
		DEL_CLASS(listener->listener);
		MemFree(listener);
		return false;
	}
	else
	{
		this->listeners.Add(listener);
	}
	return false;
}

Bool Net::MQTTBroker::Start()
{
	Bool found = false;
	Listener *listener;
	UOSInt i = this->listeners.GetCount();
	while (i-- > 0)
	{
		listener = this->listeners.GetItem(i);
		if (listener->svr)
		{
			found = true;
			if (!listener->svr->Start())
				return false;
		}
		else if (listener->listener)
		{
			found = true;
			if (!listener->listener->Start())
				return false;
		}
	}
	return found;
}

Bool Net::MQTTBroker::IsError()
{
	return this->listeners.GetCount() == 0;
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
		TopicInfo *topic;
		UOSInt i;
		UOSInt j;
		Sync::MutexUsage mutUsage(this->topicMut);
		i = 0;
		j = this->topicMap.GetCount();
		while (i < j)
		{
			topic = this->topicMap.GetItem(i);
			this->topicUpdHdlr(this->topicUpdObj, topic->topic->ToCString(), topic->message, topic->msgSize);
			i++;
		}
	}
}
