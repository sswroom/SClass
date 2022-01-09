#include "Stdafx.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRProtoDecForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	ProtocolItem *item = (ProtocolItem*)me->lvLogs->GetSelectedItem();
	if (item && me->currFile)
	{
		Text::StringBuilderUTF8 sb;
		UInt8 *buff;
		buff = MemAlloc(UInt8, item->size);
		me->currFile->SeekFromBeginning(item->fileOfst);
		me->currFile->Read(buff, item->size);
		sb.AppendHex(buff, item->size, ' ', Text::LineBreakType::CRLF);
		sb.AppendC(UTF8STRC("\r\n\r\n"));
		me->currDec->GetProtocolDetail(buff, item->size, &sb);
		me->txtLogs->SetText(sb.ToString());
		MemFree(buff);
	}
	else
	{
		me->txtLogs->SetText((const UTF8Char*)"");
	}
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog *dlg;
	me->txtFile->GetText(&sb);
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"ProtoDec", false));
	dlg->AddFilter((const UTF8Char*)"*.dat", (const UTF8Char*)"RAW data file");
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	Text::StringBuilderUTF8 sb;
	IO::ProtoDec::IProtocolDecoder *protoDec;
	me->txtFile->GetText(&sb);
	protoDec = (IO::ProtoDec::IProtocolDecoder*)me->cboDecoder->GetSelectedItem();
	if (sb.GetLength() > 0 && protoDec != 0)
	{
		me->ClearList();
		SDEL_CLASS(me->currFile);
		NEW_CLASS(me->currFile, IO::FileStream(sb.ToString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
		if (me->currFile->IsError())
		{
			DEL_CLASS(me->currFile);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening the file", (const UTF8Char*)"Protocol Decoder", me);
			return;
		}
		me->currDec = protoDec;
		UInt8 *buff;
		UOSInt buffSize;
		UOSInt readSize;
		UInt64 fileOfst;
		buff = MemAlloc(UInt8, 65536);
		buffSize = 0;
		fileOfst = 0;
		while (true)
		{
			readSize = me->currFile->Read(&buff[buffSize], 65536 - buffSize);
			if (readSize == 0)
				break;
			buffSize += readSize;
			readSize = protoDec->ParseProtocol(OnProtocolEntry, me, fileOfst, buff, buffSize);
			fileOfst += readSize;
			if (readSize >= buffSize)
			{
				buffSize = 0;
			}
			else if (readSize > 0)
			{
				MemCopyO(buff, &buff[readSize], buffSize - readSize);
				buffSize -= readSize;
			}
			else
			{
				break;
			}
		}
		MemFree(buff);
	}
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnProtocolEntry(void *userObj, UInt64 fileOfst, UOSInt size, const UTF8Char *typeName)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	ProtocolItem *item;
	UTF8Char sbuff[32];
	UOSInt i;
	item = MemAlloc(ProtocolItem, 1);
	item->fileOfst = fileOfst;
	item->size = size;
	Text::StrUInt64(sbuff, fileOfst);
	i = me->lvLogs->AddItem(sbuff, item);
	Text::StrInt32(sbuff, (Int32)size);
	me->lvLogs->SetSubItem(i, 1, sbuff);
	me->lvLogs->SetSubItem(i, 2, typeName);
	me->itemList->Add(item);
}

void SSWR::AVIRead::AVIRProtoDecForm::ClearList()
{
	UOSInt i = this->itemList->GetCount();
	while (i-- > 0)
	{
		MemFree(this->itemList->GetItem(i));
	}
	this->itemList->Clear();
	this->lvLogs->ClearItems();
}

SSWR::AVIRead::AVIRProtoDecForm::AVIRProtoDecForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Protocol Decoder");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->currFile = 0;
	this->currDec = 0;
	NEW_CLASS(this->decList, IO::ProtoDec::ProtoDecList());
	NEW_CLASS(this->itemList, Data::ArrayList<ProtocolItem*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 80, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"B&rowse"));
	this->btnFile->SetRect(504, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	NEW_CLASS(this->lblDecoder, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Decoder"));
	this->lblDecoder->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboDecoder, UI::GUIComboBox(ui, this->pnlCtrl, false));
	this->cboDecoder->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnLoad, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"&Load"));
	this->btnLoad->SetRect(104, 52, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	NEW_CLASS(this->txtLogs, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtLogs->SetReadOnly(true);
	this->txtLogs->SetRect(0, 0, 100, 144, false);
	this->txtLogs->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspLogs, UI::GUIVSplitter(ui, this, 3, true));
	NEW_CLASS(this->lvLogs, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvLogs->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogs->SetShowGrid(true);
	this->lvLogs->SetFullRowSelect(true);
	this->lvLogs->AddColumn((const UTF8Char*)"Offset", 80);
	this->lvLogs->AddColumn((const UTF8Char*)"Size", 80);
	this->lvLogs->AddColumn((const UTF8Char*)"Type", 200);
	this->lvLogs->HandleSelChg(OnLogSelChg, this);

	UOSInt i;
	UOSInt j;
	IO::ProtoDec::IProtocolDecoder *protoDec;
	i = 0;
	j = this->decList->GetCount();
	while (i < j)
	{
		protoDec = this->decList->GetItem(i);
		this->cboDecoder->AddItem(protoDec->GetName(), protoDec);
		i++;
	}
	if (j > 0)
	{
		this->cboDecoder->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRProtoDecForm::~AVIRProtoDecForm()
{
	this->ClearList();
	SDEL_CLASS(this->currFile);
	DEL_CLASS(this->itemList);
	DEL_CLASS(this->decList);
}

void SSWR::AVIRead::AVIRProtoDecForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
