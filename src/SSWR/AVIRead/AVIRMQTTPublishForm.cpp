#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "SSWR/AVIRead/AVIRMQTTPublishForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMQTTPublishForm::OnPublishClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMQTTPublishForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMQTTPublishForm>();
	Net::SocketUtil::AddressInfo addr;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbHost;
	NN<Text::String> topic;
	NN<Text::String> message;
	Text::CString username = nullptr;
	Text::CString password = nullptr;
	UInt16 port;
	me->txtHost->GetText(sbHost);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbHost.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in parsing host"), CSTR("MQTT Publish"), me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(sb);
	if (!sb.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Port is not valid"), CSTR("MQTT Publish"), me);
		return;
	}

	sb.ClearStr();
	me->txtTopic->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter topic"), CSTR("MQTT Publish"), me);
		return;
	}
	topic = Text::String::New(sb.ToCString());
	sb.ClearStr();
	me->txtMessage->GetText(sb);
	if (sb.GetLength() == 0)
	{
		topic->Release();
		me->ui->ShowMsgOK(CSTR("Please enter message"), CSTR("MQTT Publish"), me);
		return;
	}
	message = Text::String::New(sb.ToCString());

	sb.ClearStr();
	me->txtUsername->GetText(sb);
	if (sb.GetLength() > 0)
	{
		username.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
		username.leng = sb.GetLength();
	}
	sb.ClearStr();
	me->txtPassword->GetText(sb);
	if (sb.GetLength() > 0)
	{
		password.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr();
		password.leng = sb.GetLength();
	}
	if (Net::MQTTConn::PublishMessage(me->core->GetTCPClientFactory(), 0, sbHost.ToCString(), port, username, password, topic->ToCString(), message->ToCString(), 30000))
	{
		me->txtStatus->SetText(CSTR("Success"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Failed"));
	}
	topic->Release();
	message->Release();
	SDEL_TEXT(username.v);
	SDEL_TEXT(password.v);
}

SSWR::AVIRead::AVIRMQTTPublishForm::AVIRMQTTPublishForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Publish"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblHost = ui->NewLabel(*this, CSTR("Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	this->txtHost = ui->NewTextBox(*this, CSTR("127.0.0.1"));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	this->lblPort = ui->NewLabel(*this, CSTR("Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	this->txtPort = ui->NewTextBox(*this, CSTR("1883"));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	this->lblUsername = ui->NewLabel(*this, CSTR("Username"));
	this->lblUsername->SetRect(4, 52, 100, 23, false);
	this->txtUsername = ui->NewTextBox(*this, CSTR(""));
	this->txtUsername->SetRect(104, 52, 100, 23, false);
	this->lblPassword = ui->NewLabel(*this, CSTR("Password"));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	this->txtPassword = ui->NewTextBox(*this, CSTR(""));
	this->txtPassword->SetRect(104, 76, 100, 23, false);
	this->lblTopic = ui->NewLabel(*this, CSTR("Topic"));
	this->lblTopic->SetRect(4, 100, 100, 23, false);
	this->txtTopic = ui->NewTextBox(*this, CSTR(""));
	this->txtTopic->SetRect(104, 100, 100, 23, false);
	this->lblMessage = ui->NewLabel(*this, CSTR("Message"));
	this->lblMessage->SetRect(4, 124, 100, 23, false);
	this->txtMessage = ui->NewTextBox(*this, CSTR(""));
	this->txtMessage->SetRect(104, 124, 100, 23, false);
	this->btnPublish = ui->NewButton(*this, CSTR("Publish"));
	this->btnPublish->SetRect(104, 148, 75, 23, false);
	this->btnPublish->HandleButtonClick(OnPublishClicked, this);
	this->lblStatus = ui->NewLabel(*this, CSTR("Status"));
	this->lblStatus->SetRect(4, 172, 100, 23, false);
	this->txtStatus = ui->NewTextBox(*this, CSTR(""));
	this->txtStatus->SetRect(104, 172, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
}

SSWR::AVIRead::AVIRMQTTPublishForm::~AVIRMQTTPublishForm()
{
}

void SSWR::AVIRead::AVIRMQTTPublishForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
