#include "Stdafx.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "Math/CoordinateSystemManager.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

#define NMEAMAXSIZE 128

SSWR::AVIRead::AVIRGPSTrackerForm::DisplayOffButton::DisplayOffButton(UI::GUICore *ui, UI::GUIClientControl *parent, Text::CString txt, AVIRGPSTrackerForm *frm) : UI::GUIButton(ui, parent, txt)
{
	this->frm = frm;
}

SSWR::AVIRead::AVIRGPSTrackerForm::DisplayOffButton::~DisplayOffButton()
{
}

void SSWR::AVIRead::AVIRGPSTrackerForm::DisplayOffButton::OnFocusLost()
{
	this->frm->DispOffFocusLost();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnGPSUpdate(void *userObj, Map::GPSTrack::GPSRecord3 *record, UOSInt sateCnt, Map::ILocationService::SateStatus *sates)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Double dist;
	Sync::MutexUsage mutUsage(&me->recMut);
	MemCopyNO(&me->recCurr, record, sizeof(Map::GPSTrack::GPSRecord3));
	me->recSateCnt = sateCnt;
	MemCopyNO(me->recSates, sates, sateCnt * sizeof(Map::ILocationService::SateStatus));
	me->recUpdated = true;
	if (me->gpsTrk && record->valid)
	{
		me->gpsTrk->AddRecord(record);
	}
	if (record->valid)
	{
		if (!me->lastPos.IsZero())
		{
			dist = me->wgs84->CalSurfaceDistanceXY(me->lastPos, record->pos, Math::Unit::Distance::DU_METER);
			me->dist += dist;
		}
		me->lastPos = record->pos;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnTimerTick(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Data::DateTime dt;

	if (me->locSvc->IsDown() != me->lastDown)
	{
		me->lastDown = !me->lastDown;
		if (me->lastDown)
		{
			me->txtStreamStatus->SetText(CSTR("Down"));
		}
		else
		{
			me->txtStreamStatus->SetText(CSTR("Up"));
		}
	}
	if (me->recUpdated)
	{
		me->lastUpdateTime.SetCurrTimeUTC();
		Sync::MutexUsage mutUsage(&me->recMut);
		dt.SetInstant(me->recCurr.recTime);
		sptr = dt.ToStringNoZone(sbuff);
		me->txtGPSTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->recCurr.pos.GetLat());
		me->txtLatitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->recCurr.pos.GetLon());
		me->txtLongitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->recCurr.altitude);
		me->txtAltitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->recCurr.speed);
		me->txtSpeed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->recCurr.heading);
		me->txtHeading->SetText(CSTRP(sbuff, sptr));
		if (me->recCurr.valid)
			me->txtGPSValid->SetText(CSTR("Valid"));
		else
			me->txtGPSValid->SetText(CSTR("Invalid"));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateUsed);
		me->txtNSateUsed->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateUsedGPS);
		me->txtNSateUsedGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateUsedSBAS);
		me->txtNSateUsedSBAS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateUsedGLO);
		me->txtNSateUsedGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateViewGPS);
		me->txtNSateViewGPS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateViewGLO);
		me->txtNSateViewGLO->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateViewGA);
		me->txtNSateViewGA->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateViewQZSS);
		me->txtNSateViewQZSS->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, me->recCurr.nSateViewBD);
		me->txtNSateViewBD->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, me->dist);
		me->txtDistance->SetText(CSTRP(sbuff, sptr));

		me->lvSate->ClearItems();
		UOSInt i = 0;
		while (i < me->recSateCnt)
		{
			me->lvSate->AddItem(Map::ILocationService::SateTypeGetName(me->recSates[i].sateType), 0);
			sptr = Text::StrUInt16(sbuff, me->recSates[i].prn);
			me->lvSate->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->recSates[i].elev);
			me->lvSate->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, me->recSates[i].azimuth);
			me->lvSate->SetSubItem(i, 3, CSTRP(sbuff, sptr));
			if (me->recSates[i].snr >= 0)
			{
				sptr = Text::StrInt16(sbuff, me->recSates[i].snr);
				me->lvSate->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			}
			i++;
		}


		me->recUpdated = false;
		if (me->mapNavi && me->recCurr.valid)
		{
			me->mapNavi->ShowMarkerDir(me->recCurr.pos, me->recCurr.heading, Math::Unit::Angle::AU_DEGREE);
			if (me->chkAutoPan->IsChecked() && !me->mapNavi->InMap(me->recCurr.pos))
			{
				me->mapNavi->PanToMap(me->recCurr.pos);
			}
		}
		mutUsage.EndUse();

		if (me->chkNoSleep->IsChecked())
		{
			me->ui->UseDevice(true, false);
		}
	}
	else
	{
		dt.SetCurrTimeUTC();
		if (dt.DiffMS(&me->lastUpdateTime) >= 20000)
		{
			me->lastUpdateTime.SetCurrTimeUTC();
			me->locSvc->ErrorRecover();
		}
	}
	if (me->nmeaUpdated)
	{
		Sync::MutexUsage nmeaMut(&me->nmeaMut);
		me->nmeaUpdated = false;
		UOSInt i = me->nmeaIndex;
		me->lbNMEA->ClearItems();
		if (me->nmeaBuff[i])
		{
			me->lbNMEA->AddItem(me->nmeaBuff[i]->ToCString(), 0);
		}
		i = (i + 1) & (NMEAMAXSIZE - 1);
		while (i != me->nmeaIndex)
		{
			if (me->nmeaBuff[i])
			{
				me->lbNMEA->AddItem(me->nmeaBuff[i]->ToCString(), 0);
			}
			i = (i + 1) & (NMEAMAXSIZE - 1);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKFirmwareClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	if (mtk->QueryFirmware())
	{
		me->txtMTKRelease->SetText(mtk->GetFirmwareRel()->ToCString());
		me->txtMTKBuildID->SetText(mtk->GetFirmwareBuild()->ToCString());
		me->txtMTKProdMode->SetText(mtk->GetProductMode()->ToCString());
		me->txtMTKSDKVer->SetText(mtk->GetSDKVer()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKLogDownloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	if (me->mapNavi)
	{
		Map::GPSTrack *gpsTrk;
		NEW_CLASS(gpsTrk, Map::GPSTrack(CSTR("MTK_GPS"), true, 0, CSTR_NULL));
		if (mtk->ParseLog(gpsTrk))
		{
			me->mapNavi->AddLayer(gpsTrk);
		}
		else
		{
			DEL_CLASS(gpsTrk);
			UI::MessageDialog::ShowDialog(CSTR("Error in downloading log data"), CSTR("MTK GPS Tracker"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKLogDeleteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	if (UI::MessageDialog::ShowYesNoDialog(CSTR("Are you sure to delete GPS log data?"), CSTR("MTK GPS Tracker"), me))
	{
		if (mtk->DelLogData())
		{
			UI::MessageDialog::ShowDialog(CSTR("Log data is deleted"), CSTR("MTK GPS Tracker"), me);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in deleting log data"), CSTR("MTK GPS Tracker"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKTestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	mtk->GetLogFormat();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKFactoryResetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	if (UI::MessageDialog::ShowYesNoDialog(CSTR("Are you sure to factory reset the device?"), CSTR("Question"), me))
	{
		IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
		mtk->FactoryReset();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnDispOffClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	me->dispOffTime = dt.ToTicks();
	me->dispOffClk = true;
	me->ui->DisplayOff();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnTopMostChg(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	me->SetAlwaysOnTop(newState);
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnNMEALine(void *userObj, const UTF8Char *line, UOSInt lineLen)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Sync::MutexUsage mutUsage(&me->nmeaMut);
	SDEL_STRING(me->nmeaBuff[me->nmeaIndex]);
	me->nmeaBuff[me->nmeaIndex] = Text::String::New(line, lineLen);
	me->nmeaIndex = (me->nmeaIndex + 1) & (NMEAMAXSIZE - 1);
	me->nmeaUpdated = true;
}

SSWR::AVIRead::AVIRGPSTrackerForm::AVIRGPSTrackerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::ILocationService *locSvc, Bool toRelease) : UI::GUIForm(parent, 340, 540, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("GPS Tracker"));

	this->core = core;
	this->locSvc = locSvc;
	this->relLocSvc = toRelease;
	this->recUpdated = false;
	this->recSateCnt = 0;
	this->gpsTrk = 0;
	this->mapNavi = 0;
	this->lastPos = Math::Coord2DDbl(0, 0);
	this->dist = 0;
	this->lastDown = true;
	this->nmeaIndex = 0;
	this->nmeaUpdated = false;
	this->nmeaBuff = MemAlloc(Text::String*, NMEAMAXSIZE);
	MemClear(this->nmeaBuff, NMEAMAXSIZE * sizeof(Text::String*));
	this->wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	this->dispOffClk = false;
	this->dispOffTime = 0;
	this->dispIsOff = false;
	this->locSvc->RegisterLocationHandler(OnGPSUpdate, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lastUpdateTime.SetCurrTimeUTC();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLocation = this->tcMain->AddTabPage(CSTR("Location"));

	NEW_CLASS(this->lblStreamStatus, UI::GUILabel(ui, this->tpLocation, CSTR("Stream Status")));
	this->lblStreamStatus->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtStreamStatus, UI::GUITextBox(ui, this->tpLocation, CSTR("Down")));
	this->txtStreamStatus->SetRect(108, 8, 150, 23, false);
	this->txtStreamStatus->SetReadOnly(true);
	NEW_CLASS(this->lblGPSTime, UI::GUILabel(ui, this->tpLocation, CSTR("GPS Time")));
	this->lblGPSTime->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtGPSTime, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtGPSTime->SetRect(108, 32, 150, 23, false);
	this->txtGPSTime->SetReadOnly(true);
	NEW_CLASS(this->lblLatitude, UI::GUILabel(ui, this->tpLocation, CSTR("Latutide")));
	this->lblLatitude->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtLatitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtLatitude->SetRect(108, 56, 100, 23, false);
	this->txtLatitude->SetReadOnly(true);
	NEW_CLASS(this->lblLongitude, UI::GUILabel(ui, this->tpLocation, CSTR("Longitude")));
	this->lblLongitude->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->txtLongitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtLongitude->SetRect(108, 80, 100, 23, false);
	this->txtLongitude->SetReadOnly(true);
	NEW_CLASS(this->lblAltitude, UI::GUILabel(ui, this->tpLocation, CSTR("Altitude")));
	this->lblAltitude->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->txtAltitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtAltitude->SetRect(108, 104, 100, 23, false);
	this->txtAltitude->SetReadOnly(true);
	NEW_CLASS(this->lblSpeed, UI::GUILabel(ui, this->tpLocation, CSTR("Speed")));
	this->lblSpeed->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtSpeed, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtSpeed->SetRect(108, 128, 100, 23, false);
	this->txtSpeed->SetReadOnly(true);
	NEW_CLASS(this->lblHeading, UI::GUILabel(ui, this->tpLocation, CSTR("Heading")));
	this->lblHeading->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->txtHeading, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtHeading->SetRect(108, 152, 100, 23, false);
	this->txtHeading->SetReadOnly(true);
	NEW_CLASS(this->lblGPSValid, UI::GUILabel(ui, this->tpLocation, CSTR("GPSValid")));
	this->lblGPSValid->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->txtGPSValid, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtGPSValid->SetRect(108, 176, 100, 23, false);
	this->txtGPSValid->SetReadOnly(true);
	NEW_CLASS(this->lblNSateUsed, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite Used")));
	this->lblNSateUsed->SetRect(8, 200, 100, 23, false);
	NEW_CLASS(this->txtNSateUsed, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateUsed->SetRect(108, 200, 100, 23, false);
	this->txtNSateUsed->SetReadOnly(true);
	NEW_CLASS(this->lblNSateUsedGPS, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite Used GPS")));
	this->lblNSateUsedGPS->SetRect(8, 224, 100, 23, false);
	NEW_CLASS(this->txtNSateUsedGPS, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateUsedGPS->SetRect(108, 224, 100, 23, false);
	this->txtNSateUsedGPS->SetReadOnly(true);
	NEW_CLASS(this->lblNSateUsedSBAS, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite Used SBAS")));
	this->lblNSateUsedSBAS->SetRect(8, 248, 100, 23, false);
	NEW_CLASS(this->txtNSateUsedSBAS, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateUsedSBAS->SetRect(108, 248, 100, 23, false);
	this->txtNSateUsedSBAS->SetReadOnly(true);
	NEW_CLASS(this->lblNSateUsedGLO, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite Used GLO")));
	this->lblNSateUsedGLO->SetRect(8, 272, 100, 23, false);
	NEW_CLASS(this->txtNSateUsedGLO, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateUsedGLO->SetRect(108, 272, 100, 23, false);
	this->txtNSateUsedGLO->SetReadOnly(true);
	NEW_CLASS(this->lblNSateViewGPS, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite View GPS")));
	this->lblNSateViewGPS->SetRect(8, 296, 100, 23, false);
	NEW_CLASS(this->txtNSateViewGPS, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateViewGPS->SetRect(108, 296, 100, 23, false);
	this->txtNSateViewGPS->SetReadOnly(true);
	NEW_CLASS(this->lblNSateViewGLO, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite View GLO")));
	this->lblNSateViewGLO->SetRect(8, 320, 100, 23, false);
	NEW_CLASS(this->txtNSateViewGLO, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateViewGLO->SetRect(108, 320, 100, 23, false);
	this->txtNSateViewGLO->SetReadOnly(true);
	NEW_CLASS(this->lblNSateViewGA, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite View GA")));
	this->lblNSateViewGA->SetRect(8, 344, 100, 23, false);
	NEW_CLASS(this->txtNSateViewGA, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateViewGA->SetRect(108, 344, 100, 23, false);
	this->txtNSateViewGA->SetReadOnly(true);
	NEW_CLASS(this->lblNSateViewQZSS, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite View QZSS")));
	this->lblNSateViewQZSS->SetRect(8, 368, 100, 23, false);
	NEW_CLASS(this->txtNSateViewQZSS, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateViewQZSS->SetRect(108, 368, 100, 23, false);
	this->txtNSateViewQZSS->SetReadOnly(true);
	NEW_CLASS(this->lblNSateViewBD, UI::GUILabel(ui, this->tpLocation, CSTR("Satelite View BD")));
	this->lblNSateViewBD->SetRect(8, 392, 100, 23, false);
	NEW_CLASS(this->txtNSateViewBD, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateViewBD->SetRect(108, 392, 100, 23, false);
	this->txtNSateViewBD->SetReadOnly(true);
	NEW_CLASS(this->chkAutoPan, UI::GUICheckBox(ui, this->tpLocation, CSTR("Auto Pan"), true));
	this->chkAutoPan->SetRect(108, 416, 100, 23, false);
	NEW_CLASS(this->chkTopMost, UI::GUICheckBox(ui, this->tpLocation, CSTR("Top Most"), false));
	this->chkTopMost->SetRect(208, 416, 100, 23, false);
	this->chkTopMost->HandleCheckedChange(OnTopMostChg, this);
	NEW_CLASS(this->chkNoSleep, UI::GUICheckBox(ui, this->tpLocation, CSTR("Prevent Sleep"), true));
	this->chkNoSleep->SetRect(108, 440, 100, 23, false);
	NEW_CLASS(this->btnDispOff, DisplayOffButton(ui, this->tpLocation, CSTR("Display Off"), this));
	this->btnDispOff->SetRect(208, 440, 100, 23, false);
	this->btnDispOff->HandleButtonClick(OnDispOffClicked, this);
	NEW_CLASS(this->lblDistance, UI::GUILabel(ui, this->tpLocation, CSTR("H-Distance")));
	this->lblDistance->SetRect(8, 464, 100, 23, false);
	NEW_CLASS(this->txtDistance, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtDistance->SetRect(108, 464, 100, 23, false);
	this->txtDistance->SetReadOnly(true);

	this->tpAlert = this->tcMain->AddTabPage(CSTR("Alert"));
	NEW_CLASS(this->tcAlert, UI::GUITabControl(ui, this->tpAlert));
	this->tcAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertAdd = this->tcAlert->AddTabPage(CSTR("Add"));
	NEW_CLASS(this->lbAlertLyr, UI::GUIListBox(ui, this->tpAlertAdd, false));
	this->lbAlertLyr->SetRect(0, 0, 100, 23, false);
	this->lbAlertLyr->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspAlertAdd, UI::GUIHSplitter(ui, this->tpAlertAdd, 3, false));
	NEW_CLASS(this->lbAlertAdd, UI::GUIListBox(ui, this->tpAlertAdd, false));
	this->lbAlertAdd->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertView = this->tcAlert->AddTabPage(CSTR("View"));
	NEW_CLASS(this->pnlAlertView, UI::GUIPanel(ui, this->tpAlertView));
	this->pnlAlertView->SetRect(0, 0, 100, 31, false);
	this->pnlAlertView->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbAlert, UI::GUIListBox(ui, this->tpAlertView, false));
	this->lbAlert->SetDockType(UI::GUIControl::DOCK_FILL);

	if (this->locSvc->GetServiceType() == Map::ILocationService::ST_MTK)
	{
		this->tpMTK = this->tcMain->AddTabPage(CSTR("MTK"));
		NEW_CLASS(this->grpMTKFirmware, UI::GUIGroupBox(ui, this->tpMTK, CSTR("Firmware")));
		this->grpMTKFirmware->SetRect(0, 0, 340, 116, false);
		NEW_CLASS(this->lblMTKRelease, UI::GUILabel(ui, this->grpMTKFirmware, CSTR("Release")));
		this->lblMTKRelease->SetRect(0, 0, 100, 23, false);
		NEW_CLASS(this->txtMTKRelease, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKRelease->SetRect(100, 0, 150, 23, false);
		this->txtMTKRelease->SetReadOnly(true);
		NEW_CLASS(this->lblMTKBuildID, UI::GUILabel(ui, this->grpMTKFirmware, CSTR("Build")));
		this->lblMTKBuildID->SetRect(0, 24, 100, 23, false);
		NEW_CLASS(this->txtMTKBuildID, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKBuildID->SetRect(100, 24, 150, 23, false);
		this->txtMTKBuildID->SetReadOnly(true);
		NEW_CLASS(this->lblMTKProdMode, UI::GUILabel(ui, this->grpMTKFirmware, CSTR("Product Mode")));
		this->lblMTKProdMode->SetRect(0, 48, 100, 23, false);
		NEW_CLASS(this->txtMTKProdMode, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKProdMode->SetRect(100, 48, 100, 23, false);
		this->txtMTKProdMode->SetReadOnly(true);
		NEW_CLASS(this->lblMTKSDKVer, UI::GUILabel(ui, this->grpMTKFirmware, CSTR("SDK Version")));
		this->lblMTKSDKVer->SetRect(0, 72, 100, 23, false);
		NEW_CLASS(this->txtMTKSDKVer, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKSDKVer->SetRect(100, 72, 100, 23, false);
		this->txtMTKSDKVer->SetReadOnly(true);
		NEW_CLASS(this->btnMTKFirmware, UI::GUIButton(ui, this->grpMTKFirmware, CSTR("Query")));
		this->btnMTKFirmware->SetRect(200, 72, 75, 23, false);
		this->btnMTKFirmware->HandleButtonClick(OnMTKFirmwareClicked, this);
		NEW_CLASS(this->btnMTKLogDownload, UI::GUIButton(ui, this->tpMTK, CSTR("Download Log")));
		this->btnMTKLogDownload->SetRect(0, 116, 75, 23, false);
		this->btnMTKLogDownload->HandleButtonClick(OnMTKLogDownloadClicked, this);
		NEW_CLASS(this->btnMTKLogDelete, UI::GUIButton(ui, this->tpMTK, CSTR("Delete Log")));
		this->btnMTKLogDelete->SetRect(80, 116, 75, 23, false);
		this->btnMTKLogDelete->HandleButtonClick(OnMTKLogDeleteClicked, this);
		NEW_CLASS(this->btnMTKFactoryReset, UI::GUIButton(ui, this->tpMTK, CSTR("Factory Reset")));
		this->btnMTKFactoryReset->SetRect(0, 140, 75, 23, false);
		this->btnMTKFactoryReset->HandleButtonClick(OnMTKFactoryResetClicked, this);

/*		NEW_CLASS(this->btnMTKTest, UI::GUIButton(ui, this->tpMTK, CSTR("Test")));
		this->btnMTKTest->SetRect(0, 140, 75, 23, false);
		this->btnMTKTest->HandleButtonClick(OnMTKTestClicked, this);*/
	}

	this->tpSate = this->tcMain->AddTabPage(CSTR("Satellite"));
	NEW_CLASS(this->lvSate, UI::GUIListView(ui, this->tpSate, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvSate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSate->AddColumn(CSTR("Type"), 80);
	this->lvSate->AddColumn(CSTR("PRN"), 60);
	this->lvSate->AddColumn(CSTR("Elevation"), 60);
	this->lvSate->AddColumn(CSTR("Azimuth"), 60);
	this->lvSate->AddColumn(CSTR("SNR"), 60);
	this->lvSate->SetShowGrid(true);
	this->lvSate->SetFullRowSelect(true);

	this->tpNMEA = this->tcMain->AddTabPage(CSTR("NMEA"));
	NEW_CLASS(this->lbNMEA, UI::GUIListBox(ui, this->tpNMEA, false));
	this->lbNMEA->SetDockType(UI::GUIControl::DOCK_FILL);
	((IO::GPSNMEA*)this->locSvc)->HandleCommand(OnNMEALine, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSTrackerForm::~AVIRGPSTrackerForm()
{
	this->locSvc->UnregisterLocationHandler(OnGPSUpdate, this);
	if (this->mapNavi)
	{
		this->mapNavi->HideMarker();
	}
	if (this->relLocSvc)
	{
		DEL_CLASS(this->locSvc);
		this->relLocSvc = false;
	}
	DEL_CLASS(this->wgs84);
	UOSInt i = NMEAMAXSIZE;
	while (i-- > 0)
	{
		SDEL_STRING(this->nmeaBuff[i]);
	}
	MemFree(this->nmeaBuff);
}

void SSWR::AVIRead::AVIRGPSTrackerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGPSTrackerForm::OnFocus()
{
	if (this->dispIsOff)
	{
		this->dispIsOff = false;
		this->mapNavi->ResumeUpdate();
	}
}

void SSWR::AVIRead::AVIRGPSTrackerForm::SetGPSTrack(Map::GPSTrack *gpsTrk)
{
	Sync::MutexUsage mutUsage(&this->recMut);
	this->gpsTrk = gpsTrk;
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRGPSTrackerForm::SetMapNavigator(SSWR::AVIRead::IMapNavigator *mapNavi)
{
	if (this->mapNavi)
	{
		this->mapNavi->HideMarker();
	}
	this->mapNavi = mapNavi;
	if (this->mapNavi)
	{
	}
}

void SSWR::AVIRead::AVIRGPSTrackerForm::DispOffFocusLost()
{
	if (this->dispOffClk)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if ((dt.ToTicks() - this->dispOffTime) < 1000)
		{
			this->dispOffClk = false;
			if (this->mapNavi)
			{
				this->mapNavi->PauseUpdate();
				this->dispIsOff = true;
			}
		}
		else
		{
			this->dispOffClk = false;
		}
	}
}
