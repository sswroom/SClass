#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRImageUploaderForm.h"

void __stdcall SSWR::AVIRead::AVIRImageUploaderForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIRImageUploaderForm *me = (SSWR::AVIRead::AVIRImageUploaderForm*)userObj;
	UTF8Char sbuff[128];
	UOSInt i = 0;
	UOSInt j;
	while (i < fileCnt)
	{
		UInt64 fileSize = IO::Path::GetFileSize(files[i]);
		if (fileSize > 0)
		{
			FileItem *file = MemAlloc(FileItem, 1);
			file->fileName = Text::StrCopyNew(files[i]);
			file->fileSize = fileSize;
			file->status = FileStatus::Pending;
			me->items->Add(file);
			j = me->lvStatus->AddItem(file->fileName, file);
			Text::StrUInt64(sbuff, file->fileSize);
			me->lvStatus->SetSubItem(j, 1, sbuff);
			me->lvStatus->SetSubItem(j, 2, (const UTF8Char*)"-");
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRImageUploaderForm::OnUploadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageUploaderForm *me = (SSWR::AVIRead::AVIRImageUploaderForm*)userObj;
	Net::WebBrowser *browser;
	Bool errorCont = me->chkErrorCont->IsChecked();
	FileItem *item;
	UOSInt i = 0;
	UOSInt j = me->items->GetCount();
	while (i < j)
	{
		item = me->items->GetItem(i);
		if (item->status == FileStatus::Pending)
		{
			///////////////////////////
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRImageUploaderForm::FreeItem(FileItem *item)
{
	Text::StrDelNew(item->fileName);
	MemFree(item);
}

SSWR::AVIRead::AVIRImageUploaderForm::AVIRImageUploaderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 240, ui)
{
	this->SetText(CSTR("Image Uploader");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	NEW_CLASS(this->items, Data::ArrayList<FileItem*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 55, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lblUsername, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Username"));
	this->lblUsername->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtUsername, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtUsername->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Password"));
	this->lblPassword->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtPassword->SetPasswordChar('*');
	this->txtPassword->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->chkErrorCont, UI::GUICheckBox(ui, this->pnlCtrl, (const UTF8Char*)"Continue on error", false));
	this->chkErrorCont->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->btnUpload, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Upload"));
	this->btnUpload->SetRect(204, 28, 75, 23, false);
	this->btnUpload->HandleButtonClick(OnUploadClicked, this);
	NEW_CLASS(this->lvStatus, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvStatus->AddColumn((const UTF8Char*)"File Name", 300);
	this->lvStatus->AddColumn((const UTF8Char*)"File Size", 100);
	this->lvStatus->AddColumn((const UTF8Char*)"Status", 100);
}

SSWR::AVIRead::AVIRImageUploaderForm::~AVIRImageUploaderForm()
{
	LIST_FREE_FUNC(this->items, FreeItem);
	DEL_CLASS(this->items);
}

void SSWR::AVIRead::AVIRImageUploaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
