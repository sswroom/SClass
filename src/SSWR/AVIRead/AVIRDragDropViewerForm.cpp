#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDragDropViewerForm.h"
#include "Text/MyStringW.h"

void __stdcall SSWR::AVIRead::AVIRDragDropViewerForm::OnTypeSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDragDropViewerForm *me = (SSWR::AVIRead::AVIRDragDropViewerForm*)userObj;
	NotNullPtr<Text::String> s;
	if (me->lbType->GetSelectedItemTextNew().SetTo(s))
	{
		const UTF8Char *msg = me->dropMap->Get(s->v);
		s->Release();
		if (msg)
		{
			me->txtMain->SetText({msg, Text::StrCharCnt(msg)});
		}
		else
		{
			me->txtMain->SetText(CSTR(""));
		}
	}
}

void SSWR::AVIRead::AVIRDragDropViewerForm::ClearDrops()
{
	NotNullPtr<const Data::ArrayList<const UTF8Char*>> dropList = this->dropMap->GetValues();
	UOSInt i;
	const UTF8Char *dropMsg;
	i = dropList->GetCount();
	while (i-- > 0)
	{
		dropMsg = dropList->GetItem(i);
		Text::StrDelNew(dropMsg);
	}
	this->dropMap->Clear();
}

SSWR::AVIRead::AVIRDragDropViewerForm::AVIRDragDropViewerForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Drag Drop Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->dropMap, Data::StringUTF8Map<const UTF8Char*>());

	this->lbType = ui->NewListBox(*this, false);
	this->lbType->SetRect(0, 0, 200, 23, false);
	this->lbType->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbType->HandleSelectionChange(OnTypeSelChg, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->txtMain = ui->NewTextBox(*this, CSTR(""), true);
	this->txtMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMain->SetReadOnly(true);

	UI::GUIControl::DragErrorType errType = this->HandleDropEvents(this);
	if (errType == UI::GUIControl::DET_NOERROR)
	{
	}
	else if (errType == UI::GUIControl::DET_INVALIDCONTROL)
	{
		this->ui->ShowMsgOK(CSTR("Error in registering Drag Drop, Invalid HWND"), CSTR("Error"), this);
	}
	else if (errType == UI::GUIControl::DET_ALREADYREGISTER)
	{
		this->ui->ShowMsgOK(CSTR("Error in registering Drag Drop, Already Registered"), CSTR("Error"), this);
	}
	else if (errType == UI::GUIControl::DET_OUTOFMEMORY)
	{
		this->ui->ShowMsgOK(CSTR("Error in registering Drag Drop, Out of Memory"), CSTR("Error"), this);
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Error in registering Drag Drop"), CSTR("Error"), this);
	}
}

SSWR::AVIRead::AVIRDragDropViewerForm::~AVIRDragDropViewerForm()
{
	ClearDrops();
	DEL_CLASS(this->dropMap);
}

void SSWR::AVIRead::AVIRDragDropViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

UI::GUIDropHandler::DragEffect SSWR::AVIRead::AVIRDragDropViewerForm::DragEnter(UI::GUIDropData *data)
{
	return UI::GUIDropHandler::DE_COPY;
}

void SSWR::AVIRead::AVIRDragDropViewerForm::DropData(UI::GUIDropData *data, OSInt x, OSInt y)
{
	this->ClearDrops();
	this->lbType->ClearItems();

	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	const UTF8Char *csptr;
	i = 0;
	j = data->GetCount();
	while (i < j)
	{
		csptr = data->GetName(i);
		sb.ClearStr();
		if (data->GetDataText(csptr, sb))
		{
			this->dropMap->Put(csptr, Text::StrCopyNewC(sb.ToString(), sb.GetLength()).Ptr());
		}
		else
		{
			this->dropMap->Put(csptr, Text::StrCopyNewC(UTF8STRC("Cannot get data")).Ptr());
		}
		this->lbType->AddItem({csptr, Text::StrCharCnt(csptr)}, 0);
		i++;
	}
/*	WChar wbuff[512];
	FORMATETC fmt;
	IEnumFORMATETC *enumFmt;
	UInt16 fmtSURL = 0;
	UInt16 fmtIURL = 0;
	UInt16 fmtFile = 0;
	STGMEDIUM med;
	HRESULT hres;

	if (pDataObj->EnumFormatEtc(DATADIR_GET, &enumFmt) == S_OK)
	{
		while (enumFmt->Next(1, &fmt, 0) == S_OK)
		{
			if (Win32::Clipboard::GetFormatName(fmt.cfFormat, wbuff, 512) == 0)
			{
				Text::StrInt32(Text::StrConcat(wbuff, L"Format "), fmt.cfFormat);
			}
			if ((hres = pDataObj->GetData(&fmt, &med)) == S_OK)
			{
				sb.ClearStr();
				if (Win32::Clipboard::GetDataTextH(med.hGlobal, fmt.cfFormat, &sb, med.tymed))
				{
					this->dropMap->Put(fmt.cfFormat, Text::StrCopyNew(sb.ToString()));
					this->lbType->AddItem(wbuff, (void*)fmt.cfFormat);
				}
				else
				{
					this->dropMap->Put(fmt.cfFormat, Text::StrCopyNew(L"Unknown data"));
					this->lbType->AddItem(wbuff, (void*)fmt.cfFormat);
				}
				ReleaseStgMedium(&med);
			}
			else
			{
				this->dropMap->Put(fmt.cfFormat, Text::StrCopyNew(L"Cannot get data"));
				this->lbType->AddItem(wbuff, (void*)fmt.cfFormat);
			}
		}
		enumFmt->Release();
	}*/
}
