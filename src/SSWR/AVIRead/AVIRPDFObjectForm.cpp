#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRPDFObjectForm.h"
#include "Text/StringTool.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

typedef enum
{
	MNU_SAVE_ALL_IMAGE = 101,
	MNU_SAVE_SELECTED
} MenuItem;

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnObjectSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPDFObjectForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPDFObjectForm>();
	me->lvParameter->ClearItems();
	NN<Media::PDFObject> obj;
	if (me->lbObject->GetSelectedItem().GetOpt<Media::PDFObject>().SetTo(obj))
	{
		NN<Media::PDFParameter> param;
		if (obj->GetParameter().SetTo(param))
		{
			NN<Media::PDFParameter::ParamEntry> entry;
			NN<Text::String> s;
			UOSInt i = 0;
			UOSInt j = param->GetCount();
			while (i < j)
			{
				entry = param->GetItemNoCheck(i);
				me->lvParameter->AddItem(entry->type, 0);
				if (entry->value.SetTo(s))
					me->lvParameter->SetSubItem(i, 1, s);
				i++;
			}
		}
		NN<Media::ImageList> imgList;
		me->dispImage.Delete();
		if (obj->IsImage() && me->doc->CreateImage(obj->GetId(), me->core->GetParserList()).SetTo(imgList))
		{
			imgList->ToStaticImage(0);
			me->pbImage->SetImage(imgList->GetImage(0, 0), false);
			me->dispImage = imgList;
		}
		else
		{
			me->pbImage->SetImage(0, false);
		}
		Text::StringBuilderUTF8 sb;
		obj->ToString(sb);
		me->txtText->SetText(sb.ToCString());

		Bool found = false;
		NN<IO::StreamData> stmData;
		if (obj->GetData().SetTo(stmData))
		{
			UInt64 size = stmData->GetDataSize();
			if (size > 0 && size <= 1048576)
			{
				Data::ByteBuffer buff((UOSInt)size + 1);
				stmData->GetRealData(0, (UOSInt)size, buff);
				if (Text::StringTool::IsTextUTF8(buff.SubArray(0, (UOSInt)size)))
				{
					buff[(UOSInt)size] = 0;
					me->txtStream->SetText(Text::CStringNN(buff.Arr(), (UOSInt)size));
					found = true;
				}
			}
		}
		if (!found)
		{
			me->txtStream->SetText(CSTR(""));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnObjectDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPDFObjectForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPDFObjectForm>();
	NN<Media::PDFObject> obj;
	if (me->lbObject->GetSelectedItem().GetOpt<Media::PDFObject>().SetTo(obj) && obj->IsImage())
	{
		NN<Media::ImageList> imgList;
		if (me->doc->CreateImage(obj->GetId(), me->core->GetParserList()).SetTo(imgList))
			me->core->OpenObject(imgList);
	}
}

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnImageOpenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPDFObjectForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPDFObjectForm>();
	NN<Media::PDFObject> obj;
	if (me->lbObject->GetSelectedItem().GetOpt<Media::PDFObject>().SetTo(obj) && obj->IsImage())
	{
		NN<Media::ImageList> imgList;
		if (me->doc->CreateImage(obj->GetId(), me->core->GetParserList()).SetTo(imgList))
			me->core->OpenObject(imgList);
	}
}

void __stdcall SSWR::AVIRead::AVIRPDFObjectForm::OnStreamHexClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRPDFObjectForm> me = userObj.GetNN<SSWR::AVIRead::AVIRPDFObjectForm>();
	NN<Media::PDFObject> obj;
	NN<IO::StreamData> stmData;
	if (me->lbObject->GetSelectedItem().GetOpt<Media::PDFObject>().SetTo(obj) && obj->GetData().SetTo(stmData))
	{
		me->core->OpenHex(stmData, 0);
	}
}

SSWR::AVIRead::AVIRPDFObjectForm::AVIRPDFObjectForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::PDFDocument> doc) : UI::GUIForm(parent, 640, 480, ui)
{
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("PDF Objects - "));
	sb.Append(doc->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->doc = doc;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->dispImage = nullptr;
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

	this->tpText = this->tcMain->AddTabPage(CSTR("Text"));
	this->txtText = ui->NewTextBox(this->tpText, CSTR(""), true);
	this->txtText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtText->SetReadOnly(true);

	this->tpImage = this->tcMain->AddTabPage(CSTR("Image"));
	this->pnlImage = ui->NewPanel(this->tpImage);
	this->pnlImage->SetRect(0, 0, 100, 31, false);
	this->pnlImage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnImageOpen = ui->NewButton(this->pnlImage, CSTR("Open"));
	this->btnImageOpen->SetRect(4, 4, 75, 23, false);
	this->btnImageOpen->HandleButtonClick(OnImageOpenClicked, this);
	this->pbImage = ui->NewPictureBoxDD(this->tpImage, this->colorSess, true, false);
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpStream = this->tcMain->AddTabPage(CSTR("Stream"));
	this->pnlStream = ui->NewPanel(this->tpStream);
	this->pnlStream->SetRect(0, 0, 100, 31, false);
	this->pnlStream->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnStreamHex = ui->NewButton(this->pnlStream, CSTR("Show Hex"));
	this->btnStreamHex->SetRect(4, 4, 75, 23, false);
	this->btnStreamHex->HandleButtonClick(OnStreamHexClicked, this);
	this->txtStream = ui->NewTextBox(this->tpStream, CSTR(""), true);
	this->txtStream->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtStream->SetReadOnly(true);

	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	NN<UI::GUIMenu> mnu = this->mnuMain->AddSubMenu(CSTR("Objects"));
	mnu->AddItem(CSTR("Save all images..."), MNU_SAVE_ALL_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Save Selected"), MNU_SAVE_SELECTED, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);
	
	NN<Media::PDFObject> obj;
	NN<Text::String> s;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = 0;
	UOSInt j = this->doc->GetCount();
	while (i < j)
	{
		obj = this->doc->GetItemNoCheck(i);
		sptr = Text::StrUInt32(sbuff, obj->GetId());
		if (obj->GetType().SetTo(s))
		{
			*sptr++ = ' ';
			sptr = s->ConcatTo(sptr);
			if (obj->GetSubtype().SetTo(s))
			{
				sptr = s->ConcatTo(sptr);
			}
		}
		this->lbObject->AddItem(CSTRP(sbuff, sptr), obj);
		i++;
	}
}

SSWR::AVIRead::AVIRPDFObjectForm::~AVIRPDFObjectForm()
{
	this->ClearChildren();
	this->doc.Delete();
	this->dispImage.Delete();
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRPDFObjectForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_SAVE_ALL_IMAGE:
	{
		NN<UI::GUIFolderDialog> dlg = this->ui->NewFolderDialog();
		if (dlg->ShowDialog(this->GetHandle()))
		{
			NN<Text::String> folder = dlg->GetFolder();
			Text::CStringNN fileName = this->doc->GetSourceNameObj()->ToCString();
			UOSInt i = fileName.LastIndexOf(IO::Path::PATH_SEPERATOR);
			if (i != INVALID_INDEX)
				fileName = fileName.Substring(i + 1);
			Text::StringBuilderUTF8 sb;
			NN<Media::PDFObject> obj;
			i = 0;
			UOSInt j = this->doc->GetCount();
			while (i < j)
			{
				obj = this->doc->GetItemNoCheck(i);
				if (obj->IsImage())
				{
					NN<Text::String> filter;
					if (obj->GetFilter().SetTo(filter))
					{
						if (filter->Equals(UTF8STRC("/DCTDecode")))
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
		Media::PDFObject *obj = (Media::PDFObject*)this->lbObject->GetSelectedItem().p;
		if (obj == 0)
			break;
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"PDFObjectSelected", true);
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
