#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRFileTextEncryptForm.h"

void __stdcall SSWR::AVIRead::AVIRFileTextEncryptForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRFileTextEncryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileTextEncryptForm>();
	if (files.GetCount() > 0)
	{
		me->txtSrcFile->SetText(files[0]->ToCString());
		me->GenDestFileName(files[0]->ToCString(), me->chkDecrypt->IsChecked());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileTextEncryptForm::OnConvertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRFileTextEncryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileTextEncryptForm>();
	Text::StringBuilderUTF8 sbSrc;
	Text::StringBuilderUTF8 sbDest;
	NN<Text::TextBinEnc::ITextBinEnc> destEnc;
	me->txtSrcFile->GetText(sbSrc);
	me->txtDestFile->GetText(sbDest);
	if (!me->cboEncrypt->GetSelectedItem().GetOpt<Text::TextBinEnc::ITextBinEnc>().SetTo(destEnc))
	{
		me->ui->ShowMsgOK(CSTR("Please select encryption"), CSTR("File Text Encrypt"), me);
	}
	else if (sbSrc.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter source file"), CSTR("File Text Encrypt"), me);
	}
	else if (sbDest.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter dest file"), CSTR("File Text Encrypt"), me);
	}
	else if (sbSrc.Equals(sbDest.v, sbDest.leng))
	{
		me->ui->ShowMsgOK(CSTR("Source file cannot be same as dest file"), CSTR("File Text Encrypt"), me);
	}
	else
	{
		UInt64 len;
		IO::FileStream fs(sbSrc.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		len = fs.GetLength();
		if (len == 0)
		{
			me->ui->ShowMsgOK(CSTR("Error in opening source file"), CSTR("File Text Encrypt"), me);
			return;
		}
		if (len > 1048576)
		{
			me->ui->ShowMsgOK(CSTR("Source file is too large"), CSTR("File Text Encrypt"), me);
			return;
		}
		UOSInt buffSize = (UOSInt)len;
		Data::ByteBuffer srcBuff(buffSize);
		if (fs.Read(srcBuff) != buffSize)
		{
			me->ui->ShowMsgOK(CSTR("Error in reading source file"), CSTR("File Text Encrypt"), me);
			return;
		}
		if (me->chkDecrypt->IsChecked())
		{
			UInt8 *destBuff = MemAlloc(UInt8, buffSize << 1);
			UOSInt destSize = destEnc->DecodeBin(Text::CStringNN(srcBuff.Arr(), buffSize), destBuff);
			if (destSize == 0)
			{
				me->ui->ShowMsgOK(CSTR("Error in decrypting file"), CSTR("File Text Encrypt"), me);
				MemFree(destBuff);
				return;
			}
			IO::FileStream fs2(sbDest.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs2.WriteCont(destBuff, destSize);
			MemFree(destBuff);
		}
		else
		{
			sbSrc.ClearStr();
			destEnc->EncodeBin(sbSrc, srcBuff.Arr().Ptr(), buffSize);
			IO::FileStream fs2(sbDest.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs2.Write(sbSrc.v, sbSrc.leng);
		}

	}
}

void __stdcall SSWR::AVIRead::AVIRFileTextEncryptForm::OnDecryptChange(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRFileTextEncryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileTextEncryptForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSrcFile->GetText(sb);
	if (sb.leng > 0)
		me->GenDestFileName(sb.ToCString(), newState);
}

void SSWR::AVIRead::AVIRFileTextEncryptForm::GenDestFileName(Text::CStringNN fileName, Bool decrypt)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(fileName);
	if (decrypt)
	{
		if (sb.EndsWith(UTF8STRC(".enc")))
			sb.RemoveChars(4);
		else
			sb.Append(CSTR(".dec"));
	}
	else
	{
		sb.AppendC(UTF8STRC(".enc"));
	}
	this->txtDestFile->SetText(sb.ToCString());
}

SSWR::AVIRead::AVIRFileTextEncryptForm::AVIRFileTextEncryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 144, ui)
{
	this->SetText(CSTR("File Text Encrypt"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblSrcFile = ui->NewLabel(*this, CSTR("Source File"));
	this->lblSrcFile->SetRect(4, 4, 100, 23, false);
	this->txtSrcFile = ui->NewTextBox(*this, CSTR(""));
	this->txtSrcFile->SetRect(104, 4, 600, 23, false);
	this->lblDestFile = ui->NewLabel(*this, CSTR("Dest File"));
	this->lblDestFile->SetRect(4, 28, 100, 23, false);
	this->txtDestFile = ui->NewTextBox(*this, CSTR(""), true);
	this->txtDestFile->SetRect(104, 28, 600, 23, false);
	this->lblDecrypt = ui->NewLabel(*this, CSTR("Mode"));
	this->lblDecrypt->SetRect(4, 52, 100, 23, false);
	this->chkDecrypt = ui->NewCheckBox(*this, CSTR("Decrypt"), false);
	this->chkDecrypt->SetRect(104, 52, 150, 23, false);
	this->chkDecrypt->HandleCheckedChange(OnDecryptChange, this);
	this->lblEncrypt = ui->NewLabel(*this, CSTR("Encryption"));
	this->lblEncrypt->SetRect(4, 76, 100, 23, false);
	this->cboEncrypt = ui->NewComboBox(*this, false);
	this->cboEncrypt->SetRect(104, 76, 200, 23, false);
	this->btnConvert = ui->NewButton(*this, CSTR("&Convert"));
	this->btnConvert->SetRect(104, 100, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);

	NN<Data::ArrayListNN<Text::TextBinEnc::ITextBinEnc>> encs = this->encList.GetEncList();
	NN<Text::TextBinEnc::ITextBinEnc> enc;
	UOSInt i;
	UOSInt j;
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
	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRFileTextEncryptForm::~AVIRFileTextEncryptForm()
{
}

void SSWR::AVIRead::AVIRFileTextEncryptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
