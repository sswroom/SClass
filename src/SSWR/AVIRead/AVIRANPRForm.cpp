#include "Stdafx.h"
#include "Exporter/PNGExporter.h"
#include "IO/StmData/FileData.h"
#include "Math/Quadrilateral.h"
#include "SSWR/AVIRead/AVIRANPRForm.h"

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	UIntOS i = 0;
	UIntOS nFiles = files.GetCount();
	while (i < nFiles)
	{
		IO::StmData::FileData fd(files[i], false);
		NN<Media::ImageList> imgList;
		if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
		{
			imgList->ToStaticImage(0);
			NN<Media::StaticImage> img;
			if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
			{
				me->currImg.Delete();
				me->currImg = img;
				imgList->RemoveImage(0, false);
				imgList.Delete();
				me->pbImg->SetImage(img, false);

				me->lvPlate->ClearItems();
				me->ClearResults();
				me->anpr.ParseImage(img);
				break;
			}
			else
			{
				imgList.Delete();
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnPlateSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
	NN<ResultInfo> res;
	if (me->lvPlate->GetSelectedItem().GetOpt<ResultInfo>().SetTo(res))
	{
		me->pbPlate->SetImage(res->plateImg);
	}
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnSelPlateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
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

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnSelCornersClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
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

UI::EventState __stdcall SSWR::AVIRead::AVIRANPRForm::OnImgDown(AnyType userObj, Math::Coord2D<IntOS> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
	NN<Media::StaticImage> img;
	if (!me->currImg.SetTo(img))
		return UI::EventState::ContinueEvent;
	if (me->selectMode == ActionType::Corners)
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		me->points.Add(me->pbImg->Scn2ImagePos(scnPos));
		if (me->points.GetCount() >= 4)
		{
			me->selectMode = ActionType::None;
			me->lblSelStatus->SetText(CSTR(""));
			me->anpr.ParseImageQuad(img, Math::Quadrilateral::FromPolygon(me->points.Arr().Ptr()));
		}
		else
		{
			sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("Select point ")), me->points.GetCount() + 1);
			me->lblSelStatus->SetText(CSTRP(sbuff, sptr));
		}
		return UI::EventState::StopEvent;
	}
	else if (me->selectMode == ActionType::Plate)
	{
		Math::Coord2DDbl coord = me->pbImg->Scn2ImagePos(scnPos);
		me->anpr.ParseImagePlatePoint(img, Math::Coord2D<UIntOS>((UIntOS)Double2IntOS(coord.x), (UIntOS)Double2IntOS(coord.y)));
		me->selectMode = ActionType::None;
		me->lblSelStatus->SetText(CSTR(""));
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRANPRForm::OnANPRResult(AnyType userObj, NN<Media::StaticImage> simg, Math::RectArea<UIntOS> area, NN<Text::String> result, Double maxTileAngle, Double pxArea, UIntOS confidence, NN<Media::StaticImage> plateImg)
{
	NN<SSWR::AVIRead::AVIRANPRForm> me = userObj.GetNN<SSWR::AVIRead::AVIRANPRForm>();
	NN<ResultInfo> res;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	res = MemAllocNN(ResultInfo);
	res->area = area;
	res->result = result->Clone();
	res->maxTileAngle = maxTileAngle;
	res->pxArea = pxArea;
	res->confidence = confidence;
	res->plateImg = NN<Media::StaticImage>::ConvertFrom(plateImg->Clone());
	me->results.Add(res);
	i = me->lvPlate->AddItem(res->result, res);
	sptr = Text::StrDouble(sbuff, maxTileAngle);
	me->lvPlate->SetSubItem(i, 1, CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, pxArea);
	me->lvPlate->SetSubItem(i, 2, CSTRP(sbuff, sptr));
	sptr = Text::StrUIntOS(sbuff, confidence);
	me->lvPlate->SetSubItem(i, 3, CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRANPRForm::ClearResults()
{
	NN<ResultInfo> res;
	UIntOS i = this->results.GetCount();
	while (i-- > 0)
	{
		res = this->results.GetItemNoCheck(i);
		res->result->Release();
		res->plateImg.Delete();
		MemFreeNN(res);
	}
	this->results.Clear();
}

SSWR::AVIRead::AVIRANPRForm::AVIRANPRForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("ANPR"));

	this->core = core;
	this->currImg = nullptr;
	this->selectMode = ActionType::None;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->anpr.SetResultHandler(OnANPRResult, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnSelPlate = ui->NewButton(this->pnlCtrl, CSTR("Sel Plate"));
	this->btnSelPlate->SetRect(4, 4, 75, 23, false);
	this->btnSelPlate->HandleButtonClick(OnSelPlateClicked, this);
	this->btnSelCorners = ui->NewButton(this->pnlCtrl, CSTR("Sel Corners"));
	this->btnSelCorners->SetRect(84, 4, 75, 23, false);
	this->btnSelCorners->HandleButtonClick(OnSelCornersClicked, this);
	this->lblSelStatus = ui->NewLabel(this->pnlCtrl, CSTR(""));
	this->lblSelStatus->SetRect(164, 4, 200, 23, false);
	this->pnlPlate = ui->NewPanel(*this);
	this->pnlPlate->SetRect(0, 0, 250, 100, false);
	this->pnlPlate->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->pbPlate = ui->NewPictureBoxSimple(this->pnlPlate, this->core->GetDrawEngine(), false);
	this->pbPlate->SetRect(0, 0, 100, 80, false);
	this->pbPlate->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvPlate = ui->NewListView(this->pnlPlate, UI::ListViewStyle::Table, 4);
	this->lvPlate->SetRect(0, 0, 250, 100, false);
	this->lvPlate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPlate->AddColumn(CSTR("Result"), 100);
	this->lvPlate->AddColumn(CSTR("Tilt"), 50);
	this->lvPlate->AddColumn(CSTR("Area"), 50);
	this->lvPlate->AddColumn(CSTR("Confidence"), 50);
	this->lvPlate->HandleSelChg(OnPlateSelChg, this);
	this->hspPlate = ui->NewHSplitter(*this, 3, true);
	this->pbImg = ui->NewPictureBoxDD(*this, this->colorSess, true, false);
	this->pbImg->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbImg->HandleMouseDown(OnImgDown, this);

	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIRANPRForm::~AVIRANPRForm()
{
	this->ClearResults();
	this->currImg.Delete();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRANPRForm::OnMonitorChanged()
{
	this->pbImg->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
