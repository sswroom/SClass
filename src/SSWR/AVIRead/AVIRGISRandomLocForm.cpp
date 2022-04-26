#include "Stdafx.h"
#include "Data/RandomMT19937.h"
#include "Math/Polygon.h"
#include "SSWR/AVIRead/AVIRGISRandomLocForm.h"
#include "Text/StringBuilderUTF8.h"

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (!me->selecting)
		return false;
	me->isDown = true;
	me->selecting = false;
	me->downPt = scnPos;
	return true;
}

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (me->isDown)
	{
		me->isDown = false;
		Math::Coord2D<Double> mapPt1;
		Math::Coord2D<Double> mapPt2;
		Double tmpV;
		mapPt1 = me->navi->ScnXY2MapXY(me->downPt);
		mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (mapPt1.x > mapPt2.x)
		{
			tmpV = mapPt1.x;
			mapPt1.x = mapPt2.x;
			mapPt2.x = tmpV;
		}
		if (mapPt1.y > mapPt2.y)
		{
			tmpV = mapPt1.y;
			mapPt1.y = mapPt2.y;
			mapPt2.y = tmpV;
		}
		me->selPt1 = mapPt1;
		me->selPt2 = mapPt2;

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Math::Coord2D<Double> *ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->selPt1;
		ptList[1].x = me->selPt2.x;
		ptList[1].y = me->selPt1.y;
		ptList[2] = me->selPt2;
		ptList[3].x = me->selPt1.x;
		ptList[3].y = me->selPt2.y;
		ptList[4] = me->selPt1;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (me->isDown)
	{
		Math::Coord2D<Double> mapPt1;
		Math::Coord2D<Double> mapPt2;
		Double tmpV;
		mapPt1 = me->navi->ScnXY2MapXY(me->downPt);
		mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (mapPt1.x > mapPt2.x)
		{
			tmpV = mapPt1.x;
			mapPt1.x = mapPt2.x;
			mapPt2.x = tmpV;
		}
		if (mapPt1.y > mapPt2.y)
		{
			tmpV = mapPt1.y;
			mapPt1.y = mapPt2.y;
			mapPt2.y = tmpV;
		}

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Math::Coord2D<Double> *ptList = pg->GetPointList(&nPoints);
		ptList[0] = mapPt1;
		ptList[1].x = mapPt2.x;
		ptList[1].y = mapPt1.y;
		ptList[2] = mapPt2;
		ptList[3].x = mapPt1.x;
		ptList[3].y = mapPt2.y;
		ptList[4] = mapPt1;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnAreaClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	me->selecting = true;
}

void __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnRandomClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (me->selPt1 != me->selPt2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Data::RandomMT19937 r((UInt32)(dt.ToTicks() & 0xffffffff));
		Math::Coord2D<Double> pt = me->selPt1 + (me->selPt2 - me->selPt1) * Math::Coord2D<Double>(r.NextDouble(), r.NextDouble());
		me->navi->ShowMarker(pt.y, pt.x);
		if (!me->navi->InMap(pt.y, pt.x))
		{
			me->navi->PanToMap(pt.y, pt.x);
		}
	}
}

SSWR::AVIRead::AVIRGISRandomLocForm::AVIRGISRandomLocForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi) : UI::GUIForm(parent, 240, 72, ui)
{
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->navi = navi;
	this->SetText(CSTR("Random Location"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->selecting = false;
	this->isDown = false;
	this->downPt = Math::Coord2D<OSInt>(0, 0);
	this->selPt1 = Math::Coord2D<Double>(0, 0);
	this->selPt2 = this->selPt1;

	NEW_CLASS(this->btnArea, UI::GUIButton(ui, this, CSTR("Select Area")));
	this->btnArea->SetRect(4, 4, 100, 23, false);
	this->btnArea->HandleButtonClick(OnAreaClicked, this);
	NEW_CLASS(this->btnRandom, UI::GUIButton(ui, this, CSTR("Random")));
	this->btnRandom->SetRect(114, 4, 100, 23, false);
	this->btnRandom->HandleButtonClick(OnRandomClicked, this);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISRandomLocForm::~AVIRGISRandomLocForm()
{
	this->navi->SetSelectedVector(0);
	this->navi->HideMarker();
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISRandomLocForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
