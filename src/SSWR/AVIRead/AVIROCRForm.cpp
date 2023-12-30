#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIROCRForm.h"

void __stdcall SSWR::AVIRead::AVIROCRForm::OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIROCRForm *me = (SSWR::AVIRead::AVIROCRForm*)userObj;
	NotNullPtr<Parser::ParserList> parsers = me->core->GetParserList();
	UOSInt i = 0;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(fd, IO::ParserType::ImageList);
		if (imgList)
		{
			imgList->ToStaticImage(0);
			SDEL_CLASS(me->currImg);
			me->currImg = (Media::StaticImage*)imgList->GetImage(0, 0);
			imgList->RemoveImage(0, false);
			DEL_CLASS(imgList);
			me->pbImg->SetImage(me->currImg, false);

			me->lvText->ClearItems();
			me->ClearResults();
			me->ocr.SetParsingImage(me->currImg);
			me->ocr.ParseAllInImage();
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIROCRForm::OnTextSelChg(void *userObj)
{
	SSWR::AVIRead::AVIROCRForm *me = (SSWR::AVIRead::AVIROCRForm*)userObj;
	ResultInfo *res = (ResultInfo*)me->lvText->GetSelectedItem();
	if (res)
	{
		me->pbResult->SetImage(res->resImg);
	}
}

void __stdcall SSWR::AVIRead::AVIROCRForm::OnOCRResult(void *userObj, NotNullPtr<Text::String> txt, Double confidence, Math::RectArea<OSInt> boundary)
{
	SSWR::AVIRead::AVIROCRForm *me = (SSWR::AVIRead::AVIROCRForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (boundary.GetArea() >= 1000)
	{
		ResultInfo *res = MemAlloc(ResultInfo, 1);
		res->result = txt->Clone();
		res->confidence = confidence;
		res->area = boundary;
		res->resImg = me->currImg->CreateSubImage(boundary);
		me->results.Add(res);
		UOSInt i = me->lvText->AddItem(res->result, res);
		sptr = Text::StrOSInt(sbuff, boundary.GetArea());
		me->lvText->SetSubItem(i, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, res->resImg->CalcColorRate());
		me->lvText->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, confidence);
		me->lvText->SetSubItem(i, 3, CSTRP(sbuff, sptr));
	}
}

void SSWR::AVIRead::AVIROCRForm::ClearResults()
{
	ResultInfo *res;
	UOSInt i = this->results.GetCount();
	while (i-- > 0)
	{
		res = this->results.GetItem(i);
		res->result->Release();
		DEL_CLASS(res->resImg);
		MemFree(res);
	}
	this->results.Clear();
}

SSWR::AVIRead::AVIROCRForm::AVIROCRForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), ocr(Media::OCREngine::Language::English)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("OCR"));

	this->core = core;
	this->currImg = 0;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->ocr.SetCharWhiteList("0123456789ABCDEFGHIJKLMNPQRSTUVWXYZ");
	this->ocr.HandleOCRResult(OnOCRResult, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlResult = ui->NewPanel(*this);
	this->pnlResult->SetRect(0, 0, 250, 100, false);
	this->pnlResult->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->pbResult = ui->NewPictureBoxSimple(this->pnlResult, this->core->GetDrawEngine(), false);
	this->pbResult->SetRect(0, 0, 100, 80, false);
	this->pbResult->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvText = ui->NewListView(this->pnlResult, UI::ListViewStyle::Table, 4);
	this->lvText->SetRect(0, 0, 250, 100, false);
	this->lvText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvText->AddColumn(CSTR("Result"), 100);
	this->lvText->AddColumn(CSTR("Area"), 50);
	this->lvText->AddColumn(CSTR("Color Rate"), 50);
	this->lvText->AddColumn(CSTR("Confidence"), 50);
	this->lvText->HandleSelChg(OnTextSelChg, this);
	this->hspText = ui->NewHSplitter(*this, 3, true);
	NEW_CLASS(this->pbImg, UI::GUIPictureBoxDD(ui, *this, this->colorSess, true, false));
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIROCRForm::~AVIROCRForm()
{
	this->ClearResults();
	SDEL_CLASS(this->currImg);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIROCRForm::OnMonitorChanged()
{
	this->pbImg->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
