#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/StringUtil.h"
#include "Math/Math.h"
#include "UI/MessageDialog.h"
#include "SSWR/AVIRead/AVIRExportParamForm.h"

void __stdcall SSWR::AVIRead::AVIRExportParamForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRExportParamForm *me = (SSWR::AVIRead::AVIRExportParamForm *)userObj;
	UTF8Char sbuff[256];
	UOSInt i;
	UOSInt j;
	UOSInt cnt = me->exporter->GetParamCnt();
	IO::FileExporter::ParamInfo pi;
	Int32 val;
	i = 0;
	while (i < cnt)
	{
		me->exporter->GetParamInfo(i, &pi);

		if (pi.paramType == IO::FileExporter::PT_INT32)
		{
			me->ctrls[i]->GetText(sbuff);
			val = Text::StrToInt32(sbuff);
			if (!me->exporter->SetParamInt32(me->param, i, val))
			{
				Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Parameter \""), pi.name), (const UTF8Char *)"\" out of range");
				UI::MessageDialog::ShowDialog(sbuff, (const UTF8Char *)"Export Parameter", me);
				return;
			}
		}
		else if (pi.paramType == ::IO::FileExporter::PT_STRINGUTF8)
		{
		}
		else if (pi.paramType == ::IO::FileExporter::PT_SELECTION)
		{
			j = ((UI::GUIComboBox *)me->ctrls[i])->GetSelectedIndex();
			if (!me->exporter->SetParamSel(me->param, i, j))
			{
				Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char *)"Parameter \""), pi.name), (const UTF8Char *)"\" out of range");
				UI::MessageDialog::ShowDialog(sbuff, (const UTF8Char *)"Export Parameter", me);
				return;
			}
		}
		i++;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRExportParamForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRExportParamForm *me = (SSWR::AVIRead::AVIRExportParamForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRExportParamForm::AVIRExportParamForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::FileExporter *exporter, void *param) : UI::GUIForm(parent, 298, 88, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->exporter = exporter;
	this->param = param;
	this->SetText((const UTF8Char*)"Export Parameters");
	this->SetFont(0, 8.25, false);

	UTF8Char sbuff[256];
	UOSInt cnt = this->exporter->GetParamCnt();
	UOSInt i;
	UI::GUILabel *lbl;
	IO::FileExporter::ParamInfo pi;

	SetSize(298, (Int32)(88 + cnt * 24));
	this->ctrls = MemAlloc(UI::GUIControl*, this->ctrlsCnt = cnt);

	i = 0;
	while (i < cnt)
	{
		this->exporter->GetParamInfo(i, &pi);
		NEW_CLASS(lbl, UI::GUILabel(ui, this, pi.name));
		lbl->SetRect(20, (Int32)i * 24, 120, 23, false);
		//lbl->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

		if (pi.paramType == ::IO::FileExporter::PT_INT32)
		{
			UI::GUITextBox *txt;
			Text::StrInt32(sbuff, this->exporter->GetParamInt32(this->param, i));
			NEW_CLASS(txt, UI::GUITextBox(ui, this, sbuff));
			txt->SetRect(140, (Int32)i * 24, 120, 23, false);
			this->ctrls[i] = txt;
		}
		else if (pi.paramType == ::IO::FileExporter::PT_STRINGUTF8)
		{
		}
		else if (pi.paramType == ::IO::FileExporter::PT_SELECTION)
		{
			UI::GUIComboBox *cbo;
			NEW_CLASS(cbo, UI::GUIComboBox(ui, this, false));

			UOSInt j;
			j = 0;
			while (this->exporter->GetParamSelItems(this->param, i, j, sbuff))
			{
				cbo->AddItem(sbuff, 0);
				j++;
			}
			cbo->SetRect(140, (Int32)(i * 24), 120, 23, false);
			cbo->SetSelectedIndex((UOSInt)this->exporter->GetParamSel(this->param, i));
			this->ctrls[i] = cbo;
		}
		i++;
	}

	UI::GUIPanel *pnl;
	UI::GUIButton *btn;
	NEW_CLASS(pnl, UI::GUIPanel(ui, this));
	pnl->SetRect(0, 0, 292, 48, false);
	pnl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(btn, UI::GUIButton(ui, pnl, (const UTF8Char*)"OK"));
	btn->SetRect(48, 8, 75, 23, false);
	btn->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(btn, UI::GUIButton(ui, pnl, (const UTF8Char*)"Cancel"));
	btn->SetRect(168, 8, 75, 23, false);
	btn->HandleButtonClick(OnCancelClicked, this);
}

SSWR::AVIRead::AVIRExportParamForm::~AVIRExportParamForm()
{
	MemFree(this->ctrls);
}

void SSWR::AVIRead::AVIRExportParamForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
