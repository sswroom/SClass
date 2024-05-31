#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/StringUtil.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRExportParamForm.h"

void __stdcall SSWR::AVIRead::AVIRExportParamForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExportParamForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExportParamForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt cnt = me->exporter->GetParamCnt();
	IO::FileExporter::ParamInfo pi;
	Int32 val;
	i = 0;
	while (i < cnt)
	{
		me->exporter->GetParamInfo(i, pi);

		if (pi.paramType == IO::FileExporter::ParamType::INT32)
		{
			me->ctrls[i]->GetText(sbuff);
			val = Text::StrToInt32(sbuff);
			if (!me->exporter->SetParamInt32(me->param, i, val))
			{
				sptr = Text::StrConcatC(pi.name.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Parameter \""))), UTF8STRC("\" out of range"));
				me->ui->ShowMsgOK(CSTRP(sbuff, sptr), CSTR("Export Parameter"), me);
				return;
			}
		}
		else if (pi.paramType == ::IO::FileExporter::ParamType::STRINGUTF8)
		{
		}
		else if (pi.paramType == ::IO::FileExporter::ParamType::SELECTION)
		{
			j = ((UI::GUIComboBox *)me->ctrls[i])->GetSelectedIndex();
			if (!me->exporter->SetParamSel(me->param, i, j))
			{
				sptr = Text::StrConcatC(pi.name.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Parameter \""))), UTF8STRC("\" out of range"));
				me->ui->ShowMsgOK(CSTRP(sbuff, sptr), CSTR("Export Parameter"), me);
				return;
			}
		}
		i++;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRExportParamForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExportParamForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExportParamForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRExportParamForm::AVIRExportParamForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::FileExporter> exporter, Optional<IO::FileExporter::ParamData> param) : UI::GUIForm(parent, 298, 88, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->exporter = exporter;
	this->param = param;
	this->SetText(CSTR("Export Parameters"));
	this->SetFont(0, 0, 8.25, false);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt cnt = this->exporter->GetParamCnt();
	UOSInt i;
	NN<UI::GUILabel> lbl;
	IO::FileExporter::ParamInfo pi;

	SetSize(298, (Int32)(88 + cnt * 24));
	this->ctrls = MemAlloc(UI::GUIControl*, this->ctrlsCnt = cnt);

	i = 0;
	while (i < cnt)
	{
		this->exporter->GetParamInfo(i, pi);
		lbl = ui->NewLabel(*this, pi.name);
		lbl->SetRect(20, (Int32)i * 24, 120, 23, false);
		//lbl->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;

		if (pi.paramType == ::IO::FileExporter::ParamType::INT32)
		{
			NN<UI::GUITextBox> txt;
			sptr = Text::StrInt32(sbuff, this->exporter->GetParamInt32(this->param, i));
			txt = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
			txt->SetRect(140, (Int32)i * 24, 120, 23, false);
			this->ctrls[i] = txt.Ptr();
		}
		else if (pi.paramType == ::IO::FileExporter::ParamType::STRINGUTF8)
		{
		}
		else if (pi.paramType == ::IO::FileExporter::ParamType::SELECTION)
		{
			NN<UI::GUIComboBox> cbo;
			cbo = ui->NewComboBox(*this, false);

			UOSInt j;
			j = 0;
			while (this->exporter->GetParamSelItems(this->param, i, j, sbuff).SetTo(sptr))
			{
				cbo->AddItem(CSTRP(sbuff, sptr), 0);
				j++;
			}
			cbo->SetRect(140, (Int32)(i * 24), 120, 23, false);
			cbo->SetSelectedIndex((UOSInt)this->exporter->GetParamSel(this->param, i));
			this->ctrls[i] = cbo.Ptr();
		}
		i++;
	}

	NN<UI::GUIPanel> pnl;
	NN<UI::GUIButton> btn;
	pnl = ui->NewPanel(*this);
	pnl->SetRect(0, 0, 292, 48, false);
	pnl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	btn = ui->NewButton(pnl, CSTR("OK"));
	btn->SetRect(48, 8, 75, 23, false);
	btn->HandleButtonClick(OnOKClicked, this);
	btn = ui->NewButton(pnl, CSTR("Cancel"));
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
