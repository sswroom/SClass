#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIREWDTU01Form.h"
#include "Text/JSON.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnMQTTMessage(void *userObj, Text::CString topic, const UInt8 *buff, UOSInt buffSize)
{
	SSWR::AVIRead::AVIREWDTU01Form *me = (SSWR::AVIRead::AVIREWDTU01Form *)userObj;
	Text::JSONBase *jsonObj = Text::JSONBase::ParseJSONStrLen(buff, buffSize);
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
				name = obj->GetObjectString(UTF8STRC("name"));
				mac = obj->GetObjectString(UTF8STRC("mac"));
				rssi = obj->GetObjectString(UTF8STRC("rssi"));
				if (mac != 0 && rssi != 0 && mac->leng == 12)
				{
					Text::StrHex2Bytes(mac->v, &macBuff[2]);
					macBuff[0] = 0;
					macBuff[1] = 0;
					macInt = ReadMUInt64(macBuff);
					irssi = rssi->ToInt32();
					entry = me->dataMap->Get(macInt);
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
						me->dataMap->Put(macInt, entry);
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
	me->txtServer->GetText(&sbHost);
	me->txtPort->GetText(&sbPort);
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbHost.ToString(), sbHost.GetLength(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving host name", (const UTF8Char*)"EqasyWay EW-DTU01", me);
		return;
	}
	if (!sbPort.ToUInt16(&port))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid port number", (const UTF8Char*)"EqasyWay EW-DTU01", me);
		return;
	}
	NEW_CLASS(me->cli, Net::MQTTStaticClient(me->core->GetSocketFactory(), 0, sbHost.ToCString(), port, CSTR_NULL, CSTR_NULL, OnMQTTMessage, me, 30, 0));
	if (me->cli->ChannelFailure())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in connecting to MQTT server", (const UTF8Char*)"EasyWay EW-DTU01", me);
		SDEL_CLASS(me->cli);
		return;
	}
	me->cli->Subscribe((const UTF8Char*)"/+/connect_packet/adv_publish");
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
		Sync::MutexUsage mutUsage(me->dataMut);
		me->lvDevices->ClearItems();
		Data::ArrayList<DeviceEntry*> *dataList = me->dataMap->GetValues();
		UOSInt i = 0;
		UOSInt j = dataList->GetCount();
		while (i < j)
		{
			entry = dataList->GetItem(i);
			sptr = Text::StrHexBytes(sbuff, entry->mac, 6, ':');
			me->lvDevices->AddItem(CSTRP(sbuff, sptr), entry);
			if (entry->name)
			{
				me->lvDevices->SetSubItem(i, 1, entry->name);
			}
			macEntry = Net::MACInfo::GetMACInfo(entry->macInt);
			if (macEntry)
			{
				me->lvDevices->SetSubItem(i, 2, macEntry->name);
			}
			else
			{
				me->lvDevices->SetSubItem(i, 2, (const UTF8Char*)"Unknown");
			}
			Text::StrInt32(sbuff, entry->rssi);
			me->lvDevices->SetSubItem(i, 3, sbuff);
			if (entry->remark)
			{
				me->lvDevices->SetSubItem(i, 4, entry->remark);
			}
			i++;
		}
	}
}

void SSWR::AVIRead::AVIREWDTU01Form::DataClear()
{
	DeviceEntry *entry;
	Data::ArrayList<DeviceEntry*> *dataList = this->dataMap->GetValues();
	UOSInt i = dataList->GetCount();
	while (i-- > 0)
	{
		entry = dataList->GetItem(i);
		SDEL_STRING(entry->name);
		SDEL_STRING(entry->remark);
		MemFree(entry);
	}
	this->dataMap->Clear();
}

SSWR::AVIRead::AVIREWDTU01Form::AVIREWDTU01Form(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"EasyWay EW-DTU01");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->cli = 0;
	NEW_CLASS(this->dataMut, Sync::Mutex());
	this->dataChg = false;
	NEW_CLASS(this->dataMap, Data::UInt64Map<DeviceEntry*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlMQTT, UI::GUIPanel(ui, this));
	this->pnlMQTT->SetRect(0, 0, 100, 31, false);
	this->pnlMQTT->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblServer, UI::GUILabel(ui, this->pnlMQTT, (const UTF8Char*)"MQTT Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtServer, UI::GUITextBox(ui, this->pnlMQTT, CSTR("127.0.0.1")));
	this->txtServer->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblPort, UI::GUILabel(ui, this->pnlMQTT, (const UTF8Char*)"Port"));
	this->lblPort->SetRect(254, 4, 100, 23, false);
	NEW_CLASS(this->txtPort, UI::GUITextBox(ui, this->pnlMQTT, CSTR("1883")));
	this->txtPort->SetRect(354, 4, 50, 23, false);
	NEW_CLASS(this->btnConnect, UI::GUIButton(ui, this->pnlMQTT, (const UTF8Char*)"Connect"));
	this->btnConnect->SetRect(404, 4, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->AddColumn((const UTF8Char*)"MAC", 100);
	this->lvDevices->AddColumn((const UTF8Char*)"Name", 100);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor", 200);
	this->lvDevices->AddColumn((const UTF8Char*)"RSSI", 50);
	this->lvDevices->AddColumn((const UTF8Char*)"Remarks", 200);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIREWDTU01Form::~AVIREWDTU01Form()
{
	SDEL_CLASS(this->cli);
	DEL_CLASS(this->dataMut);
	this->DataClear();
	DEL_CLASS(this->dataMap);
}

void SSWR::AVIRead::AVIREWDTU01Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
