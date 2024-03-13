#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRJTT808ServerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyStringFloat.h"

typedef struct
{
	void *cliData;
	UOSInt buffSize;
	UInt16 seqId;
	UInt8 recvBuff[4096];
} ClientData;

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRJTT808ServerForm *me = (SSWR::AVIRead::AVIRJTT808ServerForm*)userObj;
	if (me->svr)
	{
		me->ServerStop();
		me->txtPort->SetReadOnly(false);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("Error"), me);
		}
		else if (port <= 0)
		{
			me->ui->ShowMsgOK(CSTR("Port is out of range"), CSTR("Error"), me);
		}
		else
		{
			NEW_CLASS(me->cliMgr, Net::TCPClientMgr(240, OnClientEvent, OnClientData, me, Sync::ThreadUtil::GetThreadCnt(), OnClientTimeout));
			NEW_CLASS(me->svr, Net::TCPServer(me->core->GetSocketFactory(), 0, port, me->log, OnClientConn, me, CSTR("TCP: "), true));
			if (me->svr->IsV4Error())
			{
				me->ui->ShowMsgOK(CSTR("Error in starting server"), CSTR("Error"), me);
			}
			else
			{
				me->txtPort->SetReadOnly(true);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRJTT808ServerForm *me = (SSWR::AVIRead::AVIRJTT808ServerForm*)userObj;
	NotNullPtr<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	SSWR::AVIRead::AVIRJTT808ServerForm *me = (SSWR::AVIRead::AVIRJTT808ServerForm*)userObj;
	ClientData *data = (ClientData*)cliData;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Client "));
		sptr = cli->GetRemoteName(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" disconnect"));
		me->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);

		me->protoHdlr->DeleteStreamData(cli, data->cliData);
		MemFree(data);
		cli.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff)
{
	SSWR::AVIRead::AVIRJTT808ServerForm *me = (SSWR::AVIRead::AVIRJTT808ServerForm*)userObj;
	ClientData *data = (ClientData*)cliData;

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Received "));
	sptr = Text::StrUOSInt(sptr, buff.GetSize());
	sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes"));
	me->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);

	MemCopyNO(&data->recvBuff[data->buffSize], buff.Ptr(), buff.GetSize());
	data->buffSize += buff.GetSize();
	
	UOSInt size = me->protoHdlr->ParseProtocol(cli, data, data->cliData, Data::ByteArrayR(data->recvBuff, data->buffSize));
	if (size >= 2048 || size <= 0)
	{
		data->buffSize = 0;
	}
	else if (size != data->buffSize)
	{
		MemCopyO(data->recvBuff, &data->recvBuff[data->buffSize - size], size);
		data->buffSize = size;
	}
}

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{
}

void __stdcall SSWR::AVIRead::AVIRJTT808ServerForm::OnClientConn(Socket *s, void *userObj)
{
	SSWR::AVIRead::AVIRJTT808ServerForm *me = (SSWR::AVIRead::AVIRJTT808ServerForm*)userObj;
	NotNullPtr<Net::TCPClient> cli;
	ClientData *data;
	NEW_CLASSNN(cli, Net::TCPClient(me->core->GetSocketFactory(), s));
	data = MemAlloc(ClientData, 1);
	data->buffSize = 0;
	data->seqId = 0;
	data->cliData = me->protoHdlr->CreateStreamData(cli);
	me->cliMgr->AddClient(cli, data);
}

void SSWR::AVIRead::AVIRJTT808ServerForm::ServerStop()
{
	if (this->svr)
	{
		DEL_CLASS(this->svr);
		DEL_CLASS(this->cliMgr);
		this->svr = 0;
		this->cliMgr = 0;
	}
}

SSWR::AVIRead::AVIRJTT808ServerForm::AVIRJTT808ServerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("JT/T808 Server"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->lblPort = ui->NewLabel(this->tpStatus, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->tpStatus, CSTR("42463"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->btnStart = ui->NewButton(this->tpStatus, CSTR("Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoJTT808Handler(*this, 0));
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 100, false));
	this->logger->SetTimeFormat("yyyy-MM-dd HH:mm:ss.fff");
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->svr = 0;
	this->cliMgr = 0;
}

SSWR::AVIRead::AVIRJTT808ServerForm::~AVIRJTT808ServerForm()
{
	this->ServerStop();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	DEL_CLASS(this->protoHdlr);
}

void SSWR::AVIRead::AVIRJTT808ServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRJTT808ServerForm::DataParsed(NotNullPtr<IO::Stream> stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	ClientData *data = (ClientData*)stmObj;
	UInt8 packet[256];
	UInt8 tmpPacket[64];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Packet: seq = "));
	sptr = Text::StrInt32(sptr, seqId);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", type = 0x"));
	sptr = Text::StrHexVal16(sptr, (UInt16)cmdType);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", size = "));
	sptr = Text::StrUOSInt(sptr, cmdSize);
	sptr = Text::StrConcatC(sptr, UTF8STRC(", devId = "));
	sptr = Text::StrHexBytes(sptr, &cmd[5], 6, 0);
	this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);

	UOSInt contSize;
	UOSInt i;
	OSInt j;
	UInt8 c;
	const UInt8 *packetContent = this->protoHdlr->GetPacketContent(cmd, &contSize);

	switch (cmdType)
	{
	case 2: //Keep Alive
		this->log.LogMessage(CSTR("Cmd: Keep Alive"), IO::LogHandler::LogLevel::Action);
		break;
	case 3: //Logout
		this->log.LogMessage(CSTR("Cmd: Device Logout"), IO::LogHandler::LogLevel::Action);
		break;
	case 0x100: //Login
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Cmd: Login County = "));
		sptr = Text::StrUInt32(sptr, ReadMUInt16(&packetContent[0]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", City = "));
		sptr = Text::StrUInt32(sptr, ReadMUInt16(&packetContent[2]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Make = "));
		i = 5;
		j = 4;
		while (i-- > 0)
		{
			c = packetContent[j++];
			if (c == 0)
				break;
			*sptr++ = c;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Model = "));
		i = 20;
		j = 9;
		while (i-- > 0)
		{
			c = packetContent[j++];
			if (c == 0)
				break;
			*sptr++ = c;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC(", SN = "));
		i = 7;
		j = 29;
		while (i-- > 0)
		{
			c = packetContent[j++];
			if (c == 0)
				break;
			*sptr++ = c;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Color = "));
		sptr = Text::StrUInt32(sptr, packetContent[36]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", License = "));
		i = contSize - 37;
		j = 37;
		while (i-- > 0)
		{
			c = packetContent[j++];
			if (c == 0)
				break;
			*sptr++ = c;
		}
		*sptr = 0;
		this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);


		WriteMInt16(&tmpPacket[0], seqId);
		tmpPacket[2] = 0;
		tmpPacket[3] = '0';
		tmpPacket[4] = '0';
		tmpPacket[5] = '0';
		tmpPacket[6] = '0';
		tmpPacket[7] = '0';
		tmpPacket[8] = '0';
		i = this->protoHdlr->BuildPacket(packet, 0x8100, data->seqId++, tmpPacket, 9, data->cliData);
		stm->Write(packet, i);
		break;
	case 0x102: //Authentication
		this->log.LogMessage(CSTR("Cmd: Authentication"), IO::LogHandler::LogLevel::Action);
		WriteMInt16(&tmpPacket[0], seqId);
		WriteMInt16(&tmpPacket[2], 0x102);
		tmpPacket[4] = 0;
		i = this->protoHdlr->BuildPacket(packet, 0x8001, data->seqId++, tmpPacket, 5, data->cliData);
		stm->Write(packet, i);
		break;
	case 0x200:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Cmd: Location: Alert = 0x"));
		sptr = Text::StrHexVal32(sptr, ReadMUInt32(&packetContent[0]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Status = 0x"));
		sptr = Text::StrHexVal32(sptr, ReadMUInt32(&packetContent[4]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Latitude = "));
		sptr = Text::StrDouble(sptr, ReadMUInt32(&packetContent[8]) * 0.000001);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Longitude = "));
		sptr = Text::StrDouble(sptr, ReadMUInt32(&packetContent[12]) * 0.000001);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Altitude = "));
		sptr = Text::StrUInt32(sptr, ReadMUInt16(&packetContent[16]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Speed = "));
		sptr = Text::StrDouble(sptr, ReadMUInt16(&packetContent[18]) * 0.1);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Dir = "));
		sptr = Text::StrUInt32(sptr, ReadMUInt16(&packetContent[20]));
		{
			Data::DateTime dt;
			dt.SetValue((UInt16)(2000 + Data::ByteTool::GetBCD8(packetContent[22])), Data::ByteTool::GetBCD8(packetContent[23]), Data::ByteTool::GetBCD8(packetContent[24]), Data::ByteTool::GetBCD8(packetContent[25]), Data::ByteTool::GetBCD8(packetContent[26]), Data::ByteTool::GetBCD8(packetContent[27]), 0, 32);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Time = "));
			sptr = dt.ToString(sptr);
		}
		i = 28;
		while (i < contSize)
		{
			if (i + 2 + packetContent[i + 1] > contSize)
				break;
			switch (packetContent[i])
			{
			case 1:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", Mileage = "));
				if (packetContent[i + 1] == 4)
				{
					sptr = Text::StrDouble(sptr, ReadMUInt32(&packetContent[i + 2]) * 0.1);
				}
				break;
			case 2:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", Fuel Level = "));
				if (packetContent[i + 1] == 2)
				{
					sptr = Text::StrDouble(sptr, ReadMUInt16(&packetContent[i + 2]) * 0.1);
					sptr = Text::StrConcatC(sptr, UTF8STRC("L"));
				}
				break;
			case 3:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", Record Speed = "));
				if (packetContent[i + 1] == 2)
				{
					sptr = Text::StrDouble(sptr, ReadMUInt16(&packetContent[i + 2]) * 0.1);
				}
				break;
			case 0x25:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", Status2 = "));
				if (packetContent[i + 1] == 4)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("0x"));
					sptr = Text::StrHexVal32(sptr, ReadMUInt32(&packetContent[i + 2]));
				}
				break;
			case 0x2A:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", IOStatus = "));
				if (packetContent[i + 1] == 2)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("0x"));
					sptr = Text::StrHexVal16(sptr, ReadMUInt16(&packetContent[i + 2]));
				}
				break;
			case 0x2B:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", ADC: "));
				if (packetContent[i + 1] == 4)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("AD0 = "));
					sptr = Text::StrUInt16(sptr, ReadMUInt16(&packetContent[i + 4]));
					sptr = Text::StrConcatC(sptr, UTF8STRC(", AD1 = "));
					sptr = Text::StrUInt16(sptr, ReadMUInt16(&packetContent[i + 2]));
				}
				break;
			case 0x30:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", GSM RSSI = "));
				if (packetContent[i + 1] == 1)
				{
					sptr = Text::StrUInt16(sptr, packetContent[i + 2]);
				}
				break;
			case 0x31:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", GNSS Cnt = "));
				if (packetContent[i + 1] == 1)
				{
					sptr = Text::StrUInt16(sptr, packetContent[i + 2]);
				}
				break;
			default:
				sptr = Text::StrConcatC(sptr, UTF8STRC(", Unk Type (0x"));
				sptr = Text::StrHexByte(sptr, packetContent[i]);
				sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
				break;
			}
			i += (UOSInt)packetContent[i + 1] + 2;
		}
		this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);
		break;
	case 0x704:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Cmd: Batch Upload, Count = "));
		sptr = Text::StrUInt32(sptr, ReadMUInt16(&packetContent[0]));
		sptr = Text::StrConcatC(sptr, UTF8STRC(", Buffer Data = "));
		sptr = Text::StrUInt16(sptr, packetContent[2]);
		this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);
		j = 3;
		//////////////////////////////////////
		break;

	case 0x900:
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Cmd: Wrapped Data, type = 0x"));
		sptr = Text::StrHexByte(sptr, packetContent[0]);
		sptr = Text::StrConcatC(sptr, UTF8STRC(", data = "));
		sptr = Text::StrHexBytes(sptr, &packetContent[1], contSize - 1, ' ');
		this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);
		break;

	default:
		this->log.LogMessage(CSTR("Cmd: Unknown"), IO::LogHandler::LogLevel::Action);
		break;
	}
}

void SSWR::AVIRead::AVIRJTT808ServerForm::DataSkipped(NotNullPtr<IO::Stream> stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("Data skipped "));
	sptr = Text::StrUOSInt(sptr, buffSize);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes"));
	this->log.LogMessage(CSTRP(sbuff, sptr), IO::LogHandler::LogLevel::Action);
}
