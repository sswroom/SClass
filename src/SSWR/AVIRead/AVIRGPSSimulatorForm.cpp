#include "Stdafx.h"
#include "Data/DateTime.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGPSSimulatorForm.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FolderDialog.h"

Bool __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	if (!me->chkAddPoints->IsChecked())
		return false;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
	Math::Coord2DDbl pos;
	if (me->navi->GetCoordinateSystem()->Equals(me->wgs84))
	{
		pos = mapPos;
	}
	else
	{
		pos = Math::CoordinateSystem::Convert(me->navi->GetCoordinateSystem(), me->wgs84, mapPos);
	}
	if (me->currPos.IsZero())
	{
		me->currPos = pos;
		sptr = Text::StrDouble(sbuff, pos.GetLat());
		me->txtCurrLat->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, pos.GetLon());
		me->txtCurrLon->SetText(CSTRP(sbuff, sptr));
		me->navi->ShowMarker(pos);
	}
	else
	{
		sptr = Text::StrDouble(Text::StrConcatC(Text::StrDouble(sbuff, pos.GetLat()), UTF8STRC(", ")), pos.GetLon());
		me->lbPoints->AddItem(CSTRP(sbuff, sptr), 0);
		me->points.Add(pos);
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
		me->txtStreamType->SetText(CSTR("-"));
	}
	IO::StreamType st;
	IO::Stream *stm = me->core->OpenStream(&st, me, 0, false);
	if (stm)
	{
		me->stm = stm;
		me->txtStreamType->SetText(IO::StreamTypeGetName(st));
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnSpeedClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Double v;
	me->txtSpeedInput->GetText(sb);
	if (Text::StrToDouble(sb.ToString(), v))
	{
		if (v > 0 && v < 300)
		{
			me->speed = v;
			me->txtSpeed->SetText(sb.ToCString());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSSimulatorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRGPSSimulatorForm *me = (SSWR::AVIRead::AVIRGPSSimulatorForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->stm)
	{
		if (me->points.GetCount() > 0)
		{
			Math::Coord2DDbl destPos = me->points.GetItem(0);
			Double dir = Math_ArcTan2(destPos.y - me->currPos.y, destPos.x - me->currPos.x);
			Double dist = me->wgs84->CalSurfaceDistance(me->currPos, destPos, Math::Unit::Distance::DU_METER);
			Double maxDist = me->speed / 3.6;
			if (dist < maxDist)
			{
				me->currPos = destPos;
				me->GenRecord(me->currPos, dir * 180 / Math::PI, dist * 3.6 / 1.852, true);
				me->lbPoints->RemoveItem(0);
				me->points.RemoveAt(0);
			}
			else
			{
				me->currPos = me->currPos + (destPos - me->currPos) * maxDist / dist;
				me->GenRecord(me->currPos, dir * 180 / Math::PI, maxDist * 3.6 / 1.852, true);
			}
			me->navi->ShowMarker(me->currPos);
			sptr = Text::StrDouble(sbuff, me->currPos.GetLon());
			me->txtCurrLon->SetText(CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, me->currPos.GetLat());
			me->txtCurrLat->SetText(CSTRP(sbuff, sptr));
		}
		else if (me->currPos.IsZero())
		{
			me->GenRecord(me->currPos, 0, 0, false);
		}
		else
		{
			me->GenRecord(me->currPos, 0, 0, true);
		}
	}
}

void SSWR::AVIRead::AVIRGPSSimulatorForm::GenRecord(Math::Coord2DDbl pt, Double dir, Double speed, Bool isValid)
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
		cptr = GenLat(cptr, pt.GetLat());
		cptr = Text::StrConcat(cptr, ",");
		cptr = GenLon(cptr, pt.GetLon());
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
		cptr = GenLat(cptr, pt.GetLat());
		cptr = Text::StrConcat(cptr, ",");
		cptr = GenLon(cptr, pt.GetLon());
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
	this->stm->Write((UInt8*)buff, (UOSInt)(cptr - buff));
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
	return Text::StrHexByte(ptr, (UInt8)c);
}

SSWR::AVIRead::AVIRGPSSimulatorForm::AVIRGPSSimulatorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi) : UI::GUIForm(parent, 480, 480, ui)
{
	this->core = core;
	this->navi = navi;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("GPS Simulator"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->currPos = Math::Coord2DDbl(0, 0);
	this->speed = 50;
	this->stm = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateDefaultCsys();

	this->lblStreamType = ui->NewLabel(*this, CSTR("Stream Type"));
	this->lblStreamType->SetRect(4, 4, 100, 23, false);
	this->txtStreamType = ui->NewTextBox(*this, CSTR("-"));
	this->txtStreamType->SetRect(104, 4, 200, 23, false);
	this->txtStreamType->SetReadOnly(true);
	this->btnStream = ui->NewButton(*this, CSTR("Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->chkAddPoints, UI::GUICheckBox(ui, *this, CSTR("Add Points"), false));
	this->chkAddPoints->SetRect(104, 28, 200, 23, false);
	this->lblSpeed = ui->NewLabel(*this, CSTR("Speed (km/s)"));
	this->lblSpeed->SetRect(4, 52, 100, 23, false);
	this->txtSpeed = ui->NewTextBox(*this, CSTR("50"));
	this->txtSpeed->SetRect(104, 52, 100, 23, false);
	this->txtSpeed->SetReadOnly(true);
	this->txtSpeedInput = ui->NewTextBox(*this, CSTR("50"));
	this->txtSpeedInput->SetRect(204, 52, 100, 23, false);
	this->btnSpeed = ui->NewButton(*this, CSTR("Set"));
	this->btnSpeed->SetRect(304, 52, 75, 23, false);
	this->btnSpeed->HandleButtonClick(OnSpeedClicked, this);
	this->lblCurrLat = ui->NewLabel(*this, CSTR("Current Lat"));
	this->lblCurrLat->SetRect(4, 76, 100, 23, false);
	this->txtCurrLat = ui->NewTextBox(*this, CSTR("0"));
	this->txtCurrLat->SetRect(104, 76, 150, 23, false);
	this->txtCurrLat->SetReadOnly(true);
	this->lblCurrLon = ui->NewLabel(*this, CSTR("Current Lon"));
	this->lblCurrLon->SetRect(4, 100, 100, 23, false);
	this->txtCurrLon = ui->NewTextBox(*this, CSTR("0"));
	this->txtCurrLon->SetRect(104, 100, 150, 23, false);
	this->txtCurrLon->SetReadOnly(true);
	this->lblPoints = ui->NewLabel(*this, CSTR("Points"));
	this->lblPoints->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->lbPoints, UI::GUIListBox(ui, *this, false));
	this->lbPoints->SetRect(104, 124, 300, 300, false);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSSimulatorForm::~AVIRGPSSimulatorForm()
{
	SDEL_CLASS(this->stm);
	this->wgs84.Delete();
	this->navi->HideMarker();
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGPSSimulatorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
