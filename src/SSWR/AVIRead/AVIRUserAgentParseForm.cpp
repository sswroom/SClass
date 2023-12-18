#include "Stdafx.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentParseForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRUserAgentParseForm::OnParseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRUserAgentParseForm *me = (SSWR::AVIRead::AVIRUserAgentParseForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtUserAgent->GetText(sb);
	if (sb.GetLength() > 0)
	{
		Net::UserAgentDB::UAEntry ent;
		Net::UserAgentDB::ParseUserAgent(&ent, sb.ToCString());
		me->txtBrowser->SetText(Net::BrowserInfo::GetName(ent.browser));
		if (ent.browserVer)
			me->txtBrowserVer->SetText({(const UTF8Char*)ent.browserVer, ent.browserVerLen});
		else
			me->txtBrowserVer->SetText(CSTR("-"));
		me->txtOS->SetText(Manage::OSInfo::GetName(ent.os));
		if (ent.osVer)
			me->txtOSVer->SetText({(const UTF8Char*)ent.osVer, ent.osVerLen});
		else
			me->txtOSVer->SetText(CSTR("-"));
		if (ent.devName)
			me->txtDeviceName->SetText({(const UTF8Char*)ent.devName, ent.devNameLen});
		else
			me->txtDeviceName->SetText(CSTR("-"));
		SDEL_TEXT(ent.browserVer);
		SDEL_TEXT(ent.osVer);
		SDEL_TEXT(ent.devName);
	}
}

SSWR::AVIRead::AVIRUserAgentParseForm::AVIRUserAgentParseForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 200, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("User Agent Parse"));
	this->SetNoResize(true);
	
	this->core = core;

	NEW_CLASS(this->lblUserAgent, UI::GUILabel(ui, *this, CSTR("User Agent")));
	this->lblUserAgent->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtUserAgent, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtUserAgent->SetRect(104, 4, 600, 23, false);
	this->btnParse = ui->NewButton(*this, CSTR("Parse"));
	this->btnParse->SetRect(104, 28, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	NEW_CLASS(this->lblBrowser, UI::GUILabel(ui, *this, CSTR("Browser")));
	this->lblBrowser->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtBrowser, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtBrowser->SetRect(104, 52, 200, 23, false);
	this->txtBrowser->SetReadOnly(true);
	NEW_CLASS(this->lblBrowserVer, UI::GUILabel(ui, *this, CSTR("Browser Ver")));
	this->lblBrowserVer->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtBrowserVer, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtBrowserVer->SetRect(104, 76, 200, 23, false);
	this->txtBrowserVer->SetReadOnly(true);
	NEW_CLASS(this->lblOS, UI::GUILabel(ui, *this, CSTR("OS")));
	this->lblOS->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtOS, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtOS->SetRect(104, 100, 200, 23, false);
	this->txtOS->SetReadOnly(true);
	NEW_CLASS(this->lblOSVer, UI::GUILabel(ui, *this, CSTR("OS Version")));
	this->lblOSVer->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtOSVer, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtOSVer->SetRect(104, 124, 200, 23, false);
	this->txtOSVer->SetReadOnly(true);
	NEW_CLASS(this->lblDeviceName, UI::GUILabel(ui, *this, CSTR("Device Name")));
	this->lblDeviceName->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtDeviceName, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtDeviceName->SetRect(104, 148, 200, 23, false);
	this->txtDeviceName->SetReadOnly(true);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::AVIRead::AVIRUserAgentParseForm::~AVIRUserAgentParseForm()
{
}

void SSWR::AVIRead::AVIRUserAgentParseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
