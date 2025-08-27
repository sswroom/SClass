#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIREWDTU01Form.h"
#include "Text/JSON.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnMQTTMessage(AnyType userObj, Text::CStringNN topic, const Data::ByteArrayR &buff)
{
	NN<SSWR::AVIRead::AVIREWDTU01Form> me = userObj.GetNN<SSWR::AVIRead::AVIREWDTU01Form>();
	NN<Text::JSONBase> jsonObj;
	NN<Text::JSONObject> obj;
	NN<Text::JSONBase> baseObj;
	NN<Text::JSONArray> arr;
	Optional<Text::String> name;
	NN<Text::String> mac;
	NN<Text::String> rssi;
	NN<DeviceEntry> entry;
	UInt8 macBuff[8];
	Int32 irssi;
	UInt64 macInt;
	UOSInt i;
	UOSInt j;
	if (!Text::JSONBase::ParseJSONBytes(buff.Arr(), buff.GetSize()).SetTo(jsonObj))
	{
		return;
	}
	if (jsonObj->GetType() == Text::JSONType::Array)
	{
		Sync::MutexUsage mutUsage(me->dataMut);
		me->dataChg = true;
		me->DataClear();
		arr = NN<Text::JSONArray>::ConvertFrom(jsonObj);
		i = 0;
		j = arr->GetArrayLength();
		while (i < j)
		{
			if (arr->GetArrayValue(i).SetTo(baseObj) && baseObj->GetType() == Text::JSONType::Object)
			{
				obj = NN<Text::JSONObject>::ConvertFrom(baseObj);
				name = obj->GetObjectString(CSTR("name"));
				if (obj->GetObjectString(CSTR("mac")).SetTo(mac) && obj->GetObjectString(CSTR("rssi")).SetTo(rssi) && mac->leng == 12)
				{
					Text::StrHex2Bytes(mac->v, &macBuff[2]);
					macBuff[0] = 0;
					macBuff[1] = 0;
					macInt = ReadMUInt64(macBuff);
					irssi = rssi->ToInt32();
					if (me->dataMap.Get(macInt).SetTo(entry))
					{
						if (entry->name.IsNull())
						{
							entry->name = Text::String::CopyOrNull(name);
						}
						if (entry->rssi == 127 || irssi > entry->rssi)
						{
							entry->rssi = irssi;
						}
					}
					else
					{
						entry = MemAllocNN(DeviceEntry);
						entry->mac[0] = macBuff[2];
						entry->mac[1] = macBuff[3];
						entry->mac[2] = macBuff[4];
						entry->mac[3] = macBuff[5];
						entry->mac[4] = macBuff[6];
						entry->mac[5] = macBuff[7];
						entry->macInt = macInt;
						entry->rssi = irssi;
						entry->name = Text::String::CopyOrNull(name);
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

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnConnectClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREWDTU01Form> me = userObj.GetNN<SSWR::AVIRead::AVIREWDTU01Form>();
	if (me->cli.NotNull())
	{
		me->cli.Delete();
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
	NN<Net::MQTTStaticClient> cli;
	NEW_CLASSNN(cli, Net::MQTTStaticClient(me->core->GetTCPClientFactory(), 0, sbHost.ToCString(), port, nullptr, nullptr, false, OnMQTTMessage, me, 30, 0));
	if (cli->ChannelFailure())
	{
		me->ui->ShowMsgOK(CSTR("Error in connecting to MQTT server"), CSTR("EasyWay EW-DTU01"), me);
		cli.Delete();
		return;
	}
	me->cli = cli;
	cli->Subscribe(CSTR("/+/connect_packet/adv_publish"));
}

void __stdcall SSWR::AVIRead::AVIREWDTU01Form::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREWDTU01Form> me = userObj.GetNN<SSWR::AVIRead::AVIREWDTU01Form>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (me->dataChg)
	{
		NN<DeviceEntry> entry;
		NN<const Net::MACInfo::MACEntry> macEntry;
		NN<Text::String> s;
		Sync::MutexUsage mutUsage(me->dataMut);
		me->lvDevices->ClearItems();
		UOSInt i = 0;
		UOSInt j = me->dataMap.GetCount();
		while (i < j)
		{
			entry = me->dataMap.GetItemNoCheck(i);
			sptr = Text::StrHexBytes(sbuff, entry->mac, 6, ':');
			me->lvDevices->AddItem(CSTRP(sbuff, sptr), entry);
			if (entry->name.SetTo(s))
			{
				me->lvDevices->SetSubItem(i, 1, s);
			}
			macEntry = Net::MACInfo::GetMACInfo(entry->macInt);
			me->lvDevices->SetSubItem(i, 2, {macEntry->name, macEntry->nameLen});
			sptr = Text::StrInt32(sbuff, entry->rssi);
			me->lvDevices->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			if (entry->remark.SetTo(s))
			{
				me->lvDevices->SetSubItem(i, 4, s);
			}
			i++;
		}
	}
}

void SSWR::AVIRead::AVIREWDTU01Form::DataClear()
{
	NN<DeviceEntry> entry;
	UOSInt i = this->dataMap.GetCount();
	while (i-- > 0)
	{
		entry = this->dataMap.GetItemNoCheck(i);
		OPTSTR_DEL(entry->name);
		OPTSTR_DEL(entry->remark);
		MemFreeNN(entry);
	}
	this->dataMap.Clear();
}

SSWR::AVIRead::AVIREWDTU01Form::AVIREWDTU01Form(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("EasyWay EW-DTU01"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->cli = 0;
	this->dataChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMQTT = ui->NewPanel(*this);
	this->pnlMQTT->SetRect(0, 0, 100, 31, false);
	this->pnlMQTT->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblServer = ui->NewLabel(this->pnlMQTT, CSTR("MQTT Server"));
	this->lblServer->SetRect(4, 4, 100, 23, false);
	this->txtServer = ui->NewTextBox(this->pnlMQTT, CSTR("127.0.0.1"));
	this->txtServer->SetRect(104, 4, 150, 23, false);
	this->lblPort = ui->NewLabel(this->pnlMQTT, CSTR("Port"));
	this->lblPort->SetRect(254, 4, 100, 23, false);
	this->txtPort = ui->NewTextBox(this->pnlMQTT, CSTR("1883"));
	this->txtPort->SetRect(354, 4, 50, 23, false);
	this->btnConnect = ui->NewButton(this->pnlMQTT, CSTR("Connect"));
	this->btnConnect->SetRect(404, 4, 75, 23, false);
	this->btnConnect->HandleButtonClick(OnConnectClicked, this);

	this->lvDevices = ui->NewListView(*this, UI::ListViewStyle::Table, 5);
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
	this->cli.Delete();
	this->DataClear();
}

void SSWR::AVIRead::AVIREWDTU01Form::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
