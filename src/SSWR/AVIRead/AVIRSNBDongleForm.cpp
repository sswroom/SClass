#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"
#include "SSWR/AVIRead/AVIRSNBHandlerForm.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnProtocolReceived(void *userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Received cmd 0x"));
	sb.AppendHex8(cmdType);
	sb.AppendC(UTF8STRC(": "));
	if (cmdSize > 0)
	{
		sb.AppendHexBuff(cmd, cmdSize, ' ', Text::LineBreakType::None);
	}
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDongleInfoClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	me->snb->SendGetDongleInfo();
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnCheckDongleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	me->snb->SendCheckDongle();
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnCheckDevicesClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	me->snb->SendCheckDevices();
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnResetNetworkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	me->snb->SendResetNetwork();
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnAddDeviceClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	me->snb->SendAddDevice(10);
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnLogClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	NotNullPtr<Text::String> s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Data::ArrayList<DeviceInfo*> devList;
	DeviceInfo *dev;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::DateTime dt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Text::StringBuilderUTF8 sb;

	if (me->devChg)
	{
		me->devMut.LockRead();
		devList.AddAll(&me->devMap);
		me->devChg = false;
		me->devMut.UnlockRead();

		me->lvDevice->ClearItems();
		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItem(i);
			dev->mut.LockRead();
			dev->readingChg = false;
			
			sptr = Text::StrUInt64(sbuff, dev->devId);
			k = me->lvDevice->AddItem(CSTRP(sbuff, sptr), dev);
			sptr = Text::StrUInt16(sbuff, (UInt16)dev->shortAddr);
			me->lvDevice->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			me->lvDevice->SetSubItem(k, 2, IO::SNBDongle::GetHandleName(dev->handType));
			if (dev->readingTime)
			{
				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				me->lvDevice->SetSubItem(k, 3, CSTRP(sbuff, sptr));

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sensor Type = "));
				sb.Append(IO::SNBDongle::SensorTypeGetName(dev->sensorType));
				l = 0;
				while (l < dev->nReading)
				{
					if (sb.GetLength() > 0)
					{
						sb.AppendC(UTF8STRC(", "));
					}
					sb.Append(IO::SNBDongle::GetReadingName(dev->readingTypes[l]));
					sb.AppendC(UTF8STRC(" = "));
					Text::SBAppendF64(&sb, dev->readings[l]);
					l++;
				}
				me->lvDevice->SetSubItem(k, 4, sb.ToCString());
			}
			else
			{
				me->lvDevice->SetSubItem(k, 3, CSTR("-"));
				me->lvDevice->SetSubItem(k, 4, CSTR("-"));
			}
			dev->mut.UnlockRead();
			i++;
		}
	}
	else
	{
		me->devMut.LockRead();
		devList.AddAll(&me->devMap);
		me->devMut.UnlockRead();

		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItem(i);
			dev->mut.LockRead();
			if (dev->shortAddrChg)
			{
				dev->shortAddrChg = false;
				sptr = Text::StrInt32(sbuff, (UInt16)dev->shortAddr);
				me->lvDevice->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			if (dev->readingChg)
			{
				dev->readingChg = false;

				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				me->lvDevice->SetSubItem(i, 3, CSTRP(sbuff, sptr));

				sb.ClearStr();
				sb.AppendC(UTF8STRC("Sensor Type = "));
				sb.Append(IO::SNBDongle::SensorTypeGetName(dev->sensorType));
				l = 0;
				while (l < dev->nReading)
				{
					if (sb.GetLength() > 0)
					{
						sb.AppendC(UTF8STRC(", "));
					}
					sb.Append(IO::SNBDongle::GetReadingName(dev->readingTypes[l]));
					sb.AppendC(UTF8STRC(" = "));
					Text::SBAppendF64(&sb, dev->readings[l]);
					l++;
				}
				me->lvDevice->SetSubItem(i, 4, sb.ToCString());
			}
			dev->mut.UnlockRead();
			i++;
		}
	}

	if (me->dongleUpdated)
	{
		UInt64 id;
		me->dongleUpdated = false;
		id = me->snb->GetDongleId();
		if (id == 0)
		{
			me->txtDongleId->SetText(CSTR(""));
		}
		else
		{
			sptr = Text::StrUInt64(sbuff, id);
			me->txtDongleId->SetText(CSTRP(sbuff, sptr));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnCopyDevIdClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevReportTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();
		me->snb->SendGetReportTime(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevSetReportTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();
		me->snb->SendSetReportTime(devId, 30);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevOnClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();
		me->snb->SendDevTurnOn(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevOffClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();
		me->snb->SendDevTurnOff(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevStatusClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetSelectedItemTextNew();
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();
		me->snb->SendDevGetStatus(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDeviceDblClk(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::String *s = me->lvDevice->GetItemTextNew(index);
	if (s)
	{
		UInt64 devId = s->ToUInt64();
		s->Release();

		DeviceInfo *dev;
		me->devMut.LockRead();
		dev = me->devMap.Get(devId);
		me->devMut.UnlockRead();

		if (dev)
		{
			SSWR::AVIRead::AVIRSNBHandlerForm frm(0, me->ui, me->core, dev->handType);
			if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
			{
				dev->handType = frm.GetHandleType();
				me->devMut.LockWrite();
				me->devHandlerMap.Put(devId, (Int32)dev->handType);
				me->devMut.UnlockWrite();
				me->snb->SetDevHandleType(dev->devId, dev->handType);
				me->lvDevice->SetSubItem(index, 2, IO::SNBDongle::GetHandleName(dev->handType));
			}
	//		me->snb->SendSetReportTime(devId, 3, true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnUploadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	UInt64 dongleId = me->snb->GetDongleId();
	UInt32 baudRate = me->snb->GetBaudRate();
	if (dongleId == 0 || baudRate == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Dongle info missing"), CSTR("Error"), me);
		return;
	}
	Text::StringBuilderUTF8 url;
	me->txtURL->GetText(&url);
	if (!url.StartsWith(UTF8STRC("http://")))
	{
		UI::MessageDialog::ShowDialog(CSTR("URL is not valid"), CSTR("Error"), me);
		return;
	}

	Text::StringBuilderUTF8 remarks;
	me->txtRemarks->GetText(&remarks);
	if (remarks.IndexOf('\r') != INVALID_INDEX || remarks.IndexOf('\n') != INVALID_INDEX || remarks.IndexOf('\'') != INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("Remarks contain invalid characters"), CSTR("Error"), me);
		return;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("dongle_id="));
	sb.AppendU64(dongleId);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.AppendC(UTF8STRC("baud_rate="));
	sb.AppendU32(baudRate);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.AppendC(UTF8STRC("remarks="));
	sb.Append(remarks);
	sb.AppendC(UTF8STRC("\r\n"));
	sb.AppendC(UTF8STRC("sensor_list="));
	me->devMut.LockRead();
	DeviceInfo *dev;
	UOSInt i;
	UOSInt j;
	if (me->devMap.GetCount() == 0)
	{
		me->devMut.UnlockRead();
		UI::MessageDialog::ShowDialog(CSTR("No devices found"), CSTR("Error"), me);
		return;
	}
	i = 0;
	j = me->devMap.GetCount();
	while (i < j)
	{
		dev = me->devMap.GetItem(i);
		if (dev->readingTime == 0 || dev->nReading == 0)
		{
			me->devMut.UnlockRead();
			UI::MessageDialog::ShowDialog(CSTR("Some devices do not have reading yet"), CSTR("Error"), me);
			return;
		}
		if (i > 0)
		{
			sb.AppendC(UTF8STRC("|"));
		}
		sb.AppendU64(dev->devId);
		sb.AppendC(UTF8STRC(","));
		sb.AppendI32((Int32)dev->handType);
		sb.AppendC(UTF8STRC(","));
		sb.AppendUOSInt(dev->nReading);
		sb.AppendC(UTF8STRC(","));
		sb.AppendU16(dev->shortAddr);
		i++;
	}
	sb.AppendC(UTF8STRC("\r\n"));
	me->devMut.UnlockRead();


	Int32 status = 0;
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(me->core->GetSocketFactory(), me->ssl, CSTR_NULL, false, url.StartsWith(UTF8STRC("https://")));
	cli->Connect(url.ToCString(), Net::WebUtil::RequestMethod::HTTP_POST, 0, 0, false);
	cli->AddHeaderC(CSTR("Iot-Program"), CSTR("margorpnomis"));
	if (cli->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Some devices do not have reading yet"), CSTR("Error"), me);
		status = -1;
	}
	else
	{
		cli->AddContentLength(sb.GetLength());
		cli->Write(sb.ToString(), sb.GetLength());
		cli->EndRequest(0, 0);
		status = cli->GetRespStatus();
	}
	cli.Delete();
	if (status == -1)
	{

	}
	else if (status == 200)
	{
		UI::MessageDialog::ShowDialog(CSTR("Uploaded successfully"), CSTR("Upload"), me);
	}
	else
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error, server response "));
		sb.AppendI32(status);
		UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Upload"), me);
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::LoadFile()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("snb.dat"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		UInt64 flen = fs.GetLength();
		if (flen > 0 && (flen % 12) == 0)
		{
			UInt8 *dataBuff = MemAlloc(UInt8, (UOSInt)flen);
			UOSInt i;
			fs.Read(dataBuff, (UOSInt)flen);
			this->devMut.LockWrite();
			i = 0;
			while (i < flen)
			{
				this->devHandlerMap.Put(ReadUInt64(&dataBuff[i]), ReadInt32(&dataBuff[i + 8]));
				i += 12;
			}
			this->devMut.UnlockWrite();
			MemFree(dataBuff);
		}
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::SaveFile()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt8 *dataBuff;
	this->devMut.LockRead();
	i = 0;
	j = this->devHandlerMap.GetCount();
	k = 0;
	dataBuff = MemAlloc(UInt8, j * 12);
	while (i < j)
	{
		WriteUInt64(&dataBuff[k], this->devHandlerMap.GetKey(i));
		WriteInt32(&dataBuff[k + 8], this->devHandlerMap.GetItem(i));
		i++;
		k += 12;
	}

	this->devMut.UnlockRead();

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("snb.dat"));
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.Write(dataBuff, k);
	}
	MemFree(dataBuff);
}

SSWR::AVIRead::AVIRSNBDongleForm::AVIRSNBDongleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, NotNullPtr<IO::Stream> stm) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SnB Dongle"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->stm = stm;
	this->devChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->dongleUpdated = false;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevice->SetRect(0, 0, 100, 103, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDongleInfo, UI::GUIButton(ui, this->pnlDevice, CSTR("Get Dongle Info")));
	this->btnDongleInfo->SetRect(4, 4, 95, 23, false);
	this->btnDongleInfo->HandleButtonClick(OnDongleInfoClicked, this);
	NEW_CLASS(this->lblDongleId, UI::GUILabel(ui, this->pnlDevice, CSTR("Dongle Id")));
	this->lblDongleId->SetRect(104, 4, 80, 23, false);
	NEW_CLASS(this->txtDongleId, UI::GUITextBox(ui, this->pnlDevice, CSTR("")));
	this->txtDongleId->SetRect(184, 4, 200, 23, false);
	this->txtDongleId->SetReadOnly(true);

	NEW_CLASS(this->btnCheckDongle, UI::GUIButton(ui, this->pnlDevice, CSTR("Check Dongle")));
	this->btnCheckDongle->SetRect(4, 28, 75, 23, false);
	this->btnCheckDongle->HandleButtonClick(OnCheckDongleClicked, this);
	NEW_CLASS(this->btnCheckDevices, UI::GUIButton(ui, this->pnlDevice, CSTR("Check Device")));
	this->btnCheckDevices->SetRect(84, 28, 75, 23, false);
	this->btnCheckDevices->HandleButtonClick(OnCheckDevicesClicked, this);
	NEW_CLASS(this->btnResetNetwork, UI::GUIButton(ui, this->pnlDevice, CSTR("Reset Network")));
	this->btnResetNetwork->SetRect(164, 28, 75, 23, false);
	this->btnResetNetwork->HandleButtonClick(OnResetNetworkClicked, this);
	NEW_CLASS(this->btnAddDevice, UI::GUIButton(ui, this->pnlDevice, CSTR("AddDevice")));
	this->btnAddDevice->SetRect(244, 28, 75, 23, false);
	this->btnAddDevice->HandleButtonClick(OnAddDeviceClicked, this);
	NEW_CLASS(this->lblRemarks, UI::GUILabel(ui, this->pnlDevice, CSTR("Remarks")));
	this->lblRemarks->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtRemarks, UI::GUITextBox(ui, this->pnlDevice, CSTR("")));
	this->txtRemarks->SetRect(104, 52, 600, 23, false);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlDevice, CSTR("URL")));
	this->lblURL->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlDevice, CSTR("")));
	this->txtURL->SetRect(104, 76, 600, 23, false);
	NEW_CLASS(this->btnUpload, UI::GUIButton(ui, this->pnlDevice, CSTR("Upload")));
	this->btnUpload->SetRect(704, 76, 75, 23, false);
	this->btnUpload->HandleButtonClick(OnUploadClicked, this);
	NEW_CLASS(this->grpDevice, UI::GUIGroupBox(ui, this->tpDevice, CSTR("Device")));
	this->grpDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlDevCtrl, UI::GUIPanel(ui, this->grpDevice));
	this->pnlDevCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlDevCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnCopyDevId, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Copy Device Id")));
	this->btnCopyDevId->SetRect(4, 4, 95, 23, false);
	this->btnCopyDevId->HandleButtonClick(OnCopyDevIdClicked, this);
	NEW_CLASS(this->btnDevReportTime, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Get Report Time")));
	this->btnDevReportTime->SetRect(104, 4, 95, 23, false);
	this->btnDevReportTime->HandleButtonClick(OnDevReportTimeClicked, this);
	NEW_CLASS(this->btnDevSetReportTime, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Set Report Time (30s)")));
	this->btnDevSetReportTime->SetRect(204, 4, 115, 23, false);
	this->btnDevSetReportTime->HandleButtonClick(OnDevSetReportTimeClicked, this);
	NEW_CLASS(this->btnDevOn, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Turn On")));
	this->btnDevOn->SetRect(324, 4, 75, 23, false);
	this->btnDevOn->HandleButtonClick(OnDevOnClicked, this);
	NEW_CLASS(this->btnDevOff, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Turn Off")));
	this->btnDevOff->SetRect(404, 4, 75, 23, false);
	this->btnDevOff->HandleButtonClick(OnDevOffClicked, this);
	NEW_CLASS(this->btnDevStatus, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Get Status")));
	this->btnDevStatus->SetRect(484, 4, 75, 23, false);
	this->btnDevStatus->HandleButtonClick(OnDevStatusClicked, this);
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->grpDevice, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->AddColumn(CSTR("Device Id"), 120);
	this->lvDevice->AddColumn(CSTR("Short Addr"), 60);
	this->lvDevice->AddColumn(CSTR("Handle Type"), 120);
	this->lvDevice->AddColumn(CSTR("Reading Time"), 120);
	this->lvDevice->AddColumn(CSTR("Values"), 400);
	this->lvDevice->HandleDblClk(OnDeviceDblClk, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogClicked, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);

	this->LoadFile();
	NEW_CLASS(this->snb, IO::SNBDongle(stm, this));
	this->snb->HandleProtocolReceived(OnProtocolReceived, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->snb->SendGetDongleInfo();
}

SSWR::AVIRead::AVIRSNBDongleForm::~AVIRSNBDongleForm()
{
	this->SaveFile();
	DEL_CLASS(this->snb);
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
	DeviceInfo *dev;
	UOSInt i = this->devMap.GetCount();
	while (i-- > 0)
	{
		dev = this->devMap.GetItem(i);
		DEL_CLASS(dev);
	}
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSNBDongleForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceAdded(UInt64 devId)
{
	DeviceInfo *dev;
	this->devMut.LockWrite();
	dev = this->devMap.Get(devId);
	if (dev == 0)
	{
		NEW_CLASS(dev, DeviceInfo());
		dev->devId = devId;
		dev->shortAddr = 0;
		dev->handType = (IO::SNBDongle::HandleType)this->devHandlerMap.Get(devId);
		dev->nReading = 0;
		dev->readingChg = false;
		dev->readingTime = 0;
		this->devMap.Put(devId, dev);
		if (dev->handType != IO::SNBDongle::HT_UNKNOWN)
		{
			this->snb->SetDevHandleType(devId, dev->handType);
		}
		this->devChg = true;
	}
	this->devMut.UnlockWrite();
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceSensor(UInt64 devId, IO::SNBDongle::SensorType sensorType, UOSInt nReading, IO::SNBDongle::ReadingType *readingTypes, Double *readingVals)
{
	DeviceInfo *dev;
	this->devMut.LockRead();
	dev = this->devMap.Get(devId);
	this->devMut.UnlockRead();
	if (dev)
	{
		Data::DateTime dt;
		UOSInt i;
		dt.SetCurrTimeUTC();

		dev->mut.LockWrite();
		dev->readingChg = true;
		dev->readingTime = dt.ToTicks();
		dev->sensorType = sensorType;
		dev->nReading = nReading;
		i = 0;
		while (i < nReading)
		{
			dev->readings[i] = readingVals[i];
			dev->readingTypes[i] = readingTypes[i];
			i++;
		}
		dev->mut.UnlockWrite();
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceUpdated(UInt64 devId, UInt16 shortAddr)
{
	DeviceInfo *dev;
	this->devMut.LockRead();
	dev = this->devMap.Get(devId);
	this->devMut.UnlockRead();
	if (dev)
	{
		dev->mut.LockWrite();
		dev->shortAddr = shortAddr;
		dev->shortAddrChg = true;
		dev->mut.UnlockWrite();
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::DongleInfoUpdated()
{
	this->dongleUpdated = true;
}
