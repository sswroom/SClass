#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIREWDTU01Form.h"
#include "Text/JSON.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnMQTTMessage(void *userObj, Text::CString topic, const Data::ByteArrayR &buff)
{
	SSWR::AVIRead::AVIREWDTU01Form *me = (SSWR::AVIRead::AVIREWDTU01Form *)userObj;
	Text::JSONBase *jsonObj = Text::JSONBase::ParseJSONBytes(buff.Ptr(), buff.GetSize());
	Text::JSONObject *obj;
	Text::JSONBase *baseObj;
	Text::JSONArray *arr;
	Text::String *name;
	Text::String *mac;
	Text::String *rssi;
	DeviceEntry *entry;
	UInt8 macBuff[8];
	Int32 irssi;
	UInt64 macInt;
	UOSInt i;
	UOSInt j;
	if (jsonObj == 0)
	{
		return;
	}
	if (jsonObj->GetType() == Text::JSONType::Array)
	{
		Sync::MutexUsage mutUsage(me->dataMut);
		me->dataChg = true;
		me->DataClear();
		arr = (Text::JSONArray*)jsonObj;
		i = 0;
		j = arr->GetArrayLength();
		while (i < j)
		{
			baseObj = arr->GetArrayValue(i);
			if (baseObj && baseObj->GetType() == Text::JSONType::Object)
			{
				obj = (Text::JSONObject*)baseObj;
				name = obj->GetObjectString(CSTR("name"));
				mac = obj->GetObjectString(CSTR("mac"));
				rssi = obj->GetObjectString(CSTR("rssi"));
				if (mac != 0 && rssi != 0 && mac->leng == 12)
				{
					Text::StrHex2Bytes(mac->v, &macBuff[2]);
					macBuff[0] = 0;
					macBuff[1] = 0;
					macInt = ReadMUInt64(macBuff);
					irssi = rssi->ToInt32();
					entry = me->dataMap.Get(macInt);
					if (entry)
					{
						if (entry->name == 0)
						{
							entry->name = SCOPY_STRING(name);
						}
						if (entry->rssi == 127 || irssi > entry->rssi)
						{
							entry->rssi = irssi;
						}
					}
					else
					{
						entry = MemAlloc(DeviceEntry, 1);
						entry->mac[0] = macBuff[2];
						entry->mac[1] = macBuff[3];
						entry->mac[2] = macBuff[4];
						entry->mac[3] = macBuff[5];
						entry->mac[4] = macBuff[6];
						entry->mac[5] = macBuff[7];
						entry->macInt = macInt;
						entry->rssi = irssi;
						entry->name = SCOPY_STRING(name);
						entry->remark = 0;
						me->dataMap.Put(macInt, entry);
					}
				}
			}
			i++;
		}
	}
	jsonObj->EndUse();
}

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnConnectClicked(void *userObj)
{
	SSWR::AVIRead::AVIREWDTU01Form *me = (SSWR::AVIRead::AVIREWDTU01Form *)userObj;
	if (me->cli)
	{
		SDEL_CLASS(me->cli);
		return;
	}
	Text::StringBuilderUTF8 sbHost;
	Text::StringBuilderUTF8 sbPort;
	me->txtServer->GetText(sbHost);
	me->txtPort->GetText(sbPort);
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbHost.ToCString(), addr))
	{
		me->ui->ShowMsgOK(CSTR("Error in resolving host name"), CSTR("EqasyWay EW-DTU01"), me);
		return;
	}
	if (!sbPort.ToUInt16(port))
	{
		me->ui->ShowMsgOK(CSTR("Invalid port number"), CSTR("EqasyWay EW-DTU01"), me);
		return;
	}
	NEW_CLASS(me->cli, Net::MQTTStaticClient(me->core->GetSocketFactory(), 0, sbHost.ToCString(), port, CSTR_NULL, CSTR_NULL, false, OnMQTTMessage, me, 30, 0));
	if (me->cli->ChannelFailure())
	{
		me->ui->ShowMsgOK(CSTR("Error in connecting to MQTT server"), CSTR("EasyWay EW-DTU01"), me);
		SDEL_CLASS(me->cli);
		return;
	}
	me->cli->Subscribe(CSTR("/+/connect_packet/adv_publish"));
}

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIREWDTU01Form *me = (SSWR::AVIRead::AVIREWDTU01Form *)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->dataChg)
	{
		DeviceEntry *entry;
		const Net::MACInfo::MACEntry *macEntry;
		NotNullPtr<Text::String> s;
		Sync::MutexUsage mutUsage(me->dataMut);
		me->lvDevices->ClearItems();
		UOSInt i = 0;
		UOSInt j = me->dataMap.GetCount();
		while (i < j)
		{
			entry = me->dataMap.GetItem(i);
			sptr = Text::StrHexBytes(sbuff, entry->mac, 6, ':');
			me->lvDevices->AddItem(CSTRP(sbuff, sptr), entry);
			if (s.Set(entry->name))
			{
				me->lvDevices->SetSubItem(i, 1, s);
			}
			macEntry = Net::MACInfo::GetMACInfo(entry->macInt);
			if (macEntry)
			{
				me->lvDevices->SetSubItem(i, 2, {macEntry->name, macEntry->nameLen});
			}
			else
			{
				me->lvDevices->SetSubItem(i, 2, CSTR("Unknown"));
			}
			sptr = Text::StrInt32(sbuff, entry->rssi);
			me->lvDevices->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			if (s.Set(entry->remark))
			{
				me->lvDevices->SetSubItem(i, 4, s);
			}
			i++;
		}
	}
}

void SSWR::AVIRead::AVIREWDTU01Form::DataClear()
{
	DeviceEntry *entry;
	UOSInt i = this->dataMap.GetCount();
	while (i-- > 0)
	{
		entry = this->dataMap.GetItem(i);
		SDEL_STRING(entry->name);
		SDEL_STRING(entry->remark);
		MemFree(entry);
	}
	this->dataMap.Clear();
}

SSWR::AVIRead::AVIREWDTU01Form::AVIREWDTU01Form(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("EasyWay EW-DTU01"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->cli = 0;
	this->dataChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlMQTT, UI::GUIPanel(ui, *this));
	this->pnlMQTT->SetRect(0, 0, 100, 31, false);
	this->pnlMQTT->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblServer = ui->NewLabel(this->pnlMQTT, CSTR("MQTT Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this->pnlMQTT, CSTR("127.0.0.1")));
	this->txtServer->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(this->pnlMQTT, CSTR("Port"));
	this->lblPort->SetRect(254, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlMQTT, CSTR("1883")));
	this->txtPort->SetRect(354, 4, 50, 23, false);
	this->btnConnect = ui->NewButton(this->pnlMQTT, CSTR("Connect"));
	this->btnConnect->SetRect(404, 4, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->AddColumn(CSTR("MAC"), 100);
	this->lvDevices->AddColumn(CSTR("Name"), 100);
	this->lvDevices->AddColumn(CSTR("Vendor"), 200);
	this->lvDevices->AddColumn(CSTR("RSSI"), 50);
	this->lvDevices->AddColumn(CSTR("Remarks"), 200);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIREWDTU01Form::~AVIREWDTU01Form()
{
	SDEL_CLASS(this->cli);
	this->DataClear();
}

void SSWR::AVIRead::AVIREWDTU01Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
