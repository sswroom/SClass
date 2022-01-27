#include "Stdafx.h"
#include "Math/Unit/Distance.h"
#include "SSWR/AVIRead/AVIRGISDistanceForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnTypeSelChg(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	me->ptList->Clear();
	me->pathDist = 0;
	me->lastMapX = 0;
	me->lastMapY = 0;
}

void __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnMeasureSelChg(void *userObj, Bool newState)
{
//	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
}

void __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnDistanceUnitChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	me->UpdateDistDisp();
}

Bool __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnMapMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	if (me->radActionMeasure->IsSelected())
	{
		Double mapX;
		Double mapY;
		me->navi->ScnXY2MapXY(x, y, &mapX, &mapY);
		if (me->radTypePath->IsSelected())
		{
			me->ptList->Add(mapX);
			me->ptList->Add(mapY);
			if (me->lastMapX != 0 || me->lastMapY != 0)
			{
				me->pathDist += me->csys->CalSurfaceDistanceXY(mapX, mapY, me->lastMapX, me->lastMapY, Math::Unit::Distance::DU_METER);
				me->dispDist = me->pathDist;
				me->UpdateDistDisp();
			}
			me->lastMapX = mapX;
			me->lastMapY = mapY;
		}
		else
		{
			if (me->lastMapX != 0 || me->lastMapY != 0)
			{
				Double pts[4];
				pts[0] = mapX;
				pts[1] = mapY;
				pts[2] = me->lastMapX;
				pts[3] = me->lastMapY;
				Double dist = me->csys->CalSurfaceDistanceXY(mapX, mapY, me->lastMapX, me->lastMapY, Math::Unit::Distance::DU_METER);
				me->dispDist = dist;
				me->UpdateDistDisp();
				Math::Polyline *pl;
				NEW_CLASS(pl, Math::Polyline(me->csys->GetSRID(), pts, 2));
				me->navi->SetSelectedVector(pl);
				me->lastMapX = 0;
				me->lastMapY = 0;
			}
			else
			{
				me->lastMapX = mapX;
				me->lastMapY = mapY;
				me->dispDist = 0;
				me->UpdateDistDisp();
			}
		}
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnMapMouseMove(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	if ((me->lastMapX != 0 || me->lastMapY != 0) && me->radActionMeasure->IsSelected())
	{
		Double mapX;
		Double mapY;
		me->navi->ScnXY2MapXY(x, y, &mapX, &mapY);

		Double dist = me->csys->CalSurfaceDistanceXY(mapX, mapY, me->lastMapX, me->lastMapY, Math::Unit::Distance::DU_METER);
		me->dispDist = me->pathDist + dist;
		me->UpdateDistDisp();

		if (me->radTypePath->IsSelected())
		{
			Math::Polyline *pl;
			Double *pts;
			UOSInt i;
			UOSInt j;
			NEW_CLASS(pl, Math::Polyline(me->csys->GetSRID(), 1, (me->ptList->GetCount() >> 1) + 1));
			pts = pl->GetPointList(&j);
			i = 0;
			j = me->ptList->GetCount();
			while (i < j)
			{
				pts[i] = me->ptList->GetItem(i);
				i++;
			}
			pts[j] = mapX;
			pts[j + 1] = mapY;
			me->navi->SetSelectedVector(pl);
		}
		else
		{
			Math::Polyline *pl;
			Double pts[4];
			pts[0] = mapX;
			pts[1] = mapY;
			pts[2] = me->lastMapX;
			pts[3] = me->lastMapY;
			NEW_CLASS(pl, Math::Polyline(me->csys->GetSRID(), pts, 2));
			me->navi->SetSelectedVector(pl);
		}
	}
	return false;
}

void SSWR::AVIRead::AVIRGISDistanceForm::UpdateDistDisp()
{
	UTF8Char sbuff[64];
	Text::StrDouble(sbuff, Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, (Math::Unit::Distance::DistanceUnit)(OSInt)this->cboDistanceUnit->GetSelectedItem(), this->dispDist));
	this->txtDistance->SetText(sbuff);
}

SSWR::AVIRead::AVIRGISDistanceForm::AVIRGISDistanceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi) : UI::GUIForm(parent, 320, 360, ui)
{
	this->core = core;
	this->navi = navi;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText((const UTF8Char*)"Distance Tool");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetAlwaysOnTop(true);
	NEW_CLASS(this->ptList, Data::ArrayList<Double>());
	this->pathDist = 0;
	this->dispDist = 0;
	this->lastMapX = 0;
	this->lastMapY = 0;
	this->csys = this->navi->GetCoordinateSystem()->Clone();

	NEW_CLASS(this->lblType, UI::GUILabel(ui, this, (const UTF8Char*)"Type"));
	this->lblType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->pnlType, UI::GUIPanel(ui, this));
	this->pnlType->SetRect(104, 4, 200, 24, false);
	NEW_CLASS(this->radTypeLine, UI::GUIRadioButton(ui, this->pnlType, (const UTF8Char*)"Line", true));
	this->radTypeLine->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radTypePath, UI::GUIRadioButton(ui, this->pnlType, (const UTF8Char*)"Path", false));
	this->radTypePath->SetRect(100, 0, 100, 23, false);
	this->radTypePath->HandleSelectedChange(OnTypeSelChg, this);
	NEW_CLASS(this->lblAction, UI::GUILabel(ui, this, (const UTF8Char*)"Action"));
	this->lblAction->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->pnlAction, UI::GUIPanel(ui, this));
	this->pnlAction->SetRect(104, 28, 200, 24, false);
	NEW_CLASS(this->radActionPause, UI::GUIRadioButton(ui, this->pnlAction, (const UTF8Char*)"Pause", false));
	this->radActionPause->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->radActionMeasure, UI::GUIRadioButton(ui, this->pnlAction, (const UTF8Char*)"Measure", true));
	this->radActionMeasure->SetRect(100, 0, 100, 23, false);
	this->radActionMeasure->HandleSelectedChange(OnMeasureSelChg, this);
	NEW_CLASS(this->lblDistance, UI::GUILabel(ui, this, (const UTF8Char*)"Distance"));
	this->lblDistance->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDistance, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtDistance->SetRect(104, 52, 140, 23, false);
	this->txtDistance->SetReadOnly(true);
	NEW_CLASS(this->cboDistanceUnit, UI::GUIComboBox(ui, this, false));
	this->cboDistanceUnit->SetRect(244, 52, 60, 23, false);
	this->cboDistanceUnit->HandleSelectionChange(OnDistanceUnitChg, this);
	
	UOSInt i;
	Math::Unit::Distance::DistanceUnit du = Math::Unit::Distance::DU_FIRST;
	while (du <= Math::Unit::Distance::DU_LAST)
	{
		i = this->cboDistanceUnit->AddItem(Math::Unit::Distance::GetUnitShortName(du).v, (void*)(OSInt)du);
		if (du == Math::Unit::Distance::DU_METER)
		{
			this->cboDistanceUnit->SetSelectedIndex(i);
		}
		du = (Math::Unit::Distance::DistanceUnit)(du + 1);
	}
	this->navi->HandleMapMouseDown(OnMapMouseDown, this);
	this->navi->HandleMapMouseMove(OnMapMouseMove, this);
}

SSWR::AVIRead::AVIRGISDistanceForm::~AVIRGISDistanceForm()
{
	DEL_CLASS(this->ptList);
	DEL_CLASS(this->csys);
	this->navi->UnhandleMapMouse(this);
	this->navi->SetSelectedVector(0);
}

void SSWR::AVIRead::AVIRGISDistanceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
