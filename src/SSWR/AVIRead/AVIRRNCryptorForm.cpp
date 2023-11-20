#include "Stdafx.h"
#include "Crypto/Encrypt/RNCryptor.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRRNCryptorForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRRNCryptorForm::OnProcessClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRNCryptorForm *me = (SSWR::AVIRead::AVIRRNCryptorForm*)userObj;
	Text::StringBuilderUTF8 sbSrcFile;
	Text::StringBuilderUTF8 sbPassword;
	me->txtSourceFile->GetText(sbSrcFile);
	me->txtPassword->GetText(sbPassword);
	if (sbSrcFile.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please drag and drop source file first"), CSTR("RNCryptor"), me);
		return;
	}
	if (sbPassword.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input password"), CSTR("RNCryptor"), me);
		return;
	}
	if (sbSrcFile.EndsWith(UTF8STRC(".aes")))
	{
		IO::FileStream srcFS(sbSrcFile.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sbSrcFile.RemoveChars(4);
		Bool succ;
		{
			IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			succ = Crypto::Encrypt::RNCryptor::Decrypt(&srcFS, &destFS, sbPassword.ToCString());
		}
		if (!succ)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in decrypting file"), CSTR("RNCryptor"), me);
			IO::Path::DeleteFile(sbSrcFile.ToString());
		}
	}
	else
	{
		IO::FileStream srcFS(sbSrcFile.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		sbSrcFile.AppendC(UTF8STRC(".aes"));
		Bool succ;
		{
			IO::FileStream destFS(sbSrcFile.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			succ = Crypto::Encrypt::RNCryptor::Encrypt(&srcFS, &destFS, sbPassword.ToCString());
		}
		if (!succ)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in encrypting file"), CSTR("RNCryptor"), me);
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

	NEW_CLASS(this->lblSourceFile, UI::GUILabel(ui, *this, CSTR("Source File")));
	this->lblSourceFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSourceFile, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtSourceFile->SetRect(104, 4, 400, 23, false);
	this->txtSourceFile->SetReadOnly(true);
	NEW_CLASS(this->lblDestFile, UI::GUILabel(ui, *this, CSTR("Dest File")));
	this->lblDestFile->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDestFile, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtDestFile->SetRect(104, 28, 400, 23, false);
	this->txtDestFile->SetReadOnly(true);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, *this, CSTR("Password")));
	this->lblPassword->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtPassword->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->btnProcess, UI::GUIButton(ui, *this, CSTR("Process")));
	this->btnProcess->SetRect(104, 76, 75, 23, false);
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
