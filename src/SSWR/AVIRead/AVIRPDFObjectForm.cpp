#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRPDFObjectForm.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

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
			NotNullPtr<Text::String> s;
			UOSInt i = 0;
			UOSInt j = param->GetCount();
			while (i < j)
			{
				entry = param->GetItem(i);
				me->lvParameter->AddItem(entry->type, 0);
				if (entry->value.SetTo(s))
					me->lvParameter->SetSubItem(i, 1, s);
				i++;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnObjectDblClk(void *userObj)
{
	SSWR::AVIRead::AVIRPDFObjectForm *me = (SSWR::AVIRead::AVIRPDFObjectForm*)userObj;
	Media::PDFObject *obj = (Media::PDFObject*)me->lbObject->GetSelectedItem();
	if (obj && obj->IsImage())
	{
		NotNullPtr<Media::ImageList> imgList;
		if (imgList.Set(me->doc->CreateImage(obj->GetId(), me->core->GetParserList())))
			me->core->OpenObject(imgList);
	}
}

SSWR::AVIRead::AVIRPDFObjectForm::AVIRPDFObjectForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::PDFDocument *doc) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("PDF Objects"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->doc = doc;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lbObject = ui->NewListBox(*this, false);
	this->lbObject->SetRect(0, 0, 100, 23, false);
	this->lbObject->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbObject->HandleSelectionChange(OnObjectSelChg, this);
	this->lbObject->HandleDoubleClicked(OnObjectDblClk, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpParameter = this->tcMain->AddTabPage(CSTR("Parameter"));
	this->lvParameter = ui->NewListView(this->tpParameter, UI::ListViewStyle::Table, 2);
	this->lvParameter->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvParameter->SetFullRowSelect(true);
	this->lvParameter->SetShowGrid(true);
	this->lvParameter->AddColumn(CSTR("Type"), 100);
	this->lvParameter->AddColumn(CSTR("Value"), 400);

	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	NotNullPtr<UI::GUIMenu> mnu = this->mnuMain->AddSubMenu(CSTR("Objects"));
	mnu->AddItem(CSTR("Save all images..."), MNU_SAVE_ALL_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Save Selected"), MNU_SAVE_SELECTED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);
	
	Media::PDFObject *obj;
	Optional<Text::String> type;
	NotNullPtr<Text::String> s;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->doc->GetCount();
	while (i < j)
	{
		obj = this->doc->GetItem(i);
		sptr = Text::StrUInt32(sbuff, obj->GetId());
		type = obj->GetType();
		if (type.SetTo(s) && s->Equals(UTF8STRC("XObject")))
		{
			type = obj->GetSubtype();
		}
		if (type.SetTo(s))
		{
			*sptr++ = ' ';
			sptr = s->ConcatTo(sptr);
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
		NotNullPtr<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();
		if (dlg->ShowDialog(this->GetHandle()))
		{
			NotNullPtr<Text::String> folder = dlg->GetFolder();
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
					NotNullPtr<Text::String> filter;
					if (obj->GetFilter().SetTo(filter))
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
		dlg.Delete();
		break;	
	}
	case MNU_SAVE_SELECTED:
	{
		Media::PDFObject *obj = (Media::PDFObject*)this->lbObject->GetSelectedItem();
		if (obj == 0)
			break;
		NotNullPtr<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"PDFObjectSelected", true);
		dlg->AddFilter(CSTR("*.dat"), CSTR("Data File"));
		Text::StringBuilderUTF8 sb;
		sb.Append(doc->GetSourceNameObj());
		sb.AppendUTF8Char('.');
		sb.AppendU32(obj->GetId());
		sb.AppendC(UTF8STRC(".dat"));
		dlg->SetFileName(sb.ToCString());
		if (dlg->ShowDialog(this->GetHandle()))
		{
			obj->SaveFile(dlg->GetFileName()->ToCString());
		}
		dlg.Delete();
		break;	
	}
	}
}

void SSWR::AVIRead::AVIRPDFObjectForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
