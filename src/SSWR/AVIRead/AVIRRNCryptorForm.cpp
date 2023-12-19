#include "Stdafx.h"
#include "Crypto/Encrypt/RNCryptor.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/MemoryReadingStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRRNCryptorForm.h"
#include "Text/TextBinEnc/Base64Enc.h"

void __stdcall SSWR::AVIRead::AVIRRNCryptorForm::OnProcessClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRNCryptorForm *me = (SSWR::AVIRead::AVIRRNCryptorForm*)userObj;
	Text::StringBuilderUTF8 sbSrcFile;
	Text::StringBuilderUTF8 sbPassword;
	me->txtSourceFile->GetText(sbSrcFile);
	me->txtPassword->GetText(sbPassword);
	if (sbSrcFile.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please drag and drop source file first"), CSTR("RNCryptor"), me);
		return;
	}
	if (sbPassword.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input password"), CSTR("RNCryptor"), me);
		return;
	}
	if (sbSrcFile.EndsWith(UTF8STRC(".aes")))
	{
		IO::FileStream srcFS(sbSrcFile.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sbSrcFile.RemoveChars(4);
		Bool succ;
		if (me->chkBase64->IsChecked())
		{
			IO::MemoryStream mstm;
			succ = Crypto::Encrypt::RNCryptor::Decrypt(srcFS, mstm, sbPassword.ToCString());
			if (succ)
			{
				Text::TextBinEnc::Base64Enc enc;
				UOSInt outLen = (UOSInt)(mstm.GetLength() >> 2) * 3;
				UInt8 *destBuff = MemAlloc(UInt8, outLen);
				UOSInt retSize = enc.DecodeBin(mstm.GetBuff(), (UOSInt)mstm.GetLength(), destBuff);
				IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				succ = destFS.WriteCont(destBuff, retSize);
				MemFree(destBuff);
			}
		}
		else
		{
			IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			succ = Crypto::Encrypt::RNCryptor::Decrypt(srcFS, destFS, sbPassword.ToCString());
		}
		if (!succ)
		{
			me->ui->ShowMsgOK(CSTR("Error in decrypting file"), CSTR("RNCryptor"), me);
			IO::Path::DeleteFile(sbSrcFile.ToString());
		}
	}
	else
	{
		IO::FileStream srcFS(sbSrcFile.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sbSrcFile.AppendC(UTF8STRC(".aes"));
		Bool succ;
		if (me->chkBase64->IsChecked())
		{
			UInt64 fileLen = srcFS.GetLength();
			if (fileLen > 1048576)
			{
				succ = false;
			}
			else
			{
				Data::ByteBuffer fileBuff((UOSInt)fileLen);
				Text::TextBinEnc::Base64Enc enc;
				if (srcFS.Read(fileBuff) == fileLen)
				{
					Text::StringBuilderUTF8 sb;
					enc.EncodeBin(sb, fileBuff.Ptr(), fileBuff.GetSize());
					IO::MemoryReadingStream mstm(sb.v, sb.leng);
					IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = Crypto::Encrypt::RNCryptor::Encrypt(mstm, destFS, sbPassword.ToCString());
				}
				else
				{
					succ = false;
				}
			}
		}
		else
		{
			IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			succ = Crypto::Encrypt::RNCryptor::Encrypt(srcFS, destFS, sbPassword.ToCString());
		}
		if (!succ)
		{
			me->ui->ShowMsgOK(CSTR("Error in encrypting file"), CSTR("RNCryptor"), me);
			IO::Path::DeleteFile(sbSrcFile.ToString());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRNCryptorForm::OnFiles(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRRNCryptorForm *me = (SSWR::AVIRead::AVIRRNCryptorForm*)userObj;
	if (nFiles > 0)
	{
		me->txtSourceFile->SetText(files[0]->ToCString());
		Text::StringBuilderUTF8 sb;
		sb.Append(files[0]);
		if (files[0]->EndsWith(UTF8STRC(".aes")))
		{
			sb.RemoveChars(4);
			me->txtDestFile->SetText(sb.ToCString());
		}
		else
		{
			sb.AppendC(UTF8STRC(".aes"));
			me->txtDestFile->SetText(sb.ToCString());
		}
	}
}

SSWR::AVIRead::AVIRRNCryptorForm::AVIRRNCryptorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 160, ui)
{
	this->SetText(CSTR("RNCryptor"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblSourceFile = ui->NewLabel(*this, CSTR("Source File"));
	this->lblSourceFile->SetRect(4, 4, 100, 23, false);
	this->txtSourceFile = ui->NewTextBox(*this, CSTR(""));
	this->txtSourceFile->SetRect(104, 4, 400, 23, false);
	this->txtSourceFile->SetReadOnly(true);
	this->lblDestFile = ui->NewLabel(*this, CSTR("Dest File"));
	this->lblDestFile->SetRect(4, 28, 100, 23, false);
	this->txtDestFile = ui->NewTextBox(*this, CSTR(""));
	this->txtDestFile->SetRect(104, 28, 400, 23, false);
	this->txtDestFile->SetReadOnly(true);
	this->lblPassword = ui->NewLabel(*this, CSTR("Password"));
	this->lblPassword->SetRect(4, 52, 100, 23, false);
	this->txtPassword = ui->NewTextBox(*this, CSTR(""));
	this->txtPassword->SetRect(104, 52, 200, 23, false);
	this->lblOptions = ui->NewLabel(*this, CSTR("Options"));
	this->lblOptions->SetRect(4, 76, 100, 23, false);
	this->chkBase64 = ui->NewCheckBox(*this, CSTR("With Base64"), false);
	this->chkBase64->SetRect(104, 76, 200, 23, false);
	this->btnProcess = ui->NewButton(*this, CSTR("Process"));
	this->btnProcess->SetRect(104, 100, 75, 23, false);
	this->btnProcess->HandleButtonClick(OnProcessClicked, this);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRRNCryptorForm::~AVIRRNCryptorForm()
{
}

void SSWR::AVIRead::AVIRRNCryptorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
