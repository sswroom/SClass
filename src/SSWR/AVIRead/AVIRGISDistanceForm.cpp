#include "Stdafx.h"
#include "Math/Geometry/LineString.h"
#include "Math/Unit/Distance.h"
#include "SSWR/AVIRead/AVIRGISDistanceForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnTypeSelChg(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	me->ptList->Clear();
	me->pathDist = 0;
	me->lastMapPos = Math::Coord2DDbl(0, 0);
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

Bool __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnMapMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	if (me->radActionMeasure->IsSelected())
	{
		Math::Coord2DDbl mapPt = me->navi->ScnXY2MapXY(scnPos);
		if (me->radTypePath->IsSelected())
		{
			me->ptList->Add(mapPt.x);
			me->ptList->Add(mapPt.y);
			if (me->lastMapPos.x != 0 || me->lastMapPos.y != 0)
			{
				me->pathDist += me->csys->CalSurfaceDistance(mapPt, me->lastMapPos, Math::Unit::Distance::DU_METER);
				me->dispDist = me->pathDist;
				me->UpdateDistDisp();
			}
			me->lastMapPos = mapPt;
		}
		else
		{
			if (me->lastMapPos.x != 0 || me->lastMapPos.y != 0)
			{
				Math::Coord2DDbl pts[2];
				pts[0] = mapPt;
				pts[1] = me->lastMapPos;
				Double dist = me->csys->CalSurfaceDistance(mapPt, me->lastMapPos, Math::Unit::Distance::DU_METER);
				me->dispDist = dist;
				me->UpdateDistDisp();
				Math::Geometry::LineString *pl;
				NEW_CLASS(pl, Math::Geometry::LineString(me->csys->GetSRID(), pts, 2, 0, 0));
				me->navi->SetSelectedVector(pl);
				me->lastMapPos = Math::Coord2DDbl(0, 0);
			}
			else
			{
				me->lastMapPos = mapPt;
				me->dispDist = 0;
				me->UpdateDistDisp();
			}
		}
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISDistanceForm::OnMapMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISDistanceForm *me = (SSWR::AVIRead::AVIRGISDistanceForm*)userObj;
	if ((me->lastMapPos.x != 0 || me->lastMapPos.y != 0) && me->radActionMeasure->IsSelected())
	{
		Math::Coord2DDbl mapPt = me->navi->ScnXY2MapXY(scnPos);

		Double dist = me->csys->CalSurfaceDistance(mapPt, me->lastMapPos, Math::Unit::Distance::DU_METER);
		me->dispDist = me->pathDist + dist;
		me->UpdateDistDisp();

		if (me->radTypePath->IsSelected())
		{
			Math::Geometry::LineString *pl;
			Math::Coord2DDbl *pts;
			UOSInt i;
			UOSInt j;
			NEW_CLASS(pl, Math::Geometry::LineString(me->csys->GetSRID(), (me->ptList->GetCount() >> 1) + 1, false, false));
			pts = pl->GetPointList(j);
			i = 0;
			j = me->ptList->GetCount() >> 1;
			while (i < j)
			{
				pts[i].x = me->ptList->GetItem((i << 1));
				pts[i].y = me->ptList->GetItem((i << 1) + 1);
				i++;
			}
			pts[j] = mapPt;
			me->navi->SetSelectedVector(pl);
		}
		else
		{
			Math::Geometry::LineString *pl;
			Math::Coord2DDbl pts[2];
			pts[0] = mapPt;
			pts[1] = me->lastMapPos;
			NEW_CLASS(pl, Math::Geometry::LineString(me->csys->GetSRID(), pts, 2, 0, 0));
			me->navi->SetSelectedVector(pl);
		}
	}
	return false;
}

void SSWR::AVIRead::AVIRGISDistanceForm::UpdateDistDisp()
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Text::StrDouble(sbuff, Math::Unit::Distance::Convert(Math::Unit::Distance::DU_METER, (Math::Unit::Distance::DistanceUnit)(OSInt)this->cboDistanceUnit->GetSelectedItem(), this->dispDist));
	this->txtDistance->SetText(CSTRP(sbuff, sptr));
}

SSWR::AVIRead::AVIRGISDistanceForm::AVIRGISDistanceForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi) : UI::GUIForm(parent, 320, 360, ui)
{
	this->core = core;
	this->navi = navi;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Distance Tool"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetAlwaysOnTop(true);
	NEW_CLASS(this->ptList, Data::ArrayList<Double>());
	this->pathDist = 0;
	this->dispDist = 0;
	this->lastMapPos = {0, 0};
	this->csys = this->navi->GetCoordinateSystem()->Clone();

	this->lblType = ui->NewLabel(*this, CSTR("Type"));
	this->lblType->SetRect(4, 4, 100, 23, false);
	this->pnlType = ui->NewPanel(*this);
	this->pnlType->SetRect(104, 4, 200, 24, false);
	this->radTypeLine = ui->NewRadioButton(this->pnlType, CSTR("Line"), true);
	this->radTypeLine->SetRect(0, 0, 100, 23, false);
	this->radTypePath = ui->NewRadioButton(this->pnlType, CSTR("Path"), false);
	this->radTypePath->SetRect(100, 0, 100, 23, false);
	this->radTypePath->HandleSelectedChange(OnTypeSelChg, this);
	this->lblAction = ui->NewLabel(*this, CSTR("Action"));
	this->lblAction->SetRect(4, 28, 100, 23, false);
	this->pnlAction = ui->NewPanel(*this);
	this->pnlAction->SetRect(104, 28, 200, 24, false);
	this->radActionPause = ui->NewRadioButton(this->pnlAction, CSTR("Pause"), false);
	this->radActionPause->SetRect(0, 0, 100, 23, false);
	this->radActionMeasure = ui->NewRadioButton(this->pnlAction, CSTR("Measure"), true);
	this->radActionMeasure->SetRect(100, 0, 100, 23, false);
	this->radActionMeasure->HandleSelectedChange(OnMeasureSelChg, this);
	this->lblDistance = ui->NewLabel(*this, CSTR("Distance"));
	this->lblDistance->SetRect(4, 52, 100, 23, false);
	this->txtDistance = ui->NewTextBox(*this, CSTR(""));
	this->txtDistance->SetRect(104, 52, 140, 23, false);
	this->txtDistance->SetReadOnly(true);
	this->cboDistanceUnit = ui->NewComboBox(*this, false);
	this->cboDistanceUnit->SetRect(244, 52, 60, 23, false);
	this->cboDistanceUnit->HandleSelectionChange(OnDistanceUnitChg, this);
	
	UOSInt i;
	Math::Unit::Distance::DistanceUnit du = Math::Unit::Distance::DU_FIRST;
	while (du <= Math::Unit::Distance::DU_LAST)
	{
		i = this->cboDistanceUnit->AddItem(Math::Unit::Distance::GetUnitShortName(du), (void*)(OSInt)du);
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
	this->csys.Delete();
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISDistanceForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
