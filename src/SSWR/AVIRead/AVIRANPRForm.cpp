#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/StmData/FileData.h"
#include "Math/Quadrilateral.h"
#include "SSWR/AVIRead/AVIRANPRForm.h"

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
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

			me->lvPlate->ClearItems();
			me->ClearResults();
			me->anpr.ParseImage(me->currImg);
			break;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnPlateSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	ResultInfo *res = (ResultInfo*)me->lvPlate->GetSelectedItem();
	if (res)
	{
		me->pbPlate->SetImage(res->plateImg);
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnSelPlateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	if (me->selectMode == ActionType::Plate)
	{
		me->selectMode = ActionType::None;
		me->lblSelStatus->SetText(CSTR(""));
	}
	else
	{
		me->selectMode = ActionType::Plate;
		me->lblSelStatus->SetText(CSTR("Select plate bg color"));
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnSelCornersClicked(void *userObj)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	if (me->selectMode == ActionType::Corners)
	{
		me->selectMode = ActionType::None;
		me->points.Clear();
		me->lblSelStatus->SetText(CSTR(""));
	}
	else
	{
		me->selectMode = ActionType::Corners;
		me->points.Clear();
		me->lblSelStatus->SetText(CSTR("Select point 1"));
	}
}

Bool __stdcall SSWR::AVIRead::AVIRANPRForm::OnImgDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	if (me->selectMode == ActionType::Corners)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		me->points.Add(me->pbImg->Scn2ImagePos(scnPos));
		if (me->points.GetCount() >= 4)
		{
			UOSInt i;
			me->selectMode = ActionType::None;
			me->lblSelStatus->SetText(CSTR(""));
			me->anpr.ParseImageQuad(me->currImg, Math::Quadrilateral::FromPolygon(me->points.GetArray(&i)));
		}
		else
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Select point ")), me->points.GetCount() + 1);
			me->lblSelStatus->SetText(CSTRP(sbuff, sptr));
		}
		return true;
	}
	else if (me->selectMode == ActionType::Plate)
	{
		Math::Coord2DDbl coord = me->pbImg->Scn2ImagePos(scnPos);
		me->anpr.ParseImagePlatePoint(me->currImg, Math::Coord2D<UOSInt>((UOSInt)Double2OSInt(coord.x), (UOSInt)Double2OSInt(coord.y)));
		me->selectMode = ActionType::None;
		me->lblSelStatus->SetText(CSTR(""));
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnANPRResult(void *userObj, Media::StaticImage *simg, Math::RectArea<UOSInt> *area, Text::String *result, Double maxTileAngle, Double pxArea, UOSInt confidence, Media::StaticImage *plateImg)
{
	SSWR::AVIRead::AVIRANPRForm *me = (SSWR::AVIRead::AVIRANPRForm*)userObj;
	ResultInfo *res;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	UOSInt i;
	res = MemAlloc(ResultInfo, 1);
	res->area = *area;
	res->result = result->Clone();
	res->maxTileAngle = maxTileAngle;
	res->pxArea = pxArea;
	res->confidence = confidence;
	res->plateImg = (Media::StaticImage*)plateImg->Clone();
	me->results.Add(res);
	i = me->lvPlate->AddItem(res->result, res);
	sptr = Text::StrDouble(sbuff, maxTileAngle);
	me->lvPlate->SetSubItem(i, 1, CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, pxArea);
	me->lvPlate->SetSubItem(i, 2, CSTRP(sbuff, sptr));
	sptr = Text::StrUOSInt(sbuff, confidence);
	me->lvPlate->SetSubItem(i, 3, CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRANPRForm::ClearResults()
{
	ResultInfo *res;
	UOSInt i = this->results.GetCount();
	while (i-- > 0)
	{
		res = this->results.GetItem(i);
		res->result->Release();
		DEL_CLASS(res->plateImg);
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
	this->selectMode = ActionType::None;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->anpr.SetResultHandler(OnANPRResult, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnSelPlate, UI::GUIButton(ui, this->pnlCtrl, CSTR("Sel Plate")));
	this->btnSelPlate->SetRect(4, 4, 75, 23, false);
	this->btnSelPlate->HandleButtonClick(OnSelPlateClicked, this);
	NEW_CLASS(this->btnSelCorners, UI::GUIButton(ui, this->pnlCtrl, CSTR("Sel Corners")));
	this->btnSelCorners->SetRect(84, 4, 75, 23, false);
	this->btnSelCorners->HandleButtonClick(OnSelCornersClicked, this);
	NEW_CLASS(this->lblSelStatus, UI::GUILabel(ui, this->pnlCtrl, CSTR("")));
	this->lblSelStatus->SetRect(164, 4, 200, 23, false);
	NEW_CLASS(this->pnlPlate, UI::GUIPanel(ui, this));
	this->pnlPlate->SetRect(0, 0, 250, 100, false);
	this->pnlPlate->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->pbPlate, UI::GUIPictureBoxSimple(ui, this->pnlPlate, this->core->GetDrawEngine(), false));
	this->pbPlate->SetRect(0, 0, 100, 80, false);
	this->pbPlate->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lvPlate, UI::GUIListView(ui, this->pnlPlate, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvPlate->SetRect(0, 0, 250, 100, false);
	this->lvPlate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPlate->AddColumn(CSTR("Result"), 100);
	this->lvPlate->AddColumn(CSTR("Tilt"), 50);
	this->lvPlate->AddColumn(CSTR("Area"), 50);
	this->lvPlate->AddColumn(CSTR("Confidence"), 50);
	this->lvPlate->HandleSelChg(OnPlateSelChg, this);
	NEW_CLASS(this->hspPlate, UI::GUIHSplitter(ui, this, 3, true));
	NEW_CLASS(this->pbImg, UI::GUIPictureBoxDD(ui, this, this->colorSess, true, false));
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbImg->HandleMouseDown(OnImgDown, this);

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
