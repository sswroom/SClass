#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRCodeImageGenForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRCodeImageGenForm::OnCodeTypeChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCodeImageGenForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCodeImageGenForm>();
	me->codeImgGen.Delete();
	me->codeImgGen = Media::CodeImageGen::CodeImageGen::CreateGenerator((Media::CodeImageGen::CodeImageGen::CodeType)me->cboCodeType->GetSelectedItem().GetOSInt());
	NN<Media::CodeImageGen::CodeImageGen> codeImgGen;
	if (me->codeImgGen.SetTo(codeImgGen))
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Length: "));
		sb.AppendUOSInt(codeImgGen->GetMinLength());
		sb.AppendC(UTF8STRC(" - "));
		sb.AppendUOSInt(codeImgGen->GetMaxLength());
		me->lblCodeInfo->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRCodeImageGenForm::OnCodeGenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCodeImageGenForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCodeImageGenForm>();
	NN<Media::CodeImageGen::CodeImageGen> codeImgGen;
	if (me->codeImgGen.SetTo(codeImgGen))
	{
		Text::StringBuilderUTF8 sb;
		UInt32 codeWidth;
		me->txtCodeSize->GetText(sb);
		if (sb.ToUInt32(codeWidth) && codeWidth > 0)
		{
			sb.ClearStr();
			me->txtCode->GetText(sb);
			NN<Media::DrawImage> dimg;
			if (codeImgGen->GenCode(sb.ToCString(), codeWidth, me->core->GetDrawEngine()).SetTo(dimg))
			{
				Media::StaticImage *simg = dimg->ToStaticImage();
				me->pbMain->SetImage(simg, false);
				me->core->GetDrawEngine()->DeleteImage(dimg);
				SDEL_CLASS(me->simg);
				me->simg = simg;
			}
			else
			{
				me->ui->ShowMsgOK(CSTR("Error in generating the code"), CSTR("Code Image Generator"), me);
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Code size is not valid"), CSTR("Code Image Generator"), me);
		}
	}
}

SSWR::AVIRead::AVIRCodeImageGenForm::AVIRCodeImageGenForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Code Image Generator"));
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->codeImgGen = 0;
	this->simg = 0;

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 84, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbMain = ui->NewPictureBoxDD(*this, this->colorSess, true, false);
	this->pbMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblCodeType = ui->NewLabel(this->pnlMain, CSTR("Code Type"));
	this->lblCodeType->SetRect(4, 4, 100, 23, false);
	this->cboCodeType = ui->NewComboBox(this->pnlMain, false);
	this->cboCodeType->SetRect(104, 4, 100, 23, false);
	this->cboCodeType->HandleSelectionChange(OnCodeTypeChanged, this);
	this->lblCodeSize = ui->NewLabel(this->pnlMain, CSTR("Code Width"));
	this->lblCodeSize->SetRect(4, 28, 100, 23, false);
	this->txtCodeSize = ui->NewTextBox(this->pnlMain, CSTR("1"));
	this->txtCodeSize->SetRect(104, 28, 100, 23, false);
	this->lblCode = ui->NewLabel(this->pnlMain, CSTR("Code"));
	this->lblCode->SetRect(4, 52, 100, 23, false);
	this->txtCode = ui->NewTextBox(this->pnlMain, CSTR(""));
	this->txtCode->SetRect(104, 52, 400, 23, false);
	this->btnCodeGen = ui->NewButton(this->pnlMain, CSTR("Generate"));
	this->btnCodeGen->SetRect(504, 52, 75, 23, false);
	this->btnCodeGen->HandleButtonClick(OnCodeGenClicked, this);
	this->lblCodeInfo = ui->NewLabel(this->pnlMain, CSTR(""));
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
	this->codeImgGen.Delete();
	this->ClearChildren();
	SDEL_CLASS(this->simg);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRCodeImageGenForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
