#include "Stdafx.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Math.h"
#include "SSWR/OrganMgr/OrganTimeAdjForm.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnSpeciesChg(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	UserFileInfo *userFile;
	UOSInt i;
	UOSInt j;
	Int32 spId;
	spId = (Int32)(OSInt)me->cboSpecies->GetSelectedItem();
	me->currFileList->Clear();
	me->lbPictures->ClearItems();
	
	i = 0;
	j = me->userFileList->GetCount();
	while (i < j)
	{
		userFile = me->userFileList->GetItem(i);
		if (spId == 0 || userFile->speciesId == spId)
		{
			me->currFileList->Add(userFile);
			me->lbPictures->AddItem(userFile->oriFileName, userFile);
		}
		i++;
	}
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTileUpdated(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	me->mapUpdated = true;
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimerTick(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	if (me->mapUpdated)
	{
		me->mapUpdated = false;
		me->mapMain->UpdateMap();
		me->mapMain->Redraw();
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnMapScaleScroll(void *userObj, Int32 newVal)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	me->mapMain->SetMapScale(Math::Double2Int32(me->mapTile->GetLevelScale(newVal)));
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTrackChg(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Map::GPSTrack::GPSRecord *rec = (Map::GPSTrack::GPSRecord *)me->lbTrack->GetSelectedItem();
	if (rec)
	{
		if (!me->mapView->InViewXY(rec->lon, rec->lat))
		{
			me->mapView->SetCenterXY(rec->lon, rec->lat);
			me->mapMain->UpdateMap();
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnPictureChg(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	UserFileInfo *userFile = (UserFileInfo*)me->lbPictures->GetSelectedItem();
	if (userFile)
	{
		me->selImgCamera = userFile->camera;
		me->selImgTimeTicks = userFile->fileTimeTicks;
		me->pbPreview->SetImage(0, false);
		SDEL_CLASS(me->dispImg);
		me->dispImg = me->env->ParseFileImage(userFile);
		if (me->dispImg)
		{
			me->pbPreview->SetImage(me->dispImg->GetImage(0, 0), false);
		}

		Int32 timeAdj;
		timeAdj = me->cameraMap->Get(userFile->camera);
		me->UpdateSelTime(userFile->camera, timeAdj);
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnCameraChg(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	Int32 timeAdj;
	me->cboCamera->GetText(&sb);
	timeAdj = me->cameraMap->Get(sb.ToString());
	Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(sbuff);
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnPasteClicked(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	if (Win32::Clipboard::GetString(me->GetHandle(), &sb))
	{
		sb.Trim();

		Int32 timeAdj;
		if (sb.ToInt32(&timeAdj))
		{
			Text::StrInt32(sbuff, timeAdj);
			me->txtTimeAdj->SetText(sbuff);
			sb.ClearStr();
			me->cboCamera->GetText(&sb);
			me->cameraMap->Put(sb.ToString(), timeAdj);
			me->adjLyr->SetTimeAdj(sb.ToString(), timeAdj);
			me->mapMain->UpdateMap();
			me->mapMain->Redraw();
			me->UpdateSelTime(sb.ToString(), timeAdj);
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeAddClicked(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	Int32 timeAdj;
	me->cboCamera->GetText(&sb);
	timeAdj = me->cameraMap->Get(sb.ToString());
	timeAdj++;
	me->adjLyr->SetTimeAdj(sb.ToString(), timeAdj);
	me->UpdateSelTime(sb.ToString(), timeAdj);
	me->cameraMap->Put(sb.ToString(), timeAdj);
	Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(sbuff);
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeSubClicked(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[16];
	Int32 timeAdj;
	me->cboCamera->GetText(&sb);
	timeAdj = me->cameraMap->Get(sb.ToString());
	timeAdj--;
	me->adjLyr->SetTimeAdj(sb.ToString(), timeAdj);
	me->UpdateSelTime(sb.ToString(), timeAdj);
	me->cameraMap->Put(sb.ToString(), timeAdj);
	Text::StrInt32(sbuff, timeAdj);
	me->txtTimeAdj->SetText(sbuff);
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::OrganMgr::OrganTimeAdjForm::OnTimeApplyClicked(void *userObj)
{
	OrganTimeAdjForm *me = (OrganTimeAdjForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 timeAdj;
	UOSInt i;
	UOSInt j;
	UserFileInfo *userFile;
	Int32 succCnt = 0;
	Int32 failCnt = 0;
	Double lat;
	Double lon;
	Data::DateTime dt;

	me->cboCamera->GetText(&sb);
	timeAdj = me->cameraMap->Get(sb.ToString());
	i = 0;
	j = me->userFileList->GetCount();
	while (i < j)
	{
		userFile = me->userFileList->GetItem(i);
		if (userFile->camera && Text::StrEquals(userFile->camera, sb.ToString()))
		{
			dt.SetTicks(userFile->fileTimeTicks);
			dt.AddSecond(timeAdj);
			me->gpsTrk->GetLatLonByTime(&dt, &lat, &lon);
			if (me->env->UpdateUserFilePos(userFile, &dt, lat, lon))
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
	sb.Append((const UTF8Char*)" succeed, ");
	sb.AppendI32(failCnt);
	sb.Append((const UTF8Char*)" failed");
	UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Time Adjust", me);
}

void SSWR::OrganMgr::OrganTimeAdjForm::UpdateSelTime(const UTF8Char *camera, Int32 timeAdj)
{
	Data::DateTime dt;
	if (this->selImgCamera && Text::StrEquals(this->selImgCamera, camera))
	{
		Double lat;
		Double lon;
		dt.SetTicks(this->selImgTimeTicks);
		dt.AddSecond(timeAdj);
		this->gpsTrk->GetLatLonByTime(&dt, &lat, &lon);
		this->mapMain->ShowMarkerMapXY(lon, lat);

		if (!this->mapView->InViewXY(lon, lat))
		{
			this->mapView->SetCenterXY(lon, lat);
			this->mapMain->UpdateMap();
		}
	}
}

SSWR::OrganMgr::OrganTimeAdjForm::OrganTimeAdjForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, DataFileInfo *dataFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);
	this->SetFont(0, 10.5, false);

	this->env = env;
	this->dataFile = dataFile;
	this->gpsTrk = this->env->OpenGPSTrack(dataFile);
	this->colorSess = this->env->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->dispImg = 0;
	this->selImgCamera = 0;
	this->selImgTimeTicks = 0;
	NEW_CLASS(this->userFileList, Data::ArrayList<UserFileInfo *>());
	NEW_CLASS(this->currFileList, Data::ArrayList<UserFileInfo *>());
	NEW_CLASS(this->cameraMap, Data::StringUTF8Map<Int32>());

	this->SetText(this->env->GetLang((const UTF8Char*)"TimeAdjTitle"));

	Map::OSM::OSMTileMap *tileMap;
	Media::StaticImage *stimg;
	Media::ImageList *imgList;
	UOSInt i;
	UOSInt j;
	OSInt k;

	this->mapUpdated = false;
	NEW_CLASS(tileMap, Map::OSM::OSMTileMap((const UTF8Char*)"http://a.tile.opencyclemap.org/cycle/", this->env->GetCacheDir(), 18, this->env->GetSocketFactory(), this->env->GetSSLEngine()));
	tileMap->AddAlternateURL((const UTF8Char*)"http://b.tile.opencyclemap.org/cycle/");
	tileMap->AddAlternateURL((const UTF8Char*)"http://c.tile.opencyclemap.org/cycle/");
	this->mapTile = tileMap;
	NEW_CLASS(this->mapTileLyr, Map::TileMapLayer(tileMap, this->env->GetParserList()));
	this->mapTileLyr->AddUpdatedHandler(OnTileUpdated, this);
	NEW_CLASS(this->mapEnv, Map::MapEnv((const UTF8Char*)"File", 0, this->mapTileLyr->GetCoordinateSystem()->Clone()));
	Media::ColorProfile srcColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(stimg, Media::StaticImage(7, 7, 0, 32, Media::PF_B8G8R8A8, 0, &srcColor, Media::ColorProfile::YUVT_UNKNOWN, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	stimg->FillColor(0xff40ffff);
	NEW_CLASS(imgList, Media::ImageList((const UTF8Char*)"PointImage"));
	imgList->AddImage(stimg, 0);
	this->mapEnv->AddImage((const UTF8Char*)"PointImage", imgList);
	NEW_CLASS(this->adjLyr, OrganTimeAdjLayer(this->gpsTrk, this->currFileList));
	this->mapEnv->ChgLineStyleLayer(0, 0, 0xff0000ff, 3, 0, 0);
	i = this->mapEnv->AddLayer(0, this->mapTileLyr, true);
	i = this->mapEnv->AddLayer(0, this->gpsTrk, false);
	this->mapEnv->AddLayer(0, this->adjLyr, true);
	Media::ColorProfile dispColor(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->mapRenderer, Map::DrawMapRenderer(this->env->GetDrawEngine(), this->mapEnv, &dispColor, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	this->mapView = this->mapEnv->CreateMapView(1024, 768);

	this->env->GetUserFiles(this->userFileList, this->dataFile->startTimeTicks, this->dataFile->endTimeTicks);

	NEW_CLASS(this->pnlLeft, UI::GUIPanel(ui, this));
	this->pnlLeft->SetRect(0, 0, 300, 23, false);
	this->pnlLeft->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->pbPreview, UI::GUIPictureBoxDD(ui, this->pnlLeft, this->colorSess, true, false));
	this->pbPreview->SetRect(0, 0, 300, 240, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbTrack, UI::GUIListBox(ui, this->pnlLeft, false));
	this->lbTrack->SetRect(0, 0, 130, 23, false);
	this->lbTrack->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTrack->HandleSelectionChange(OnTrackChg, this);
	NEW_CLASS(this->cboSpecies, UI::GUIComboBox(ui, this->pnlLeft, false));
	this->cboSpecies->SetRect(0, 0, 100, 23, false);
	this->cboSpecies->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboSpecies->HandleSelectionChange(OnSpeciesChg, this);
	NEW_CLASS(this->lbPictures, UI::GUIListBox(ui, this->pnlLeft, false));
	this->lbPictures->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbPictures->HandleSelectionChange(OnPictureChg, this);
	NEW_CLASS(this->pnlMapCtrl, UI::GUIPanel(ui, this));
	this->pnlMapCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlMapCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->tbMapScale, UI::GUITrackBar(ui, this->pnlMapCtrl, 0, 18, 16));
	this->tbMapScale->SetRect(0, 0, 100, 23, false);
	this->tbMapScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->tbMapScale->HandleScrolled(OnMapScaleScroll, this);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->mapMain, UI::GUIMapControl(ui, this, this->env->GetDrawEngine(), 0xff000000, this->mapRenderer, this->mapView, this->colorSess));
	this->mapMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->cboCamera, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboCamera->SetRect(4, 4, 150, 23, false);
	this->cboCamera->HandleSelectionChange(OnCameraChg, this);
	NEW_CLASS(this->txtTimeAdj, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"0", false));
	this->txtTimeAdj->SetRect(154, 4, 50, 23, false);
	this->txtTimeAdj->SetReadOnly(true);
	NEW_CLASS(this->btnPaste, UI::GUIButton(ui, this->pnlControl, this->env->GetLang((const UTF8Char*)"TimeAdjPaste")));
	this->btnPaste->SetRect(204, 4, 50, 23, false);
	this->btnPaste->HandleButtonClick(OnPasteClicked, this);
	NEW_CLASS(this->btnTimeSub, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"-"));
	this->btnTimeSub->SetRect(254, 4, 25, 23, false);
	this->btnTimeSub->HandleButtonClick(OnTimeSubClicked, this);
	NEW_CLASS(this->btnTimeAdd, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"+"));
	this->btnTimeAdd->SetRect(279, 4, 25, 23, false);
	this->btnTimeAdd->HandleButtonClick(OnTimeAddClicked, this);
	NEW_CLASS(this->btnTimeApply, UI::GUIButton(ui, this->pnlControl, this->env->GetLang((const UTF8Char*)"TimeAdjApply")));
	this->btnTimeApply->SetRect(329, 4, 50, 23, false);
	this->btnTimeApply->HandleButtonClick(OnTimeApplyClicked, this);

	UserFileInfo *userFile;
	if (this->gpsTrk)
	{
		UTF8Char sbuff[32];
		Map::GPSTrack::GPSRecord *records;
		Data::DateTime dt;
		records = this->gpsTrk->GetTrack(0, &j);
		i = 0;
		while (i < j)
		{
			dt.SetTicks(records[i].utcTimeTicks);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			this->lbTrack->AddItem(sbuff, &records[i]);
			i++;
		}
	}

	Data::ArrayListInt32 spList;
	Data::ArrayList<OrganGroupItem*> spItems;
	OrganSpecies *sp;
	i = 0;
	j = this->userFileList->GetCount();
	while (i < j)
	{
		userFile = userFileList->GetItem(i);
		k = spList.SortedIndexOf(userFile->speciesId);
		if (k < 0)
		{
			spList.SortedInsert(userFile->speciesId);
		}
		if (userFile->camera)
		{
			this->cameraMap->Put(userFile->camera, (Int32)((userFile->captureTimeTicks - userFile->fileTimeTicks) / 1000));
		}
		i++;
	}
	Data::ArrayList<const UTF8Char *> *cameraList = this->cameraMap->GetKeys();
	Text::StringBuilderUTF8 sbu8;
	i = 0;
	j = cameraList->GetCount();
	while (i < j)
	{
		this->cboCamera->AddItem(cameraList->GetItem(i), 0);
		i++;
	}
	if (cameraList->GetCount() > 0)
	{
		UTF8Char sbuff[16];
		this->cboCamera->SetSelectedIndex(0);
		Text::StrInt32(sbuff, this->cameraMap->Get(cameraList->GetItem(0)));
		this->txtTimeAdj->SetText(sbuff);
	}

	this->cboSpecies->AddItem(this->env->GetLang((const UTF8Char*)"TimeAdjAllSp"), 0);
	this->env->GetSpeciesItems(&spItems, &spList);
	i = 0;
	j = spItems.GetCount();
	while (i < j)
	{
		sp = (OrganSpecies*)spItems.GetItem(i);
		sbu8.ClearStr();
		sbu8.Append(sp->GetCName());
		sbu8.Append((const UTF8Char*)" ");
		sbu8.Append(sp->GetSName());

		this->cboSpecies->AddItem(sbu8.ToString(), (void*)(OSInt)sp->GetSpeciesId());
		DEL_CLASS(sp);
		i++;
	}
	this->cboSpecies->SetSelectedIndex(0);
	this->AddTimer(1000, OnTimerTick, this);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganTimeAdjForm::~OrganTimeAdjForm()
{
	DEL_CLASS(this->userFileList);
	DEL_CLASS(this->currFileList);
	DEL_CLASS(this->cameraMap);
	this->ClearChildren();
	DEL_CLASS(this->mapEnv);
	DEL_CLASS(this->mapRenderer);
	SDEL_CLASS(this->gpsTrk);
	SDEL_CLASS(this->dispImg);
	this->env->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::OrganMgr::OrganTimeAdjForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
