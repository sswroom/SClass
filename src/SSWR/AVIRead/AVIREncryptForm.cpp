#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "SSWR/AVIRead/AVIREncryptForm.h"
#include "SSWR/AVIRead/AVIRHexViewerForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIREncryptForm::OnConvertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptForm>();
	Text::StringBuilderUTF8 sb;
	UInt8 *decBuff;
	UOSInt buffSize;
	NN<Text::TextBinEnc::TextBinEnc> srcEnc;
	Optional<Text::TextBinEnc::TextBinEnc> destEnc = me->cboDest->GetSelectedItem().GetOpt<Text::TextBinEnc::TextBinEnc>();
	NN<Text::TextBinEnc::TextBinEnc> nndestEnc;
	me->txtSrc->GetText(sb);
	if (!me->cboSrc->GetSelectedItem().GetOpt<Text::TextBinEnc::TextBinEnc>().SetTo(srcEnc))
	{
		me->ui->ShowMsgOK(CSTR("Please select source encryption"), CSTR("Encrypt"), me);
	}
	else if (destEnc.IsNull() && me->cboDest->GetSelectedIndex() != me->fileIndex)
	{
		me->ui->ShowMsgOK(CSTR("Please select dest encryption"), CSTR("Encrypt"), me);
	}
	else if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter source text"), CSTR("Encrypt"), me);
	}
	else
	{
		buffSize = srcEnc->CalcBinSize(sb.ToCString());
		if (buffSize > 0)
		{
			decBuff = MemAlloc(UInt8, buffSize);
			if (srcEnc->DecodeBin(sb.ToCString(), decBuff) != buffSize)
			{
				me->ui->ShowMsgOK(CSTR("Error in decrypting the text"), CSTR("Encrypt"), me);
			}
			else if (!destEnc.SetTo(nndestEnc))
			{
				NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"TextEncFile", true);
				if (dlg->ShowDialog(me->GetHandle()))
				{
					IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					if (fs.Write(Data::ByteArrayR(decBuff, buffSize)) != buffSize)
					{
						me->ui->ShowMsgOK(CSTR("Error in writing to file"), CSTR("Encrypt"), me);
					}
				}
				dlg.Delete();
			}
			else
			{
				sb.ClearStr();
				nndestEnc->EncodeBin(sb, decBuff, buffSize);
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

void __stdcall SSWR::AVIRead::AVIREncryptForm::OnHexClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREncryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIREncryptForm>();
	Text::StringBuilderUTF8 sb;
	UInt8 *decBuff;
	UOSInt buffSize;
	NN<Text::TextBinEnc::TextBinEnc> srcEnc;
	me->txtSrc->GetText(sb);
	if (!me->cboSrc->GetSelectedItem().GetOpt<Text::TextBinEnc::TextBinEnc>().SetTo(srcEnc))
	{
		me->ui->ShowMsgOK(CSTR("Please select source encryption"), CSTR("Encrypt"), me);
	}
	else if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter source text"), CSTR("Encrypt"), me);
	}
	else
	{
		buffSize = srcEnc->CalcBinSize(sb.ToCString());
		if (buffSize > 0)
		{
			decBuff = MemAlloc(UInt8, buffSize);
			if (srcEnc->DecodeBin(sb.ToCString(), decBuff) != buffSize)
			{
				me->ui->ShowMsgOK(CSTR("Error in decrypting the text"), CSTR("Encrypt"), me);
			}
			else
			{
				IO::StmData::MemoryDataCopy fd(decBuff, buffSize);
				NN<SSWR::AVIRead::AVIRHexViewerForm> frm;
				NEW_CLASSNN(frm, SSWR::AVIRead::AVIRHexViewerForm(0, me->ui, me->core));
				frm->SetData(fd, 0);
				me->core->ShowForm(frm);
			}
			MemFree(decBuff);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Unsupported decryption"), CSTR("Encrypt"), me);
		}
	}
}

SSWR::AVIRead::AVIREncryptForm::AVIREncryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Text Encrypt"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->pnlSrc = ui->NewPanel(*this);
	this->pnlSrc->SetRect(0, 0, 512, 100, false);
	this->pnlSrc->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->pnlSrcCtrl = ui->NewPanel(this->pnlSrc);
	this->pnlSrcCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlSrcCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->pnlDest = ui->NewPanel(*this);
	this->pnlDest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlDestCtrl = ui->NewPanel(this->pnlDest);
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
	this->btnHex = ui->NewButton(this->pnlDestCtrl, CSTR("&Hex"));
	this->btnHex->SetRect(0, 0, 75, 23, false);
	this->btnHex->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnHex->HandleButtonClick(OnHexClicked, this);
	this->btnConvert = ui->NewButton(this->pnlDestCtrl, CSTR("&Convert"));
	this->btnConvert->SetRect(0, 0, 75, 23, false);
	this->btnConvert->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
	this->cboDest = ui->NewComboBox(this->pnlDestCtrl, false);
	this->cboDest->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDest = ui->NewTextBox(this->pnlDest, CSTR(""), true);
	this->txtDest->SetReadOnly(true);
	this->txtDest->SetDockType(UI::GUIControl::DOCK_FILL);

	NN<Data::ArrayListNN<Text::TextBinEnc::TextBinEnc>> encs = this->encList.GetEncList();
	NN<Text::TextBinEnc::TextBinEnc> enc;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItemNoCheck(i);
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
