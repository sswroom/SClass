#include "Stdafx.h"
#include "Crypto/Hash/HashCreator.h"
#include "SSWR/AVIRead/AVIRTextHashForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRTextHashForm::OnGenerateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRTextHashForm> me = userObj.GetNN<SSWR::AVIRead::AVIRTextHashForm>();
	UInt8 buff[64];
	Text::StringBuilderUTF8 sb;
	me->txtText->GetText(sb);
	if (sb.GetLength() <= 0)
	{
		me->txtText->Focus();
		return;
	}
	UIntOS i = me->cboHashType->GetSelectedIndex();
	if (i == INVALID_INDEX)
	{
		me->cboHashType->Focus();
		return;
	}
	NN<Text::TextBinEnc::TextBinEnc> srcEnc;
	if (!me->cboEncrypt->GetSelectedItem().GetOpt<Text::TextBinEnc::TextBinEnc>().SetTo(srcEnc))
	{
		me->ui->ShowMsgOK(CSTR("Please select text encryption"), CSTR("Text Hash"), me);
		return;
	}
	UIntOS buffSize = srcEnc->CalcBinSize(sb.ToCString());
	if (buffSize > 0)
	{
		NN<Crypto::Hash::HashAlgorithm> hash;
		UInt8 *decBuff = MemAlloc(UInt8, buffSize);
		if (srcEnc->DecodeBin(sb.ToCString(), decBuff) != buffSize)
		{
			me->ui->ShowMsgOK(CSTR("Error in decrypting the text"), CSTR("Text Hash"), me);
		}
		else if (!Crypto::Hash::HashCreator::CreateHash((Crypto::Hash::HashType)me->cboHashType->GetItem(i).GetIntOS()).SetTo(hash))
		{
			me->ui->ShowMsgOK(CSTR("Hash Type not supported"), CSTR("Text Hash"), me);
		}
		else
		{
			hash->Calc(decBuff, buffSize);
			hash->GetValue(buff);
			sb.ClearStr();
			sb.AppendHexBuff(buff, hash->GetResultSize(), 0, Text::LineBreakType::None);
			hash.Delete();
			me->txtHashValue->SetText(sb.ToCString());
		}
		MemFree(decBuff);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unsupported decryption"), CSTR("Text Hash"), me);
	}
}

SSWR::AVIRead::AVIRTextHashForm::AVIRTextHashForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Text Hash"));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblText = ui->NewLabel(*this, CSTR("Text"));
	this->lblText->SetRect(4, 4, 100, 23, false);
	this->lblText->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 104, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtText = ui->NewTextBox(*this, CSTR(""), true);
	this->txtText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblEncrypt = ui->NewLabel(this->pnlControl, CSTR("Text Encrypt"));
	this->lblEncrypt->SetRect(4, 4, 100, 23, false);
	this->cboEncrypt = ui->NewComboBox(this->pnlControl, false);
	this->cboEncrypt->SetRect(104, 4, 300, 23, false);
	this->lblHashType = ui->NewLabel(this->pnlControl, CSTR("Hash Type"));
	this->lblHashType->SetRect(4, 28, 100, 23, false);
	this->cboHashType = ui->NewComboBox(this->pnlControl, false);
	this->cboHashType->SetRect(104, 28, 300, 23, false);
	this->btnGenerate = ui->NewButton(this->pnlControl, CSTR("Generate"));
	this->btnGenerate->SetRect(104, 52, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);
	this->lblHashValue = ui->NewLabel(this->pnlControl, CSTR("Hash Value"));
	this->lblHashValue->SetRect(4, 76, 100, 23, false);
	this->txtHashValue = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtHashValue->SetRect(104, 76, 400, 23, false);
	this->txtHashValue->SetReadOnly(true);

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Crypto::Hash::HashAlgorithm> hash;
	Crypto::Hash::HashType currHash = Crypto::Hash::HashType::First;
	UIntOS i;
	UIntOS j;
	while (currHash <= Crypto::Hash::HashType::Last)
	{
		if (Crypto::Hash::HashCreator::CreateHash(currHash).SetTo(hash))
		{
			sptr = hash->GetName(sbuff);
			this->cboHashType->AddItem(CSTRP(sbuff, sptr), (void*)currHash);
			hash.Delete();
		}
		currHash = (Crypto::Hash::HashType)((IntOS)currHash + 1);
	}
	NN<Data::ArrayListNN<Text::TextBinEnc::TextBinEnc>> encs = this->encList.GetEncList();
	NN<Text::TextBinEnc::TextBinEnc> enc;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItemNoCheck(i);
		this->cboEncrypt->AddItem(enc->GetName(), enc);
		i++;
	}
	if (j > 0)
	{
		this->cboEncrypt->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRTextHashForm::~AVIRTextHashForm()
{
}

void SSWR::AVIRead::AVIRTextHashForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
