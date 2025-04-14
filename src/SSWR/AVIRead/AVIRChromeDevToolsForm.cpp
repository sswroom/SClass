#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRChromeDevToolsForm.h"

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools> devTools;
	if (me->devTools.NotNull())
	{
		me->devTools.Delete();
		me->txtPort->SetReadOnly(false);
		me->btnStart->SetText(CSTR("Start"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UInt16 port;
		me->txtPort->GetText(sb);
		if (!sb.ToUInt16(port))
		{
			me->ui->ShowMsgOK(CSTR("Port number is not valid"), CSTR("Chrome DevTools"), me);
			return;
		}
		NEW_CLASSNN(devTools, Net::ChromeDevTools(me->clif, me->ssl, port));
		NN<Net::ChromeDevTools::ChromeVersion> ver;
		if (!devTools->GetVersion().SetTo(ver))
		{
			me->ui->ShowMsgOK(CSTR("Error in communicating with browser"), CSTR("Chrome DevTools"), me);
			return;
		}
		me->txtVersionBrowser->SetText(ver->GetBrowser()->ToCString());
		me->txtVersionProtocol->SetText(ver->GetProtocolVersion()->ToCString());
		me->txtVersionUserAgent->SetText(ver->GetUserAgent()->ToCString());
		me->txtVersionV8->SetText(ver->GetV8Version()->ToCString());
		me->txtVersionWebKit->SetText(ver->GetWebKitVersion()->ToCString());
		me->txtVersionWSDebuggerUrl->SetText(ver->GetWebSocketDebuggerUrl()->ToCString());
		ver.Delete();

		me->txtPort->SetReadOnly(true);
		me->btnStart->SetText(CSTR("Stop"));
		me->devTools = devTools;
		me->ReloadTargets();
	}
}

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnTargetClicked(AnyType userObj)
{
	userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>()->ReloadTargets();
}

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnTargetSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools::ChromeTargets> targets;
	NN<Net::ChromeDevTools::ChromeTarget> target;
	if (me->targets.SetTo(targets))
	{
		UOSInt i = me->lbTarget->GetSelectedIndex();
		if (i != INVALID_INDEX && targets->GetNewItem(i).SetTo(target))
		{
			me->txtTargetDescription->SetText(target->GetDescription()->ToCString());
			me->txtTargetDevToolsFrontendUrl->SetText(target->GetDevToolsFrontendUrl()->ToCString());
			me->txtTargetId->SetText(target->GetId()->ToCString());
			me->txtTargetTitle->SetText(target->GetTitle()->ToCString());
			me->txtTargetType->SetText(target->GetType()->ToCString());
			me->txtTargetURL->SetText(target->GetURL()->ToCString());
			me->txtTargetWebSocketDebuggerUrl->SetText(target->GetWebSocketDebuggerUrl()->ToCString());
			target.Delete();
		}
	}
}

void SSWR::AVIRead::AVIRChromeDevToolsForm::ReloadTargets()
{
	NN<Net::ChromeDevTools::ChromeTargets> targets;
	NN<Net::ChromeDevTools> devTools;
	NN<Net::ChromeDevTools::ChromeTarget> target;
	if (this->devTools.SetTo(devTools))
	{
		if (devTools->GetTargets().SetTo(targets))
		{
			this->targets.Delete();
			this->targets = targets;
			this->lbTarget->ClearItems();
			UOSInt i = 0;
			UOSInt j = targets->GetCount();
			while (i < j)
			{
				if (targets->GetNewItem(i).SetTo(target))
				{
					this->lbTarget->AddItem(target->GetTitle()->ToCString(), 0);
					target.Delete();
				}
				else
				{
					this->lbTarget->AddItem(CSTR("Target Missing"), 0);
				}
				i++;
			}
		}
	}
}

SSWR::AVIRead::AVIRChromeDevToolsForm::AVIRChromeDevToolsForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Chrome DevTools"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->clif = core->GetTCPClientFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->clif, false);
	this->devTools = 0;
	this->targets = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlPort = ui->NewPanel(*this);
	this->pnlPort->SetRect(0, 0, 100, 31, false);
	this->pnlPort->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPort = ui->NewLabel(this->pnlPort, CSTR("Port"));
	this->lblPort->SetRect(4, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlPort, CSTR("9222"));
	this->txtPort->SetRect(104, 4, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlPort, CSTR("Start"));
	this->btnStart->SetRect(204, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpVersion = this->tcMain->AddTabPage(CSTR("Version"));
	this->lblVersionBrowser = ui->NewLabel(this->tpVersion, CSTR("Browser"));
	this->lblVersionBrowser->SetRect(4, 4, 100, 23, false);
	this->txtVersionBrowser = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionBrowser->SetRect(104, 4, 300, 23, false);
	this->txtVersionBrowser->SetReadOnly(true);
	this->lblVersionProtocol = ui->NewLabel(this->tpVersion, CSTR("ProtocolVersion"));
	this->lblVersionProtocol->SetRect(4, 28, 100, 23, false);
	this->txtVersionProtocol = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionProtocol->SetRect(104, 28, 200, 23, false);
	this->txtVersionProtocol->SetReadOnly(true);
	this->lblVersionUserAgent = ui->NewLabel(this->tpVersion, CSTR("User-Agent"));
	this->lblVersionUserAgent->SetRect(4, 52, 100, 23, false);
	this->txtVersionUserAgent = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionUserAgent->SetRect(104, 52, 500, 23, false);
	this->txtVersionUserAgent->SetReadOnly(true);
	this->lblVersionV8 = ui->NewLabel(this->tpVersion, CSTR("V8 Version"));
	this->lblVersionV8->SetRect(4, 76, 100, 23, false);
	this->txtVersionV8 = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionV8->SetRect(104, 76, 200, 23, false);
	this->txtVersionV8->SetReadOnly(true);
	this->lblVersionWebKit = ui->NewLabel(this->tpVersion, CSTR("WebKit Version"));
	this->lblVersionWebKit->SetRect(4, 100, 100, 23, false);
	this->txtVersionWebKit = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionWebKit->SetRect(104, 100, 200, 23, false);
	this->txtVersionWebKit->SetReadOnly(true);
	this->lblVersionWSDebuggerUrl = ui->NewLabel(this->tpVersion, CSTR("WS Debugger URL"));
	this->lblVersionWSDebuggerUrl->SetRect(4, 124, 100, 23, false);
	this->txtVersionWSDebuggerUrl = ui->NewTextBox(this->tpVersion, CSTR(""));
	this->txtVersionWSDebuggerUrl->SetRect(104, 124, 500, 23, false);
	this->txtVersionWSDebuggerUrl->SetReadOnly(true);

	this->tpTarget = this->tcMain->AddTabPage(CSTR("Target"));
	this->pnlTarget = ui->NewPanel(this->tpTarget);
	this->pnlTarget->SetRect(0, 0, 100, 31, false);
	this->pnlTarget->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnTarget = ui->NewButton(this->pnlTarget, CSTR("Refresh"));
	this->btnTarget->SetRect(4, 4, 75, 23, false);
	this->btnTarget->HandleButtonClick(OnTargetClicked, this);
	this->lbTarget = ui->NewListBox(this->tpTarget, false);
	this->lbTarget->SetRect(0, 0, 200, 23, false);
	this->lbTarget->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTarget->HandleSelectionChange(OnTargetSelChg, this);
	this->pnlTargetDetail = ui->NewPanel(this->tpTarget);
	this->pnlTargetDetail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblTargetDescription = ui->NewLabel(this->pnlTargetDetail, CSTR("Description"));
	this->lblTargetDescription->SetRect(4, 4, 100, 23, false);
	this->txtTargetDescription = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetDescription->SetRect(104, 4, 500, 23, false);
	this->txtTargetDescription->SetReadOnly(true);
	this->lblTargetDevToolsFrontendUrl = ui->NewLabel(this->pnlTargetDetail, CSTR("DevTools Frontend Url"));
	this->lblTargetDevToolsFrontendUrl->SetRect(4, 28, 100, 23, false);
	this->txtTargetDevToolsFrontendUrl = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetDevToolsFrontendUrl->SetRect(104, 28, 500, 23, false);
	this->txtTargetDevToolsFrontendUrl->SetReadOnly(true);
	this->lblTargetId = ui->NewLabel(this->pnlTargetDetail, CSTR("Id"));
	this->lblTargetId->SetRect(4, 52, 100, 23, false);
	this->txtTargetId = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetId->SetRect(104, 52, 500, 23, false);
	this->txtTargetId->SetReadOnly(true);
	this->lblTargetTitle = ui->NewLabel(this->pnlTargetDetail, CSTR("Title"));
	this->lblTargetTitle->SetRect(4, 76, 100, 23, false);
	this->txtTargetTitle = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetTitle->SetRect(104, 76, 300, 23, false);
	this->txtTargetTitle->SetReadOnly(true);
	this->lblTargetType = ui->NewLabel(this->pnlTargetDetail, CSTR("Type"));
	this->lblTargetType->SetRect(4, 100, 100, 23, false);
	this->txtTargetType = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetType->SetRect(104, 100, 200, 23, false);
	this->txtTargetType->SetReadOnly(true);
	this->lblTargetURL = ui->NewLabel(this->pnlTargetDetail, CSTR("URL"));
	this->lblTargetURL->SetRect(4, 124, 100, 23, false);
	this->txtTargetURL = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetURL->SetRect(104, 124, 500, 23, false);
	this->txtTargetURL->SetReadOnly(true);
	this->lblTargetWebSocketDebuggerUrl = ui->NewLabel(this->pnlTargetDetail, CSTR("WS Debugger URL"));
	this->lblTargetWebSocketDebuggerUrl->SetRect(4, 148, 100, 23, false);
	this->txtTargetWebSocketDebuggerUrl = ui->NewTextBox(this->pnlTargetDetail, CSTR(""));
	this->txtTargetWebSocketDebuggerUrl->SetRect(104, 148, 500, 23, false);
	this->txtTargetWebSocketDebuggerUrl->SetReadOnly(true);
}

SSWR::AVIRead::AVIRChromeDevToolsForm::~AVIRChromeDevToolsForm()
{
	this->devTools.Delete();
	this->ssl.Delete();
	this->targets.Delete();
}

void SSWR::AVIRead::AVIRChromeDevToolsForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
