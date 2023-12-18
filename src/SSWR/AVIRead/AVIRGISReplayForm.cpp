#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/Polyline.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringUtil.h"

typedef enum
{
	MNU_MARK_START = 101,
	MNU_MARK_END,
	MNU_MARK_COPY,
	MNU_MARK_DELETE
} MenuItems;

UInt32 __stdcall SSWR::AVIRead::AVIRGISReplayForm::AddressThread(void *userObj)
{
	SSWR::AVIRead::AVIRGISReplayForm *me = (SSWR::AVIRead::AVIRGISReplayForm*)userObj;
	Math::Coord2DDbl *latLon;
	UOSInt recCnt;
	UOSInt i;
	Map::GPSTrack::GPSRecord3 *recs;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	me->threadRunning = true;
	recs = me->track->GetTrack(me->currTrackId, recCnt);
	latLon = MemAllocA(Math::Coord2DDbl, recCnt);
	i = 0;
	while (i < recCnt)
	{
		latLon[i] = recs[i].pos;
		i++;
	}
	i = 0;
	while (!me->threadToStop && i < recCnt)
	{
		if ((sptr = me->navi->ResolveAddress(sbuff, latLon[i])) != 0)
		{
			me->names[i] = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		i++;
	}
	MemFreeA(latLon);
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRGISReplayForm::OnCboNameChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISReplayForm *me = (SSWR::AVIRead::AVIRGISReplayForm*)userObj;
	me->StopThread();
	me->currTrackId = (UOSInt)me->cboName->GetSelectedIndex();
	me->UpdateRecList();
}

void __stdcall SSWR::AVIRead::AVIRGISReplayForm::OnLbRecordChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISReplayForm *me = (SSWR::AVIRead::AVIRGISReplayForm*)userObj;
	UOSInt i = me->lbRecord->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		UOSInt recCnt = 0;
		Map::GPSTrack::GPSRecord3 *recs = me->track->GetTrack(me->currTrackId, recCnt);
		Data::DateTime dt;
		dt.SetInstant(recs[i].recTime);
		sptr = dt.ToStringNoZone(sbuff);
		me->txtGPSTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, recs[i].pos.GetLat());
		me->txtLatitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, recs[i].pos.GetLon());
		me->txtLongitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDoubleFmt(sbuff, recs[i].altitude, "0.##########");
		me->txtAltitude->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDoubleFmt(sbuff, recs[i].speed, "0.##########");
		me->txtSpeedKnot->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDoubleFmt(sbuff, recs[i].speed * 1.852, "0.##########");
		me->txtSpeedKM->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, recs[i].heading);
		me->txtHeading->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, recs[i].valid);
		me->txtValid->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, recs[i].nSateViewGPS);
		me->txtNSateView->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, recs[i].nSateUsed);
		me->txtNSateUsed->SetText(CSTRP(sbuff, sptr));
		if (me->names != 0)
		{
			if (me->names[i] == 0)
			{
				me->txtAddress->SetText(CSTR("Loading..."));
			}
			else
			{
				me->txtAddress->SetText(me->names[i]->ToCString());
			}
		}
		UOSInt j;
		UOSInt k = me->track->GetExtraCount(me->currTrackId, (UOSInt)i);
		UOSInt l;
		Text::StringBuilderUTF8 sb;
		me->lvExtra->ClearItems();
		j = 0;
		while (j < k)
		{
			sb.ClearStr();
			me->track->GetExtraName(me->currTrackId, (UOSInt)i, j, sb);
			l = me->lvExtra->AddItem(sb.ToCString(), 0);
			sb.ClearStr();
			me->track->GetExtraValueStr(me->currTrackId, (UOSInt)i, j, sb);
			me->lvExtra->SetSubItem(l, 1, sb.ToCString());
			j++;
		}

		if (recs[i].heading == 0)
		{
			if (i <= 0 || (recs[i - 1].pos == recs[i].pos))
			{
				me->navi->ShowMarker(recs[i].pos);
			}
			else
			{
				me->navi->ShowMarkerDir(recs[i].pos, Math_ArcTan2(recs[i - 1].pos.GetLat() - recs[i].pos.GetLat(), recs[i].pos.GetLon() - recs[i - 1].pos.GetLon()) + Math::PI * 0.5, Math::Unit::Angle::AU_RADIAN);
			}
		}
		else
		{
			me->navi->ShowMarkerDir(recs[i].pos, recs[i].heading, Math::Unit::Angle::AU_DEGREE);
		}
		if (!recs[i].pos.IsZero())
		{
			if (!me->navi->InMap(recs[i].pos))
			{
				me->navi->PanToMap(recs[i].pos);
			}
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISReplayForm::OnLbRecordRClick(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn)
{
	SSWR::AVIRead::AVIRGISReplayForm *me = (SSWR::AVIRead::AVIRGISReplayForm*)userObj;
	me->mnuRecord->ShowMenu(me, scnPos);
	return false;
}

void SSWR::AVIRead::AVIRGISReplayForm::FreeNames()
{
	if (names)
	{
		UOSInt i = namesCnt;
		while (i-- > 0)
		{
			if (names[i])
			{
				names[i]->Release();
				names[i] = 0;
			}
		}
		MemFree(names);
		names = 0;
	}
}

SSWR::AVIRead::AVIRGISReplayForm::AVIRGISReplayForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::GPSTrack> track, IMapNavigator *navi) : UI::GUIForm(parent, 416, 560, ui)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	this->core = core;
	this->track = track;
	this->navi = navi;
	this->currTrackId = 0;
	this->names = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Replay"));
	this->SetFont(0, 0, 8.25, false);

	NotNullPtr<UI::GUIPanel> pnl;
	NotNullPtr<UI::GUIPanel> pnl2;
	NotNullPtr<UI::GUILabel> lbl;
	NotNullPtr<UI::GUIHSplitter> splitter;

	NEW_CLASSNN(pnl, UI::GUIPanel(ui, *this));
	pnl->SetRect(0, 0, 408, 24, false);
	pnl->SetDockType(UI::GUIControl::DOCK_TOP);
	lbl = ui->NewLabel(pnl, CSTR("Name"));
	lbl->SetRect(0, 0, 100, 23, false);
	this->cboName = ui->NewComboBox(pnl, false);
	this->cboName->SetRect(104, 0, 121, 20, false);
	this->cboName->HandleSelectionChange(OnCboNameChg, this);
	this->lblDist = ui->NewLabel(pnl, CSTR(""));
	this->lblDist->SetRect(245, 0, 200, 23, false);

	NEW_CLASS(this->lbRecord, UI::GUIListBox(ui, *this, false));
	this->lbRecord->SetRect(0, 24, 150, 152, false);
	this->lbRecord->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbRecord->HandleSelectionChange(OnLbRecordChg, this);
	this->lbRecord->HandleRightClicked(OnLbRecordRClick, this);
	splitter = ui->NewHSplitter(*this, 3, false);
	NEW_CLASSNN(pnl2, UI::GUIPanel(ui, *this));
	pnl2->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASSNN(pnl, UI::GUIPanel(ui, pnl2));
	pnl->SetRect(0, 0, 100, 348, false);
	pnl->SetDockType(UI::GUIControl::DOCK_TOP);

	lbl = ui->NewLabel(pnl, CSTR("Start Mark"));
	lbl->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtStartMark, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtStartMark->SetReadOnly(true);
	this->txtStartMark->SetRect(104, 0, 148, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("End Mark"));
	lbl->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtEndMark, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtEndMark->SetReadOnly(true);
	this->txtEndMark->SetRect(104, 24, 148, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("GPS Time"));
	lbl->SetRect(0, 80, 100, 23, false);
	NEW_CLASS(this->txtGPSTime, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtGPSTime->SetReadOnly(true);
	this->txtGPSTime->SetRect(104, 80, 148, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Latitude"));
	lbl->SetRect(0, 104, 100, 23, false);
	NEW_CLASS(this->txtLatitude, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtLatitude->SetReadOnly(true);
	this->txtLatitude->SetRect(104, 104, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Longitude"));
	lbl->SetRect(0, 128, 100, 23, false);
	NEW_CLASS(this->txtLongitude, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtLongitude->SetReadOnly(true);
	this->txtLongitude->SetRect(104, 128, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Altitude"));
	lbl->SetRect(0, 152, 100, 23, false);
	NEW_CLASS(this->txtAltitude, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtAltitude->SetReadOnly(true);
	this->txtAltitude->SetRect(104, 152, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Speed (knot)"));
	lbl->SetRect(0, 176, 100, 23, false);
	NEW_CLASS(this->txtSpeedKnot, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtSpeedKnot->SetReadOnly(true);
	this->txtSpeedKnot->SetRect(104, 176, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Speed (km/h)"));
	lbl->SetRect(0, 200, 100, 23, false);
	NEW_CLASS(this->txtSpeedKM, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtSpeedKM->SetReadOnly(true);
	this->txtSpeedKM->SetRect(104, 200, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Heading"));
	lbl->SetRect(0, 224, 100, 23, false);
	NEW_CLASS(this->txtHeading, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtHeading->SetReadOnly(true);
	this->txtHeading->SetRect(104, 224, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("Valid"));
	lbl->SetRect(0, 248, 100, 23, false);
	NEW_CLASS(this->txtValid, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtValid->SetReadOnly(true);
	this->txtValid->SetRect(104, 248, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("nSateView"));
	lbl->SetRect(0, 272, 100, 23, false);
	NEW_CLASS(this->txtNSateView, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtNSateView->SetReadOnly(true);
	this->txtNSateView->SetRect(104, 272, 128, 21, false);
	lbl = ui->NewLabel(pnl, CSTR("nSateUsed"));
	lbl->SetRect(0, 296, 100, 23, false);
	NEW_CLASS(this->txtNSateUsed, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtNSateUsed->SetReadOnly(true);
	this->txtNSateUsed->SetRect(104, 296, 128, 21, false);
	this->lblAddress = ui->NewLabel(pnl, CSTR("Address"));
	this->lblAddress->SetRect(0, 320, 100, 23, false);
	NEW_CLASS(this->txtAddress, UI::GUITextBox(ui, pnl, CSTR("")));
	this->txtAddress->SetReadOnly(true);
	this->txtAddress->SetRect(104, 320, 256, 21, false);
	this->lblAddress->SetVisible(false);
	this->txtAddress->SetVisible(false);

	NEW_CLASS(this->lvExtra, UI::GUIListView(ui, pnl2, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvExtra->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvExtra->SetShowGrid(true);
	this->lvExtra->SetFullRowSelect(true);
	this->lvExtra->AddColumn(CSTR("Name"), 100);
	this->lvExtra->AddColumn(CSTR("Value"), 128);

	NEW_CLASS(this->mnuRecord, UI::GUIPopupMenu());
	this->mnuRecord->AddItem(CSTR("Mark &Start"), MNU_MARK_START, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_S);
	this->mnuRecord->AddItem(CSTR("Mark &End"), MNU_MARK_END, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_E);
	this->mnuRecord->AddSeperator();
	this->mnuRecord->AddItem(CSTR("&Copy Marked"), MNU_MARK_COPY, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_C);
	this->mnuRecord->AddItem(CSTR("&Delete Marked"), MNU_MARK_DELETE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_DELETE);

	Data::ArrayListString *nameArr;
	NEW_CLASS(nameArr, Data::ArrayListString());
	this->track->GetTrackNames(nameArr);
	NotNullPtr<Text::String> s;
	UOSInt i = 0;
	UOSInt j = nameArr->GetCount();
	while (i < j)
	{
		if (!s.Set(nameArr->GetItem(i)))
		{
			sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Track")), (Int32)i);
			this->cboName->AddItem(CSTRP(sbuff, sptr), 0);
		}
		else
		{
			this->cboName->AddItem(s, 0);
		}
		i++;
	}
	if (j > 0)
	{
		this->cboName->SetSelectedIndex(0);
	}
	DEL_CLASS(nameArr);
}

SSWR::AVIRead::AVIRGISReplayForm::~AVIRGISReplayForm()
{
	this->StopThread();
	DEL_CLASS(this->mnuRecord);
	FreeNames();
}

void SSWR::AVIRead::AVIRGISReplayForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	switch (cmdId)
	{
	case MNU_MARK_START:
		{
			this->startMark = (UOSInt)this->lbRecord->GetSelectedIndex();
			sptr = this->lbRecord->GetItemText(sbuff, this->startMark);
			this->txtStartMark->SetText(CSTRP(sbuff, sptr));
		}
		break;
	case MNU_MARK_END:
		{
			this->endMark = (UOSInt)this->lbRecord->GetSelectedIndex();
			sptr = this->lbRecord->GetItemText(sbuff, this->endMark);
			this->txtEndMark->SetText(CSTRP(sbuff, sptr));
		}
		break;
	case MNU_MARK_COPY:
		{
			if (this->startMark > this->endMark)
			{
				return;
			}
			UOSInt recCnt;
			UOSInt i;
			NotNullPtr<Map::GPSTrack> newTrack;
			NEW_CLASSNN(newTrack, Map::GPSTrack(this->track->GetSourceNameObj(), this->track->GetHasAltitude(), this->track->GetCodePage(), this->track->GetName().Ptr()));
			Text::String *trackName = track->GetTrackName(this->currTrackId);
			if (trackName)
				newTrack->SetTrackName(trackName->ToCString());
			Map::GPSTrack::GPSRecord3 *recs = track->GetTrack(this->currTrackId, recCnt);
			i = this->startMark;
			while (i <= this->endMark)
			{
				newTrack->AddRecord(recs[i]);
				i++;
			}
			this->navi->AddLayer(newTrack);
		}
		break;
	case MNU_MARK_DELETE:
		{
			if (this->startMark > this->endMark)
			{
				return;
			}
			if (this->track->RemoveRecordRange(this->currTrackId, this->startMark, this->endMark))
			{
				this->StopThread();
				this->UpdateRecList();
				this->navi->RedrawMap();
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRGISReplayForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISReplayForm::UpdateRecList()
{
	this->StopThread();
	UOSInt i;

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Data::DateTime dt;
	UOSInt recCnt = 0;
	Map::GPSTrack::GPSRecord3 *recs = this->track->GetTrack(this->currTrackId, recCnt);
	if (recs)
	{
		Double dist = 0;
		NotNullPtr<Math::CoordinateSystem> coord = this->track->GetCoordinateSystem();
		NotNullPtr<Math::Geometry::LineString> pl;
		if (pl.Set((Math::Geometry::LineString*)this->track->GetNewVectorById(0, (Int64)this->currTrackId)))
		{
			NotNullPtr<Math::Geometry::Polyline> pl2 = pl->CreatePolyline();
			if (pl->HasZ())
			{
				dist = coord->CalPLDistance3D(pl2, Math::Unit::Distance::DU_METER);
			}
			else
			{
				dist = coord->CalPLDistance(pl2, Math::Unit::Distance::DU_METER);
			}
			pl2.Delete();
			pl.Delete();
		}
		sptr = Text::StrConcatC(Text::StrDoubleFmt(Text::StrConcatC(sbuff, UTF8STRC("Distance: ")), dist, "0.0"), UTF8STRC(" m"));
		this->lblDist->SetText(CSTRP(sbuff, sptr));

		i = 0;
		this->lbRecord->ClearItems();
		while (i < recCnt)
		{
			dt.SetInstant(recs[i].recTime);
			sptr = dt.ToStringNoZone(sbuff);
			this->lbRecord->AddItem(CSTRP(sbuff, sptr), 0);
			i++;
		}
		this->startMark = 0;
		this->endMark = recCnt - 1;
		if (recCnt > 0)
		{
			this->lbRecord->SetSelectedIndex(0);

			sptr = this->lbRecord->GetItemText(sbuff, this->startMark);
			this->txtStartMark->SetText(CSTRP(sbuff, sptr));
			sptr = this->lbRecord->GetItemText(sbuff, this->endMark);
			this->txtEndMark->SetText(CSTRP(sbuff, sptr));

			if (this->navi->HasKMap())
			{
				this->lblAddress->SetVisible(true);
				this->txtAddress->SetVisible(true);
				FreeNames();
				this->names = MemAlloc(Text::String*, this->namesCnt = recCnt);
				while (recCnt-- > 0)
				{
					this->names[recCnt] = 0;
				}
				this->threadToStop = false;
				this->threadRunning = false;
				Sync::ThreadUtil::Create(AddressThread, this);
			}
		}
	}
	else
	{
		this->lbRecord->ClearItems();
	}
}

void SSWR::AVIRead::AVIRGISReplayForm::StopThread()
{
	this->threadToStop = true;
	if (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}
