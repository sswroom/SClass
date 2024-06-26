#include "Stdafx.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRUserAgentParseForm.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRUserAgentParseForm::OnParseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRUserAgentParseForm> me = userObj.GetNN<SSWR::AVIRead::AVIRUserAgentParseForm>();
	Text::StringBuilderUTF8 sb;
	me->txtUserAgent->GetText(sb);
	if (sb.GetLength() > 0)
	{
		UnsafeArray<const UTF8Char> nns;
		Net::UserAgentDB::UAEntry ent;
		Net::UserAgentDB::ParseUserAgent(ent, sb.ToCString());
		me->txtBrowser->SetText(Net::BrowserInfo::GetName(ent.browser));
		if (ent.browserVer.SetTo(nns))
			me->txtBrowserVer->SetText({nns, ent.browserVerLen});
		else
			me->txtBrowserVer->SetText(CSTR("-"));
		me->txtOS->SetText(Manage::OSInfo::GetName(ent.os));
		if (ent.osVer.SetTo(nns))
			me->txtOSVer->SetText({nns, ent.osVerLen});
		else
			me->txtOSVer->SetText(CSTR("-"));
		if (ent.devName.SetTo(nns))
			me->txtDeviceName->SetText({nns, ent.devNameLen});
		else
			me->txtDeviceName->SetText(CSTR("-"));
		SDEL_TEXT(ent.browserVer);
		SDEL_TEXT(ent.osVer);
		SDEL_TEXT(ent.devName);
	}
}

SSWR::AVIRead::AVIRUserAgentParseForm::AVIRUserAgentParseForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 200, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("User Agent Parse"));
	this->SetNoResize(true);
	
	this->core = core;

	this->lblUserAgent = ui->NewLabel(*this, CSTR("User Agent"));
	this->lblUserAgent->SetRect(4, 4, 100, 23, false);
	this->txtUserAgent = ui->NewTextBox(*this, CSTR(""));
	this->txtUserAgent->SetRect(104, 4, 600, 23, false);
	this->btnParse = ui->NewButton(*this, CSTR("Parse"));
	this->btnParse->SetRect(104, 28, 75, 23, false);
	this->btnParse->HandleButtonClick(OnParseClicked, this);
	this->lblBrowser = ui->NewLabel(*this, CSTR("Browser"));
	this->lblBrowser->SetRect(4, 52, 100, 23, false);
	this->txtBrowser = ui->NewTextBox(*this, CSTR(""));
	this->txtBrowser->SetRect(104, 52, 200, 23, false);
	this->txtBrowser->SetReadOnly(true);
	this->lblBrowserVer = ui->NewLabel(*this, CSTR("Browser Ver"));
	this->lblBrowserVer->SetRect(4, 76, 100, 23, false);
	this->txtBrowserVer = ui->NewTextBox(*this, CSTR(""));
	this->txtBrowserVer->SetRect(104, 76, 200, 23, false);
	this->txtBrowserVer->SetReadOnly(true);
	this->lblOS = ui->NewLabel(*this, CSTR("OS"));
	this->lblOS->SetRect(4, 100, 100, 23, false);
	this->txtOS = ui->NewTextBox(*this, CSTR(""));
	this->txtOS->SetRect(104, 100, 200, 23, false);
	this->txtOS->SetReadOnly(true);
	this->lblOSVer = ui->NewLabel(*this, CSTR("OS Version"));
	this->lblOSVer->SetRect(4, 124, 100, 23, false);
	this->txtOSVer = ui->NewTextBox(*this, CSTR(""));
	this->txtOSVer->SetRect(104, 124, 200, 23, false);
	this->txtOSVer->SetReadOnly(true);
	this->lblDeviceName = ui->NewLabel(*this, CSTR("Device Name"));
	this->lblDeviceName->SetRect(4, 148, 100, 23, false);
	this->txtDeviceName = ui->NewTextBox(*this, CSTR(""));
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
