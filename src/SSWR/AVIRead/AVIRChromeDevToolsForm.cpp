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
		me->ReloadProtocol();
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

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnProtocolSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools::ChromeDomain> domain;
	NN<const Data::ArrayListStringNN> dependencies;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeCommand>> commands;
	NN<Net::ChromeDevTools::ChromeCommand> command;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeEvent>> events;
	NN<Net::ChromeDevTools::ChromeEvent> evt;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeType>> types;
	NN<Net::ChromeDevTools::ChromeType> type;
	UOSInt i;
	UOSInt j;
	me->lbProtocolDependencies->ClearItems();
	me->lbProtocolCommands->ClearItems();
	me->lbProtocolEvents->ClearItems();
	me->lbProtocolTypes->ClearItems();
	if (me->lbProtocol->GetSelectedItem().GetOpt<Net::ChromeDevTools::ChromeDomain>().SetTo(domain))
	{
		me->txtProtocolDomain->SetText(domain->GetDomain()->ToCString());
		me->txtProtocolDescription->SetText(Text::String::OrEmpty(domain->GetDescription())->ToCString());
		me->chkProtocolDeprecated->SetChecked(domain->IsDeprecated());
		me->chkProtocolExperimental->SetChecked(domain->IsExperimental());
		if (domain->GetDependencies().SetTo(dependencies))
		{
			i = 0;
			j = dependencies->GetCount();
			while (i < j)
			{
				me->lbProtocolDependencies->AddItem(dependencies->GetItemNoCheck(i), 0);
				i++;
			}
		}
		if (domain->GetCommands().SetTo(commands))
		{
			i = 0;
			j = commands->GetCount();
			while (i < j)
			{
				command = commands->GetItemNoCheck(i);
				me->lbProtocolCommands->AddItem(command->GetName(), command);
				i++;
			}
		}
		if (domain->GetEvents().SetTo(events))
		{
			i = 0;
			j = events->GetCount();
			while (i < j)
			{
				evt = events->GetItemNoCheck(i);
				me->lbProtocolEvents->AddItem(evt->GetName(), evt);
				i++;
			}
		}
		if (domain->GetTypes().SetTo(types))
		{
			i = 0;
			j = types->GetCount();
			while (i < j)
			{
				type = types->GetItemNoCheck(i);
				me->lbProtocolTypes->AddItem(type->GetId(), type);
				i++;
			}
		}
	}
	else
	{
		me->txtProtocolDomain->SetText(CSTR(""));
		me->txtProtocolDescription->SetText(CSTR(""));
	}
	OnProtocolCommandsSelChg(userObj);
	OnProtocolEventsSelChg(userObj);
}

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnProtocolCommandsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools::ChromeCommand> command;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeParameter>> params;
	me->lvProtocolCommandsParams->ClearItems();
	me->lvProtocolCommandsRets->ClearItems();
	if (me->lbProtocolCommands->GetSelectedItem().GetOpt<Net::ChromeDevTools::ChromeCommand>().SetTo(command))
	{
		me->txtProtocolCommandsName->SetText(command->GetName()->ToCString());
		me->txtProtocolCommandsDesc->SetText(Text::String::OrEmpty(command->GetDescription())->ToCString());
		me->chkProtocolCommandsDeprecated->SetChecked(command->IsDeprecated());
		me->chkProtocolCommandsExperimental->SetChecked(command->IsExperimental());
		if (command->GetParameters().SetTo(params))
		{
			AppendParameters(me->lvProtocolCommandsParams, params);
		}
		if (command->GetReturns().SetTo(params))
		{
			AppendParameters(me->lvProtocolCommandsRets, params);
		}
		Text::StringBuilderUTF8 sb;
		command->ToJSONWF(sb);
		me->txtProtocolCommandJSON->SetText(sb.ToCString());
		sb.ClearStr();
		command->ToString(sb);
		me->txtProtocolCommandText->SetText(sb.ToCString());
	}
	else
	{
		me->txtProtocolCommandsName->SetText(CSTR(""));
		me->txtProtocolCommandsDesc->SetText(CSTR(""));
		me->txtProtocolCommandJSON->SetText(CSTR(""));
		me->txtProtocolCommandText->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnProtocolEventsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools::ChromeEvent> evt;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeParameter>> params;
	me->lvProtocolEventsParams->ClearItems();
	if (me->lbProtocolEvents->GetSelectedItem().GetOpt<Net::ChromeDevTools::ChromeEvent>().SetTo(evt))
	{
		me->txtProtocolEventsName->SetText(evt->GetName()->ToCString());
		me->txtProtocolEventsDesc->SetText(evt->GetDescription()->ToCString());
		me->chkProtocolEventsDeprecated->SetChecked(evt->IsDeprecated());
		me->chkProtocolEventsExperimental->SetChecked(evt->IsExperimental());
		if (evt->GetParameters().SetTo(params))
		{
			AppendParameters(me->lvProtocolCommandsParams, params);
		}
	}
	else
	{
		me->txtProtocolEventsName->SetText(CSTR(""));
		me->txtProtocolEventsDesc->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRChromeDevToolsForm::OnProtocolTypesSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRChromeDevToolsForm> me = userObj.GetNN<SSWR::AVIRead::AVIRChromeDevToolsForm>();
	NN<Net::ChromeDevTools::ChromeType> type;
	NN<Net::ChromeDevTools::ChromeReturnItem> items;
	NN<const Data::ArrayListStringNN> enums;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeParameter>> params;
	me->lvProtocolTypesProp->ClearItems();
	me->lbProtocolTypesEnum->ClearItems();
	if (me->lbProtocolTypes->GetSelectedItem().GetOpt<Net::ChromeDevTools::ChromeType>().SetTo(type))
	{
		me->txtProtocolTypesId->SetText(type->GetId()->ToCString());
		me->txtProtocolTypesDesc->SetText(Text::String::OrEmpty(type->GetDescription())->ToCString());
		me->txtProtocolTypesType->SetText(type->GetType()->ToCString());
		if (type->GetItems().SetTo(items))
		{
			NN<Text::String> s;
			if (items->GetType().SetTo(s))
				me->txtProtocolTypesItemsType->SetText(s->ToCString());
			else if (items->GetRef().SetTo(s))
				me->txtProtocolTypesItemsType->SetText(s->ToCString());
			else
				me->txtProtocolTypesItemsType->SetText(CSTR(""));
		}
		else
		{
			me->txtProtocolTypesItemsType->SetText(CSTR(""));
		}
		me->chkProtocolTypesDeprecated->SetChecked(type->IsDeprecated());
		me->chkProtocolTypesExperimental->SetChecked(type->IsExperimental());
		if (type->GetProperties().SetTo(params))
		{
			AppendParameters(me->lvProtocolTypesProp, params);
		}
		if (type->GetEnum().SetTo(enums))
		{
			UOSInt i = 0;
			UOSInt j = enums->GetCount();
			while (i < j)
			{
				me->lbProtocolTypesEnum->AddItem(enums->GetItemNoCheck(i), 0);
				i++;
			}
		}
		Text::StringBuilderUTF8 sb;
		type->ToJSONWF(sb);
		me->txtProtocolTypeJSON->SetText(sb.ToCString());
		sb.ClearStr();
		type->ToString(sb);
		me->txtProtocolTypeText->SetText(sb.ToCString());
	}
	else
	{
		me->txtProtocolTypesId->SetText(CSTR(""));
		me->txtProtocolTypesType->SetText(CSTR(""));
		me->txtProtocolTypesDesc->SetText(CSTR(""));
		me->txtProtocolTypesItemsType->SetText(CSTR(""));
		me->txtProtocolTypeJSON->SetText(CSTR(""));
		me->txtProtocolTypeText->SetText(CSTR(""));
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

void SSWR::AVIRead::AVIRChromeDevToolsForm::ReloadProtocol()
{
	NN<Net::ChromeDevTools::ChromeProtocol> protocol;
	NN<Net::ChromeDevTools> devTools;
	NN<Net::ChromeDevTools::ChromeProtocolVersion> ver;
	NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeDomain>> domains;
	NN<Net::ChromeDevTools::ChromeDomain> domain;
	UOSInt i;
	UOSInt j;
	if (this->devTools.SetTo(devTools))
	{
		if (devTools->GetProtocol().SetTo(protocol))
		{
			if (protocol->GetVersion().SetTo(ver))
			{
				this->txtProtocolVerMajor->SetText(ver->GetMajor()->ToCString());
				this->txtProtocolVerMinor->SetText(ver->GetMinor()->ToCString());
			}
			else
			{
				this->txtProtocolVerMajor->SetText(CSTR("-"));
				this->txtProtocolVerMinor->SetText(CSTR("-"));
			}
			this->lbProtocol->ClearItems();
			if (protocol->GetDomains().SetTo(domains))
			{
				i = 0;
				j = domains->GetCount();
				while (i < j)
				{
					domain = domains->GetItemNoCheck(i);
					this->lbProtocol->AddItem(domain->GetDomain()->ToCString(), domain);
					i++;
				}
			}
			this->protocol.Delete();
			this->protocol = protocol;
		}
	}
}

void SSWR::AVIRead::AVIRChromeDevToolsForm::AppendParameters(NN<UI::GUIListView> lv, NN<const Data::ArrayListNN<Net::ChromeDevTools::ChromeParameter>> params)
{
	UOSInt i = 0;
	UOSInt j = params->GetCount();
	while (i < j)
	{
		AppendParameter(lv, params->GetItemNoCheck(i));
		i++;
	}
}

void SSWR::AVIRead::AVIRChromeDevToolsForm::AppendParameter(NN<UI::GUIListView> lv, NN<Net::ChromeDevTools::ChromeParameter> param)
{
	NN<Text::String> s;
	NN<const Data::ArrayListStringNN> enums;
	NN<Net::ChromeDevTools::ChromeReturnItem> item;
	UOSInt i = lv->AddItem(param->GetName()->ToCString(), param);
	if (param->GetType().SetTo(s)) lv->SetSubItem(i, 1, s->ToCString());
	else if (param->GetRef().SetTo(s)) lv->SetSubItem(i, 1, s->ToCString());
	if (param->GetItems().SetTo(item))
	{
		if (item->GetType().SetTo(s)) lv->SetSubItem(i, 2, s->ToCString());
		else if (item->GetRef().SetTo(s)) lv->SetSubItem(i, 2, s->ToCString());
	}
	lv->SetSubItem(i, 3, param->IsOptional()?CSTR("Y"):CSTR("N"));
	if (param->GetEnum().SetTo(enums))
	{
		s = enums->JoinString(CSTR("\r\n"));
		lv->SetSubItem(i, 4, s->ToCString());
		s->Release();
	}
	if (param->GetDescription().SetTo(s)) lv->SetSubItem(i, 5, s->ToCString());
	lv->SetSubItem(i, 6, param->IsExperimental()?CSTR("Y"):CSTR("N"));
	lv->SetSubItem(i, 7, param->IsDeprecated()?CSTR("Y"):CSTR("N"));
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
	this->protocol = 0;
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

	this->tpProtocol = this->tcMain->AddTabPage(CSTR("Protocol"));
	this->pnlProtocol = ui->NewPanel(this->tpProtocol);
	this->pnlProtocol->SetRect(0, 0, 100, 55, false);
	this->pnlProtocol->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProtocolVerMajor = ui->NewLabel(this->pnlProtocol, CSTR("Version Major"));
	this->lblProtocolVerMajor->SetRect(4, 4, 100, 23, false);
	this->txtProtocolVerMajor = ui->NewTextBox(this->pnlProtocol, CSTR(""));
	this->txtProtocolVerMajor->SetRect(104, 4, 100, 23, false);
	this->txtProtocolVerMajor->SetReadOnly(true);
	this->lblProtocolVerMinor = ui->NewLabel(this->pnlProtocol, CSTR("Version Minor"));
	this->lblProtocolVerMinor->SetRect(4, 28, 100, 23, false);
	this->txtProtocolVerMinor = ui->NewTextBox(this->pnlProtocol, CSTR(""));
	this->txtProtocolVerMinor->SetRect(104, 28, 100, 23, false);
	this->txtProtocolVerMinor->SetReadOnly(true);
	this->lbProtocol = ui->NewListBox(this->tpProtocol, false);
	this->lbProtocol->SetRect(0, 0, 150, 23, false);
	this->lbProtocol->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProtocol->HandleSelectionChange(OnProtocolSelChg, this);
	this->hspProtocol = ui->NewHSplitter(this->tpProtocol, 3, false);
	this->pnlProtocolDomain = ui->NewPanel(this->tpProtocol);
	this->pnlProtocolDomain->SetRect(0, 0, 100, 79, false);
	this->pnlProtocolDomain->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProtocolDomain = ui->NewLabel(this->pnlProtocolDomain, CSTR("Domain"));
	this->lblProtocolDomain->SetRect(4, 4, 100, 23, false);
	this->txtProtocolDomain = ui->NewTextBox(this->pnlProtocolDomain, CSTR(""));
	this->txtProtocolDomain->SetRect(104, 4, 200, 23, false);
	this->txtProtocolDomain->SetReadOnly(true);
	this->lblProtocolDescription = ui->NewLabel(this->pnlProtocolDomain, CSTR("Description"));
	this->lblProtocolDescription->SetRect(4, 28, 100, 23, false);
	this->txtProtocolDescription = ui->NewTextBox(this->pnlProtocolDomain, CSTR(""));
	this->txtProtocolDescription->SetRect(104, 28, 500, 23, false);
	this->txtProtocolDescription->SetReadOnly(true);
	this->chkProtocolDeprecated = ui->NewCheckBox(this->pnlProtocolDomain, CSTR("Deprecated"), false);
	this->chkProtocolDeprecated->SetRect(4, 52, 200, 23, false);
	this->chkProtocolDeprecated->SetEnabled(false);
	this->chkProtocolExperimental = ui->NewCheckBox(this->pnlProtocolDomain, CSTR("Experimental"), false);
	this->chkProtocolExperimental->SetRect(204, 52, 200, 23, false);
	this->chkProtocolExperimental->SetEnabled(false);
	this->tcProtocol = ui->NewTabControl(this->tpProtocol);
	this->tcProtocol->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProtocolCommands = this->tcProtocol->AddTabPage(CSTR("Commands"));
	this->lbProtocolCommands = ui->NewListBox(this->tpProtocolCommands, false);
	this->lbProtocolCommands->SetRect(0, 0, 150, 23, false);
	this->lbProtocolCommands->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProtocolCommands->HandleSelectionChange(OnProtocolCommandsSelChg, this);
	this->hspProtocolCommands = ui->NewHSplitter(this->tpProtocolCommands, 3, false);
	this->tcProtocolCommands = ui->NewTabControl(this->tpProtocolCommands);
	this->tcProtocolCommands->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpProtocolCommandDetail = this->tcProtocolCommands->AddTabPage(CSTR("Detail"));
	this->pnlProtocolCommands = ui->NewPanel(this->tpProtocolCommandDetail);
	this->pnlProtocolCommands->SetRect(0, 0, 100, 79, false);
	this->pnlProtocolCommands->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProtocolCommandsName = ui->NewLabel(this->pnlProtocolCommands, CSTR("Name"));
	this->lblProtocolCommandsName->SetRect(4, 4, 100, 23, false);
	this->txtProtocolCommandsName = ui->NewTextBox(this->pnlProtocolCommands, CSTR(""));
	this->txtProtocolCommandsName->SetRect(104, 4, 200, 23, false);
	this->txtProtocolCommandsName->SetReadOnly(true);
	this->lblProtocolCommandsDesc = ui->NewLabel(this->pnlProtocolCommands, CSTR("Description"));
	this->lblProtocolCommandsDesc->SetRect(4, 28, 100, 23, false);
	this->txtProtocolCommandsDesc = ui->NewTextBox(this->pnlProtocolCommands, CSTR(""));
	this->txtProtocolCommandsDesc->SetRect(104, 28, 500, 23, false);
	this->txtProtocolCommandsDesc->SetReadOnly(true);
	this->chkProtocolCommandsDeprecated = ui->NewCheckBox(this->pnlProtocolCommands, CSTR("Deprecated"), false);
	this->chkProtocolCommandsDeprecated->SetRect(4, 52, 200, 23, false);
	this->chkProtocolCommandsDeprecated->SetEnabled(false);
	this->chkProtocolCommandsExperimental = ui->NewCheckBox(this->pnlProtocolCommands, CSTR("Experimental"), false);
	this->chkProtocolCommandsExperimental->SetRect(204, 52, 200, 23, false);
	this->chkProtocolCommandsExperimental->SetEnabled(false);
	this->grpProtocolCommandsParams = ui->NewGroupBox(this->tpProtocolCommandDetail, CSTR("Parameters"));
	this->grpProtocolCommandsParams->SetRect(0, 0, 100, 256, false);
	this->grpProtocolCommandsParams->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvProtocolCommandsParams = ui->NewListView(this->grpProtocolCommandsParams, UI::ListViewStyle::Table, 8);
	this->lvProtocolCommandsParams->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolCommandsParams->SetFullRowSelect(true);
	this->lvProtocolCommandsParams->SetShowGrid(true);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Name"), 100);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Type"), 80);
	this->lvProtocolCommandsParams->AddColumn(CSTR("ItemsType"), 80);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Optional"), 80);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Enum"), 150);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Description"), 300);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Experimental"), 80);
	this->lvProtocolCommandsParams->AddColumn(CSTR("Deprecated"), 80);
	this->vspProtocolCommands = ui->NewVSplitter(this->tpProtocolCommandDetail, 3, false);
	this->grpProtocolCommandsRets = ui->NewGroupBox(this->tpProtocolCommandDetail, CSTR("Returns"));
	this->grpProtocolCommandsRets->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolCommandsRets = ui->NewListView(this->grpProtocolCommandsRets, UI::ListViewStyle::Table, 8);
	this->lvProtocolCommandsRets->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolCommandsRets->SetFullRowSelect(true);
	this->lvProtocolCommandsRets->SetShowGrid(true);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Name"), 100);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Type"), 80);
	this->lvProtocolCommandsRets->AddColumn(CSTR("ItemsType"), 80);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Optional"), 80);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Enum"), 150);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Description"), 300);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Experimental"), 80);
	this->lvProtocolCommandsRets->AddColumn(CSTR("Deprecated"), 80);
	this->tpProtocolCommandJSON = this->tcProtocolCommands->AddTabPage(CSTR("JSON"));
	this->txtProtocolCommandJSON = ui->NewTextBox(this->tpProtocolCommandJSON, CSTR(""), true);
	this->txtProtocolCommandJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtProtocolCommandJSON->SetReadOnly(true);
	this->tpProtocolCommandText = this->tcProtocolCommands->AddTabPage(CSTR("Text"));
	this->txtProtocolCommandText = ui->NewTextBox(this->tpProtocolCommandText, CSTR(""), true);
	this->txtProtocolCommandText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtProtocolCommandText->SetReadOnly(true);

	this->tpProtocolDependencies = this->tcProtocol->AddTabPage(CSTR("Dependencies"));
	this->lbProtocolDependencies = ui->NewListBox(this->tpProtocolDependencies, false);
	this->lbProtocolDependencies->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProtocolEvents = this->tcProtocol->AddTabPage(CSTR("Events"));
	this->lbProtocolEvents = ui->NewListBox(this->tpProtocolEvents, false);
	this->lbProtocolEvents->SetRect(0, 0, 150, 23, false);
	this->lbProtocolEvents->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProtocolEvents->HandleSelectionChange(OnProtocolEventsSelChg, this);
	this->hspProtocolEvents = ui->NewHSplitter(this->tpProtocolEvents, 3, false);
	this->pnlProtocolEvents = ui->NewPanel(this->tpProtocolEvents);
	this->pnlProtocolEvents->SetRect(0, 0, 100, 79, false);
	this->pnlProtocolEvents->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProtocolEventsName = ui->NewLabel(this->pnlProtocolEvents, CSTR("Name"));
	this->lblProtocolEventsName->SetRect(4, 4, 100, 23, false);
	this->txtProtocolEventsName = ui->NewTextBox(this->pnlProtocolEvents, CSTR(""));
	this->txtProtocolEventsName->SetRect(104, 4, 200, 23, false);
	this->txtProtocolEventsName->SetReadOnly(true);
	this->lblProtocolEventsDesc = ui->NewLabel(this->pnlProtocolEvents, CSTR("Description"));
	this->lblProtocolEventsDesc->SetRect(4, 28, 100, 23, false);
	this->txtProtocolEventsDesc = ui->NewTextBox(this->pnlProtocolEvents, CSTR(""));
	this->txtProtocolEventsDesc->SetRect(104, 28, 500, 23, false);
	this->txtProtocolEventsDesc->SetReadOnly(true);
	this->chkProtocolEventsDeprecated = ui->NewCheckBox(this->pnlProtocolEvents, CSTR("Deprecated"), false);
	this->chkProtocolEventsDeprecated->SetRect(4, 52, 200, 23, false);
	this->chkProtocolEventsDeprecated->SetEnabled(false);
	this->chkProtocolEventsExperimental = ui->NewCheckBox(this->pnlProtocolEvents, CSTR("Experimental"), false);
	this->chkProtocolEventsExperimental->SetRect(204, 52, 200, 23, false);
	this->chkProtocolEventsExperimental->SetEnabled(false);
	this->lvProtocolEventsParams = ui->NewListView(this->tpProtocolEvents, UI::ListViewStyle::Table, 8);
	this->lvProtocolEventsParams->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolEventsParams->SetFullRowSelect(true);
	this->lvProtocolEventsParams->SetShowGrid(true);
	this->lvProtocolEventsParams->AddColumn(CSTR("Name"), 100);
	this->lvProtocolEventsParams->AddColumn(CSTR("Type"), 80);
	this->lvProtocolEventsParams->AddColumn(CSTR("ItemsType"), 80);
	this->lvProtocolEventsParams->AddColumn(CSTR("Optional"), 80);
	this->lvProtocolEventsParams->AddColumn(CSTR("Enum"), 150);
	this->lvProtocolEventsParams->AddColumn(CSTR("Description"), 300);
	this->lvProtocolEventsParams->AddColumn(CSTR("Experimental"), 80);
	this->lvProtocolEventsParams->AddColumn(CSTR("Deprecated"), 80);

	this->tpProtocolTypes = this->tcProtocol->AddTabPage(CSTR("Types"));
	this->lbProtocolTypes = ui->NewListBox(this->tpProtocolTypes, false);
	this->lbProtocolTypes->SetRect(0, 0, 150, 23, false);
	this->lbProtocolTypes->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProtocolTypes->HandleSelectionChange(OnProtocolTypesSelChg, this);
	this->hspProtocolTypes = ui->NewHSplitter(this->tpProtocolTypes, 3, false);
	this->tcProtocolTypes = ui->NewTabControl(this->tpProtocolTypes);
	this->tcProtocolTypes->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpProtocolTypeDetail = this->tcProtocolTypes->AddTabPage(CSTR("Detail"));
	this->pnlProtocolTypes = ui->NewPanel(this->tpProtocolTypeDetail);
	this->pnlProtocolTypes->SetRect(0, 0, 100, 127, false);
	this->pnlProtocolTypes->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProtocolTypesId = ui->NewLabel(this->pnlProtocolTypes, CSTR("Id"));
	this->lblProtocolTypesId->SetRect(4, 4, 100, 23, false);
	this->txtProtocolTypesId = ui->NewTextBox(this->pnlProtocolTypes, CSTR(""));
	this->txtProtocolTypesId->SetRect(104, 4, 200, 23, false);
	this->txtProtocolTypesId->SetReadOnly(true);
	this->lblProtocolTypesType = ui->NewLabel(this->pnlProtocolTypes, CSTR("Type"));
	this->lblProtocolTypesType->SetRect(4, 28, 100, 23, false);
	this->txtProtocolTypesType = ui->NewTextBox(this->pnlProtocolTypes, CSTR(""));
	this->txtProtocolTypesType->SetRect(104, 28, 200, 23, false);
	this->txtProtocolTypesType->SetReadOnly(true);
	this->lblProtocolTypesItemsType = ui->NewLabel(this->pnlProtocolTypes, CSTR("ItemsType"));
	this->lblProtocolTypesItemsType->SetRect(4, 52, 100, 23, false);
	this->txtProtocolTypesItemsType = ui->NewTextBox(this->pnlProtocolTypes, CSTR(""));
	this->txtProtocolTypesItemsType->SetRect(104, 52, 150, 23, false);
	this->txtProtocolTypesItemsType->SetReadOnly(true);
	this->lblProtocolTypesDesc = ui->NewLabel(this->pnlProtocolTypes, CSTR("Description"));
	this->lblProtocolTypesDesc->SetRect(4, 76, 100, 23, false);
	this->txtProtocolTypesDesc = ui->NewTextBox(this->pnlProtocolTypes, CSTR(""));
	this->txtProtocolTypesDesc->SetRect(104, 76, 500, 23, false);
	this->txtProtocolTypesDesc->SetReadOnly(true);
	this->chkProtocolTypesDeprecated = ui->NewCheckBox(this->pnlProtocolTypes, CSTR("Deprecated"), false);
	this->chkProtocolTypesDeprecated->SetRect(4, 100, 200, 23, false);
	this->chkProtocolTypesDeprecated->SetEnabled(false);
	this->chkProtocolTypesExperimental = ui->NewCheckBox(this->pnlProtocolTypes, CSTR("Experimental"), false);
	this->chkProtocolTypesExperimental->SetRect(204, 100, 200, 23, false);
	this->chkProtocolTypesExperimental->SetEnabled(false);
	this->grpProtocolTypesEnum = ui->NewGroupBox(this->tpProtocolTypeDetail, CSTR("Enum"));
	this->grpProtocolTypesEnum->SetRect(0, 0, 100, 256, false);
	this->grpProtocolTypesEnum->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbProtocolTypesEnum = ui->NewListBox(this->grpProtocolTypesEnum, false);
	this->lbProtocolTypesEnum->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspProtocolTypes = ui->NewVSplitter(this->tpProtocolTypeDetail, 3, false);
	this->grpProtocolTypesProp = ui->NewGroupBox(this->tpProtocolTypeDetail, CSTR("Properties"));
	this->grpProtocolTypesProp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolTypesProp = ui->NewListView(this->grpProtocolTypesProp, UI::ListViewStyle::Table, 8);
	this->lvProtocolTypesProp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProtocolTypesProp->SetFullRowSelect(true);
	this->lvProtocolTypesProp->SetShowGrid(true);
	this->lvProtocolTypesProp->AddColumn(CSTR("Name"), 100);
	this->lvProtocolTypesProp->AddColumn(CSTR("Type"), 80);
	this->lvProtocolTypesProp->AddColumn(CSTR("ItemsType"), 80);
	this->lvProtocolTypesProp->AddColumn(CSTR("Optional"), 80);
	this->lvProtocolTypesProp->AddColumn(CSTR("Enum"), 150);
	this->lvProtocolTypesProp->AddColumn(CSTR("Description"), 300);
	this->lvProtocolTypesProp->AddColumn(CSTR("Experimental"), 80);
	this->lvProtocolTypesProp->AddColumn(CSTR("Deprecated"), 80);
	this->tpProtocolTypeJSON = this->tcProtocolTypes->AddTabPage(CSTR("JSON"));
	this->txtProtocolTypeJSON = ui->NewTextBox(this->tpProtocolTypeJSON, CSTR(""), true);
	this->txtProtocolTypeJSON->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtProtocolTypeJSON->SetReadOnly(true);
	this->tpProtocolTypeText = this->tcProtocolTypes->AddTabPage(CSTR("Text"));
	this->txtProtocolTypeText = ui->NewTextBox(this->tpProtocolTypeText, CSTR(""), true);
	this->txtProtocolTypeText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtProtocolTypeText->SetReadOnly(true);
}

SSWR::AVIRead::AVIRChromeDevToolsForm::~AVIRChromeDevToolsForm()
{
	this->devTools.Delete();
	this->ssl.Delete();
	this->targets.Delete();
	this->protocol.Delete();
}

void SSWR::AVIRead::AVIRChromeDevToolsForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
