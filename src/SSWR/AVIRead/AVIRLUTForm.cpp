#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRLUTForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRLUTForm::OnChannelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLUTForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLUTForm>();
	me->UpdateValues();
}

void SSWR::AVIRead::AVIRLUTForm::UpdateValues()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt currCh = this->cboChannels->GetSelectedItem().GetUOSInt();
	UOSInt inputLev = this->lut->GetInputLevel();
	UOSInt outputCh = this->lut->GetOutputCh();
	Media::LUT::DataFormat fmt = this->lut->GetFormat();
	UInt32 inputValues[4];
	inputValues[0] = 0;
	inputValues[1] = 0;
	inputValues[2] = 0;
	inputValues[3] = 0;
	this->lvValues->ClearItems();
	if (fmt == Media::LUT::DF_UINT8)
	{
		UInt8 values[4];
		if (currCh == (UOSInt)-1)
		{
			currCh = this->lut->GetInputCh();
			while (inputValues[0] < inputLev)
			{
				this->lut->GetValueUInt8(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					sptr = Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, 1, CSTRP(sbuff, sptr));
					i++;
				}
				
				i = currCh;
				while (i-- > 0)
				{
					inputValues[i]++;
				}
			}
		}
		else
		{
			while (inputValues[currCh] < inputLev)
			{
				this->lut->GetValueUInt8(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[currCh]);
				i = 1;
				while (i < outputCh)
				{
					sptr = Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i, CSTRP(sbuff, sptr));
					i++;
				}
				
				inputValues[currCh]++;
			}
		}
	}
	else if (fmt == Media::LUT::DF_UINT16)
	{
		UInt16 values[4];
		if (currCh == (UOSInt)-1)
		{
			currCh = this->lut->GetInputCh();
			while (inputValues[0] < inputLev)
			{
				this->lut->GetValueUInt16(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					sptr = Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, CSTRP(sbuff, sptr));
					i++;
				}
				
				i = currCh;
				while (i-- > 0)
				{
					inputValues[i]++;
				}
			}
		}
		else
		{
			while (inputValues[currCh] < inputLev)
			{
				this->lut->GetValueUInt16(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[currCh]);
				i = 0;
				while (i < outputCh)
				{
					sptr = Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, CSTRP(sbuff, sptr));
					i++;
				}
				
				inputValues[currCh]++;
			}
		}
	}
	else if (fmt == Media::LUT::DF_SINGLE)
	{
		Single values[4];
		if (currCh == (UOSInt)-1)
		{
			currCh = this->lut->GetInputCh();
			while (inputValues[0] < inputLev)
			{
				this->lut->GetValueSingle(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					sptr = Text::StrDouble(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, CSTRP(sbuff, sptr));
					i++;
				}
				
				i = currCh;
				while (i-- > 0)
				{
					inputValues[i]++;
				}
			}
		}
		else
		{
			while (inputValues[currCh] < inputLev)
			{
				this->lut->GetValueSingle(inputValues, values);
				sptr = Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)inputValues[currCh]);
				i = 0;
				while (i < outputCh)
				{
					sptr = Text::StrDouble(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, CSTRP(sbuff, sptr));
					i++;
				}
				
				inputValues[currCh]++;
			}
		}
	}
}

SSWR::AVIRead::AVIRLUTForm::AVIRLUTForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Media::LUT *lut) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	this->lut = lut;
	sptr = lut->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("LUT Viewer - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->pnlInfo = ui->NewPanel(this->tpInfo);
	this->pnlInfo->SetRect(0, 0, 100, 36, false);
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFileName = ui->NewLabel(this->pnlInfo, CSTR("File Name"));
	this->lblFileName->SetRect(4, 4, 100, 23, false);
	this->txtFileName = ui->NewTextBox(this->pnlInfo, CSTR(""));
	this->txtFileName->SetRect(104, 4, 500, 23, false);
	this->txtFileName->SetReadOnly(true);
	this->txtRemark = ui->NewTextBox(this->tpInfo, CSTR(""), true);
	this->txtRemark->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtRemark->SetReadOnly(true);

	this->tpValues = this->tcMain->AddTabPage(CSTR("Values"));
	this->pnlValues = ui->NewPanel(this->tpValues);
	this->pnlValues->SetRect(0, 0, 100, 32, false);
	this->pnlValues->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboChannels = ui->NewComboBox(this->pnlValues, false);
	this->cboChannels->SetRect(4, 4, 100, 23, false);
	this->cboChannels->HandleSelectionChange(OnChannelChg, this);
	this->lvValues = ui->NewListView(this->tpValues, UI::ListViewStyle::Table, 2 + this->lut->GetInputCh());
	this->lvValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvValues->SetFullRowSelect(true);

	this->lvValues->AddColumn(CSTR("Input"), 60);
	i = 0;
	j = this->lut->GetInputCh();
	while (i < j)
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Channel ")), i);
		this->cboChannels->AddItem(CSTRP(sbuff, sptr), (void*)i);
		this->lvValues->AddColumn(CSTRP(sbuff, sptr), 60);
		i++;
	}
	this->cboChannels->AddItem(CSTR("All Channels"), (void*)-1);
	this->cboChannels->SetSelectedIndex(0);
	
	this->txtFileName->SetText(this->lut->GetSourceNameObj()->ToCString());
	this->txtRemark->SetText(Text::String::OrEmpty(this->lut->GetRemark())->ToCString());
	this->UpdateValues();
}

SSWR::AVIRead::AVIRLUTForm::~AVIRLUTForm()
{
	DEL_CLASS(this->lut);
}

void SSWR::AVIRead::AVIRLUTForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
