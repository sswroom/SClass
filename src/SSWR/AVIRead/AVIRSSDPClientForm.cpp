#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSSDPClientForm.h"
#include "Text/StringTool.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSSDPClientForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSSDPClientForm *me = (SSWR::AVIRead::AVIRSSDPClientForm*)userObj;
	Sync::MutexUsage mutUsage;
	Data::ArrayList<Net::SSDPClient::SSDPDevice*> *devList = me->ssdp->GetDevices(&mutUsage);
	UInt8 sbuff[128];
	if (devList->GetCount() != me->lbDevice->GetCount())
	{
		UOSInt i = 0;
		UOSInt j = devList->GetCount();
		Net::SSDPClient::SSDPDevice *dev;
		me->lbDevice->ClearItems();
		while (i < j)
		{
			dev = devList->GetItem(i);
			Net::SocketUtil::GetAddrName(sbuff, &dev->addr);
			me->lbDevice->AddItem(sbuff, dev);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSSDPClientForm::OnDeviceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSSDPClientForm *me = (SSWR::AVIRead::AVIRSSDPClientForm*)userObj;
	Sync::MutexUsage mutUsage;
	me->ssdp->GetDevices(&mutUsage);
	Net::SSDPClient::SSDPDevice *dev = (Net::SSDPClient::SSDPDevice*)me->lbDevice->GetSelectedItem();
	me->lbService->ClearItems();
	if (dev == 0)
	{
		return;
	}
	Net::SSDPClient::SSDPService *svc;
	UOSInt i = 0;
	UOSInt j = dev->services->GetCount();
	while (i < j)
	{
		svc = dev->services->GetItem(i);
		me->lbService->AddItem(svc->st, svc);
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSSDPClientForm::OnServiceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSSDPClientForm *me = (SSWR::AVIRead::AVIRSSDPClientForm*)userObj;
	Sync::MutexUsage mutUsage;
	me->ssdp->GetDevices(&mutUsage);
	Net::SSDPClient::SSDPService *svc = (Net::SSDPClient::SSDPService*)me->lbService->GetSelectedItem();
	if (svc)
	{
		UTF8Char sbuff[128];
		me->txtLocation->SetText(Text::StringTool::Null2Empty(svc->location));
		if (svc->time)
		{
			Data::DateTime dt;
			dt.SetTicks(svc->time);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
			me->txtDate->SetText(sbuff);
		}
		else
		{
			me->txtDate->SetText((const UTF8Char*)"-");
		}
		me->txtUSN->SetText(Text::StringTool::Null2Empty(svc->usn));
		me->txtST->SetText(Text::StringTool::Null2Empty(svc->st));
		me->txtServer->SetText(Text::StringTool::Null2Empty(svc->server));
		me->txtOpt->SetText(Text::StringTool::Null2Empty(svc->opt));
		me->txtUserAgent->SetText(Text::StringTool::Null2Empty(svc->userAgent));

		if (svc->location)
		{
			Net::SSDPClient::SSDPRoot *root = me->rootMap->Get(svc->location);
			if (root == 0)
			{
				Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(me->sockf, me->ssl, svc->location, "GET", true);
				if (cli == 0)
				{
					root = MemAlloc(Net::SSDPClient::SSDPRoot, 1);
					MemClear(root, sizeof(Net::SSDPClient::SSDPRoot));
				}
				else
				{
					root = Net::SSDPClient::SSDPRootParse(me->core->GetEncFactory(), cli);
					DEL_CLASS(cli);
				}
				me->rootMap->Put(svc->location, root);
			}

			me->txtUDN->SetText(Text::StringTool::Null2Empty(root->udn));
			me->txtFriendlyName->SetText(Text::StringTool::Null2Empty(root->friendlyName));
			me->txtManufacturer->SetText(Text::StringTool::Null2Empty(root->manufacturer));
			me->txtManufacturerURL->SetText(Text::StringTool::Null2Empty(root->manufacturerURL));
			me->txtModelName->SetText(Text::StringTool::Null2Empty(root->modelName));
			me->txtModelNumber->SetText(Text::StringTool::Null2Empty(root->modelNumber));
			me->txtModelURL->SetText(Text::StringTool::Null2Empty(root->modelURL));
			me->txtSerialNumber->SetText(Text::StringTool::Null2Empty(root->serialNumber));
			me->txtPresentationURL->SetText(Text::StringTool::Null2Empty(root->presentationURL));
			me->txtDeviceType->SetText(Text::StringTool::Null2Empty(root->deviceType));
			me->txtDeviceURL->SetText(Text::StringTool::Null2Empty(root->deviceURL));
		}
		else
		{
			me->txtUDN->SetText((const UTF8Char*)"");
			me->txtFriendlyName->SetText((const UTF8Char*)"");
			me->txtManufacturer->SetText((const UTF8Char*)"");
			me->txtManufacturerURL->SetText((const UTF8Char*)"");
			me->txtModelName->SetText((const UTF8Char*)"");
			me->txtModelNumber->SetText((const UTF8Char*)"");
			me->txtModelURL->SetText((const UTF8Char*)"");
			me->txtSerialNumber->SetText((const UTF8Char*)"");
			me->txtPresentationURL->SetText((const UTF8Char*)"");
			me->txtDeviceType->SetText((const UTF8Char*)"");
			me->txtDeviceURL->SetText((const UTF8Char*)"");
		}
	}
	else
	{
		me->txtLocation->SetText((const UTF8Char*)"");
		me->txtDate->SetText((const UTF8Char*)"");
		me->txtUSN->SetText((const UTF8Char*)"");
		me->txtST->SetText((const UTF8Char*)"");
		me->txtServer->SetText((const UTF8Char*)"");
		me->txtOpt->SetText((const UTF8Char*)"");
		me->txtUserAgent->SetText((const UTF8Char*)"");

		me->txtUDN->SetText((const UTF8Char*)"");
		me->txtFriendlyName->SetText((const UTF8Char*)"");
		me->txtManufacturer->SetText((const UTF8Char*)"");
		me->txtManufacturerURL->SetText((const UTF8Char*)"");
		me->txtModelName->SetText((const UTF8Char*)"");
		me->txtModelNumber->SetText((const UTF8Char*)"");
		me->txtModelURL->SetText((const UTF8Char*)"");
		me->txtSerialNumber->SetText((const UTF8Char*)"");
		me->txtPresentationURL->SetText((const UTF8Char*)"");
		me->txtDeviceType->SetText((const UTF8Char*)"");
		me->txtDeviceURL->SetText((const UTF8Char*)"");
	}
}

SSWR::AVIRead::AVIRSSDPClientForm::AVIRSSDPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"SSDP Client");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, false);
	NEW_CLASS(this->ssdp, Net::SSDPClient(this->sockf, 0));
	NEW_CLASS(this->rootMap, Data::StringUTF8Map<Net::SSDPClient::SSDPRoot*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, this, false));
	this->lbDevice->SetRect(0, 0, 120, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDeviceSelChg, this);
	NEW_CLASS(this->hspDevice, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->lbService, UI::GUIListBox(ui, this, false));
	this->lbService->SetRect(0, 0, 200, 23, false);
	this->lbService->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbService->HandleSelectionChange(OnServiceSelChg, this);
	NEW_CLASS(this->hspService, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlService, UI::GUIPanel(ui, this));
	this->pnlService->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblLocation, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Location"));
	this->lblLocation->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtLocation, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtLocation->SetRect(104, 4, 300, 23, false);
	this->txtLocation->SetReadOnly(true);
	NEW_CLASS(this->lblDate, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Date"));
	this->lblDate->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDate, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtDate->SetRect(104, 28, 300, 23, false);
	this->txtDate->SetReadOnly(true);
	NEW_CLASS(this->lblUSN, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"USN"));
	this->lblUSN->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUSN, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtUSN->SetRect(104, 52, 300, 23, false);
	this->txtUSN->SetReadOnly(true);
	NEW_CLASS(this->lblST, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"ST"));
	this->lblST->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtST, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtST->SetRect(104, 76, 300, 23, false);
	this->txtST->SetReadOnly(true);
	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Server"));
	this->lblServer->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtServer->SetRect(104, 100, 300, 23, false);
	this->txtServer->SetReadOnly(true);
	NEW_CLASS(this->lblOpt, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Opt"));
	this->lblOpt->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtOpt, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtOpt->SetRect(104, 124, 300, 23, false);
	this->txtOpt->SetReadOnly(true);
	NEW_CLASS(this->lblUserAgent, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"UserAgent"));
	this->lblUserAgent->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtUserAgent, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtUserAgent->SetRect(104, 148, 300, 23, false);
	this->txtUserAgent->SetReadOnly(true);

	NEW_CLASS(this->lblUDN, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"UDN"));
	this->lblUDN->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtUDN, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtUDN->SetRect(104, 196, 300, 23, false);
	this->txtUDN->SetReadOnly(true);
	NEW_CLASS(this->lblFriendlyName, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Friendly Name"));
	this->lblFriendlyName->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtFriendlyName, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtFriendlyName->SetRect(104, 220, 300, 23, false);
	this->txtFriendlyName->SetReadOnly(true);
	NEW_CLASS(this->lblManufacturer, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Manufacturer"));
	this->lblManufacturer->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtManufacturer, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtManufacturer->SetRect(104, 244, 300, 23, false);
	this->txtManufacturer->SetReadOnly(true);
	NEW_CLASS(this->lblManufacturerURL, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Manfacturer URL"));
	this->lblManufacturerURL->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtManufacturerURL, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtManufacturerURL->SetRect(104, 268, 300, 23, false);
	this->txtManufacturerURL->SetReadOnly(true);
	NEW_CLASS(this->lblModelName, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Model Name"));
	this->lblModelName->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtModelName, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtModelName->SetRect(104, 292, 300, 23, false);
	this->txtModelName->SetReadOnly(true);
	NEW_CLASS(this->lblModelNumber, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Moden Number"));
	this->lblModelNumber->SetRect(4, 316, 100, 23, false);
	NEW_CLASS(this->txtModelNumber, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtModelNumber->SetRect(104, 316, 300, 23, false);
	this->txtModelNumber->SetReadOnly(true);
	NEW_CLASS(this->lblModelURL, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Model URL"));
	this->lblModelURL->SetRect(4, 340, 100, 23, false);
	NEW_CLASS(this->txtModelURL, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtModelURL->SetRect(104, 340, 300, 23, false);
	this->txtModelURL->SetReadOnly(true);
	NEW_CLASS(this->lblSerialNumber, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Serial Number"));
	this->lblSerialNumber->SetRect(4, 364, 100, 23, false);
	NEW_CLASS(this->txtSerialNumber, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtSerialNumber->SetRect(104, 364, 300, 23, false);
	this->txtSerialNumber->SetReadOnly(true);
	NEW_CLASS(this->lblPresentationURL, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Presentation URL"));
	this->lblPresentationURL->SetRect(4, 388, 100, 23, false);
	NEW_CLASS(this->txtPresentationURL, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtPresentationURL->SetRect(104, 388, 300, 23, false);
	this->txtPresentationURL->SetReadOnly(true);
	NEW_CLASS(this->lblDeviceType, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Device Type"));
	this->lblDeviceType->SetRect(4, 412, 100, 23, false);
	NEW_CLASS(this->txtDeviceType, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtDeviceType->SetRect(104, 412, 300, 23, false);
	this->txtDeviceType->SetReadOnly(true);
	NEW_CLASS(this->lblDeviceURL, UI::GUILabel(ui, this->pnlService, (const UTF8Char*)"Device URL"));
	this->lblDeviceURL->SetRect(4, 436, 100, 23, false);
	NEW_CLASS(this->txtDeviceURL, UI::GUITextBox(ui, this->pnlService, (const UTF8Char*)""));
	this->txtDeviceURL->SetRect(104, 436, 300, 23, false);
	this->txtDeviceURL->SetReadOnly(true);

	if (this->ssdp->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in initializing SSDP Client", (const UTF8Char*)"SSDP Client", this);
	}
	else if (!this->ssdp->Scan())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in scanning SSDP Client", (const UTF8Char*)"SSDP Client", this);
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSSDPClientForm::~AVIRSSDPClientForm()
{
	DEL_CLASS(this->ssdp);
	Data::ArrayList<Net::SSDPClient::SSDPRoot*> *rootList = this->rootMap->GetValues();
	LIST_FREE_FUNC(rootList, Net::SSDPClient::SSDPRootFree);
	DEL_CLASS(this->rootMap);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSSDPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
