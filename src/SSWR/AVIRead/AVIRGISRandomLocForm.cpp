#include "Stdafx.h"
#include "Data/RandomMT19937.h"
#include "Math/Polygon.h"
#include "SSWR/AVIRead/AVIRGISRandomLocForm.h"
#include "Text/StringBuilderUTF8.h"

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (!me->selecting)
		return false;
	me->isDown = true;
	me->selecting = false;
	me->downX = x;
	me->downY = y;
	return true;
}

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseUp(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (me->isDown)
	{
		me->isDown = false;
		Double mapX1;
		Double mapY1;
		Double mapX2;
		Double mapY2;
		Double tmpV;
		me->navi->ScnXY2MapXY(me->downX, me->downY, &mapX1, &mapY1);
		me->navi->ScnXY2MapXY(x, y, &mapX2, &mapY2);
		if (mapX1 > mapX2)
		{
			tmpV = mapX1;
			mapX1 = mapX2;
			mapX2 = tmpV;
		}
		if (mapY1 > mapY2)
		{
			tmpV = mapY1;
			mapY1 = mapY2;
			mapY2 = tmpV;
		}
		me->selX1 = mapX1;
		me->selY1 = mapY1;
		me->selX2 = mapX2;
		me->selY2 = mapY2;

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Math::Coord2D<Double> *ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->selX1;
		ptList[0].y = me->selY1;
		ptList[1].x = me->selX2;
		ptList[1].y = me->selY1;
		ptList[2].x = me->selX2;
		ptList[2].y = me->selY2;
		ptList[3].x = me->selX1;
		ptList[3].y = me->selY2;
		ptList[4].x = me->selX1;
		ptList[4].y = me->selY1;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISRandomLocForm::OnMouseMove(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISRandomLocForm *me = (SSWR::AVIRead::AVIRGISRandomLocForm*)userObj;
	if (me->isDown)
	{
		Double mapX1;
		Double mapY1;
		Double mapX2;
		Double mapY2;
		Double tmpV;
		me->navi->ScnXY2MapXY(me->downX, me->downY, &mapX1, &mapY1);
		me->navi->ScnXY2MapXY(x, y, &mapX2, &mapY2);
		if (mapX1 > mapX2)
		{
			tmpV = mapX1;
			mapX1 = mapX2;
			mapX2 = tmpV;
		}
		if (mapY1 > mapY2)
		{
			tmpV = mapY1;
			mapY1 = mapY2;
			mapY2 = tmpV;
		}

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Math::Coord2D<Double> *ptList = pg->GetPointList(&nPoints);
		ptList[0].x = mapX1;
		ptList[0].y = mapY1;
		ptList[1].x = mapX2;
		ptList[1].y = mapY1;
		ptList[2].x = mapX2;
		ptList[2].y = mapY2;
		ptList[3].x = mapX1;
		ptList[3].y = mapY2;
		ptList[4].x = mapX1;
		ptList[4].y = mapY1;
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
	if (me->selX1 != me->selX2 && me->selY1 != me->selY2)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Data::RandomMT19937 r((UInt32)(dt.ToTicks() & 0xffffffff));
		Double x = me->selX1 + (me->selX2 - me->selX1) * r.NextDouble();
		Double y = me->selY1 + (me->selY2 - me->selY1) * r.NextDouble();
		me->navi->ShowMarker(y, x);
		if (!me->navi->InMap(y, x))
		{
			me->navi->PanToMap(y, x);
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
	this->downX = 0;
	this->downY = 0;
	this->selX1 = 0;
	this->selY1 = 0;
	this->selX2 = 0;
	this->selY2 = 0;

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
