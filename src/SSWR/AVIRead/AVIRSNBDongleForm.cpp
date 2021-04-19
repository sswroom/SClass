#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/HTTPClient.h"
#include "SSWR/AVIRead/AVIRSNBDongleForm.h"
#include "SSWR/AVIRead/AVIRSNBHandlerForm.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnProtocolReceived(void *userObj, UInt8 cmdType, OSInt cmdSize, UInt8 *cmd)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Received cmd 0x");
	sb.AppendHex8(cmdType);
	sb.Append((const UTF8Char*)": ");
	if (cmdSize > 0)
	{
		sb.AppendHex(cmd, cmdSize, ' ', Text::LBT_NONE);
	}
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_COMMAND);
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
	const UTF8Char *txt = me->lbLog->GetSelectedItemTextNew();
	if (txt)
	{
		me->txtLog->SetText(txt);
		me->lbLog->DelTextNew(txt);
	}
	else
	{
		me->txtLog->SetText(txt);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Data::ArrayList<DeviceInfo*> devList;
	DeviceInfo *dev;
	UTF8Char sbuff[64];
	Data::DateTime dt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	Text::StringBuilderUTF8 sb;

	if (me->devChg)
	{
		me->devMut->LockRead();
		devList.AddRange(me->devMap->GetValues());
		me->devChg = false;
		me->devMut->UnlockRead();

		me->lvDevice->ClearItems();
		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItem(i);
			dev->mut->LockRead();
			dev->readingChg = false;
			
			Text::StrInt64(sbuff, dev->devId);
			k = me->lvDevice->AddItem(sbuff, dev);
			Text::StrInt32(sbuff, (UInt16)dev->shortAddr);
			me->lvDevice->SetSubItem(k, 1, sbuff);
			me->lvDevice->SetSubItem(k, 2, IO::SNBDongle::GetHandleName(dev->handType));
			if (dev->readingTime)
			{
				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				me->lvDevice->SetSubItem(k, 3, sbuff);

				sb.ClearStr();
				switch (dev->sensorType)
				{
				case IO::SNBDongle::ST_DOOR_CONTACT:
					sb.Append((const UTF8Char*)"Sensor Type = Door Contact");
					break;
				case IO::SNBDongle::ST_MOTION:
					sb.Append((const UTF8Char*)"Sensor Type = Motion");
					break;
				case IO::SNBDongle::ST_SENSOR:
					sb.Append((const UTF8Char*)"Sensor Type = Mixed");
					break;
				case IO::SNBDongle::ST_TEMPERATURE:
					sb.Append((const UTF8Char*)"Sensor Type = Temperature");
					break;
				case IO::SNBDongle::ST_RETURN_NORMAL:
					sb.Append((const UTF8Char*)"Sensor Type = Return normal");
					break;
				default:
					break;
				}
				l = 0;
				while (l < dev->nReading)
				{
					if (sb.GetLength() > 0)
					{
						sb.Append((const UTF8Char*)", ");
					}
					sb.Append(IO::SNBDongle::GetReadingName(dev->readingTypes[l]));
					sb.Append((const UTF8Char*)" = ");
					Text::SBAppendF64(&sb, dev->readings[l]);
					l++;
				}
				me->lvDevice->SetSubItem(k, 4, sb.ToString());
			}
			else
			{
				me->lvDevice->SetSubItem(k, 3, L"-");
				me->lvDevice->SetSubItem(k, 4, L"-");
			}
			dev->mut->UnlockRead();
			i++;
		}
	}
	else
	{
		me->devMut->LockRead();
		devList.AddRange(me->devMap->GetValues());
		me->devMut->UnlockRead();

		i = 0;
		j = devList.GetCount();
		while (i < j)
		{
			dev = devList.GetItem(i);
			dev->mut->LockRead();
			if (dev->shortAddrChg)
			{
				dev->shortAddrChg = false;
				Text::StrInt32(sbuff, (UInt16)dev->shortAddr);
				me->lvDevice->SetSubItem(i, 1, sbuff);
			}
			if (dev->readingChg)
			{
				dev->readingChg = false;

				dt.SetTicks(dev->readingTime);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
				me->lvDevice->SetSubItem(i, 3, sbuff);

				sb.ClearStr();
				switch (dev->sensorType)
				{
				case IO::SNBDongle::ST_DOOR_CONTACT:
					sb.Append((const UTF8Char*)"Sensor Type = Door Contact");
					break;
				case IO::SNBDongle::ST_MOTION:
					sb.Append((const UTF8Char*)"Sensor Type = Motion");
					break;
				case IO::SNBDongle::ST_SENSOR:
					sb.Append((const UTF8Char*)"Sensor Type = Mixed");
					break;
				case IO::SNBDongle::ST_TEMPERATURE:
					sb.Append((const UTF8Char*)"Sensor Type = Temperature");
					break;
				case IO::SNBDongle::ST_RETURN_NORMAL:
					sb.Append((const UTF8Char*)"Sensor Type = Return normal");
					break;
				default:
					break;
				}
				l = 0;
				while (l < dev->nReading)
				{
					if (sb.GetLength() > 0)
					{
						sb.Append((const UTF8Char*)", ");
					}
					sb.Append(IO::SNBDongle::GetReadingName(dev->readingTypes[l]));
					sb.Append((const UTF8Char*)" = ");
					Text::SBAppendF64(&sb, dev->readings[l]);
					l++;
				}
				me->lvDevice->SetSubItem(i, 4, sb.ToString());
			}
			dev->mut->UnlockRead();
			i++;
		}
	}

	if (me->dongleUpdated)
	{
		Int64 id;
		me->dongleUpdated = false;
		id = me->snb->GetDongleId();
		if (id == 0)
		{
			me->txtDongleId->SetText((const UTF8Char*)"");
		}
		else
		{
			Text::StrInt64(sbuff, id);
			me->txtDongleId->SetText(sbuff);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnCopyDevIdClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Win32::Clipboard::SetString(me->GetHandle(), txt);
		me->lvDevice->DelTextNew(txt);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevReportTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		me->snb->SendGetReportTime(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevSetReportTimeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		me->snb->SendSetReportTime(devId, 30);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevOnClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		me->snb->SendDevTurnOn(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevOffClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		me->snb->SendDevTurnOff(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDevStatusClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetSelectedItemTextNew();
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		me->snb->SendDevGetStatus(devId);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnDeviceDblClk(void *userObj, OSInt index)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	const UTF8Char *txt = me->lvDevice->GetItemTextNew(index);
	if (txt)
	{
		Int64 devId = Text::StrToInt64(txt);
		me->lvDevice->DelTextNew(txt);

		DeviceInfo *dev;
		me->devMut->LockRead();
		dev = me->devMap->Get(devId);
		me->devMut->UnlockRead();

		if (dev)
		{
			SSWR::AVIRead::AVIRSNBHandlerForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSNBHandlerForm(0, me->ui, me->core, dev->handType));
			if (frm->ShowDialog(me) == UI::GUIForm::DR_OK)
			{
				dev->handType = frm->GetHandleType();
				me->devMut->LockWrite();
				me->devHandlerMap->Put(devId, (Int32)dev->handType);
				me->devMut->UnlockWrite();
				me->snb->SetDevHandleType(dev->devId, dev->handType);
				me->lvDevice->SetSubItem(index, 2, IO::SNBDongle::GetHandleName(dev->handType));
			}
			DEL_CLASS(frm);
	//		me->snb->SendSetReportTime(devId, 3, true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRSNBDongleForm::OnUploadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNBDongleForm *me = (SSWR::AVIRead::AVIRSNBDongleForm*)userObj;
	Int64 dongleId = me->snb->GetDongleId();
	Int32 baudRate = me->snb->GetBaudRate();
	if (dongleId == 0 || baudRate == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Dongle info missing", (const UTF8Char *)"Error", me);
		return;
	}
	Text::StringBuilderUTF8 url;
	me->txtURL->GetText(&url);
	if (!url.StartsWith((const UTF8Char *)"http://"))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"URL is not valid", (const UTF8Char *)"Error", me);
		return;
	}

	Text::StringBuilderUTF8 remarks;
	me->txtRemarks->GetText(&remarks);
	if (remarks.IndexOf('\r') >= 0 || remarks.IndexOf('\n') >= 0 || remarks.IndexOf('\'') >= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Remarks contain invalid characters", (const UTF8Char *)"Error", me);
		return;
	}
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char *)"dongle_id=");
	sb.AppendI64(dongleId);
	sb.Append((const UTF8Char *)"\r\n");
	sb.Append((const UTF8Char *)"baud_rate=");
	sb.AppendI32(baudRate);
	sb.Append((const UTF8Char *)"\r\n");
	sb.Append((const UTF8Char *)"remarks=");
	sb.Append(remarks.ToString());
	sb.Append((const UTF8Char *)"\r\n");
	sb.Append((const UTF8Char *)"sensor_list=");
	me->devMut->LockRead();
	Data::ArrayList<DeviceInfo*> *sensors = me->devMap->GetValues();
	DeviceInfo *dev;
	OSInt i;
	OSInt j;
	if (sensors->GetCount() == 0)
	{
		me->devMut->UnlockRead();
		UI::MessageDialog::ShowDialog((const UTF8Char *)"No devices found", (const UTF8Char *)"Error", me);
		return;
	}
	i = 0;
	j = sensors->GetCount();
	while (i < j)
	{
		dev = sensors->GetItem(i);
		if (dev->readingTime == 0 || dev->nReading == 0)
		{
			me->devMut->UnlockRead();
			UI::MessageDialog::ShowDialog((const UTF8Char *)"Some devices do not have reading yet", (const UTF8Char *)"Error", me);
			return;
		}
		if (i > 0)
		{
			sb.Append((const UTF8Char *)"|");
		}
		sb.AppendI64(dev->devId);
		sb.Append((const UTF8Char *)",");
		sb.AppendI32((Int32)dev->handType);
		sb.Append((const UTF8Char *)",");
		sb.AppendOSInt(dev->nReading);
		sb.Append((const UTF8Char *)",");
		sb.AppendI32(dev->shortAddr);
		i++;
	}
	sb.Append((const UTF8Char *)"\r\n");
	me->devMut->UnlockRead();


	Int32 status = 0;
	UTF8Char sbuff[32];
	Net::HTTPClient *cli;
	cli = Net::HTTPClient::CreateClient(me->core->GetSocketFactory(), 0, false, url.StartsWith((const UTF8Char*)"https://"));
	cli->Connect(url.ToString(), "POST", 0, 0, false);
	cli->AddHeader((const UTF8Char*)"Iot-Program", (const UTF8Char*)"margorpnomis");
	if (cli->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Some devices do not have reading yet", (const UTF8Char *)"Error", me);
		status = -1;
	}
	else
	{
		Text::StrOSInt(sbuff, sb.GetLength());
		cli->AddHeader((const UTF8Char *)"Content-Length", sbuff);
		cli->Write(sb.ToString(), sb.GetLength());
		cli->EndRequest(0, 0);
		status = cli->GetRespStatus();
	}
	DEL_CLASS(cli);
	if (status == -1)
	{

	}
	else if (status == 200)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char *)"Uploaded successfully", (const UTF8Char *)"Upload", me);
	}
	else
	{
		sb.ClearStr();
		sb.Append((const UTF8Char *)"Error, server response ");
		sb.AppendI32(status);
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char *)"Upload", me);
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::LoadFile()
{
	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char *)"snb.dat");
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		Int64 flen = fs->GetLength();
		if (flen > 0 && (flen % 12) == 0)
		{
			UInt8 *dataBuff = MemAlloc(UInt8, (OSInt)flen);
			OSInt i;
			fs->Read(dataBuff, (OSInt)flen);
			this->devMut->LockWrite();
			i = 0;
			while (i < flen)
			{
				this->devHandlerMap->Put(ReadInt64(&dataBuff[i]), ReadInt32(&dataBuff[i + 8]));
				i += 12;
			}
			this->devMut->UnlockWrite();
			MemFree(dataBuff);
		}
	}
	DEL_CLASS(fs);
}

void SSWR::AVIRead::AVIRSNBDongleForm::SaveFile()
{
	OSInt i;
	OSInt j;
	OSInt k;
	UInt8 *dataBuff;
	Data::ArrayList<Int64> *keys;
	Data::ArrayList<Int32> *vals;
	this->devMut->LockRead();
	i = 0;
	j = this->devHandlerMap->GetCount();
	k = 0;
	dataBuff = MemAlloc(UInt8, j * 12);
	keys = this->devHandlerMap->GetKeys();
	vals = this->devHandlerMap->GetValues();
	while (i < j)
	{
		WriteInt64(&dataBuff[k], keys->GetItem(i));
		WriteInt32(&dataBuff[k + 8], vals->GetItem(i));
		i++;
		k += 12;
	}

	this->devMut->UnlockRead();

	UTF8Char sbuff[512];
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char *)"snb.dat");
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	fs->Write(dataBuff, k);
	DEL_CLASS(fs);
	MemFree(dataBuff);
}

SSWR::AVIRead::AVIRSNBDongleForm::AVIRSNBDongleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::Stream *stm) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"SnB Dongle");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->stm = stm;
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->devMut, Sync::RWMutex());
	NEW_CLASS(this->devMap, Data::Int64Map<DeviceInfo*>());
	NEW_CLASS(this->devHandlerMap, Data::Int64Map<Int32>());
	this->devChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->dongleUpdated = false;

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDevice = this->tcMain->AddTabPage((const UTF8Char*)"Device");
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevice->SetRect(0, 0, 100, 103, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDongleInfo, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Get Dongle Info"));
	this->btnDongleInfo->SetRect(4, 4, 95, 23, false);
	this->btnDongleInfo->HandleButtonClick(OnDongleInfoClicked, this);
	NEW_CLASS(this->lblDongleId, UI::GUILabel(ui, this->pnlDevice, (const UTF8Char*)"Dongle Id"));
	this->lblDongleId->SetRect(104, 4, 80, 23, false);
	NEW_CLASS(this->txtDongleId, UI::GUITextBox(ui, this->pnlDevice, (const UTF8Char*)""));
	this->txtDongleId->SetRect(184, 4, 200, 23, false);
	this->txtDongleId->SetReadOnly(true);

	NEW_CLASS(this->btnCheckDongle, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Check Dongle"));
	this->btnCheckDongle->SetRect(4, 28, 75, 23, false);
	this->btnCheckDongle->HandleButtonClick(OnCheckDongleClicked, this);
	NEW_CLASS(this->btnCheckDevices, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Check Device"));
	this->btnCheckDevices->SetRect(84, 28, 75, 23, false);
	this->btnCheckDevices->HandleButtonClick(OnCheckDevicesClicked, this);
	NEW_CLASS(this->btnResetNetwork, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Reset Network"));
	this->btnResetNetwork->SetRect(164, 28, 75, 23, false);
	this->btnResetNetwork->HandleButtonClick(OnResetNetworkClicked, this);
	NEW_CLASS(this->btnAddDevice, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"AddDevice"));
	this->btnAddDevice->SetRect(244, 28, 75, 23, false);
	this->btnAddDevice->HandleButtonClick(OnAddDeviceClicked, this);
	NEW_CLASS(this->lblRemarks, UI::GUILabel(ui, this->pnlDevice, (const UTF8Char*)"Remarks"));
	this->lblRemarks->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtRemarks, UI::GUITextBox(ui, this->pnlDevice, (const UTF8Char*)""));
	this->txtRemarks->SetRect(104, 52, 600, 23, false);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlDevice, (const UTF8Char*)"URL"));
	this->lblURL->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlDevice, (const UTF8Char*)""));
	this->txtURL->SetRect(104, 76, 600, 23, false);
	NEW_CLASS(this->btnUpload, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Upload"));
	this->btnUpload->SetRect(704, 76, 75, 23, false);
	this->btnUpload->HandleButtonClick(OnUploadClicked, this);
	NEW_CLASS(this->grpDevice, UI::GUIGroupBox(ui, this->tpDevice, (const UTF8Char*)"Device"));
	this->grpDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlDevCtrl, UI::GUIPanel(ui, this->grpDevice));
	this->pnlDevCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlDevCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnCopyDevId, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Copy Device Id"));
	this->btnCopyDevId->SetRect(4, 4, 95, 23, false);
	this->btnCopyDevId->HandleButtonClick(OnCopyDevIdClicked, this);
	NEW_CLASS(this->btnDevReportTime, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Get Report Time"));
	this->btnDevReportTime->SetRect(104, 4, 95, 23, false);
	this->btnDevReportTime->HandleButtonClick(OnDevReportTimeClicked, this);
	NEW_CLASS(this->btnDevSetReportTime, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Set Report Time (30s)"));
	this->btnDevSetReportTime->SetRect(204, 4, 115, 23, false);
	this->btnDevSetReportTime->HandleButtonClick(OnDevSetReportTimeClicked, this);
	NEW_CLASS(this->btnDevOn, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Turn On"));
	this->btnDevOn->SetRect(324, 4, 75, 23, false);
	this->btnDevOn->HandleButtonClick(OnDevOnClicked, this);
	NEW_CLASS(this->btnDevOff, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Turn Off"));
	this->btnDevOff->SetRect(404, 4, 75, 23, false);
	this->btnDevOff->HandleButtonClick(OnDevOffClicked, this);
	NEW_CLASS(this->btnDevStatus, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Get Status"));
	this->btnDevStatus->SetRect(484, 4, 75, 23, false);
	this->btnDevStatus->HandleButtonClick(OnDevStatusClicked, this);
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->grpDevice, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->AddColumn((const UTF8Char*)"Device Id", 120);
	this->lvDevice->AddColumn((const UTF8Char*)"Short Addr", 60);
	this->lvDevice->AddColumn((const UTF8Char*)"Handle Type", 120);
	this->lvDevice->AddColumn((const UTF8Char*)"Reading Time", 120);
	this->lvDevice->AddColumn((const UTF8Char*)"Values", 400);
	this->lvDevice->HandleDblClk(OnDeviceDblClk, this);

	this->tpLog = this->tcMain->AddTabPage((const UTF8Char*)"Log");
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, (const UTF8Char*)""));
	this->txtLog->SetReadOnly(true);
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogClicked, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 300, true));
	this->log->AddLogHandler(this->logger, IO::ILogHandler::LOG_LEVEL_RAW);

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
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
	Data::ArrayList<DeviceInfo*> *devList = this->devMap->GetValues();
	DeviceInfo *dev;
	OSInt i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		DEL_CLASS(dev->mut);
		MemFree(dev);
	}
	DEL_CLASS(this->devHandlerMap);
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);
}

void SSWR::AVIRead::AVIRSNBDongleForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceAdded(Int64 devId)
{
	DeviceInfo *dev;
	this->devMut->LockWrite();
	dev = this->devMap->Get(devId);
	if (dev == 0)
	{
		dev = MemAlloc(DeviceInfo, 1);
		dev->devId = devId;
		dev->shortAddr = 0;
		NEW_CLASS(dev->mut, Sync::RWMutex());
		dev->handType = (IO::SNBDongle::HandleType)this->devHandlerMap->Get(devId);
		dev->nReading = 0;
		dev->readingChg = false;
		dev->readingTime = 0;
		this->devMap->Put(devId, dev);
		if (dev->handType != IO::SNBDongle::HT_UNKNOWN)
		{
			this->snb->SetDevHandleType(devId, dev->handType);
		}
		this->devChg = true;
	}
	this->devMut->UnlockWrite();
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceSensor(Int64 devId, IO::SNBDongle::SensorType sensorType, OSInt nReading, IO::SNBDongle::ReadingType *readingTypes, Double *readingVals)
{
	DeviceInfo *dev;
	this->devMut->LockRead();
	dev = this->devMap->Get(devId);
	this->devMut->UnlockRead();
	if (dev)
	{
		Data::DateTime dt;
		OSInt i;
		dt.SetCurrTimeUTC();

		dev->mut->LockWrite();
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
		dev->mut->UnlockWrite();
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::DeviceUpdated(Int64 devId, Int16 shortAddr)
{
	DeviceInfo *dev;
	this->devMut->LockRead();
	dev = this->devMap->Get(devId);
	this->devMut->UnlockRead();
	if (dev)
	{
		dev->mut->LockWrite();
		dev->shortAddr = shortAddr;
		dev->shortAddrChg = true;
		dev->mut->UnlockWrite();
	}
}

void SSWR::AVIRead::AVIRSNBDongleForm::DongleInfoUpdated()
{
	this->dongleUpdated = true;
}
