#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRCodeImageGenForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRCodeImageGenForm::OnCodeTypeChanged(void *userObj)
{
	SSWR::AVIRead::AVIRCodeImageGenForm *me = (SSWR::AVIRead::AVIRCodeImageGenForm*)userObj;
	SDEL_CLASS(me->codeImgGen);
	me->codeImgGen = Media::CodeImageGen::CodeImageGen::CreateGenerator((Media::CodeImageGen::CodeImageGen::CodeType)(OSInt)me->cboCodeType->GetSelectedItem());
	if (me->codeImgGen)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Length: "));
		sb.AppendUOSInt(me->codeImgGen->GetMinLength());
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendUOSInt(me->codeImgGen->GetMaxLength());
		me->lblCodeInfo->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRCodeImageGenForm::OnCodeGenClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCodeImageGenForm *me = (SSWR::AVIRead::AVIRCodeImageGenForm*)userObj;
	if (me->codeImgGen)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 codeWidth;
		me->txtCodeSize->GetText(sb);
		if (sb.ToUInt32(codeWidth) && codeWidth > 0)
		{
			sb.ClearStr();
			me->txtCode->GetText(sb);
			Media::DrawImage *dimg = me->codeImgGen->GenCode(sb.ToCString(), codeWidth, me->core->GetDrawEngine());
			if (dimg)
			{
				Media::StaticImage *simg = dimg->ToStaticImage();
				me->pbMain->SetImage(simg, false);
				me->core->GetDrawEngine()->DeleteImage(dimg);
				SDEL_CLASS(me->simg);
				me->simg = simg;
			}
			else
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in generating the code"), CSTR("Code Image Generator"), me);
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Code size is not valid"), CSTR("Code Image Generator"), me);
		}
	}
}

SSWR::AVIRead::AVIRCodeImageGenForm::AVIRCodeImageGenForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Code Image Generator"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->codeImgGen = 0;
	this->simg = 0;

	NEW_CLASS(this->pnlMain, UI::GUIPanel(ui, this));
	this->pnlMain->SetRect(0, 0, 100, 84, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pbMain, UI::GUIPictureBoxDD(ui, this, this->colorSess, true, false));
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblCodeType, UI::GUILabel(ui, this->pnlMain, CSTR("Code Type")));
	this->lblCodeType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboCodeType, UI::GUIComboBox(ui, this->pnlMain, false));
	this->cboCodeType->SetRect(104, 4, 100, 23, false);
	this->cboCodeType->HandleSelectionChange(OnCodeTypeChanged, this);
	NEW_CLASS(this->lblCodeSize, UI::GUILabel(ui, this->pnlMain, CSTR("Code Width")));
	this->lblCodeSize->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCodeSize, UI::GUITextBox(ui, this->pnlMain, CSTR("1")));
	this->txtCodeSize->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblCode, UI::GUILabel(ui, this->pnlMain, CSTR("Code")));
	this->lblCode->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtCode, UI::GUITextBox(ui, this->pnlMain, CSTR("")));
	this->txtCode->SetRect(104, 52, 400, 23, false);
	NEW_CLASS(this->btnCodeGen, UI::GUIButton(ui, this->pnlMain, CSTR("Generate")));
	this->btnCodeGen->SetRect(504, 52, 75, 23, false);
	this->btnCodeGen->HandleButtonClick(OnCodeGenClicked, this);
	NEW_CLASS(this->lblCodeInfo, UI::GUILabel(ui, this->pnlMain, CSTR("")));
	this->lblCodeInfo->SetRect(584, 52, 100, 23, false);

	OSInt i;
	OSInt j;
	i = Media::CodeImageGen::CodeImageGen::CT_FIRST;
	j = Media::CodeImageGen::CodeImageGen::CT_LAST;
	while (i <= j)
	{
		this->cboCodeType->AddItem(Media::CodeImageGen::CodeImageGen::GetCodeName((Media::CodeImageGen::CodeImageGen::CodeType)i), (void*)i);
		i++;
	}
	this->cboCodeType->SetSelectedIndex(0);
}

SSWR::AVIRead::AVIRCodeImageGenForm::~AVIRCodeImageGenForm()
{
	SDEL_CLASS(this->codeImgGen);
	this->ClearChildren();
	SDEL_CLASS(this->simg);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRCodeImageGenForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
