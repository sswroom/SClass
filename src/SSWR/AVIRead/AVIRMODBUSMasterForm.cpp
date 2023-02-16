#include "Stdafx.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/MODBUSTCPMaster.h"
#include "IO/SerialPort.h"
#include "IO/Device/AMGU4241.h"
#include "IO/Device/ED516.h"
#include "IO/Device/ED527.h"
#include "IO/Device/ED538.h"
#include "IO/Device/ED588.h"
#include "IO/Device/SDM120M.h"
#include "SSWR/AVIRead/AVIRMODBUSMasterForm.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->devStm)
	{
		me->StopStream(true);
	}
	else
	{
		IO::StreamType st;
		me->devStm = me->core->OpenStream(&st, me, 0, false);
		if (me->devStm)
		{
			NEW_CLASS(me->stm, IO::DataCaptureStream(me->devStm, OnDataRecv, OnDataSend, me));
			if (me->radMODBUSTCP->IsSelected())
			{
				NEW_CLASS(me->modbus, IO::MODBUSTCPMaster(me->stm));
			}
			else
			{
				NEW_CLASS(me->modbus, IO::MODBUSRTUMaster(me->stm));
			}
			NEW_CLASS(me->modbusCtrl, IO::MODBUSController(me->modbus));
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnU8GetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		UInt8 val;
		me->txtU8DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtU8Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtU8RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtU8Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->ReadRegisterU8(devAddr, regAddr, &val))
		{
			sb.ClearStr();
			sb.AppendU16(val);
			me->txtU8Value->SetText(sb.ToCString());
		}
		else
		{
			me->txtU8Value->SetText(CSTR("-"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnU16GetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		UInt16 val;
		me->txtU16DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtU16Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtU16RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtU16Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->ReadRegisterU16(devAddr, regAddr, &val))
		{
			sb.ClearStr();
			sb.AppendU16(val);
			me->txtU16Value->SetText(sb.ToCString());
		}
		else
		{
			me->txtU16Value->SetText(CSTR("-"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnI32GetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		Int32 val;
		me->txtI32DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtI32Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtI32RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtI32Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->ReadRegisterI32(devAddr, regAddr, &val))
		{
			sb.ClearStr();
			sb.AppendI32(val);
			me->txtI32Value->SetText(sb.ToCString());
		}
		else
		{
			me->txtI32Value->SetText(CSTR("-"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnF32GetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		Single val;
		me->txtF32DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtF32Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtF32RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtF32Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->ReadRegisterF32(devAddr, regAddr, &val))
		{
			sb.ClearStr();
			Text::SBAppendF32(&sb, val);
			me->txtF32Value->SetText(sb.ToCString());
		}
		else
		{
			me->txtF32Value->SetText(CSTR("-"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnSetU8LowClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		me->txtSetU8DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtSetU8Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtSetU8RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtSetU8Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->WriteRegisterBool(devAddr, regAddr, false))
		{
			me->txtSetU8Value->SetText(CSTR("Success"));
		}
		else
		{
			me->txtSetU8Value->SetText(CSTR("Failed"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnSetU8HighClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 devAddr;
		UInt32 regAddr;
		me->txtSetU8DevAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			me->txtSetU8Value->SetText(CSTR("DevAddr"));
		}
		sb.ClearStr();
		me->txtSetU8RegAddr->GetText(&sb);
		if (!Text::StrToUInt32(sb.ToString(), &regAddr))
		{
			me->txtSetU8Value->SetText(CSTR("RegAddr"));
		}
		if (me->modbusCtrl->WriteRegisterBool(devAddr, regAddr, true))
		{
			me->txtSetU8Value->SetText(CSTR("Success"));
		}
		else
		{
			me->txtSetU8Value->SetText(CSTR("Failed"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnDeviceAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		UInt8 devAddr;
		Text::StringBuilderUTF8 sb;
		me->txtDeviceAddr->GetText(&sb);
		if (!Text::StrToUInt8(sb.ToString(), &devAddr))
		{
			return;
		}
		DeviceType dt = (DeviceType)(OSInt)me->cboDevice->GetSelectedItem();
		switch (dt)
		{
		case DT_SDM120:
			IO::Device::SDM120M::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		case DT_AMGU4241:
			IO::Device::AMGU4241::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		case DT_ED516:
			IO::Device::ED516::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		case DT_ED527:
			IO::Device::ED527::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		case DT_ED538:
			IO::Device::ED538::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		case DT_ED588:
			IO::Device::ED588::GetDataEntries(devAddr, OnMODBUSEntry, me);
			break;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	if (me->stm)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		MODBUSEntry *entry;
		UOSInt i = 0;
		UOSInt j = me->entryList.GetCount();
		while (i < j)
		{
			Single f32Val;
			Int32 i32Val;
			UInt16 u16Val;
			UInt8 u8Val;
			entry = me->entryList.GetItem(i);
			switch (entry->dt)
			{
			case IO::MODBUSController::DT_F32:
				if (me->modbusCtrl->ReadRegisterF32(entry->devAddr, entry->regAddr, &f32Val))
				{
					sptr = Text::StrDouble(sbuff, f32Val / (Double)entry->denorm);
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_I32:
				if (me->modbusCtrl->ReadRegisterI32(entry->devAddr, entry->regAddr, &i32Val))
				{
					if (entry->denorm == 1)
					{
						sptr = Text::StrInt32(sbuff, i32Val);
					}
					else if (entry->denorm == 0)
					{
						sptr = Text::StrHexVal32(sbuff, (UInt32)i32Val);
					}
					else
					{
						sptr = Text::StrDouble(sbuff, i32Val / (Double)entry->denorm);
					}
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_U16:
				if (me->modbusCtrl->ReadRegisterU16(entry->devAddr, entry->regAddr, &u16Val))
				{
					if (entry->denorm == 1)
					{
						sptr = Text::StrUInt16(sbuff, u16Val);
					}
					else if (entry->denorm == 0)
					{
						sptr = Text::StrHexVal16(sbuff, u16Val);
					}
					else
					{
						sptr = Text::StrDouble(sbuff, u16Val / (Double)entry->denorm);
					}
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_IU32:
				if (me->modbusCtrl->ReadRegisterII32(entry->devAddr, entry->regAddr, &i32Val))
				{
					if (entry->denorm == 1)
					{
						sptr = Text::StrUInt32(sbuff, (UInt32)i32Val);
					}
					else if (entry->denorm == 0)
					{
						sptr = Text::StrHexVal32(sbuff, (UInt32)i32Val);
					}
					else
					{
						sptr = Text::StrDouble(sbuff, ((UInt32)i32Val) / (Double)entry->denorm);
					}
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_IU16:
				if (me->modbusCtrl->ReadRegisterIU16(entry->devAddr, entry->regAddr, &u16Val))
				{
					if (entry->denorm == 1)
					{
						sptr = Text::StrUInt16(sbuff, u16Val);
					}
					else if (entry->denorm == 0)
					{
						sptr = Text::StrHexVal16(sbuff, u16Val);
					}
					else
					{
						sptr = Text::StrDouble(sbuff, u16Val / (Double)entry->denorm);
					}
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_OS16:
				if (me->modbusCtrl->ReadRegisterU16(entry->devAddr, entry->regAddr, &u16Val))
				{
					if (u16Val & 0x8000)
					{
						i32Val = -(u16Val & 0x7fff);
					}
					else
					{
						i32Val = u16Val;
					}
					if (entry->denorm == 1)
					{
						sptr = Text::StrInt32(sbuff, i32Val);
					}
					else if (entry->denorm == 0)
					{
						sptr = Text::StrHexVal16(sbuff, (UInt16)i32Val);
					}
					else
					{
						sptr = Text::StrDouble(sbuff, i32Val / (Double)entry->denorm);
					}
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			case IO::MODBUSController::DT_U8:
				if (me->modbusCtrl->ReadRegisterU8(entry->devAddr, entry->regAddr, &u8Val))
				{
					sptr = Text::StrUInt16(sbuff, u8Val);
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				}
				break;
			default:
				sptr = Text::StrConcatC(sbuff, UTF8STRC("-"));
				break;
			}
			sptr = Math::Unit::UnitBase::GetUnitShortName(entry->vt, entry->unit).ConcatTo(sptr);

			me->lvDevice->SetSubItem(entry->lvIndex, 3, CSTRP(sbuff, sptr));
			i++;
		}
	}
	if (me->recvUpdated)
	{
		Text::StringBuilderUTF8 sb;
		Sync::MutexUsage mutUsage(&me->recvMut);
		me->recvBuff.ToString(&sb);
		me->recvUpdated = false;
		mutUsage.EndUse();
		me->txtRAWRecv->SetText(sb.ToCString());
	}
	if (me->sendUpdated)
	{
		Text::StringBuilderUTF8 sb;
		Sync::MutexUsage mutUsage(&me->sendMut);
		me->sendBuff.ToString(&sb);
		me->sendUpdated = false;
		mutUsage.EndUse();
		me->txtRAWSend->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnDataRecv(void *userObj, const UInt8 *data, UOSInt dataSize)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm*)userObj;
	Sync::MutexUsage mutUsage(&me->recvMut);
	me->recvBuff.AppendBytes(data, dataSize);
	me->recvUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnDataSend(void *userObj, const UInt8 *data, UOSInt dataSize)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm*)userObj;
	Sync::MutexUsage mutUsage(&me->sendMut);
	me->sendBuff.AppendBytes(data, dataSize);
	me->sendUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRMODBUSMasterForm::OnMODBUSEntry(void *userObj, Text::CString name, UInt8 devAddr, UInt32 regAddr, IO::MODBUSController::DataType dt, Math::Unit::UnitBase::ValueType vt, Int32 unit, Int32 denorm)
{
	SSWR::AVIRead::AVIRMODBUSMasterForm *me = (SSWR::AVIRead::AVIRMODBUSMasterForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	MODBUSEntry *entry = MemAlloc(MODBUSEntry, 1);
	entry->name = Text::String::New(name);
	entry->devAddr = devAddr;
	entry->regAddr = regAddr;
	entry->dt = dt;
	entry->vt = vt;
	entry->unit = unit;
	entry->denorm = denorm;
	sptr = Text::StrUInt16(sbuff, devAddr);
	entry->lvIndex = me->lvDevice->AddItem(CSTRP(sbuff, sptr), entry);
	entry->val = 0;
	me->entryList.Add(entry);
	sptr = Text::StrUInt16(sbuff, (UInt16)regAddr);
	me->lvDevice->SetSubItem(entry->lvIndex, 1, CSTRP(sbuff, sptr));
	me->lvDevice->SetSubItem(entry->lvIndex, 2, name);
	me->lvDevice->SetSubItem(entry->lvIndex, 3, CSTR("-"));
}

void SSWR::AVIRead::AVIRMODBUSMasterForm::StopStream(Bool clearUI)
{
	if (this->stm)
	{
		DEL_CLASS(this->modbusCtrl);
		DEL_CLASS(this->modbus);
		DEL_CLASS(this->stm);
		DEL_CLASS(this->devStm);
		this->stm = 0;
		this->devStm = 0;
		if (clearUI)
		{
			this->txtStream->SetText(CSTR("-"));
			this->btnStream->SetText(CSTR("&Open"));
		}
	}
}

SSWR::AVIRead::AVIRMODBUSMasterForm::AVIRMODBUSMasterForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 576, 480, ui), recvBuff(4096), sendBuff(4096)
{
	this->SetText(CSTR("MODBUS Master"));
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->stm = 0;
	this->recvUpdated = false;
	this->sendUpdated = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this, CSTR("Stream")));
	this->grpStream->SetRect(0, 0, 100, 72, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, CSTR("Stream Type")));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->radMODBUSRTU, UI::GUIRadioButton(ui, this->grpStream, CSTR("MODBUS RTU"), true));
	this->radMODBUSRTU->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->radMODBUSTCP, UI::GUIRadioButton(ui, this->grpStream, CSTR("MODBUS TCP"), false));
	this->radMODBUSTCP->SetRect(204, 28, 100, 23, false);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpGetValue = this->tcMain->AddTabPage(CSTR("GetValue"));
	NEW_CLASS(this->lblDevAddr, UI::GUILabel(ui, this->tpGetValue, CSTR("Dev Addr")));
	this->lblDevAddr->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblRegAddr, UI::GUILabel(ui, this->tpGetValue, CSTR("Reg Addr")));
	this->lblRegAddr->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->lblValue, UI::GUILabel(ui, this->tpGetValue, CSTR("Value")));
	this->lblValue->SetRect(384, 4, 100, 23, false);
	NEW_CLASS(this->lblU8Name, UI::GUILabel(ui, this->tpGetValue, CSTR("U8")));
	this->lblU8Name->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtU8DevAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("1")));
	this->txtU8DevAddr->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->txtU8RegAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("00001")));
	this->txtU8RegAddr->SetRect(204, 28, 100, 23, false);
	NEW_CLASS(this->btnU8Get, UI::GUIButton(ui, this->tpGetValue, CSTR("Get")));
	this->btnU8Get->SetRect(304, 28, 75, 23, false);
	this->btnU8Get->HandleButtonClick(OnU8GetClicked, this);
	NEW_CLASS(this->txtU8Value, UI::GUITextBox(ui, this->tpGetValue, CSTR("")));
	this->txtU8Value->SetRect(384, 28, 200, 23, false);
	this->txtU8Value->SetReadOnly(true);
	NEW_CLASS(this->lblU16Name, UI::GUILabel(ui, this->tpGetValue, CSTR("U16")));
	this->lblU16Name->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtU16DevAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("1")));
	this->txtU16DevAddr->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->txtU16RegAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("30001")));
	this->txtU16RegAddr->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->btnU16Get, UI::GUIButton(ui, this->tpGetValue, CSTR("Get")));
	this->btnU16Get->SetRect(304, 52, 75, 23, false);
	this->btnU16Get->HandleButtonClick(OnU16GetClicked, this);
	NEW_CLASS(this->txtU16Value, UI::GUITextBox(ui, this->tpGetValue, CSTR("")));
	this->txtU16Value->SetRect(384, 52, 200, 23, false);
	this->txtU16Value->SetReadOnly(true);
	NEW_CLASS(this->lblI32Name, UI::GUILabel(ui, this->tpGetValue, CSTR("I32")));
	this->lblI32Name->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtI32DevAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("1")));
	this->txtI32DevAddr->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->txtI32RegAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("30001")));
	this->txtI32RegAddr->SetRect(204, 76, 100, 23, false);
	NEW_CLASS(this->btnI32Get, UI::GUIButton(ui, this->tpGetValue, CSTR("Get")));
	this->btnI32Get->SetRect(304, 76, 75, 23, false);
	this->btnI32Get->HandleButtonClick(OnI32GetClicked, this);
	NEW_CLASS(this->txtI32Value, UI::GUITextBox(ui, this->tpGetValue, CSTR("")));
	this->txtI32Value->SetRect(384, 76, 200, 23, false);
	this->txtI32Value->SetReadOnly(true);
	NEW_CLASS(this->lblF32Name, UI::GUILabel(ui, this->tpGetValue, CSTR("F32")));
	this->lblF32Name->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtF32DevAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("1")));
	this->txtF32DevAddr->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->txtF32RegAddr, UI::GUITextBox(ui, this->tpGetValue, CSTR("30001")));
	this->txtF32RegAddr->SetRect(204, 100, 100, 23, false);
	NEW_CLASS(this->btnF32Get, UI::GUIButton(ui, this->tpGetValue, CSTR("Get")));
	this->btnF32Get->SetRect(304, 100, 75, 23, false);
	this->btnF32Get->HandleButtonClick(OnF32GetClicked, this);
	NEW_CLASS(this->txtF32Value, UI::GUITextBox(ui, this->tpGetValue, CSTR("")));
	this->txtF32Value->SetRect(384, 100, 200, 23, false);
	this->txtF32Value->SetReadOnly(true);

	this->tpSetValue = this->tcMain->AddTabPage(CSTR("SetValue"));
	NEW_CLASS(this->lblSetDevAddr, UI::GUILabel(ui, this->tpSetValue, CSTR("Dev Addr")));
	this->lblSetDevAddr->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblSetRegAddr, UI::GUILabel(ui, this->tpSetValue, CSTR("Reg Addr")));
	this->lblSetRegAddr->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->lblSetValue, UI::GUILabel(ui, this->tpSetValue, CSTR("Value")));
	this->lblSetValue->SetRect(384, 4, 100, 23, false);
	NEW_CLASS(this->lblSetU8Name, UI::GUILabel(ui, this->tpSetValue, CSTR("Bool")));
	this->lblSetU8Name->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSetU8DevAddr, UI::GUITextBox(ui, this->tpSetValue, CSTR("1")));
	this->txtSetU8DevAddr->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->txtSetU8RegAddr, UI::GUITextBox(ui, this->tpSetValue, CSTR("00001")));
	this->txtSetU8RegAddr->SetRect(204, 28, 100, 23, false);
	NEW_CLASS(this->btnSetU8Low, UI::GUIButton(ui, this->tpSetValue, CSTR("Set Low")));
	this->btnSetU8Low->SetRect(304, 28, 75, 23, false);
	this->btnSetU8Low->HandleButtonClick(OnSetU8LowClicked, this);
	NEW_CLASS(this->btnSetU8High, UI::GUIButton(ui, this->tpSetValue, CSTR("Set High")));
	this->btnSetU8High->SetRect(384, 28, 75, 23, false);
	this->btnSetU8High->HandleButtonClick(OnSetU8HighClicked, this);
	NEW_CLASS(this->txtSetU8Value, UI::GUITextBox(ui, this->tpSetValue, CSTR("")));
	this->txtSetU8Value->SetRect(464, 28, 200, 23, false);
	this->txtSetU8Value->SetReadOnly(true);


	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevice->SetRect(0, 0, 100, 31, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblDeviceAddr, UI::GUILabel(ui, this->pnlDevice, CSTR("Addr")));
	this->lblDeviceAddr->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDeviceAddr, UI::GUITextBox(ui, this->pnlDevice, CSTR("1")));
	this->txtDeviceAddr->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblDevice, UI::GUILabel(ui, this->pnlDevice, CSTR("Device")));
	this->lblDevice->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->cboDevice, UI::GUIComboBox(ui, this->pnlDevice, false));
	this->cboDevice->SetRect(304, 4, 100, 23, false);
	DeviceType dt = DT_FIRST;
	while (dt <= DT_LAST)
	{
		this->cboDevice->AddItem(DeviceTypeGetName(dt), (void*)(OSInt)dt);
		dt = (DeviceType)(dt + 1);
	}
	this->cboDevice->SetSelectedIndex(0);
	NEW_CLASS(this->btnDeviceAdd, UI::GUIButton(ui, this->pnlDevice, CSTR("Add")));
	this->btnDeviceAdd->SetRect(404, 4, 75, 23, false);
	this->btnDeviceAdd->HandleButtonClick(OnDeviceAddClicked, this);
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->tpDevice, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->SetFullRowSelect(true);
	this->lvDevice->SetShowGrid(true);
	this->lvDevice->AddColumn(CSTR("DevAddr"), 60);
	this->lvDevice->AddColumn(CSTR("RegAddr"), 60);
	this->lvDevice->AddColumn(CSTR("Name"), 150);
	this->lvDevice->AddColumn(CSTR("Value"), 150);

	this->tpRAWSend = this->tcMain->AddTabPage(CSTR("RAW Send"));
	NEW_CLASS(this->txtRAWSend, UI::GUITextBox(ui, this->tpRAWSend, CSTR(""), true));
	this->txtRAWSend->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtRAWSend->SetReadOnly(true);
	
	this->tpRAWRecv = this->tcMain->AddTabPage(CSTR("RAW Recv"));
	NEW_CLASS(this->txtRAWRecv, UI::GUITextBox(ui, this->tpRAWRecv, CSTR(""), true));
	this->txtRAWRecv->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtRAWRecv->SetReadOnly(true);

	this->AddTimer(10000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMODBUSMasterForm::~AVIRMODBUSMasterForm()
{
	StopStream(false);
	MODBUSEntry *entry;
	UOSInt i = this->entryList.GetCount();
	while (i-- > 0)
	{
		entry = this->entryList.GetItem(i);
		entry->name->Release();
		MemFree(entry);
	}
}

void SSWR::AVIRead::AVIRMODBUSMasterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Text::CString SSWR::AVIRead::AVIRMODBUSMasterForm::DeviceTypeGetName(DeviceType dt)
{
	switch (dt)
	{
	case DT_SDM120:
		return CSTR("SDM120");
	case DT_AMGU4241:
		return CSTR("AMGU4241");
	case DT_ED516:
		return CSTR("ED516");
	case DT_ED527:
		return CSTR("ED527");
	case DT_ED538:
		return CSTR("ED538");
	case DT_ED588:
		return CSTR("ED588");
	default:
		return CSTR("Unknown");
	}
}
