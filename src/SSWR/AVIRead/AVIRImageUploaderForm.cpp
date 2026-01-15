#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRImageUploaderForm.h"

void __stdcall SSWR::AVIRead::AVIRImageUploaderForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRImageUploaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageUploaderForm>();
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UIntOS i = 0;
	UIntOS fileCnt = files.GetCount();
	UIntOS j;
	while (i < fileCnt)
	{
		UInt64 fileSize = IO::Path::GetFileSize(files[i]->v);
		if (fileSize > 0)
		{
			NN<FileItem> file = MemAllocNN(FileItem);
			file->fileName = files[i]->Clone();
			file->fileSize = fileSize;
			file->status = FileStatus::Pending;
			me->items.Add(file);
			j = me->lvStatus->AddItem(file->fileName, file);
			sptr = Text::StrUInt64(sbuff, file->fileSize);
			me->lvStatus->SetSubItem(j, 1, CSTRP(sbuff, sptr));
			me->lvStatus->SetSubItem(j, 2, CSTR("-"));
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRImageUploaderForm::OnUploadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageUploaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageUploaderForm>();
	Net::WebBrowser *browser;
	Bool errorCont = me->chkErrorCont->IsChecked();
	NN<FileItem> item;
	UIntOS i = 0;
	UIntOS j = me->items.GetCount();
	while (i < j)
	{
		item = me->items.GetItemNoCheck(i);
		if (item->status == FileStatus::Pending)
		{
			///////////////////////////
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRImageUploaderForm::FreeItem(NN<FileItem> item)
{
	item->fileName->Release();
	MemFreeNN(item);
}

SSWR::AVIRead::AVIRImageUploaderForm::AVIRImageUploaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText(CSTR("Image Uploader"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 55, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblUsername = ui->NewLabel(this->pnlCtrl, CSTR("Username"));
	this->lblUsername->SetRect(4, 4, 100, 23, false);
	this->txtUsername = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtUsername->SetRect(104, 4, 100, 23, false);
	this->lblPassword = ui->NewLabel(this->pnlCtrl, CSTR("Password"));
	this->lblPassword->SetRect(4, 28, 100, 23, false);
	this->txtPassword = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtPassword->SetPasswordChar('*');
	this->txtPassword->SetRect(104, 28, 100, 23, false);
	this->chkErrorCont = ui->NewCheckBox(this->pnlCtrl, CSTR("Continue on error"), false);
	this->chkErrorCont->SetRect(204, 4, 100, 23, false);
	this->btnUpload = ui->NewButton(this->pnlCtrl, CSTR("Upload"));
	this->btnUpload->SetRect(204, 28, 75, 23, false);
	this->btnUpload->HandleButtonClick(OnUploadClicked, this);
	this->lvStatus = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStatus->AddColumn(CSTR("File Name"), 300);
	this->lvStatus->AddColumn(CSTR("File Size"), 100);
	this->lvStatus->AddColumn(CSTR("Status"), 100);
}

SSWR::AVIRead::AVIRImageUploaderForm::~AVIRImageUploaderForm()
{
	this->items.FreeAll(FreeItem);
}

void SSWR::AVIRead::AVIRImageUploaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
