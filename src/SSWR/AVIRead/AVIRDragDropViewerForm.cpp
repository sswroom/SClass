#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDragDropViewerForm.h"
#include "Text/MyStringW.h"

void __stdcall SSWR::AVIRead::AVIRDragDropViewerForm::OnTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDragDropViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDragDropViewerForm>();
	NN<Text::String> s;
	if (me->lbType->GetSelectedItemTextNew().SetTo(s))
	{
		NN<Text::String> msg;
		if (me->dropMap.Get(s).SetTo(msg))
		{
			me->txtMain->SetText(msg->ToCString());
		}
		else
		{
			me->txtMain->SetText(CSTR(""));
		}
		s->Release();
	}
}

void SSWR::AVIRead::AVIRDragDropViewerForm::ClearDrops()
{
	NNLIST_FREE_STRING(&this->dropMap);
}

SSWR::AVIRead::AVIRDragDropViewerForm::AVIRDragDropViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Drag Drop Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

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
}

void SSWR::AVIRead::AVIRDragDropViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

UI::GUIDropHandler::DragEffect SSWR::AVIRead::AVIRDragDropViewerForm::DragEnter(NN<UI::GUIDropData> data)
{
	return UI::GUIDropHandler::DE_COPY;
}

void SSWR::AVIRead::AVIRDragDropViewerForm::DropData(NN<UI::GUIDropData> data, OSInt x, OSInt y)
{
	this->ClearDrops();
	this->lbType->ClearItems();

	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UnsafeArray<const UTF8Char> csptr;
	i = 0;
	j = data->GetCount();
	while (i < j)
	{
		csptr = data->GetName(i).Or(U8STR(""));
		sb.ClearStr();
		if (data->GetDataText(csptr, sb))
		{
			this->dropMap.PutC(Text::CStringNN::FromPtr(csptr), Text::String::New(sb.ToCString()));
		}
		else
		{
			this->dropMap.PutC(Text::CStringNN::FromPtr(csptr), Text::String::New(UTF8STRC("Cannot get data")));
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
