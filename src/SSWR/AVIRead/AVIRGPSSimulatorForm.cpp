#include "Stdafx.h"
#include "Data/DateTime.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Polygon.h"
#include "SSWR/AVIRead/AVIRGPSSimulatorForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FolderDialog.h"

Bool __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	if (!me->chkAddPoints->IsChecked())
		return false;
	Double mapX;
	Double mapY;
	UTF8Char sbuff[128];
	me->navi->ScnXY2MapXY(x, y, &mapX, &mapY);
	Double lat;
	Double lon;
	if (me->navi->GetCoordinateSystem()->Equals(me->wgs84))
	{
		lat = mapY;
		lon = mapX;
	}
	else
	{
		Double z;
		Math::CoordinateSystem::ConvertXYZ(me->navi->GetCoordinateSystem(), me->wgs84, mapX, mapY, 0, &lon, &lat, &z);
	}
	if (me->currX == 0 && me->currY == 0)
	{
		me->currX = lon;
		me->currY = lat;
		Text::StrDouble(sbuff, lat);
		me->txtCurrLat->SetText(sbuff);
		Text::StrDouble(sbuff, lon);
		me->txtCurrLon->SetText(sbuff);
		me->navi->ShowMarker(lat, lon);
	}
	else
	{
		Text::StrDouble(Text::StrConcat(Text::StrDouble(sbuff, lat), (const UTF8Char*)", "), lon);
		me->lbPoints->AddItem(sbuff, 0);
		me->points->Add(lon);
		me->points->Add(lat);
	}
	return true;
}

void __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	if (me->stm)
	{
		DEL_CLASS(me->stm);
		me->stm = 0;
		me->txtStreamType->SetText((const UTF8Char*)"-");
	}
	SSWR::AVIRead::AVIRCore::StreamType st;
	IO::Stream *stm = me->core->OpenStream(&st, me, 0, false);
	if (stm)
	{
		me->stm = stm;
		me->txtStreamType->SetText(SSWR::AVIRead::AVIRCore::GetStreamTypeName(st));
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnSpeedClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Double v;
	me->txtSpeedInput->GetText(&sb);
	if (Text::StrToDouble(sb.ToString(), &v))
	{
		if (v > 0 && v < 300)
		{
			me->speed = v;
			me->txtSpeed->SetText(sb.ToString());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	UTF8Char sbuff[64];
	if (me->stm)
	{
		if (me->points->GetCount() > 0)
		{
			Double destX = me->points->GetItem(0);
			Double destY = me->points->GetItem(1);
			Double dir = Math::ArcTan2(destY - me->currY, destX - me->currX);
			Double dist = me->wgs84->CalSurfaceDistanceXY(me->currX, me->currY, destX, destY, Math::Unit::Distance::DU_METER);
			Double maxDist = me->speed / 3.6;
			if (dist < maxDist)
			{
				me->currX = destX;
				me->currY = destY;
				me->GenRecord(me->currY, me->currX, dir * 180 / Math::PI, dist * 3.6 / 1.852, true);
				me->lbPoints->RemoveItem(0);
				me->points->RemoveRange(0, 2);
			}
			else
			{
				me->currX = me->currX + (destX - me->currX) * maxDist / dist;
				me->currY = me->currY + (destY - me->currY) * maxDist / dist;
				me->GenRecord(me->currY, me->currX, dir * 180 / Math::PI, maxDist * 3.6 / 1.852, true);
			}
			me->navi->ShowMarker(me->currY, me->currX);
			Text::StrDouble(sbuff, me->currX);
			me->txtCurrLon->SetText(sbuff);
			Text::StrDouble(sbuff, me->currY);
			me->txtCurrLat->SetText(sbuff);
		}
		else if (me->currX == 0 && me->currY == 0)
		{
			me->GenRecord(me->currY, me->currX, 0, 0, false);
		}
		else
		{
			me->GenRecord(me->currY, me->currX, 0, 0, true);
		}
	}
}

void SSWR::AVIRead::AVIRGPSSimulatorForm::GenRecord(Double lat, Double lon, Double dir, Double speed, Bool isValid)
{
	Char buff[256];
	Char *cptr;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (dir < 0)
	{
		dir += 360;
	}

	cptr = Text::StrConcat(buff, "$GPGGA,");
	cptr = dt.ToString(cptr, "HHmmss.fff");
	if (isValid)
	{
		cptr = Text::StrConcat(cptr, ",");
		cptr = GenLat(cptr, lat);
		cptr = Text::StrConcat(cptr, ",");
		cptr = GenLon(cptr, lon);
		cptr = Text::StrConcat(cptr, ",1"); //GPS fix
		cptr = Text::StrConcat(cptr, ",9"); //nSate View
		cptr = Text::StrConcat(cptr, ",1.5"); //HDOP
		cptr = Text::StrConcat(cptr, ",0.0,M"); //Altitude
		cptr = Text::StrConcat(cptr, ",0.0,M"); //Height of geoid
		cptr = Text::StrConcat(cptr, ","); //Time since last DGPS update
		cptr = Text::StrConcat(cptr, ","); //DGPS reference station id
	}
	else
	{
		cptr = Text::StrConcat(cptr, ",,,,,0,0,,,M,,M,,");
	}
	cptr = GenCheck(cptr, buff);
	*cptr++ = 13;
	*cptr++ = 10;
	this->stm->Write((UInt8*)buff, (UOSInt)(cptr - buff));

	cptr = Text::StrConcat(buff, "$GPGSA,A,3,,,,,,,,,,,,,1.5,1.5,1.5");
	cptr = GenCheck(cptr, buff);
	*cptr++ = 13;
	*cptr++ = 10;
	this->stm->Write((UInt8*)buff, (UOSInt)(cptr - buff));


	cptr = Text::StrConcat(buff, "$GPGSV,1,1,0");
	cptr = GenCheck(cptr, buff);
	*cptr++ = 13;
	*cptr++ = 10;
	this->stm->Write((UInt8*)buff, (UOSInt)(cptr - buff));

	cptr = Text::StrConcat(buff, "$GPRMC,");
	cptr = dt.ToString(cptr, "HHmmss.fff");
	if (isValid)
	{
		cptr = Text::StrConcat(cptr, ",A,");
		cptr = GenLat(cptr, lat);
		cptr = Text::StrConcat(cptr, ",");
		cptr = GenLon(cptr, lon);
		cptr = Text::StrConcat(cptr, ",");
		cptr = Text::StrDoubleFmt(cptr, speed, "0.00");
		cptr = Text::StrConcat(cptr, ",");
		cptr = Text::StrDoubleFmt(cptr, dir, "0.00");
		cptr = Text::StrConcat(cptr, ",");
	}
	else
	{
		cptr = Text::StrConcat(cptr, ",V,,,,,0.00,0.00,");
	}
	cptr = dt.ToString(cptr, "ddMMyy");
	cptr = Text::StrConcat(cptr, ",,,N");
	cptr = GenCheck(cptr, buff);
	*cptr++ = 13;
	*cptr++ = 10;
	this->stm->Write((UInt8*)buff, cptr - buff);
}

Char *SSWR::AVIRead::AVIRGPSSimulatorForm::GenLat(Char *ptr, Double lat)
{
	Bool neg = false;
	Int32 ival;
	if (lat < 0)
	{
		lat = -lat;
		neg = true;
	}
	ival = (Int32)lat;
	if (ival < 10)
	{
		ptr = Text::StrConcat(ptr, "0");
	}
	ptr = Text::StrInt32(ptr, ival);
	lat = (lat - ival) * 60.0;
	ptr = Text::StrDoubleFmt(ptr, lat, "00.0000");
	if (neg)
	{
		ptr = Text::StrConcat(ptr, ",S");
	}
	else
	{
		ptr = Text::StrConcat(ptr, ",N");
	}
	return ptr;
}

Char *SSWR::AVIRead::AVIRGPSSimulatorForm::GenLon(Char *ptr, Double lon)
{
	Bool neg = false;
	Int32 ival;
	if (lon < 0)
	{
		lon = -lon;
		neg = true;
	}
	ival = (Int32)lon;
	if (ival < 10)
	{
		ptr = Text::StrConcat(ptr, "00");
	}
	else if (ival < 100)
	{
		ptr = Text::StrConcat(ptr, "0");
	}
	ptr = Text::StrInt32(ptr, ival);
	lon = (lon - ival) * 60.0;
	ptr = Text::StrDoubleFmt(ptr, lon, "00.0000");
	if (neg)
	{
		ptr = Text::StrConcat(ptr, ",W");
	}
	else
	{
		ptr = Text::StrConcat(ptr, ",E");
	}
	return ptr;
}

Char *SSWR::AVIRead::AVIRGPSSimulatorForm::GenCheck(Char *ptr, Char *start)
{
	Char c = 0;
	start++; //skip $
	while (start < ptr)
	{
		c = c ^ *start++;
	}
	*ptr++ = '*';
	return Text::StrHexByte(ptr, c);
}

SSWR::AVIRead::AVIRGPSSimulatorForm::AVIRGPSSimulatorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi) : UI::GUIForm(parent, 480, 480, ui)
{
	this->core = core;
	this->navi = navi;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText((const UTF8Char*)"GPS Simulator");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);
	this->currX = 0;
	this->currY = 0;
	this->speed = 50;
	this->stm = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	NEW_CLASS(this->points, Data::ArrayList<Double>());

	NEW_CLASS(this->lblStreamType, UI::GUILabel(ui, this, (const UTF8Char*)"Stream Type"));
	this->lblStreamType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStreamType, UI::GUITextBox(ui, this, (const UTF8Char*)"-"));
	this->txtStreamType->SetRect(104, 4, 200, 23, false);
	this->txtStreamType->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this, (const UTF8Char*)"Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->chkAddPoints, UI::GUICheckBox(ui, this, (const UTF8Char*)"Add Points", false));
	this->chkAddPoints->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblSpeed, UI::GUILabel(ui, this, (const UTF8Char*)"Speed (km/s)"));
	this->lblSpeed->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSpeed, UI::GUITextBox(ui, this, (const UTF8Char*)"50"));
	this->txtSpeed->SetRect(104, 52, 100, 23, false);
	this->txtSpeed->SetReadOnly(true);
	NEW_CLASS(this->txtSpeedInput, UI::GUITextBox(ui, this, (const UTF8Char*)"50"));
	this->txtSpeedInput->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->btnSpeed, UI::GUIButton(ui, this, (const UTF8Char*)"Set"));
	this->btnSpeed->SetRect(304, 52, 75, 23, false);
	this->btnSpeed->HandleButtonClick(OnSpeedClicked, this);
	NEW_CLASS(this->lblCurrLat, UI::GUILabel(ui, this, (const UTF8Char*)"Current Lat"));
	this->lblCurrLat->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtCurrLat, UI::GUITextBox(ui, this, (const UTF8Char*)"0"));
	this->txtCurrLat->SetRect(104, 76, 150, 23, false);
	this->txtCurrLat->SetReadOnly(true);
	NEW_CLASS(this->lblCurrLon, UI::GUILabel(ui, this, (const UTF8Char*)"Current Lon"));
	this->lblCurrLon->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtCurrLon, UI::GUITextBox(ui, this, (const UTF8Char*)"0"));
	this->txtCurrLon->SetRect(104, 100, 150, 23, false);
	this->txtCurrLon->SetReadOnly(true);
	NEW_CLASS(this->lblPoints, UI::GUILabel(ui, this, (const UTF8Char*)"Points"));
	this->lblPoints->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->lbPoints, UI::GUIListBox(ui, this, false));
	this->lbPoints->SetRect(104, 124, 300, 300, false);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSSimulatorForm::~AVIRGPSSimulatorForm()
{
	SDEL_CLASS(this->stm);
	DEL_CLASS(this->points);
	DEL_CLASS(this->wgs84);
	this->navi->HideMarker();
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGPSSimulatorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
