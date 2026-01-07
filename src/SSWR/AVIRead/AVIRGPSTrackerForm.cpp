#include "Stdafx.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "Math/CoordinateSystemManager.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

#define NMEAMAXSIZE 128

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnGPSUpdate(AnyType userObj, NN<Map::GPSTrack::GPSRecord3> record, Data::DataArray<Map::LocationService::SateStatus> sates)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	Double dist;
	NN<Map::GPSTrack> gpsTrk;
	Sync::MutexUsage mutUsage(me->recMut);
	MemCopyNO(&me->recCurr, record.Ptr(), sizeof(Map::GPSTrack::GPSRecord3));
	me->recSateCnt = sates.GetCount();
	MemCopyNO(me->recSates, sates.Arr().Ptr(), sates.GetCount() * sizeof(Map::LocationService::SateStatus));
	me->recUpdated = true;
	if (me->gpsTrk.SetTo(gpsTrk) && record->valid)
	{
		gpsTrk->AddRecord(record);
	}
	if (record->valid)
	{
		if (!me->lastPos.IsZero())
		{
			dist = me->wgs84->CalSurfaceDistance(me->lastPos, record->pos, Math::Unit::Distance::DU_METER);
			me->dist += dist;
		}
		me->lastPos = record->pos;
	}
	mutUsage.EndUse();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnTimerTick(AnyType userObj)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	Data::DateTime dt;
	NN<SSWR::AVIRead::AVIRMapNavigator> mapNavi;

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
		Sync::MutexUsage mutUsage(me->recMut);
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
			me->lvSate->AddItem(Map::LocationService::SateTypeGetName(me->recSates[i].sateType), 0);
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
		if (me->mapNavi.SetTo(mapNavi) && me->recCurr.valid)
		{
			mapNavi->ShowMarkerDir(me->recCurr.pos, me->recCurr.heading, Math::Unit::Angle::AU_DEGREE);
			if (me->chkAutoPan->IsChecked() && !mapNavi->InMap(me->recCurr.pos))
			{
				mapNavi->PanToMap(me->recCurr.pos);
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
			me->lastUpdateTime.SetCurrTimeUTC();
			me->locSvc->ErrorRecover();
		}
	}
	if (me->nmeaUpdated)
	{
		Sync::MutexUsage nmeaMutUsage(me->nmeaMut);
		me->nmeaUpdated = false;
		NN<Text::String> s;
		UOSInt i = me->nmeaIndex;
		me->lbNMEA->ClearItems();
		if (me->nmeaBuff[i].SetTo(s))
		{
			me->lbNMEA->AddItem(s->ToCString(), 0);
		}
		i = (i + 1) & (NMEAMAXSIZE - 1);
		while (i != me->nmeaIndex)
		{
			if (me->nmeaBuff[i].SetTo(s))
			{
				me->lbNMEA->AddItem(s->ToCString(), 0);
			}
			i = (i + 1) & (NMEAMAXSIZE - 1);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKFirmwareClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	NN<IO::Device::MTKGPSNMEA> mtk = NN<IO::Device::MTKGPSNMEA>::ConvertFrom(me->locSvc);
	if (mtk->QueryFirmware())
	{
		me->txtMTKRelease->SetText(Text::String::OrEmpty(mtk->GetFirmwareRel())->ToCString());
		me->txtMTKBuildID->SetText(Text::String::OrEmpty(mtk->GetFirmwareBuild())->ToCString());
		me->txtMTKProdMode->SetText(Text::String::OrEmpty(mtk->GetProductMode())->ToCString());
		me->txtMTKSDKVer->SetText(Text::String::OrEmpty(mtk->GetSDKVer())->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKLogDownloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	NN<IO::Device::MTKGPSNMEA> mtk = NN<IO::Device::MTKGPSNMEA>::ConvertFrom(me->locSvc);
	NN<SSWR::AVIRead::AVIRMapNavigator> mapNavi;
	if (me->mapNavi.SetTo(mapNavi))
	{
		NN<Map::GPSTrack> gpsTrk;
		NEW_CLASSNN(gpsTrk, Map::GPSTrack(CSTR("MTK_GPS"), true, 0, nullptr));
		if (mtk->ParseLog(gpsTrk))
		{
			mapNavi->AddLayer(gpsTrk);
		}
		else
		{
			gpsTrk.Delete();
			me->ui->ShowMsgOK(CSTR("Error in downloading log data"), CSTR("MTK GPS Tracker"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKLogDeleteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	NN<IO::Device::MTKGPSNMEA> mtk = NN<IO::Device::MTKGPSNMEA>::ConvertFrom(me->locSvc);
	if (me->ui->ShowMsgYesNo(CSTR("Are you sure to delete GPS log data?"), CSTR("MTK GPS Tracker"), me))
	{
		if (mtk->DelLogData())
		{
			me->ui->ShowMsgOK(CSTR("Log data is deleted"), CSTR("MTK GPS Tracker"), me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in deleting log data"), CSTR("MTK GPS Tracker"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKTestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	NN<IO::Device::MTKGPSNMEA> mtk = NN<IO::Device::MTKGPSNMEA>::ConvertFrom(me->locSvc);
	mtk->GetLogFormat();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnMTKFactoryResetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	if (me->ui->ShowMsgYesNo(CSTR("Are you sure to factory reset the device?"), CSTR("Question"), me))
	{
		NN<IO::Device::MTKGPSNMEA> mtk = NN<IO::Device::MTKGPSNMEA>::ConvertFrom(me->locSvc);
		mtk->FactoryReset();
	}
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnDispOffClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	me->dispOffTime = dt.ToTicks();
	me->dispOffClk = true;
	me->ui->DisplayOff();
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnTopMostChg(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	me->SetAlwaysOnTop(newState);
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::OnNMEALine(AnyType userObj, UnsafeArray<const UTF8Char> line, UOSInt lineLen)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	Sync::MutexUsage mutUsage(me->nmeaMut);
	OPTSTR_DEL(me->nmeaBuff[me->nmeaIndex]);
	me->nmeaBuff[me->nmeaIndex] = Text::String::New(line, lineLen);
	me->nmeaIndex = (me->nmeaIndex + 1) & (NMEAMAXSIZE - 1);
	me->nmeaUpdated = true;
}

void __stdcall SSWR::AVIRead::AVIRGPSTrackerForm::SignalFocusLost(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGPSTrackerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGPSTrackerForm>();
	me->DispOffFocusLost();
}

NN<UI::GUIButton> SSWR::AVIRead::AVIRGPSTrackerForm::NewDisplayOffButton(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, Text::CStringNN txt)
{
	NN<UI::GUIButton> btn = ui->NewButton(parent, txt);
	btn->HandleFocusLost(SignalFocusLost, this);
	return btn;
}

SSWR::AVIRead::AVIRGPSTrackerForm::AVIRGPSTrackerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::LocationService> locSvc, Bool toRelease) : UI::GUIForm(parent, 340, 540, ui)
{
	this->SetFont(nullptr, 8.25, false);
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
	this->nmeaBuff = MemAllocArr(Optional<Text::String>, NMEAMAXSIZE);
	MemClear(this->nmeaBuff.Ptr(), NMEAMAXSIZE * sizeof(Optional<Text::String>));
	this->wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->dispOffClk = false;
	this->dispOffTime = 0;
	this->dispIsOff = false;
	this->locSvc->RegisterLocationHandler(OnGPSUpdate, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lastUpdateTime.SetCurrTimeUTC();

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLocation = this->tcMain->AddTabPage(CSTR("Location"));

	this->lblStreamStatus = ui->NewLabel(this->tpLocation, CSTR("Stream Status"));
	this->lblStreamStatus->SetRect(8, 8, 100, 23, false);
	this->txtStreamStatus = ui->NewTextBox(this->tpLocation, CSTR("Down"));
	this->txtStreamStatus->SetRect(108, 8, 150, 23, false);
	this->txtStreamStatus->SetReadOnly(true);
	this->lblGPSTime = ui->NewLabel(this->tpLocation, CSTR("GPS Time"));
	this->lblGPSTime->SetRect(8, 32, 100, 23, false);
	this->txtGPSTime = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtGPSTime->SetRect(108, 32, 150, 23, false);
	this->txtGPSTime->SetReadOnly(true);
	this->lblLatitude = ui->NewLabel(this->tpLocation, CSTR("Latutide"));
	this->lblLatitude->SetRect(8, 56, 100, 23, false);
	this->txtLatitude = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtLatitude->SetRect(108, 56, 100, 23, false);
	this->txtLatitude->SetReadOnly(true);
	this->lblLongitude = ui->NewLabel(this->tpLocation, CSTR("Longitude"));
	this->lblLongitude->SetRect(8, 80, 100, 23, false);
	this->txtLongitude = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtLongitude->SetRect(108, 80, 100, 23, false);
	this->txtLongitude->SetReadOnly(true);
	this->lblAltitude = ui->NewLabel(this->tpLocation, CSTR("Altitude"));
	this->lblAltitude->SetRect(8, 104, 100, 23, false);
	this->txtAltitude = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtAltitude->SetRect(108, 104, 100, 23, false);
	this->txtAltitude->SetReadOnly(true);
	this->lblSpeed = ui->NewLabel(this->tpLocation, CSTR("Speed"));
	this->lblSpeed->SetRect(8, 128, 100, 23, false);
	this->txtSpeed = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtSpeed->SetRect(108, 128, 100, 23, false);
	this->txtSpeed->SetReadOnly(true);
	this->lblHeading = ui->NewLabel(this->tpLocation, CSTR("Heading"));
	this->lblHeading->SetRect(8, 152, 100, 23, false);
	this->txtHeading = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtHeading->SetRect(108, 152, 100, 23, false);
	this->txtHeading->SetReadOnly(true);
	this->lblGPSValid = ui->NewLabel(this->tpLocation, CSTR("GPSValid"));
	this->lblGPSValid->SetRect(8, 176, 100, 23, false);
	this->txtGPSValid = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtGPSValid->SetRect(108, 176, 100, 23, false);
	this->txtGPSValid->SetReadOnly(true);
	this->lblNSateUsed = ui->NewLabel(this->tpLocation, CSTR("Satelite Used"));
	this->lblNSateUsed->SetRect(8, 200, 100, 23, false);
	this->txtNSateUsed = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateUsed->SetRect(108, 200, 100, 23, false);
	this->txtNSateUsed->SetReadOnly(true);
	this->lblNSateUsedGPS = ui->NewLabel(this->tpLocation, CSTR("Satelite Used GPS"));
	this->lblNSateUsedGPS->SetRect(8, 224, 100, 23, false);
	this->txtNSateUsedGPS = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateUsedGPS->SetRect(108, 224, 100, 23, false);
	this->txtNSateUsedGPS->SetReadOnly(true);
	this->lblNSateUsedSBAS = ui->NewLabel(this->tpLocation, CSTR("Satelite Used SBAS"));
	this->lblNSateUsedSBAS->SetRect(8, 248, 100, 23, false);
	this->txtNSateUsedSBAS = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateUsedSBAS->SetRect(108, 248, 100, 23, false);
	this->txtNSateUsedSBAS->SetReadOnly(true);
	this->lblNSateUsedGLO = ui->NewLabel(this->tpLocation, CSTR("Satelite Used GLO"));
	this->lblNSateUsedGLO->SetRect(8, 272, 100, 23, false);
	this->txtNSateUsedGLO = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateUsedGLO->SetRect(108, 272, 100, 23, false);
	this->txtNSateUsedGLO->SetReadOnly(true);
	this->lblNSateViewGPS = ui->NewLabel(this->tpLocation, CSTR("Satelite View GPS"));
	this->lblNSateViewGPS->SetRect(8, 296, 100, 23, false);
	this->txtNSateViewGPS = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateViewGPS->SetRect(108, 296, 100, 23, false);
	this->txtNSateViewGPS->SetReadOnly(true);
	this->lblNSateViewGLO = ui->NewLabel(this->tpLocation, CSTR("Satelite View GLO"));
	this->lblNSateViewGLO->SetRect(8, 320, 100, 23, false);
	this->txtNSateViewGLO = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateViewGLO->SetRect(108, 320, 100, 23, false);
	this->txtNSateViewGLO->SetReadOnly(true);
	this->lblNSateViewGA = ui->NewLabel(this->tpLocation, CSTR("Satelite View GA"));
	this->lblNSateViewGA->SetRect(8, 344, 100, 23, false);
	this->txtNSateViewGA = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateViewGA->SetRect(108, 344, 100, 23, false);
	this->txtNSateViewGA->SetReadOnly(true);
	this->lblNSateViewQZSS = ui->NewLabel(this->tpLocation, CSTR("Satelite View QZSS"));
	this->lblNSateViewQZSS->SetRect(8, 368, 100, 23, false);
	this->txtNSateViewQZSS = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateViewQZSS->SetRect(108, 368, 100, 23, false);
	this->txtNSateViewQZSS->SetReadOnly(true);
	this->lblNSateViewBD = ui->NewLabel(this->tpLocation, CSTR("Satelite View BD"));
	this->lblNSateViewBD->SetRect(8, 392, 100, 23, false);
	this->txtNSateViewBD = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtNSateViewBD->SetRect(108, 392, 100, 23, false);
	this->txtNSateViewBD->SetReadOnly(true);
	this->chkAutoPan = ui->NewCheckBox(this->tpLocation, CSTR("Auto Pan"), true);
	this->chkAutoPan->SetRect(108, 416, 100, 23, false);
	this->chkTopMost = ui->NewCheckBox(this->tpLocation, CSTR("Top Most"), false);
	this->chkTopMost->SetRect(208, 416, 100, 23, false);
	this->chkTopMost->HandleCheckedChange(OnTopMostChg, this);
	this->chkNoSleep = ui->NewCheckBox(this->tpLocation, CSTR("Prevent Sleep"), true);
	this->chkNoSleep->SetRect(108, 440, 100, 23, false);
	this->btnDispOff = NewDisplayOffButton(ui, this->tpLocation, CSTR("Display Off"));
	this->btnDispOff->SetRect(208, 440, 100, 23, false);
	this->btnDispOff->HandleButtonClick(OnDispOffClicked, this);
	this->lblDistance = ui->NewLabel(this->tpLocation, CSTR("H-Distance"));
	this->lblDistance->SetRect(8, 464, 100, 23, false);
	this->txtDistance = ui->NewTextBox(this->tpLocation, CSTR(""));
	this->txtDistance->SetRect(108, 464, 100, 23, false);
	this->txtDistance->SetReadOnly(true);

	this->tpAlert = this->tcMain->AddTabPage(CSTR("Alert"));
	this->tcAlert = ui->NewTabControl(this->tpAlert);
	this->tcAlert->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertAdd = this->tcAlert->AddTabPage(CSTR("Add"));
	this->lbAlertLyr = ui->NewListBox(this->tpAlertAdd, false);
	this->lbAlertLyr->SetRect(0, 0, 100, 23, false);
	this->lbAlertLyr->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspAlertAdd = ui->NewHSplitter(this->tpAlertAdd, 3, false);
	this->lbAlertAdd = ui->NewListBox(this->tpAlertAdd, false);
	this->lbAlertAdd->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAlertView = this->tcAlert->AddTabPage(CSTR("View"));
	this->pnlAlertView = ui->NewPanel(this->tpAlertView);
	this->pnlAlertView->SetRect(0, 0, 100, 31, false);
	this->pnlAlertView->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbAlert = ui->NewListBox(this->tpAlertView, false);
	this->lbAlert->SetDockType(UI::GUIControl::DOCK_FILL);

	if (this->locSvc->GetServiceType() == Map::LocationService::ST_MTK)
	{
		this->tpMTK = this->tcMain->AddTabPage(CSTR("MTK"));
		this->grpMTKFirmware = ui->NewGroupBox(this->tpMTK, CSTR("Firmware"));
		this->grpMTKFirmware->SetRect(0, 0, 340, 116, false);
		this->lblMTKRelease = ui->NewLabel(this->grpMTKFirmware, CSTR("Release"));
		this->lblMTKRelease->SetRect(0, 0, 100, 23, false);
		this->txtMTKRelease = ui->NewTextBox(this->grpMTKFirmware, CSTR(""));
		this->txtMTKRelease->SetRect(100, 0, 150, 23, false);
		this->txtMTKRelease->SetReadOnly(true);
		this->lblMTKBuildID = ui->NewLabel(this->grpMTKFirmware, CSTR("Build"));
		this->lblMTKBuildID->SetRect(0, 24, 100, 23, false);
		this->txtMTKBuildID = ui->NewTextBox(this->grpMTKFirmware, CSTR(""));
		this->txtMTKBuildID->SetRect(100, 24, 150, 23, false);
		this->txtMTKBuildID->SetReadOnly(true);
		this->lblMTKProdMode = ui->NewLabel(this->grpMTKFirmware, CSTR("Product Mode"));
		this->lblMTKProdMode->SetRect(0, 48, 100, 23, false);
		this->txtMTKProdMode = ui->NewTextBox(this->grpMTKFirmware, CSTR(""));
		this->txtMTKProdMode->SetRect(100, 48, 100, 23, false);
		this->txtMTKProdMode->SetReadOnly(true);
		this->lblMTKSDKVer = ui->NewLabel(this->grpMTKFirmware, CSTR("SDK Version"));
		this->lblMTKSDKVer->SetRect(0, 72, 100, 23, false);
		this->txtMTKSDKVer = ui->NewTextBox(this->grpMTKFirmware, CSTR(""));
		this->txtMTKSDKVer->SetRect(100, 72, 100, 23, false);
		this->txtMTKSDKVer->SetReadOnly(true);
		this->btnMTKFirmware = ui->NewButton(this->grpMTKFirmware, CSTR("Query"));
		this->btnMTKFirmware->SetRect(200, 72, 75, 23, false);
		this->btnMTKFirmware->HandleButtonClick(OnMTKFirmwareClicked, this);
		this->btnMTKLogDownload = ui->NewButton(this->tpMTK, CSTR("Download Log"));
		this->btnMTKLogDownload->SetRect(0, 116, 75, 23, false);
		this->btnMTKLogDownload->HandleButtonClick(OnMTKLogDownloadClicked, this);
		this->btnMTKLogDelete = ui->NewButton(this->tpMTK, CSTR("Delete Log"));
		this->btnMTKLogDelete->SetRect(80, 116, 75, 23, false);
		this->btnMTKLogDelete->HandleButtonClick(OnMTKLogDeleteClicked, this);
		this->btnMTKFactoryReset = ui->NewButton(this->tpMTK, CSTR("Factory Reset"));
		this->btnMTKFactoryReset->SetRect(0, 140, 75, 23, false);
		this->btnMTKFactoryReset->HandleButtonClick(OnMTKFactoryResetClicked, this);

/*		this->btnMTKTest = ui->NewButton(this->tpMTK, CSTR("Test"));
		this->btnMTKTest->SetRect(0, 140, 75, 23, false);
		this->btnMTKTest->HandleButtonClick(OnMTKTestClicked, this);*/
	}

	this->tpSate = this->tcMain->AddTabPage(CSTR("Satellite"));
	this->lvSate = ui->NewListView(this->tpSate, UI::ListViewStyle::Table, 5);
	this->lvSate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSate->AddColumn(CSTR("Type"), 80);
	this->lvSate->AddColumn(CSTR("PRN"), 60);
	this->lvSate->AddColumn(CSTR("Elevation"), 60);
	this->lvSate->AddColumn(CSTR("Azimuth"), 60);
	this->lvSate->AddColumn(CSTR("SNR"), 60);
	this->lvSate->SetShowGrid(true);
	this->lvSate->SetFullRowSelect(true);

	this->tpNMEA = this->tcMain->AddTabPage(CSTR("NMEA"));
	this->lbNMEA = ui->NewListBox(this->tpNMEA, false);
	this->lbNMEA->SetDockType(UI::GUIControl::DOCK_FILL);
	NN<IO::GPSNMEA>::ConvertFrom(this->locSvc)->HandleCommand(OnNMEALine, this);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRGPSTrackerForm::~AVIRGPSTrackerForm()
{
	this->locSvc->UnregisterLocationHandler(OnGPSUpdate, this);
	NN<SSWR::AVIRead::AVIRMapNavigator> mapNavi;
	if (this->mapNavi.SetTo(mapNavi))
	{
		mapNavi->HideMarker();
	}
	if (this->relLocSvc)
	{
		this->locSvc.Delete();
		this->relLocSvc = false;
	}
	this->wgs84.Delete();
	UOSInt i = NMEAMAXSIZE;
	while (i-- > 0)
	{
		OPTSTR_DEL(this->nmeaBuff[i]);
	}
	MemFreeArr(this->nmeaBuff);
}

void SSWR::AVIRead::AVIRGPSTrackerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGPSTrackerForm::OnFocus()
{
	NN<SSWR::AVIRead::AVIRMapNavigator> mapNavi;
	if (this->dispIsOff && this->mapNavi.SetTo(mapNavi))
	{
		this->dispIsOff = false;
		mapNavi->ResumeUpdate();
	}
}

void SSWR::AVIRead::AVIRGPSTrackerForm::SetGPSTrack(Optional<Map::GPSTrack> gpsTrk)
{
	Sync::MutexUsage mutUsage(this->recMut);
	this->gpsTrk = gpsTrk;
}

void SSWR::AVIRead::AVIRGPSTrackerForm::SetMapNavigator(Optional<SSWR::AVIRead::AVIRMapNavigator> mapNavi)
{
	NN<SSWR::AVIRead::AVIRMapNavigator> nnmapNavi;
	if (this->mapNavi.SetTo(nnmapNavi))
	{
		nnmapNavi->HideMarker();
	}
	this->mapNavi = mapNavi;
	if (this->mapNavi.NotNull())
	{
	}
}

void SSWR::AVIRead::AVIRGPSTrackerForm::DispOffFocusLost()
{
	NN<SSWR::AVIRead::AVIRMapNavigator> mapNavi;
	if (this->dispOffClk)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if ((dt.ToTicks() - this->dispOffTime) < 1000)
		{
			this->dispOffClk = false;
			if (this->mapNavi.SetTo(mapNavi))
			{
				mapNavi->PauseUpdate();
				this->dispIsOff = true;
			}
		}
		else
		{
			this->dispOffClk = false;
		}
	}
}
