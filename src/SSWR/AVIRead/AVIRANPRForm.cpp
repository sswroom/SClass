#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "SSWR/AVIRead/AVIRANPRForm.h"

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnFileHandler(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();
	UOSInt i = 0;
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(&fd, IO::ParserType::ImageList);
		if (imgList)
		{
			imgList->ToStaticImage(0);
			SDEL_CLASS(me->currImg);
			me->currImg = (Media::StaticImage*)imgList->GetImage(0, 0);
			imgList->RemoveImage(0, false);
			DEL_CLASS(imgList);
			me->pbImg->SetImage(me->currImg, false);

			me->lbPlate->ClearItems();
			me->ClearResults();
			me->anpr.ParseImage(me->currImg);
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnANPRResult(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	ResultInfo *res;
	res = MemAlloc(ResultInfo, 1);
	res->area = *area;
	res->result = result->Clone();
	me->results.Add(res);
	me->lbPlate->AddItem(res->result, res);
}

void SSWR::AVIRead::AVIRANPRForm::ClearResults()
{
	ResultInfo *res;
	UOSInt i = this->results.GetCount();
	while (i-- > 0)
	{
		res = this->results.GetItem(i);
		res->result->Release();
		MemFree(res);
	}
	this->results.Clear();
}

SSWR::AVIRead::AVIRANPRForm::AVIRANPRForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ANPR"));

	this->core = core;
	this->currImg = 0;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->anpr.SetResultHandler(OnANPRResult, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbPlate, UI::GUIListBox(ui, this, false));
	this->lbPlate->SetRect(0, 0, 150, 100, false);
	this->lbPlate->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->hspPlate, UI::GUIHSplitter(ui, this, 3, true));
	NEW_CLASS(this->pbImg, UI::GUIPictureBoxDD(ui, this, this->colorSess, true, false));
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIRANPRForm::~AVIRANPRForm()
{
	this->ClearResults();
	SDEL_CLASS(this->currImg);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRANPRForm::OnMonitorChanged()
{
	this->pbImg->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
