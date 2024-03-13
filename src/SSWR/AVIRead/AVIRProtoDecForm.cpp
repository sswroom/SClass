#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRProtoDecForm.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnLogSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	ProtocolItem *item = (ProtocolItem*)me->lvLogs->GetSelectedItem();
	if (item && me->currFile)
	{
		Text::StringBuilderUTF8 sb;
		Data::ByteBuffer buff(item->size);
		me->currFile->SeekFromBeginning(item->fileOfst);
		me->currFile->Read(buff);
		sb.AppendHexBuff(buff, ' ', Text::LineBreakType::CRLF);
		sb.AppendC(UTF8STRC("\r\n\r\n"));
		me->currDec->GetProtocolDetail(buff.Ptr(), item->size, sb);
		me->txtLogs->SetText(sb.ToCString());
	}
	else
	{
		me->txtLogs->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtFile->GetText(sb);
	NotNullPtr<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ProtoDec", false);
	dlg->AddFilter(CSTR("*.dat"), CSTR("RAW data file"));
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFile->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	Text::StringBuilderUTF8 sb;
	IO::ProtoDec::IProtocolDecoder *protoDec;
	me->txtFile->GetText(sb);
	protoDec = (IO::ProtoDec::IProtocolDecoder*)me->cboDecoder->GetSelectedItem();
	if (sb.GetLength() > 0 && protoDec != 0)
	{
		me->ClearList();
		SDEL_CLASS(me->currFile);
		NEW_CLASS(me->currFile, IO::FileStream(sb.ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
		if (me->currFile->IsError())
		{
			DEL_CLASS(me->currFile);
			me->ui->ShowMsgOK(CSTR("Error in opening the file"), CSTR("Protocol Decoder"), me);
			return;
		}
		me->currDec = protoDec;
		UOSInt buffSize;
		UOSInt readSize;
		UInt64 fileOfst;
		Data::ByteBuffer buff(65536);
		buffSize = 0;
		fileOfst = 0;
		while (true)
		{
			readSize = me->currFile->Read(buff.SubArray(buffSize));
			if (readSize == 0)
				break;
			buffSize += readSize;
			readSize = protoDec->ParseProtocol(OnProtocolEntry, me, fileOfst, buff.Ptr(), buffSize);
			fileOfst += readSize;
			if (readSize >= buffSize)
			{
				buffSize = 0;
			}
			else if (readSize > 0)
			{
				buff.CopyInner(0, readSize, buffSize - readSize);
				buffSize -= readSize;
			}
			else
			{
				break;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRProtoDecForm::OnProtocolEntry(void *userObj, UInt64 fileOfst, UOSInt size, Text::CStringNN typeName)
{
	SSWR::AVIRead::AVIRProtoDecForm *me = (SSWR::AVIRead::AVIRProtoDecForm *)userObj;
	ProtocolItem *item;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	item = MemAlloc(ProtocolItem, 1);
	item->fileOfst = fileOfst;
	item->size = size;
	sptr = Text::StrUInt64(sbuff, fileOfst);
	i = me->lvLogs->AddItem(CSTRP(sbuff, sptr), item);
	sptr = Text::StrInt32(sbuff, (Int32)size);
	me->lvLogs->SetSubItem(i, 1, CSTRP(sbuff, sptr));
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

SSWR::AVIRead::AVIRProtoDecForm::AVIRProtoDecForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Protocol Decoder"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->currFile = 0;
	this->currDec = 0;
	NEW_CLASS(this->decList, IO::ProtoDec::ProtoDecList());
	NEW_CLASS(this->itemList, Data::ArrayList<ProtocolItem*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 80, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlCtrl, CSTR("File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtFile->SetRect(104, 4, 400, 23, false);
	this->txtFile->SetReadOnly(true);
	this->btnFile = ui->NewButton(this->pnlCtrl, CSTR("B&rowse"));
	this->btnFile->SetRect(504, 4, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->lblDecoder = ui->NewLabel(this->pnlCtrl, CSTR("Decoder"));
	this->lblDecoder->SetRect(4, 28, 100, 23, false);
	this->cboDecoder = ui->NewComboBox(this->pnlCtrl, false);
	this->cboDecoder->SetRect(104, 28, 200, 23, false);
	this->btnLoad = ui->NewButton(this->pnlCtrl, CSTR("&Load"));
	this->btnLoad->SetRect(104, 52, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
	this->txtLogs = ui->NewTextBox(*this, CSTR(""), true);
	this->txtLogs->SetReadOnly(true);
	this->txtLogs->SetRect(0, 0, 100, 144, false);
	this->txtLogs->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspLogs = ui->NewVSplitter(*this, 3, true);
	this->lvLogs = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvLogs->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvLogs->SetShowGrid(true);
	this->lvLogs->SetFullRowSelect(true);
	this->lvLogs->AddColumn(CSTR("Offset"), 80);
	this->lvLogs->AddColumn(CSTR("Size"), 80);
	this->lvLogs->AddColumn(CSTR("Type"), 200);
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
