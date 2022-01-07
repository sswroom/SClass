#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRLUTForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRLUTForm::OnChannelChg(void *userObj)
{
	SSWR::AVIRead::AVIRLUTForm *me = (SSWR::AVIRead::AVIRLUTForm*)userObj;
	me->UpdateValues();
}

void SSWR::AVIRead::AVIRLUTForm::UpdateValues()
{
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt currCh = (UOSInt)this->cboChannels->GetSelectedItem();
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
				Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, 1, sbuff);
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
				Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[currCh]);
				i = 1;
				while (i < outputCh)
				{
					Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i, sbuff);
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
				Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, sbuff);
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
				Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[currCh]);
				i = 0;
				while (i < outputCh)
				{
					Text::StrUInt32(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, sbuff);
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
				Text::StrUInt32(sbuff, inputValues[0]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[0]);
				i = 0;
				while (i < outputCh)
				{
					Text::StrDouble(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, sbuff);
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
				Text::StrUInt32(sbuff, inputValues[currCh]);
				j = this->lvValues->AddItem(sbuff, (void*)(OSInt)inputValues[currCh]);
				i = 0;
				while (i < outputCh)
				{
					Text::StrDouble(sbuff, values[i]);
					this->lvValues->SetSubItem(j, i + 1, sbuff);
					i++;
				}
				
				inputValues[currCh]++;
			}
		}
	}
}

SSWR::AVIRead::AVIRLUTForm::AVIRLUTForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::LUT *lut) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;
	this->lut = lut;
	lut->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("LUT Viewer - ")));
	this->SetText(sbuff);
	this->SetFont(0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->pnlInfo, UI::GUIPanel(ui, this->tpInfo));
	this->pnlInfo->SetRect(0, 0, 100, 36, false);
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this->pnlInfo, (const UTF8Char*)"File Name"));
	this->lblFileName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this->pnlInfo, (const UTF8Char*)""));
	this->txtFileName->SetRect(104, 4, 500, 23, false);
	this->txtFileName->SetReadOnly(true);
	NEW_CLASS(this->txtRemark, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", true));
	this->txtRemark->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtRemark->SetReadOnly(true);

	this->tpValues = this->tcMain->AddTabPage((const UTF8Char*)"Values");
	NEW_CLASS(this->pnlValues, UI::GUIPanel(ui, this->tpValues));
	this->pnlValues->SetRect(0, 0, 100, 32, false);
	this->pnlValues->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboChannels, UI::GUIComboBox(ui, this->pnlValues, false));
	this->cboChannels->SetRect(4, 4, 100, 23, false);
	this->cboChannels->HandleSelectionChange(OnChannelChg, this);
	NEW_CLASS(this->lvValues, UI::GUIListView(ui, this->tpValues, UI::GUIListView::LVSTYLE_TABLE, 2 + this->lut->GetInputCh()));
	this->lvValues->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvValues->SetFullRowSelect(true);

	this->lvValues->AddColumn((const UTF8Char*)"Input", 60);
	i = 0;
	j = this->lut->GetInputCh();
	while (i < j)
	{
		Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Channel ")), i);
		this->cboChannels->AddItem(sbuff, (void*)i);
		this->lvValues->AddColumn(sbuff, 60);
		i++;
	}
	this->cboChannels->AddItem((const UTF8Char*)"All Channels", (void*)-1);
	this->cboChannels->SetSelectedIndex(0);
	
	this->txtFileName->SetText(this->lut->GetSourceNameObj()->v);
	Text::String *s = this->lut->GetRemark();
	if (s)
	{
		this->txtRemark->SetText(s->v);
	}
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
