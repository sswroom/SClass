#include "Stdafx.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "Math/CoordinateSystemManager.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/MessageDialog.h"

SSWR::AVIRead::AVIRGPSTrackerForm::DisplayOffButton::DisplayOffButton(UI::GUICore *ui, UI::GUIClientControl *parent, const UTF8Char *txt, AVIRGPSTrackerForm *frm) : UI::GUIButton(ui, parent, txt)
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

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnGPSUpdate(void *userObj, Map::GPSTrack::GPSRecord *record)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Double dist;
	Sync::MutexUsage mutUsage(me->recMut);
	MemCopyNO(&me->recCurr, record, sizeof(Map::GPSTrack::GPSRecord));
	me->recUpdated = true;
	if (me->gpsTrk && record->valid)
	{
		me->gpsTrk->AddRecord(record);
	}
	if (record->valid)
	{
		if (me->lastLat != 0 || me->lastLon != 0)
		{
			dist = me->wgs84->CalSurfaceDistanceXY(me->lastLon, me->lastLat, record->lon, record->lat, Math::Unit::Distance::DU_METER);
			me->dist += dist;
		}
		me->lastLat = record->lat;
		me->lastLon = record->lon;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnTimerTick(void *userObj)
{
	UTF8Char sbuff[256];
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	Data::DateTime dt;

	if (me->recUpdated)
	{
		me->lastUpdateTime->SetCurrTimeUTC();
		Sync::MutexUsage mutUsage(me->recMut);
		dt.SetTicks(me->recCurr.utcTimeTicks);
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		me->txtGPSTime->SetText(sbuff);
		Text::StrDouble(sbuff, me->recCurr.lat);
		me->txtLatitude->SetText(sbuff);
		Text::StrDouble(sbuff, me->recCurr.lon);
		me->txtLongitude->SetText(sbuff);
		Text::StrDouble(sbuff, me->recCurr.altitude);
		me->txtAltitude->SetText(sbuff);
		Text::StrDouble(sbuff, me->recCurr.speed);
		me->txtSpeed->SetText(sbuff);
		Text::StrDouble(sbuff, me->recCurr.heading);
		me->txtHeading->SetText(sbuff);
		me->txtGPSValid->SetText(me->recCurr.valid?(const UTF8Char*)"Valid":(const UTF8Char*)"Invalid");
		Text::StrInt32(sbuff, me->recCurr.nSateUsed);
		me->txtNSateUsed->SetText(sbuff);
		Text::StrInt32(sbuff, me->recCurr.nSateView);
		me->txtNSateView->SetText(sbuff);
		Text::StrDouble(sbuff, me->dist);
		me->txtDistance->SetText(sbuff);
		me->recUpdated = false;
		if (me->mapNavi && me->recCurr.valid)
		{
			me->mapNavi->ShowMarkerDir(me->recCurr.lat, me->recCurr.lon, me->recCurr.heading, Math::Unit::Angle::AU_DEGREE);
			if (me->chkAutoPan->IsChecked() && !me->mapNavi->InMap(me->recCurr.lat, me->recCurr.lon))
			{
				me->mapNavi->PanToMap(me->recCurr.lat, me->recCurr.lon);
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
		if (dt.DiffMS(me->lastUpdateTime) >= 20000)
		{
			me->lastUpdateTime->SetCurrTimeUTC();
			me->locSvc->ErrorRecover();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKFirmwareClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	if (mtk->QueryFirmware())
	{
		me->txtMTKRelease->SetText(mtk->GetFirmwareRel());
		me->txtMTKBuildID->SetText(mtk->GetFirmwareBuild());
		me->txtMTKProdMode->SetText(mtk->GetProductMode());
		me->txtMTKSDKVer->SetText(mtk->GetSDKVer());
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
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in downloading log data", (const UTF8Char*)"MTK GPS Tracker", me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKLogDeleteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGPSTrackerForm *me = (SSWR::AVIRead::AVIRGPSTrackerForm*)userObj;
	IO::Device::MTKGPSNMEA *mtk = (IO::Device::MTKGPSNMEA*)me->locSvc;
	if (UI::MessageDialog::ShowYesNoDialog((const UTF8Char*)"Are you sure to delete GPS log data?", (const UTF8Char*)"MTK GPS Tracker", me))
	{
		if (mtk->DelLogData())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Log data is deleted", (const UTF8Char*)"MTK GPS Tracker", me);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in deleting log data", (const UTF8Char*)"MTK GPS Tracker", me);
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
	if (UI::MessageDialog::ShowYesNoDialog((const UTF8Char*)"Are you sure to factory reset the device?", (const UTF8Char*)"Question", me))
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

SSWR::AVIRead::AVIRGPSTrackerForm::AVIRGPSTrackerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::ILocationService *locSvc, Bool toRelease) : UI::GUIForm(parent, 340, 348, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetText((const UTF8Char*)"GPS Tracker");

	this->core = core;
	this->locSvc = locSvc;
	this->relLocSvc = toRelease;
	this->recUpdated = false;
	this->gpsTrk = 0;
	this->mapNavi = 0;
	this->lastLat = 0;
	this->lastLon = 0;
	this->dist = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	this->dispOffClk = false;
	this->dispOffTime = 0;
	this->dispIsOff = false;
	NEW_CLASS(this->recMut, Sync::Mutex());
	this->locSvc->RegisterLocationHandler(OnGPSUpdate, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->lastUpdateTime, Data::DateTime());
	this->lastUpdateTime->SetCurrTimeUTC();

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLocation = this->tcMain->AddTabPage((const UTF8Char*)"Location");

	NEW_CLASS(this->lblGPSTime, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"GPS Time"));
	this->lblGPSTime->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtGPSTime, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtGPSTime->SetRect(108, 8, 150, 23, false);
	this->txtGPSTime->SetReadOnly(true);
	NEW_CLASS(this->lblLatitude, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Latutide"));
	this->lblLatitude->SetRect(8, 32, 100, 23, false);
	NEW_CLASS(this->txtLatitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtLatitude->SetRect(108, 32, 100, 23, false);
	this->txtLatitude->SetReadOnly(true);
	NEW_CLASS(this->lblLongitude, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Longitude"));
	this->lblLongitude->SetRect(8, 56, 100, 23, false);
	NEW_CLASS(this->txtLongitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtLongitude->SetRect(108, 56, 100, 23, false);
	this->txtLongitude->SetReadOnly(true);
	NEW_CLASS(this->lblAltitude, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Altitude"));
	this->lblAltitude->SetRect(8, 80, 100, 23, false);
	NEW_CLASS(this->txtAltitude, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtAltitude->SetRect(108, 80, 100, 23, false);
	this->txtAltitude->SetReadOnly(true);
	NEW_CLASS(this->lblSpeed, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Speed"));
	this->lblSpeed->SetRect(8, 104, 100, 23, false);
	NEW_CLASS(this->txtSpeed, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtSpeed->SetRect(108, 104, 100, 23, false);
	this->txtSpeed->SetReadOnly(true);
	NEW_CLASS(this->lblHeading, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Heading"));
	this->lblHeading->SetRect(8, 128, 100, 23, false);
	NEW_CLASS(this->txtHeading, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtHeading->SetRect(108, 128, 100, 23, false);
	this->txtHeading->SetReadOnly(true);
	NEW_CLASS(this->lblGPSValid, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"GPSValid"));
	this->lblGPSValid->SetRect(8, 152, 100, 23, false);
	NEW_CLASS(this->txtGPSValid, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtGPSValid->SetRect(108, 152, 100, 23, false);
	this->txtGPSValid->SetReadOnly(true);
	NEW_CLASS(this->lblNSateUsed, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Satelite Used"));
	this->lblNSateUsed->SetRect(8, 176, 100, 23, false);
	NEW_CLASS(this->txtNSateUsed, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateUsed->SetRect(108, 176, 100, 23, false);
	this->txtNSateUsed->SetReadOnly(true);
	NEW_CLASS(this->lblNSateView, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"Satelite View"));
	this->lblNSateView->SetRect(8, 200, 100, 23, false);
	NEW_CLASS(this->txtNSateView, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtNSateView->SetRect(108, 200, 100, 23, false);
	this->txtNSateView->SetReadOnly(true);
	NEW_CLASS(this->chkAutoPan, UI::GUICheckBox(ui, this->tpLocation, (const UTF8Char*)"Auto Pan", true));
	this->chkAutoPan->SetRect(108, 224, 100, 23, false);
	NEW_CLASS(this->chkTopMost, UI::GUICheckBox(ui, this->tpLocation, (const UTF8Char*)"Top Most", false));
	this->chkTopMost->SetRect(208, 224, 100, 23, false);
	this->chkTopMost->HandleCheckedChange(OnTopMostChg, this);
	NEW_CLASS(this->chkNoSleep, UI::GUICheckBox(ui, this->tpLocation, (const UTF8Char*)"Prevent Sleep", true));
	this->chkNoSleep->SetRect(108, 248, 100, 23, false);
	NEW_CLASS(this->btnDispOff, DisplayOffButton(ui, this->tpLocation, (const UTF8Char*)"Display Off", this));
	this->btnDispOff->SetRect(208, 248, 100, 23, false);
	this->btnDispOff->HandleButtonClick(OnDispOffClicked, this);
	NEW_CLASS(this->lblDistance, UI::GUILabel(ui, this->tpLocation, (const UTF8Char*)"H-Distance"));
	this->lblDistance->SetRect(8, 272, 100, 23, false);
	NEW_CLASS(this->txtDistance, UI::GUITextBox(ui, this->tpLocation, CSTR("")));
	this->txtDistance->SetRect(108, 272, 100, 23, false);
	this->txtDistance->SetReadOnly(true);

	this->tpAlert = this->tcMain->AddTabPage((const UTF8Char*)"Alert");
	NEW_CLASS(this->tcAlert, UI::GUITabControl(ui, this->tpAlert));
	this->tcAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertAdd = this->tcAlert->AddTabPage((const UTF8Char*)"Add");
	NEW_CLASS(this->lbAlertLyr, UI::GUIListBox(ui, this->tpAlertAdd, false));
	this->lbAlertLyr->SetRect(0, 0, 100, 23, false);
	this->lbAlertLyr->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspAlertAdd, UI::GUIHSplitter(ui, this->tpAlertAdd, 3, false));
	NEW_CLASS(this->lbAlertAdd, UI::GUIListBox(ui, this->tpAlertAdd, false));
	this->lbAlertAdd->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertView = this->tcAlert->AddTabPage((const UTF8Char*)"View");
	NEW_CLASS(this->pnlAlertView, UI::GUIPanel(ui, this->tpAlertView));
	this->pnlAlertView->SetRect(0, 0, 100, 31, false);
	this->pnlAlertView->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbAlert, UI::GUIListBox(ui, this->tpAlertView, false));
	this->lbAlert->SetDockType(UI::GUIControl::DOCK_FILL);

	if (this->locSvc->GetServiceType() == Map::ILocationService::ST_MTK)
	{
		this->tpMTK = this->tcMain->AddTabPage((const UTF8Char*)"MTK");
		NEW_CLASS(this->grpMTKFirmware, UI::GUIGroupBox(ui, this->tpMTK, (const UTF8Char*)"Firmware"));
		this->grpMTKFirmware->SetRect(0, 0, 340, 116, false);
		NEW_CLASS(this->lblMTKRelease, UI::GUILabel(ui, this->grpMTKFirmware, (const UTF8Char*)"Release"));
		this->lblMTKRelease->SetRect(0, 0, 100, 23, false);
		NEW_CLASS(this->txtMTKRelease, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKRelease->SetRect(100, 0, 150, 23, false);
		this->txtMTKRelease->SetReadOnly(true);
		NEW_CLASS(this->lblMTKBuildID, UI::GUILabel(ui, this->grpMTKFirmware, (const UTF8Char*)"Build"));
		this->lblMTKBuildID->SetRect(0, 24, 100, 23, false);
		NEW_CLASS(this->txtMTKBuildID, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKBuildID->SetRect(100, 24, 150, 23, false);
		this->txtMTKBuildID->SetReadOnly(true);
		NEW_CLASS(this->lblMTKProdMode, UI::GUILabel(ui, this->grpMTKFirmware, (const UTF8Char*)"Product Mode"));
		this->lblMTKProdMode->SetRect(0, 48, 100, 23, false);
		NEW_CLASS(this->txtMTKProdMode, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKProdMode->SetRect(100, 48, 100, 23, false);
		this->txtMTKProdMode->SetReadOnly(true);
		NEW_CLASS(this->lblMTKSDKVer, UI::GUILabel(ui, this->grpMTKFirmware, (const UTF8Char*)"SDK Version"));
		this->lblMTKSDKVer->SetRect(0, 72, 100, 23, false);
		NEW_CLASS(this->txtMTKSDKVer, UI::GUITextBox(ui, this->grpMTKFirmware, CSTR("")));
		this->txtMTKSDKVer->SetRect(100, 72, 100, 23, false);
		this->txtMTKSDKVer->SetReadOnly(true);
		NEW_CLASS(this->btnMTKFirmware, UI::GUIButton(ui, this->grpMTKFirmware, (const UTF8Char*)"Query"));
		this->btnMTKFirmware->SetRect(200, 72, 75, 23, false);
		this->btnMTKFirmware->HandleButtonClick(OnMTKFirmwareClicked, this);
		NEW_CLASS(this->btnMTKLogDownload, UI::GUIButton(ui, this->tpMTK, (const UTF8Char*)"Download Log"));
		this->btnMTKLogDownload->SetRect(0, 116, 75, 23, false);
		this->btnMTKLogDownload->HandleButtonClick(OnMTKLogDownloadClicked, this);
		NEW_CLASS(this->btnMTKLogDelete, UI::GUIButton(ui, this->tpMTK, (const UTF8Char*)"Delete Log"));
		this->btnMTKLogDelete->SetRect(80, 116, 75, 23, false);
		this->btnMTKLogDelete->HandleButtonClick(OnMTKLogDeleteClicked, this);
		NEW_CLASS(this->btnMTKFactoryReset, UI::GUIButton(ui, this->tpMTK, (const UTF8Char*)"Factory Reset"));
		this->btnMTKFactoryReset->SetRect(0, 140, 75, 23, false);
		this->btnMTKFactoryReset->HandleButtonClick(OnMTKFactoryResetClicked, this);

/*		NEW_CLASS(this->btnMTKTest, UI::GUIButton(ui, this->tpMTK, (const UTF8Char*)"Test"));
		this->btnMTKTest->SetRect(0, 140, 75, 23, false);
		this->btnMTKTest->HandleButtonClick(OnMTKTestClicked, this);*/
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSTrackerForm::~AVIRGPSTrackerForm()
{
	this->locSvc->UnregisterLocationHandler(OnGPSUpdate, this);
	if (this->mapNavi)
	{
		this->mapNavi->HideMarker();
	}
	DEL_CLASS(this->lastUpdateTime);
	if (this->relLocSvc)
	{
		DEL_CLASS(this->locSvc);
		this->relLocSvc = false;
	}
	DEL_CLASS(this->recMut);
	DEL_CLASS(this->wgs84);
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
	Sync::MutexUsage mutUsage(this->recMut);
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
