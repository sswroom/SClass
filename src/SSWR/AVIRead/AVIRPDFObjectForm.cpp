#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRPDFObjectForm.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

typedef enum
{
	MNU_SAVE_ALL_IMAGE = 101,
	MNU_SAVE_SELECTED
} MenuItem;

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnObjectSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRPDFObjectForm *me = (SSWR::AVIRead::AVIRPDFObjectForm*)userObj;
	me->lvParameter->ClearItems();
	Media::PDFObject *obj = (Media::PDFObject*)me->lbObject->GetSelectedItem();
	if (obj)
	{
		Media::PDFParameter *param = obj->GetParameter();
		if (param)
		{
			Media::PDFParameter::ParamEntry *entry;
			UOSInt i = 0;
			UOSInt j = param->GetCount();
			while (i < j)
			{
				entry = param->GetItem(i);
				me->lvParameter->AddItem(entry->type, 0);
				if (entry->value)
					me->lvParameter->SetSubItem(i, 1, entry->value);
				i++;
			}
		}
	}
}

SSWR::AVIRead::AVIRPDFObjectForm::AVIRPDFObjectForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::PDFDocument *doc) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("PDF Objects"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->doc = doc;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lbObject, UI::GUIListBox(ui, this, false));
	this->lbObject->SetRect(0, 0, 100, 23, false);
	this->lbObject->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbObject->HandleSelectionChange(OnObjectSelChg, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpParameter = this->tcMain->AddTabPage(CSTR("Parameter"));
	NEW_CLASS(this->lvParameter, UI::GUIListView(ui, this->tpParameter, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvParameter->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvParameter->SetFullRowSelect(true);
	this->lvParameter->SetShowGrid(true);
	this->lvParameter->AddColumn(CSTR("Type"), 100);
	this->lvParameter->AddColumn(CSTR("Value"), 400);

	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	UI::GUIMenu *mnu = this->mnuMain->AddSubMenu(CSTR("Objects"));
	mnu->AddItem(CSTR("Save all images..."), MNU_SAVE_ALL_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Save Selected"), MNU_SAVE_SELECTED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);
	
	Media::PDFObject *obj;
	Text::String *type;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->doc->GetCount();
	while (i < j)
	{
		obj = this->doc->GetItem(i);
		sptr = Text::StrUInt32(sbuff, obj->GetId());
		if ((type = obj->GetType()) != 0)
		{
			*sptr++ = ' ';
			sptr = type->ConcatTo(sptr);
		}
		this->lbObject->AddItem(CSTRP(sbuff, sptr), obj);
		i++;
	}
}

SSWR::AVIRead::AVIRPDFObjectForm::~AVIRPDFObjectForm()
{
	DEL_CLASS(this->doc);
}

void SSWR::AVIRead::AVIRPDFObjectForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SAVE_ALL_IMAGE:
	{
		UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"PDFObjectAllImage");
		if (dlg.ShowDialog(this->GetHandle()))
		{
			Text::String *folder = dlg.GetFolder();
			Text::CString fileName = this->doc->GetSourceNameObj()->ToCString();
			UOSInt i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
				fileName = fileName.Substring(i + 1);
			Text::StringBuilderUTF8 sb;
			Media::PDFObject *obj;
			i = 0;
			UOSInt j = this->doc->GetCount();
			while (i < j)
			{
				obj = this->doc->GetItem(i);
				if (obj->IsImage())
				{
					Text::String *filter = obj->GetFilter();
					if (filter)
					{
						if (filter->Equals(UTF8STRC("DCTDecode")))
						{
							sb.ClearStr();
							sb.Append(folder);
							sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
							sb.Append(fileName);
							sb.AppendUTF8Char('.');
							sb.AppendU32(obj->GetId());
							sb.AppendC(UTF8STRC(".jpg"));
							obj->SaveFile(sb.ToCString());
						}
					}
				}
				i++;
			}
		}
		break;	
	}
	case MNU_SAVE_SELECTED:
	{
		Media::PDFObject *obj = (Media::PDFObject*)this->lbObject->GetSelectedItem();
		if (obj == 0)
			break;
		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"PDFObjectSelected", true);
		dlg.AddFilter(CSTR("*.dat"), CSTR("Data File"));
		Text::StringBuilderUTF8 sb;
		sb.Append(doc->GetSourceNameObj());
		sb.AppendUTF8Char('.');
		sb.AppendU32(obj->GetId());
		sb.AppendC(UTF8STRC(".dat"));
		dlg.SetFileName(sb.ToCString());
		if (dlg.ShowDialog(this->GetHandle()))
		{
			obj->SaveFile(dlg.GetFileName()->ToCString());
		}
		break;	
	}
	}
}

void SSWR::AVIRead::AVIRPDFObjectForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
