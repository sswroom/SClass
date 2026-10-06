#include "Stdafx.h"
#include "DB/RedisClient.h"
#include "SSWR/AVIRead/AVIRRedisConnForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRRedisConnForm::OnOKClicked(AnyType userObj)
{
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb3;
	Text::StringBuilderUTF8 sbPort;
	NN<SSWR::AVIRead::AVIRRedisConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRedisConnForm>();
	me->txtServer->GetText(sb);
	me->txtPort->GetText(sbPort);
	me->txtPWD->GetText(sb3);
	UInt16 port;

	NN<Net::TCPClientFactory> clif = me->core->GetTCPClientFactory();
	Net::SocketUtil::AddressInfo addr;
	if (!sbPort.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("Redis Connection"), me);
		return;
	}
	else if (!clif->GetSocketFactory()->DNSResolveIP(sb.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving server host"), CSTR("Redis Connection"), me);
		return;
	}
	NN<DB::RedisClient> conn;
	if (sb3.leng > 0)
	{
		NEW_CLASSNN(conn, DB::RedisClient(me->core->GetTCPClientFactory(), sb.ToCString(), port, sb3.ToCString(), 0));
	}
	else
	{
		NEW_CLASSNN(conn, DB::RedisClient(me->core->GetTCPClientFactory(), sb.ToCString(), port));
	}
	if (!conn->IsConnected())
	{
		conn.Delete();
		me->ui->ShowMsgOK(CSTR("Error in opening Redis connection"), CSTR("Redis Connection"), me);
		return;
	}
	me->redis = conn;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRRedisConnForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRedisConnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRedisConnForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRRedisConnForm::AVIRRedisConnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 340, 188, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Redis Connection"));

	this->core = core;
	this->redis = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetNoResize(true);

	this->lblServer = ui->NewLabel(*this, CSTR("Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	this->txtServer = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtServer->SetRect(104, 4, 200, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("6379"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblPWD = ui->NewLabel(*this, CSTR("Password"));
	this->lblPWD->SetRect(4, 52, 100, 23, false);
	this->txtPWD = ui->NewTextBox(*this, CSTR(""));
	this->txtPWD->SetRect(104, 52, 200, 23, false);
	this->txtPWD->SetPasswordChar('*');
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(104, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 76, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txtServer->Focus();
}

SSWR::AVIRead::AVIRRedisConnForm::~AVIRRedisConnForm()
{
}

void SSWR::AVIRead::AVIRRedisConnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<IO::ConfigFile> SSWR::AVIRead::AVIRRedisConnForm::GetRedis() const
{
	return this->redis;
}
