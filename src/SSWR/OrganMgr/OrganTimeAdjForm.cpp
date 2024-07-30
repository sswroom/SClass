#include "Stdafx.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Math.h"
#include "SSWR/OrganMgr/OrganTimeAdjForm.h"
#include "UI/Clipboard.h"

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnSpeciesChg(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	NN<UserFileInfo> userFile;
	UOSInt i;
	UOSInt j;
	Int32 spId;
	spId = (Int32)me->cboSpecies->GetSelectedItem().GetOSInt();
	me->currFileList.Clear();
	me->lbPictures->ClearItems();
	
	i = 0;
	j = me->userFileList.GetCount();
	while (i < j)
	{
		userFile = me->userFileList.GetItemNoCheck(i);
		if (spId == 0 || userFile->speciesId == spId)
		{
			me->currFileList.Add(userFile);
			me->lbPictures->AddItem(userFile->oriFileName->ToCString(), userFile);
		}
		i++;
	}
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTileUpdated(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	me->mapUpdated = true;
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimerTick(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	if (me->mapUpdated)
	{
		me->mapUpdated = false;
		me->mapMain->UpdateMap();
		me->mapMain->Redraw();
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnMapScaleScroll(AnyType userObj, UOSInt newVal)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	me->mapMain->SetMapScale(Double2Int32(me->mapTile->GetLevelScale(newVal)));
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTrackChg(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Map::GPSTrack::GPSRecord3 *rec = (Map::GPSTrack::GPSRecord3 *)me->lbTrack->GetSelectedItem().p;
	if (rec)
	{
		if (!me->mapView->InViewXY(rec->pos))
		{
			me->mapView->SetCenterXY(rec->pos);
			me->mapMain->UpdateMap();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnPictureChg(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	NN<UserFileInfo> userFile; 
	NN<Text::String> s;
	if (me->lbPictures->GetSelectedItem().GetOpt<UserFileInfo>().SetTo(userFile) && userFile->camera.SetTo(s))
	{
		me->selImgCamera = s.Ptr();
		me->selImgTime = userFile->fileTime;
		me->pbPreview->SetImage(0, false);
		me->dispImg.Delete();
		me->dispImg = me->env->ParseFileImage(userFile);
		NN<Media::ImageList> dispImg;
		if (me->dispImg.SetTo(dispImg))
		{
			me->pbPreview->SetImage(dispImg->GetImage(0, 0), false);
		}

		Int32 timeAdj;
		timeAdj = me->cameraMap.GetNN(s);
		me->UpdateSelTime(s->v, s->leng, timeAdj);
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnCameraChg(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	Int32 timeAdj;
	me->cboCamera->GetText(sb);
	timeAdj = me->cameraMap.Get(sb.ToCString());
	sptr = Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnPasteClicked(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	if (UI::Clipboard::GetString(me->GetHandle(), sb))
	{
		sb.Trim();

		Int32 timeAdj;
		if (sb.ToInt32(timeAdj))
		{
			sptr = Text::StrInt32(sbuff, timeAdj);
			me->txtTimeAdj->SetText(CSTRP(sbuff, sptr));
			sb.ClearStr();
			me->cboCamera->GetText(sb);
			me->cameraMap.Put(sb.ToCString(), timeAdj);
			me->adjLyr->SetTimeAdj(sb.ToCString(), timeAdj);
			me->mapMain->UpdateMap();
			me->mapMain->Redraw();
			me->UpdateSelTime(sb.ToString(), sb.GetLength(), timeAdj);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeAddClicked(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	Int32 timeAdj;
	me->cboCamera->GetText(sb);
	timeAdj = me->cameraMap.Get(sb.ToCString());
	timeAdj++;
	me->adjLyr->SetTimeAdj(sb.ToCString(), timeAdj);
	me->UpdateSelTime(sb.ToString(), sb.GetLength(), timeAdj);
	me->cameraMap.Put(sb.ToCString(), timeAdj);
	sptr = Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(CSTRP(sbuff, sptr));
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeSubClicked(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	Int32 timeAdj;
	me->cboCamera->GetText(sb);
	timeAdj = me->cameraMap.Get(sb.ToCString());
	timeAdj--;
	me->adjLyr->SetTimeAdj(sb.ToCString(), timeAdj);
	me->UpdateSelTime(sb.ToString(), sb.GetLength(), timeAdj);
	me->cameraMap.Put(sb.ToCString(), timeAdj);
	sptr = Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(CSTRP(sbuff, sptr));
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeApplyClicked(AnyType userObj)
{
	NN<OrganTimeAdjForm> me = userObj.GetNN<OrganTimeAdjForm>();
	Text::StringBuilderUTF8 sb;
	Int32 timeAdj;
	UOSInt i;
	UOSInt j;
	NN<UserFileInfo> userFile;
	Int32 succCnt = 0;
	Int32 failCnt = 0;
	Math::Coord2DDbl pos;
	Data::Timestamp ts;
	NN<Text::String> s;

	me->cboCamera->GetText(sb);
	timeAdj = me->cameraMap.Get(sb.ToCString());
	i = 0;
	j = me->userFileList.GetCount();
	while (i < j)
	{
		userFile = me->userFileList.GetItemNoCheck(i);
		if (userFile->camera.SetTo(s) && s->Equals(sb.ToString(), sb.GetLength()))
		{
			ts = userFile->fileTime.AddSecond(timeAdj);
			pos = me->gpsTrk->GetPosByTime(ts);
			if (me->env->UpdateUserFilePos(userFile, ts, pos.GetLat(), pos.GetLon()))
			{
				succCnt++;
			}
			else
			{
				failCnt++;
			}
		}
		i++;
	}
	sb.ClearStr();
	sb.AppendI32(succCnt);
	sb.AppendC(UTF8STRC(" succeed, "));
	sb.AppendI32(failCnt);
	sb.AppendC(UTF8STRC(" failed"));
	me->ui->ShowMsgOK(sb.ToCString(), CSTR("Time Adjust"), me);
}

void SSWR::OrganMgr::OrganTimeAdjForm::UpdateSelTime(UnsafeArray<const UTF8Char> camera, UOSInt cameraLen, Int32 timeAdj)
{
	if (this->selImgCamera && this->selImgCamera->Equals(camera, cameraLen))
	{
		Math::Coord2DDbl pos = this->gpsTrk->GetPosByTime(this->selImgTime.AddSecond(timeAdj));
		this->mapMain->ShowMarkerMapXY(pos);

		if (!this->mapView->InViewXY(pos))
		{
			this->mapView->SetCenterXY(pos);
			this->mapMain->UpdateMap();
		}
	}
}

SSWR::OrganMgr::OrganTimeAdjForm::OrganTimeAdjForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<OrganEnv> env, NN<DataFileInfo> dataFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);
	this->SetFont(0, 0, 10.5, false);

	this->env = env;
	this->dataFile = dataFile;
	if (!this->env->OpenGPSTrack(dataFile).SetTo(this->gpsTrk))
	{
		NEW_CLASSNN(this->gpsTrk, Map::GPSTrack(CSTR("Untitled"), false, 65001, CSTR("Untitled")));
	}
	this->colorSess = this->env->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->dispImg = 0;
	this->selImgCamera = 0;
	this->selImgTime = 0;

	this->SetText(this->env->GetLang(CSTR("TimeAdjTitle")));

	NN<Map::OSM::OSMTileMap> tileMap;
	NN<Media::StaticImage> stimg;
	NN<Media::ImageList> imgList;
	UOSInt i;
	UOSInt j;
	OSInt k;
	NN<Text::String> s;

	this->mapUpdated = false;
	NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.opencyclemap.org/cycle/"), this->env->GetCacheDir()->ToCString(), 0, 18, this->env->GetTCPClientFactory(), this->env->GetSSLEngine()));
	tileMap->AddAlternateURL(CSTR("http://b.tile.opencyclemap.org/cycle/"));
	tileMap->AddAlternateURL(CSTR("http://c.tile.opencyclemap.org/cycle/"));
	this->mapTile = tileMap;
	NEW_CLASSNN(this->mapTileLyr, Map::TileMapLayer(tileMap, this->env->GetParserList()));
	this->mapTileLyr->AddUpdatedHandler(OnTileUpdated, this);
	NEW_CLASSNN(this->mapEnv, Map::MapEnv(CSTR("File"), 0, this->mapTileLyr->GetCoordinateSystem()->Clone()));
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(stimg, Media::StaticImage(Math::Size2D<UOSInt>(7, 7), 0, 32, Media::PF_B8G8R8A8, 0, srcColor, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	stimg->FillColor(0xff40ffff);
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("PointImage")));
	imgList->AddImage(stimg, 0);
	this->mapEnv->AddImage(CSTR("PointImage"), imgList);
	NEW_CLASSNN(this->adjLyr, OrganTimeAdjLayer(this->gpsTrk, this->currFileList));
	this->adjLyr->SetCoordinateSystem(this->mapEnv->GetCoordinateSystem()->Clone());
	this->mapEnv->ChgLineStyleLayer(0, 0, 0xff0000ff, 3, 0, 0);
	i = this->mapEnv->AddLayer(0, this->mapTileLyr, true);
	i = this->mapEnv->AddLayer(0, this->gpsTrk, false);
	this->mapEnv->AddLayer(0, this->adjLyr, true);
	Media::ColorProfile dispColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->mapRenderer, Map::DrawMapRenderer(this->env->GetDrawEngine(), this->mapEnv, dispColor, this->colorSess.Ptr(), Map::DrawMapRenderer::DT_PIXELDRAW));
	this->mapView = this->mapEnv->CreateMapView(Math::Size2DDbl(1024, 768));

	this->env->GetUserFiles(this->userFileList, this->dataFile->startTime, this->dataFile->endTime);

	this->pnlLeft = ui->NewPanel(*this);
	this->pnlLeft->SetRect(0, 0, 300, 23, false);
	this->pnlLeft->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->pbPreview = ui->NewPictureBoxDD(this->pnlLeft, this->colorSess, true, false);
	this->pbPreview->SetRect(0, 0, 300, 240, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbTrack = ui->NewListBox(this->pnlLeft, false);
	this->lbTrack->SetRect(0, 0, 130, 23, false);
	this->lbTrack->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTrack->HandleSelectionChange(OnTrackChg, this);
	this->cboSpecies = ui->NewComboBox(this->pnlLeft, false);
	this->cboSpecies->SetRect(0, 0, 100, 23, false);
	this->cboSpecies->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSpecies->HandleSelectionChange(OnSpeciesChg, this);
	this->lbPictures = ui->NewListBox(this->pnlLeft, false);
	this->lbPictures->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbPictures->HandleSelectionChange(OnPictureChg, this);
	this->pnlMapCtrl = ui->NewPanel(*this);
	this->pnlMapCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlMapCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->tbMapScale = ui->NewTrackBar(this->pnlMapCtrl, 0, 18, 16);
	this->tbMapScale->SetRect(0, 0, 100, 23, false);
	this->tbMapScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->tbMapScale->HandleScrolled(OnMapScaleScroll, this);
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->mapMain, UI::GUIMapControl(ui, *this, this->env->GetDrawEngine(), 0xff000000, this->mapRenderer, this->mapView, this->colorSess));
	this->mapMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->cboCamera = ui->NewComboBox(this->pnlControl, false);
	this->cboCamera->SetRect(4, 4, 150, 23, false);
	this->cboCamera->HandleSelectionChange(OnCameraChg, this);
	this->txtTimeAdj = ui->NewTextBox(this->pnlControl, CSTR("0"), false);
	this->txtTimeAdj->SetRect(154, 4, 50, 23, false);
	this->txtTimeAdj->SetReadOnly(true);
	this->btnPaste = ui->NewButton(this->pnlControl, this->env->GetLang(CSTR("TimeAdjPaste")));
	this->btnPaste->SetRect(204, 4, 50, 23, false);
	this->btnPaste->HandleButtonClick(OnPasteClicked, this);
	this->btnTimeSub = ui->NewButton(this->pnlControl, CSTR("-"));
	this->btnTimeSub->SetRect(254, 4, 25, 23, false);
	this->btnTimeSub->HandleButtonClick(OnTimeSubClicked, this);
	this->btnTimeAdd = ui->NewButton(this->pnlControl, CSTR("+"));
	this->btnTimeAdd->SetRect(279, 4, 25, 23, false);
	this->btnTimeAdd->HandleButtonClick(OnTimeAddClicked, this);
	this->btnTimeApply = ui->NewButton(this->pnlControl, this->env->GetLang(CSTR("TimeAdjApply")));
	this->btnTimeApply->SetRect(329, 4, 50, 23, false);
	this->btnTimeApply->HandleButtonClick(OnTimeApplyClicked, this);

	NN<UserFileInfo> userFile;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<Map::GPSTrack::GPSRecord3> records;
	Data::DateTime dt;
	if (this->gpsTrk->GetTrack(0, j).SetTo(records))
	{
		i = 0;
		while (i < j)
		{
			dt.SetInstant(records[i].recTime);
			dt.ToLocalTime();
			sptr = dt.ToStringNoZone(sbuff);
			this->lbTrack->AddItem(CSTRP(sbuff, sptr), &records[i]);
			i++;
		}
	}

	Data::ArrayListInt32 spList;
	Data::ArrayListNN<OrganGroupItem> spItems;
	NN<OrganSpecies> sp;
	i = 0;
	j = this->userFileList.GetCount();
	while (i < j)
	{
		userFile = userFileList.GetItemNoCheck(i);
		k = spList.SortedIndexOf(userFile->speciesId);
		if (k < 0)
		{
			spList.SortedInsert(userFile->speciesId);
		}
		if (userFile->camera.SetTo(s))
		{
			this->cameraMap.PutNN(s, (Int32)(userFile->captureTime.inst.sec - userFile->fileTime.inst.sec));
		}
		i++;
	}
	NN<Data::ArrayList<Text::String *>> cameraList = this->cameraMap.GetKeys();
	Text::StringBuilderUTF8 sbu8;
	i = 0;
	j = cameraList->GetCount();
	while (i < j)
	{
		this->cboCamera->AddItem(Text::String::OrEmpty(cameraList->GetItem(i)), 0);
		this->adjLyr->SetTimeAdj(cameraList->GetItem(i), this->cameraMap.Get(cameraList->GetItem(i)));
		i++;
	}
	if (cameraList->GetCount() > 0)
	{
		UTF8Char sbuff[16];
		UnsafeArray<UTF8Char> sptr;
		this->cboCamera->SetSelectedIndex(0);
		sptr = Text::StrInt32(sbuff, this->cameraMap.Get(cameraList->GetItem(0)));
		this->txtTimeAdj->SetText(CSTRP(sbuff, sptr));
	}

	this->cboSpecies->AddItem(this->env->GetLang(CSTR("TimeAdjAllSp")), 0);
	this->env->GetSpeciesItems(spItems, spList);
	i = 0;
	j = spItems.GetCount();
	while (i < j)
	{
		sp = NN<OrganSpecies>::ConvertFrom(spItems.GetItemNoCheck(i));
		sbu8.ClearStr();
		sbu8.AppendOpt(sp->GetCName());
		sbu8.AppendC(UTF8STRC(" "));
		sbu8.AppendOpt(sp->GetSName());

		this->cboSpecies->AddItem(sbu8.ToCString(), (void*)(OSInt)sp->GetSpeciesId());
		sp.Delete();
		i++;
	}
	this->cboSpecies->SetSelectedIndex(0);
	this->AddTimer(1000, OnTimerTick, this);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganTimeAdjForm::~OrganTimeAdjForm()
{
	this->ClearChildren();
	this->mapEnv.Delete();
	DEL_CLASS(this->mapRenderer);
	this->gpsTrk.Delete();
	this->dispImg.Delete();
	this->env->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::OrganMgr::OrganTimeAdjForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
