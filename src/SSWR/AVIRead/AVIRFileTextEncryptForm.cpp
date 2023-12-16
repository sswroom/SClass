#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRFileTextEncryptForm.h"

void __stdcall SSWR::AVIRead::AVIRFileTextEncryptForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRFileTextEncryptForm *me = (SSWR::AVIRead::AVIRFileTextEncryptForm *)userObj;
	if (nFiles > 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(files[0]);
		sb.AppendC(UTF8STRC(".enc"));
		me->txtSrcFile->SetText(files[0]->ToCString());
		me->txtDestFile->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileTextEncryptForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileTextEncryptForm *me = (SSWR::AVIRead::AVIRFileTextEncryptForm *)userObj;
	Text::StringBuilderUTF8 sbSrc;
	Text::StringBuilderUTF8 sbDest;
	Text::TextBinEnc::ITextBinEnc *destEnc = (Text::TextBinEnc::ITextBinEnc*)me->cboEncrypt->GetSelectedItem();
	me->txtSrcFile->GetText(sbSrc);
	me->txtDestFile->GetText(sbDest);
	if (destEnc == 0)
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
		Data::ByteBuffer decBuff(buffSize);
		if (fs.Read(decBuff) != buffSize)
		{
			me->ui->ShowMsgOK(CSTR("Error in reading source file"), CSTR("File Text Encrypt"), me);
			return;
		}
		sbSrc.ClearStr();
		destEnc->EncodeBin(sbSrc, decBuff.Ptr(), buffSize);

		IO::FileStream fs2(sbDest.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs2.Write(sbSrc.v, sbSrc.leng);
	}
}

SSWR::AVIRead::AVIRFileTextEncryptForm::AVIRFileTextEncryptForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 120, ui)
{
	this->SetText(CSTR("File Text Encrypt"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblSrcFile, UI::GUILabel(ui, *this, CSTR("Source File")));
	this->lblSrcFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSrcFile, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtSrcFile->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->lblDestFile, UI::GUILabel(ui, *this, CSTR("Dest File")));
	this->lblDestFile->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDestFile, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtDestFile->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->lblEncrypt, UI::GUILabel(ui, *this, CSTR("Encryption")));
	this->lblEncrypt->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboEncrypt, UI::GUIComboBox(ui, *this, false));
	this->cboEncrypt->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, *this, CSTR("&Convert")));
	this->btnConvert->SetRect(104, 76, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);

	Data::ArrayList<Text::TextBinEnc::ITextBinEnc*> *encs = this->encList.GetEncList();
	Text::TextBinEnc::ITextBinEnc *enc;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = encs->GetCount();
	while (i < j)
	{
		enc = encs->GetItem(i);
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
