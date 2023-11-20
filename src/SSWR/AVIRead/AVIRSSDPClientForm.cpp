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
	const Data::ReadingList<Net::SSDPClient::SSDPDevice*> *devList = me->ssdp->GetDevices(mutUsage);
	UInt8 sbuff[128];
	UTF8Char *sptr;
	if (devList->GetCount() != me->lbDevice->GetCount())
	{
		UOSInt i = 0;
		UOSInt j = devList->GetCount();
		Net::SSDPClient::SSDPDevice *dev;
		me->lbDevice->ClearItems();
		while (i < j)
		{
			dev = devList->GetItem(i);
			sptr = Net::SocketUtil::GetAddrName(sbuff, dev->addr);
			me->lbDevice->AddItem(CSTRP(sbuff, sptr), dev);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSSDPClientForm::OnDeviceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSSDPClientForm *me = (SSWR::AVIRead::AVIRSSDPClientForm*)userObj;
	Sync::MutexUsage mutUsage;
	me->ssdp->GetDevices(mutUsage);
	Net::SSDPClient::SSDPDevice *dev = (Net::SSDPClient::SSDPDevice*)me->lbDevice->GetSelectedItem();
	me->lbService->ClearItems();
	if (dev == 0)
	{
		return;
	}
	Net::SSDPClient::SSDPService *svc;
	UOSInt i = 0;
	UOSInt j = dev->services.GetCount();
	while (i < j)
	{
		svc = dev->services.GetItem(i);
		me->lbService->AddItem(Text::String::OrEmpty(svc->st), svc);
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSSDPClientForm::OnServiceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSSDPClientForm *me = (SSWR::AVIRead::AVIRSSDPClientForm*)userObj;
	Sync::MutexUsage mutUsage;
	me->ssdp->GetDevices(mutUsage);
	Net::SSDPClient::SSDPService *svc = (Net::SSDPClient::SSDPService*)me->lbService->GetSelectedItem();
	if (svc)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		me->txtLocation->SetText(Text::String::OrEmpty(svc->location)->ToCString());
		if (svc->time)
		{
			Data::DateTime dt;
			dt.SetTicks(svc->time);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzz");
			me->txtDate->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->txtDate->SetText(CSTR("-"));
		}
		me->txtUSN->SetText(svc->usn->ToCString());
		me->txtST->SetText(svc->st->ToCString());
		me->txtServer->SetText(Text::String::OrEmpty(svc->server)->ToCString());
		me->txtOpt->SetText(Text::String::OrEmpty(svc->opt)->ToCString());
		me->txtUserAgent->SetText(Text::String::OrEmpty(svc->userAgent)->ToCString());

		if (svc->location)
		{
			Net::SSDPClient::SSDPRoot *root = me->rootMap->Get(svc->location);
			if (root == 0)
			{
				NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(me->sockf, me->ssl, svc->location->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
				if (cli->IsError())
				{
					root = MemAlloc(Net::SSDPClient::SSDPRoot, 1);
					MemClear(root, sizeof(Net::SSDPClient::SSDPRoot));
					cli.Delete();
				}
				else
				{
					root = Net::SSDPClient::SSDPRootParse(me->core->GetEncFactory(), cli);
					cli.Delete();
				}
				me->rootMap->Put(svc->location, root);
			}

			me->txtUDN->SetText(Text::String::OrEmpty(root->udn)->ToCString());
			me->txtFriendlyName->SetText(Text::String::OrEmpty(root->friendlyName)->ToCString());
			me->txtManufacturer->SetText(Text::String::OrEmpty(root->manufacturer)->ToCString());
			me->txtManufacturerURL->SetText(Text::String::OrEmpty(root->manufacturerURL)->ToCString());
			me->txtModelName->SetText(Text::String::OrEmpty(root->modelName)->ToCString());
			me->txtModelNumber->SetText(Text::String::OrEmpty(root->modelNumber)->ToCString());
			me->txtModelURL->SetText(Text::String::OrEmpty(root->modelURL)->ToCString());
			me->txtSerialNumber->SetText(Text::String::OrEmpty(root->serialNumber)->ToCString());
			me->txtPresentationURL->SetText(Text::String::OrEmpty(root->presentationURL)->ToCString());
			me->txtDeviceType->SetText(Text::String::OrEmpty(root->deviceType)->ToCString());
			me->txtDeviceURL->SetText(Text::String::OrEmpty(root->deviceURL)->ToCString());
		}
		else
		{
			me->txtUDN->SetText(CSTR(""));
			me->txtFriendlyName->SetText(CSTR(""));
			me->txtManufacturer->SetText(CSTR(""));
			me->txtManufacturerURL->SetText(CSTR(""));
			me->txtModelName->SetText(CSTR(""));
			me->txtModelNumber->SetText(CSTR(""));
			me->txtModelURL->SetText(CSTR(""));
			me->txtSerialNumber->SetText(CSTR(""));
			me->txtPresentationURL->SetText(CSTR(""));
			me->txtDeviceType->SetText(CSTR(""));
			me->txtDeviceURL->SetText(CSTR(""));
		}
	}
	else
	{
		me->txtLocation->SetText(CSTR(""));
		me->txtDate->SetText(CSTR(""));
		me->txtUSN->SetText(CSTR(""));
		me->txtST->SetText(CSTR(""));
		me->txtServer->SetText(CSTR(""));
		me->txtOpt->SetText(CSTR(""));
		me->txtUserAgent->SetText(CSTR(""));

		me->txtUDN->SetText(CSTR(""));
		me->txtFriendlyName->SetText(CSTR(""));
		me->txtManufacturer->SetText(CSTR(""));
		me->txtManufacturerURL->SetText(CSTR(""));
		me->txtModelName->SetText(CSTR(""));
		me->txtModelNumber->SetText(CSTR(""));
		me->txtModelURL->SetText(CSTR(""));
		me->txtSerialNumber->SetText(CSTR(""));
		me->txtPresentationURL->SetText(CSTR(""));
		me->txtDeviceType->SetText(CSTR(""));
		me->txtDeviceURL->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRSSDPClientForm::AVIRSSDPClientForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SSDP Client"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->ssl = Net::SSLEngineFactory::Create(this->sockf, false);
	NEW_CLASS(this->ssdp, Net::SSDPClient(this->sockf, CSTR_NULL, this->core->GetLog()));
	NEW_CLASS(this->rootMap, Data::FastStringMap<Net::SSDPClient::SSDPRoot*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbDevice, UI::GUIListBox(ui, *this, false));
	this->lbDevice->SetRect(0, 0, 120, 23, false);
	this->lbDevice->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevice->HandleSelectionChange(OnDeviceSelChg, this);
	NEW_CLASS(this->hspDevice, UI::GUIHSplitter(ui, *this, 3, false));
	NEW_CLASS(this->lbService, UI::GUIListBox(ui, *this, false));
	this->lbService->SetRect(0, 0, 200, 23, false);
	this->lbService->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbService->HandleSelectionChange(OnServiceSelChg, this);
	NEW_CLASS(this->hspService, UI::GUIHSplitter(ui, *this, 3, false));
	NEW_CLASSNN(this->pnlService, UI::GUIPanel(ui, *this));
	this->pnlService->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblLocation, UI::GUILabel(ui, this->pnlService, CSTR("Location")));
	this->lblLocation->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtLocation, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtLocation->SetRect(104, 4, 300, 23, false);
	this->txtLocation->SetReadOnly(true);
	NEW_CLASS(this->lblDate, UI::GUILabel(ui, this->pnlService, CSTR("Date")));
	this->lblDate->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDate, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtDate->SetRect(104, 28, 300, 23, false);
	this->txtDate->SetReadOnly(true);
	NEW_CLASS(this->lblUSN, UI::GUILabel(ui, this->pnlService, CSTR("USN")));
	this->lblUSN->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtUSN, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtUSN->SetRect(104, 52, 300, 23, false);
	this->txtUSN->SetReadOnly(true);
	NEW_CLASS(this->lblST, UI::GUILabel(ui, this->pnlService, CSTR("ST")));
	this->lblST->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtST, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtST->SetRect(104, 76, 300, 23, false);
	this->txtST->SetReadOnly(true);
	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this->pnlService, CSTR("Server")));
	this->lblServer->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtServer->SetRect(104, 100, 300, 23, false);
	this->txtServer->SetReadOnly(true);
	NEW_CLASS(this->lblOpt, UI::GUILabel(ui, this->pnlService, CSTR("Opt")));
	this->lblOpt->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtOpt, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtOpt->SetRect(104, 124, 300, 23, false);
	this->txtOpt->SetReadOnly(true);
	NEW_CLASS(this->lblUserAgent, UI::GUILabel(ui, this->pnlService, CSTR("UserAgent")));
	this->lblUserAgent->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtUserAgent, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtUserAgent->SetRect(104, 148, 300, 23, false);
	this->txtUserAgent->SetReadOnly(true);

	NEW_CLASS(this->lblUDN, UI::GUILabel(ui, this->pnlService, CSTR("UDN")));
	this->lblUDN->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtUDN, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtUDN->SetRect(104, 196, 300, 23, false);
	this->txtUDN->SetReadOnly(true);
	NEW_CLASS(this->lblFriendlyName, UI::GUILabel(ui, this->pnlService, CSTR("Friendly Name")));
	this->lblFriendlyName->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtFriendlyName, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtFriendlyName->SetRect(104, 220, 300, 23, false);
	this->txtFriendlyName->SetReadOnly(true);
	NEW_CLASS(this->lblManufacturer, UI::GUILabel(ui, this->pnlService, CSTR("Manufacturer")));
	this->lblManufacturer->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtManufacturer, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtManufacturer->SetRect(104, 244, 300, 23, false);
	this->txtManufacturer->SetReadOnly(true);
	NEW_CLASS(this->lblManufacturerURL, UI::GUILabel(ui, this->pnlService, CSTR("Manfacturer URL")));
	this->lblManufacturerURL->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtManufacturerURL, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtManufacturerURL->SetRect(104, 268, 300, 23, false);
	this->txtManufacturerURL->SetReadOnly(true);
	NEW_CLASS(this->lblModelName, UI::GUILabel(ui, this->pnlService, CSTR("Model Name")));
	this->lblModelName->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtModelName, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtModelName->SetRect(104, 292, 300, 23, false);
	this->txtModelName->SetReadOnly(true);
	NEW_CLASS(this->lblModelNumber, UI::GUILabel(ui, this->pnlService, CSTR("Moden Number")));
	this->lblModelNumber->SetRect(4, 316, 100, 23, false);
	NEW_CLASS(this->txtModelNumber, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtModelNumber->SetRect(104, 316, 300, 23, false);
	this->txtModelNumber->SetReadOnly(true);
	NEW_CLASS(this->lblModelURL, UI::GUILabel(ui, this->pnlService, CSTR("Model URL")));
	this->lblModelURL->SetRect(4, 340, 100, 23, false);
	NEW_CLASS(this->txtModelURL, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtModelURL->SetRect(104, 340, 300, 23, false);
	this->txtModelURL->SetReadOnly(true);
	NEW_CLASS(this->lblSerialNumber, UI::GUILabel(ui, this->pnlService, CSTR("Serial Number")));
	this->lblSerialNumber->SetRect(4, 364, 100, 23, false);
	NEW_CLASS(this->txtSerialNumber, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtSerialNumber->SetRect(104, 364, 300, 23, false);
	this->txtSerialNumber->SetReadOnly(true);
	NEW_CLASS(this->lblPresentationURL, UI::GUILabel(ui, this->pnlService, CSTR("Presentation URL")));
	this->lblPresentationURL->SetRect(4, 388, 100, 23, false);
	NEW_CLASS(this->txtPresentationURL, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtPresentationURL->SetRect(104, 388, 300, 23, false);
	this->txtPresentationURL->SetReadOnly(true);
	NEW_CLASS(this->lblDeviceType, UI::GUILabel(ui, this->pnlService, CSTR("Device Type")));
	this->lblDeviceType->SetRect(4, 412, 100, 23, false);
	NEW_CLASS(this->txtDeviceType, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtDeviceType->SetRect(104, 412, 300, 23, false);
	this->txtDeviceType->SetReadOnly(true);
	NEW_CLASS(this->lblDeviceURL, UI::GUILabel(ui, this->pnlService, CSTR("Device URL")));
	this->lblDeviceURL->SetRect(4, 436, 100, 23, false);
	NEW_CLASS(this->txtDeviceURL, UI::GUITextBox(ui, this->pnlService, CSTR("")));
	this->txtDeviceURL->SetRect(104, 436, 300, 23, false);
	this->txtDeviceURL->SetReadOnly(true);

	if (this->ssdp->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in initializing SSDP Client"), CSTR("SSDP Client"), this);
	}
	else if (!this->ssdp->Scan())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in scanning SSDP Client"), CSTR("SSDP Client"), this);
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRSSDPClientForm::~AVIRSSDPClientForm()
{
	DEL_CLASS(this->ssdp);
	LIST_FREE_FUNC(this->rootMap, Net::SSDPClient::SSDPRootFree);
	DEL_CLASS(this->rootMap);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSSDPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
