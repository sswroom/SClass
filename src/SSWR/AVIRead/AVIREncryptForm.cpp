#include "Stdafx.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIREncryptForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIREncryptForm *me = (SSWR::AVIRead::AVIREncryptForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt8 *decBuff;
	UOSInt buffSize;
	Text::TextBinEnc::ITextBinEnc *srcEnc = (Text::TextBinEnc::ITextBinEnc*)me->cboSrc->GetSelectedItem();
	Text::TextBinEnc::ITextBinEnc *destEnc = (Text::TextBinEnc::ITextBinEnc*)me->cboDest->GetSelectedItem();
	me->txtSrc->GetText(&sb);
	if (srcEnc == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select source encryption", (const UTF8Char*)"Encrypt", me);
	}
	else if (destEnc == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select dest encryption", (const UTF8Char*)"Encrypt", me);
	}
	else if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter source text", (const UTF8Char*)"Encrypt", me);
	}
	else
	{
		buffSize = srcEnc->CalcBinSize(sb.ToString());
		if (buffSize > 0)
		{
			decBuff = MemAlloc(UInt8, buffSize);
			if (srcEnc->DecodeBin(sb.ToString(), decBuff) != buffSize)
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in decrypting the text", (const UTF8Char*)"Encrypt", me);
			}
			else
			{
				sb.ClearStr();
				destEnc->EncodeBin(&sb, decBuff, buffSize);
				me->txtDest->SetText(sb.ToString());
			}
			MemFree(decBuff);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Unsupported decryption", (const UTF8Char*)"Encrypt", me);
		}
	}
}

SSWR::AVIRead::AVIREncryptForm::AVIREncryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Text Encrypt");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->encList, Text::TextBinEnc::TextBinEncList());
	NEW_CLASS(this->pnlSrc, UI::GUIPanel(ui, this));
	this->pnlSrc->SetRect(0, 0, 512, 100, false);
	this->pnlSrc->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->pnlSrcCtrl, UI::GUIPanel(ui, this->pnlSrc));
	this->pnlSrcCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlSrcCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlDest, UI::GUIPanel(ui, this));
	this->pnlDest->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlDestCtrl, UI::GUIPanel(ui, this->pnlDest));
	this->pnlDestCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlDestCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSrc, UI::GUILabel(ui, this->pnlSrcCtrl, (const UTF8Char*)"Source Encryption"));
	this->lblSrc->SetRect(0, 0, 100, 23, false);
	this->lblSrc->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->cboSrc, UI::GUIComboBox(ui, this->pnlSrcCtrl, false));
	this->cboSrc->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtSrc, UI::GUITextBox(ui, this->pnlSrc, (const UTF8Char*)"", true));
	this->txtSrc->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblDest, UI::GUILabel(ui, this->pnlDestCtrl, (const UTF8Char*)"Dest Encryption"));
	this->lblDest->SetRect(0, 0, 100, 23, false);
	this->lblDest->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, this->pnlDestCtrl, (const UTF8Char*)"&Convert"));
	this->btnConvert->SetRect(0, 0, 75, 23, false);
	this->btnConvert->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
	NEW_CLASS(this->cboDest, UI::GUIComboBox(ui, this->pnlDestCtrl, false));
	this->cboDest->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->txtDest, UI::GUITextBox(ui, this->pnlDest, (const UTF8Char*)"", true));
	this->txtDest->SetReadOnly(true);
	this->txtDest->SetDockType(UI::GUIControl::DOCK_FILL);

	Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *encs = this->encList->GetEncList();
	Text::TextBinEnc::ITextBinEnc *enc;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItem(i);
		this->cboSrc->AddItem(enc->GetName(), enc);
		this->cboDest->AddItem(enc->GetName(), enc);
		i++;
	}
	if (j > 0)
	{
		this->cboSrc->SetSelectedIndex(0);
		this->cboDest->SetSelectedIndex(0);
	}

}

SSWR::AVIRead::AVIREncryptForm::~AVIREncryptForm()
{
	DEL_CLASS(this->encList);
}

void SSWR::AVIRead::AVIREncryptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
