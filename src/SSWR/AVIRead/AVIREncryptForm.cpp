#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIREncryptForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIREncryptForm *me = (SSWR::AVIRead::AVIREncryptForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt8 *decBuff;
	UOSInt buffSize;
	Text::TextBinEnc::ITextBinEnc *srcEnc = (Text::TextBinEnc::ITextBinEnc*)me->cboSrc->GetSelectedItem();
	Text::TextBinEnc::ITextBinEnc *destEnc = (Text::TextBinEnc::ITextBinEnc*)me->cboDest->GetSelectedItem();
	me->txtSrc->GetText(sb);
	if (srcEnc == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please select source encryption"), CSTR("Encrypt"), me);
	}
	else if (destEnc == 0 && me->cboDest->GetSelectedIndex() != me->fileIndex)
	{
		me->ui->ShowMsgOK(CSTR("Please select dest encryption"), CSTR("Encrypt"), me);
	}
	else if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter source text"), CSTR("Encrypt"), me);
	}
	else
	{
		buffSize = srcEnc->CalcBinSize(sb.ToString(), sb.GetLength());
		if (buffSize > 0)
		{
			decBuff = MemAlloc(UInt8, buffSize);
			if (srcEnc->DecodeBin(sb.ToString(), sb.GetLength(), decBuff) != buffSize)
			{
				me->ui->ShowMsgOK(CSTR("Error in decrypting the text"), CSTR("Encrypt"), me);
			}
			else if (destEnc == 0)
			{
				NotNullPtr<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"TextEncFile", true);
				if (dlg->ShowDialog(me->GetHandle()))
				{
					IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					if (fs.Write(decBuff, buffSize) != buffSize)
					{
						me->ui->ShowMsgOK(CSTR("Error in writing to file"), CSTR("Encrypt"), me);
					}
				}
				dlg.Delete();
			}
			else
			{
				sb.ClearStr();
				destEnc->EncodeBin(sb, decBuff, buffSize);
				me->txtDest->SetText(sb.ToCString());
			}
			MemFree(decBuff);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Unsupported decryption"), CSTR("Encrypt"), me);
		}
	}
}

SSWR::AVIRead::AVIREncryptForm::AVIREncryptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Text Encrypt"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(this->pnlSrc, UI::GUIPanel(ui, *this));
	this->pnlSrc->SetRect(0, 0, 512, 100, false);
	this->pnlSrc->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASSNN(this->pnlSrcCtrl, UI::GUIPanel(ui, this->pnlSrc));
	this->pnlSrcCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlSrcCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	NEW_CLASSNN(this->pnlDest, UI::GUIPanel(ui, *this));
	this->pnlDest->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASSNN(this->pnlDestCtrl, UI::GUIPanel(ui, this->pnlDest));
	this->pnlDestCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlDestCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSrc = ui->NewLabel(this->pnlSrcCtrl, CSTR("Source Encryption"));
	this->lblSrc->SetRect(0, 0, 100, 23, false);
	this->lblSrc->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->cboSrc = ui->NewComboBox(this->pnlSrcCtrl, false);
	this->cboSrc->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSrc = ui->NewTextBox(this->pnlSrc, CSTR(""), true);
	this->txtSrc->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblDest = ui->NewLabel(this->pnlDestCtrl, CSTR("Dest Encryption"));
	this->lblDest->SetRect(0, 0, 100, 23, false);
	this->lblDest->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnConvert = ui->NewButton(this->pnlDestCtrl, CSTR("&Convert"));
	this->btnConvert->SetRect(0, 0, 75, 23, false);
	this->btnConvert->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
	this->cboDest = ui->NewComboBox(this->pnlDestCtrl, false);
	this->cboDest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDest = ui->NewTextBox(this->pnlDest, CSTR(""), true);
	this->txtDest->SetReadOnly(true);
	this->txtDest->SetDockType(UI::GUIControl::DOCK_FILL);

	Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *encs = this->encList.GetEncList();
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
	this->fileIndex = this->cboDest->AddItem(CSTR("File Output"), 0);
}

SSWR::AVIRead::AVIREncryptForm::~AVIREncryptForm()
{
}

void SSWR::AVIRead::AVIREncryptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
