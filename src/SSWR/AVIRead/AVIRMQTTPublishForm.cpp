#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "SSWR/AVIRead/AVIRMQTTPublishForm.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMQTTPublishForm::OnPublishClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMQTTPublishForm *me = (SSWR::AVIRead::AVIRMQTTPublishForm*)userObj;
	Net::SocketUtil::AddressInfo addr;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbHost;
	const UTF8Char *topic = 0;
	const UTF8Char *message = 0;
	Text::CString username = CSTR_NULL;
	Text::CString password = CSTR_NULL;
	UInt16 port;
	me->txtHost->GetText(&sbHost);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbHost.ToString(), sbHost.GetLength(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing host", (const UTF8Char*)"MQTT Publish", me);
		return;
	}
	sb.ClearStr();
	me->txtPort->GetText(&sb);
	if (!sb.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Port is not valid", (const UTF8Char*)"MQTT Publish", me);
		return;
	}

	sb.ClearStr();
	me->txtTopic->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter topic", (const UTF8Char*)"MQTT Publish", me);
		return;
	}
	topic = Text::StrCopyNew(sb.ToString());
	sb.ClearStr();
	me->txtMessage->GetText(&sb);
	if (sb.GetLength() == 0)
	{
		SDEL_TEXT(topic);
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter message", (const UTF8Char*)"MQTT Publish", me);
		return;
	}
	message = Text::StrCopyNew(sb.ToString());

	sb.ClearStr();
	me->txtUsername->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		username.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
		username.leng = sb.GetLength();
	}
	sb.ClearStr();
	me->txtPassword->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		password.v = Text::StrCopyNewC(sb.ToString(), sb.GetLength());
		password.leng = sb.GetLength();
	}
	if (Net::MQTTConn::PublishMessage(me->core->GetSocketFactory(), 0, sbHost.ToCString(), port, username, password, topic, message))
	{
		me->txtStatus->SetText(CSTR("Success"));
	}
	else
	{
		me->txtStatus->SetText(CSTR("Failed"));
	}
	SDEL_TEXT(topic);
	SDEL_TEXT(message);
	SDEL_TEXT(username.v);
	SDEL_TEXT(password.v);
}

SSWR::AVIRead::AVIRMQTTPublishForm::AVIRMQTTPublishForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MQTT Publish"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblHost, UI::GUILabel(ui, this, (const UTF8Char*)"Host"));
	this->lblHost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtHost, UI::GUITextBox(ui, this, CSTR("127.0.0.1")));
	this->txtHost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this, CSTR("1883")));
	this->txtPort->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this, (const UTF8Char*)"Username"));
	this->lblUsername->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this, CSTR("")));
	this->txtUsername->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPassword->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->lblTopic, UI::GUILabel(ui, this, (const UTF8Char*)"Topic"));
	this->lblTopic->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtTopic, UI::GUITextBox(ui, this, CSTR("")));
	this->txtTopic->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, this, (const UTF8Char*)"Message"));
	this->lblMessage->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtMessage, UI::GUITextBox(ui, this, CSTR("")));
	this->txtMessage->SetRect(104, 124, 100, 23, false);
	NEW_CLASS(this->btnPublish, UI::GUIButton(ui, this, CSTR("Publish")));
	this->btnPublish->SetRect(104, 148, 75, 23, false);
	this->btnPublish->HandleButtonClick(OnPublishClicked, this);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this, (const UTF8Char*)"Status"));
	this->lblStatus->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this, CSTR("")));
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
