#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/Geometry/Polygon.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (!me->selecting)
		return false;
	me->isDown = true;
	me->selecting = false;
	me->downPos = scnPos;
	return true;
}

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->isDown)
	{
		me->isDown = false;
		Double tmpV;
		Math::Coord2DDbl mapPt1 = me->navi->ScnXY2MapXY(me->downPos);
		Math::Coord2DDbl mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (mapPt1.x > mapPt2.x)
		{
			tmpV = mapPt1.x;
			mapPt1.x = mapPt2.x;
			mapPt2.x = tmpV;
		}
		if (mapPt1.y > mapPt2.y)
		{
			tmpV = mapPt1.y;
			mapPt1.y = mapPt2.y;
			mapPt2.y = tmpV;
		}
		me->sel1 = mapPt1;
		me->sel2 = mapPt2;

		Math::Geometry::Polygon *pg;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		UOSInt nPoints;
		Math::Coord2DDbl *ptList = pg->GetPointList(&nPoints);
		ptList[0].x = me->sel1.x;
		ptList[0].y = me->sel1.y;
		ptList[1].x = me->sel2.x;
		ptList[1].y = me->sel1.y;
		ptList[2].x = me->sel2.x;
		ptList[2].y = me->sel2.y;
		ptList[3].x = me->sel1.x;
		ptList[3].y = me->sel2.y;
		ptList[4].x = me->sel1.x;
		ptList[4].y = me->sel1.y;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->isDown)
	{
		Double tmpV;
		Math::Coord2DDbl mapPt1 = me->navi->ScnXY2MapXY(me->downPos);
		Math::Coord2DDbl mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (mapPt1.x > mapPt2.x)
		{
			tmpV = mapPt1.x;
			mapPt1.x = mapPt2.x;
			mapPt2.x = tmpV;
		}
		if (mapPt1.y > mapPt2.y)
		{
			tmpV = mapPt1.y;
			mapPt1.y = mapPt2.y;
			mapPt2.y = tmpV;
		}

		Math::Geometry::Polygon *pg;
		NEW_CLASS(pg, Math::Geometry::Polygon(me->navi->GetSRID(), 1, 5, false, false));
		UOSInt nPoints;
		Math::Coord2DDbl *ptList = pg->GetPointList(&nPoints);
		ptList[0].x = mapPt1.x;
		ptList[0].y = mapPt1.y;
		ptList[1].x = mapPt2.x;
		ptList[1].y = mapPt1.y;
		ptList[2].x = mapPt2.x;
		ptList[2].y = mapPt2.y;
		ptList[3].x = mapPt1.x;
		ptList[3].y = mapPt2.y;
		ptList[4].x = mapPt1.x;
		ptList[4].y = mapPt1.y;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnAreaClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	me->selecting = true;
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveDirClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"GISTileDown");
		if (dlg.ShowDialog(me->GetHandle()))
		{
			me->SaveTilesDir(dlg.GetFolder()->v);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"GISTileDownFile", true);
		dlg.AddFilter(CSTR("*.spk"), CSTR("SPackage File"));
		if (dlg.ShowDialog(me->GetHandle()))
		{
			me->SaveTilesFile(dlg.GetFileName()->ToCString());
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	me->stopDownload = true;
}

/*
void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTiles(const WChar *folderName)
{
	Map::TileMap *tileMap = this->lyr->GetTileMap();
	Text::StringBuilderW sb;
	WChar wbuff[32];
	OSInt currLyr;
	OSInt lyrCnt = tileMap->GetLevelCount();
	UInt8 *fileBuff = 0;
	OSInt fileBuffSize = 0;
	Data::ArrayList<Int64> imgIdList;
	Map::TileMap::ImageType it;
	Int32 blockX;
	Int32 blockY;
	IO::IStreamData *fd;
	Double bounds[4];
	Int64 fileSize;
	OSInt i;
	OSInt j;
	OSInt cnt;
	OSInt err = 0;
	this->txtError->SetText(L"0");
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		Text::StrOSInt(wbuff, currLyr);
		this->txtLayer->SetText(wbuff);

		imgIdList.Clear();
		tileMap->GetImageIDs(currLyr, this->selX1, this->selY1, this->selX2, this->selY2, &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			Text::StrOSInt(Text::StrConcat(Text::StrOSInt(wbuff, cnt - i), L"/"), cnt);
			this->txtImages->SetText(wbuff);
			this->ui->ProcessMessages();

			fd = 0;
			j = 3;
			while (fd == 0 && j-- > 0)
			{
				fd = tileMap->LoadTileImageData(currLyr, imgIdList.GetItem(i), bounds, false, &blockX, &blockY, &it);
			}
			if (fd)
			{
				fileSize = fd->GetDataSize();
				if (fileSize > fileBuffSize)
				{
					fileBuffSize = (OSInt)fileSize;
					if (fileBuff)
					{
						MemFree(fileBuff);
					}
					fileBuff = MemAlloc(UInt8, fileBuffSize);
				}
				if (fd->GetRealData(0, (OSInt)fileSize, fileBuff) == fileSize)
				{
					sb.ClearStr();
					sb.Append(folderName);
					sb.Append(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(currLyr);
					sb.Append(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(blockX);
					IO::Path::CreateDirectory(sb.ToString());
					sb.Append(IO::Path::PATH_SEPERATOR, 1);
					sb.Append(blockY);
					if (it == Map::TileMap::IT_PNG)
					{
						sb.AppendC(UTF8STRC(".png"));
					}
					else
					{
						sb.AppendC(UTF8STRC(".jpg"));
					}
					IO::FileStream fs(sb.ToString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(fileBuff, (OSInt)fileSize);
				}
				DEL_CLASS(fd);
			}
			else
			{
				err++;
				Text::StrOSInt(wbuff, err);
				this->txtError->SetText(wbuff);
			}
		}
		currLyr++;
	}
	if (fileBuff)
	{
		MemFree(fileBuff);
		fileBuff = 0;
	}
}*/

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesDir(const UTF8Char *folderName)
{
	Map::TileMap *tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt currLyr;
	UOSInt lyrCnt = tileMap->GetLevelCount();
	Data::ArrayList<Math::Coord2D<Int32>> imgIdList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText(CSTR("0"));
	this->stopDownload = false;
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		sptr = Text::StrUOSInt(sbuff, currLyr);
		this->txtLayer->SetText(CSTRP(sbuff, sptr));

		imgIdList.Clear();
		tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(this->sel1, this->sel2), &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(Text::StrUOSInt(sbuff, cnt - i), UTF8STRC("/")), cnt);
			this->txtImages->SetText(CSTRP(sbuff, sptr));
			this->ui->ProcessMessages();

			found = false;
			while (!this->stopDownload)
			{
				j = this->threadCnt;
				while (j-- > 0)
				{
					if (this->threadStat[j].threadStat == 1)
					{
						this->threadStat[j].lyrId = currLyr;
						this->threadStat[j].imageId = imgIdList.GetItem(i);
						this->threadStat[j].spkg = 0;
						this->threadStat[j].pkgMut = 0;
						this->threadStat[j].folderName = folderName;
						this->threadStat[j].threadStat = 2;

						found = true;
						break;
					}
				}
				if (found)
					break;
				this->mainEvt.Wait(100);
			}
			if (err != this->errCnt)
			{
				err = this->errCnt;
				sptr = Text::StrOSInt(sbuff, err);
				this->txtError->SetText(CSTRP(sbuff, sptr));
			}
			if (this->stopDownload)
				break;
		}
		if (this->stopDownload)
			break;
		currLyr++;
	}
	while (true)
	{
		found = false;
		j = this->threadCnt;
		while (j-- > 0)
		{
			if (this->threadStat[j].threadStat != 1)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt.Wait(100);
	}
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesFile(Text::CString fileName)
{
	Map::TileMap *tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt currLyr;
	UOSInt lyrCnt = tileMap->GetLevelCount();
	Data::ArrayList<Math::Coord2D<Int32>> imgIdList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText(CSTR("0"));
	this->stopDownload = false;

	Sync::Mutex spkgMut;
	IO::SPackageFile spkg(fileName);
	
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		sptr = Text::StrUOSInt(sbuff, currLyr);
		this->txtLayer->SetText(CSTRP(sbuff, sptr));

		imgIdList.Clear();
		tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(this->sel1, this->sel2), &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(Text::StrUOSInt(sbuff, cnt - i), UTF8STRC("/")), cnt);
			this->txtImages->SetText(CSTRP(sbuff, sptr));
			this->ui->ProcessMessages();

			found = false;
			while (!this->stopDownload)
			{
				j = this->threadCnt;
				while (j-- > 0)
				{
					if (this->threadStat[j].threadStat == 1)
					{
						this->threadStat[j].lyrId = currLyr;
						this->threadStat[j].imageId = imgIdList.GetItem(i);
						this->threadStat[j].spkg = &spkg;
						this->threadStat[j].pkgMut = &spkgMut;
						this->threadStat[j].folderName = 0;
						this->threadStat[j].threadStat = 2;
						this->threadStat[j].threadEvt->Set();

						found = true;
						break;
					}
				}
				if (found)
					break;
				this->mainEvt.Wait(100);
			}
			if (err != this->errCnt)
			{
				err = this->errCnt;
				sptr = Text::StrOSInt(sbuff, err);
				this->txtError->SetText(CSTRP(sbuff, sptr));
			}
			if (this->stopDownload)
				break;
		}
		if (this->stopDownload)
			break;
		currLyr++;
	}
	while (true)
	{
		found = false;
		j = this->threadCnt;
		while (j-- > 0)
		{
			if (this->threadStat[j].threadStat != 1)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		this->mainEvt.Wait(100);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::ProcThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat*)userObj;
	UInt8 *fileBuff = 0;
	UOSInt fileBuffSize = 0;
	Map::TileMap::ImageType it;
	IO::IStreamData *fd;
	Math::RectAreaDbl bounds;
	UInt64 fileSize;
	OSInt j;

	stat->threadStat = 1;
	{
		Text::StringBuilderUTF8 sb;
		stat->me->mainEvt.Set();
		while (true)
		{
			if (stat->threadStat == 2)
			{
				fd = 0;
				j = 3;
				while (fd == 0 && j-- > 0)
				{
					fd = stat->tileMap->LoadTileImageData(stat->lyrId, stat->imageId, &bounds, false, &it);
				}
				if (fd)
				{
					fileSize = fd->GetDataSize();
					if (fileSize > fileBuffSize)
					{
						fileBuffSize = (UOSInt)fileSize;
						if (fileBuff)
						{
							MemFree(fileBuff);
						}
						fileBuff = MemAlloc(UInt8, fileBuffSize);
					}
					if (fd->GetRealData(0, (UOSInt)fileSize, fileBuff) == fileSize)
					{
						if (stat->spkg)
						{
							sb.ClearStr();
							sb.AppendUOSInt(stat->lyrId);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.x);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.y);
							if (it == Map::TileMap::IT_PNG)
							{
								sb.AppendC(UTF8STRC(".png"));
							}
							else
							{
								sb.AppendC(UTF8STRC(".jpg"));
							}
	//						stat->pkgMut->Lock();
							stat->spkg->AddFile(fileBuff, (UOSInt)fileSize, sb.ToCString(), Data::Timestamp::UtcNow());
	//						stat->pkgMut->Unlock();
						}
						else
						{
							sb.ClearStr();
							sb.AppendSlow(stat->folderName);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendUOSInt(stat->lyrId);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.x);
							IO::Path::CreateDirectory(sb.ToCString());
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.y);
							if (it == Map::TileMap::IT_PNG)
							{
								sb.AppendC(UTF8STRC(".png"));
							}
							else
							{
								sb.AppendC(UTF8STRC(".jpg"));
							}
							IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
							fs.Write(fileBuff, (UOSInt)fileSize);
						}
					}
					DEL_CLASS(fd);
				}
				else
				{
					Sync::Interlocked::Increment(&stat->me->errCnt);
				}
				stat->threadStat = 1;
				stat->me->mainEvt.Set();
			}
			else if (stat->threadStat == 3)
			{
				break;
			}
			stat->threadEvt->Wait(1000);
		}
		if (fileBuff)
		{
			MemFree(fileBuff);
			fileBuff = 0;
		}
	}
	stat->threadStat = 0;
	return 0;
}

SSWR::AVIRead::AVIRGISTileDownloadForm::AVIRGISTileDownloadForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::TileMapLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 360, 144, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Tile Downloader - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->selecting = false;
	this->isDown = false;
	this->downPos = Math::Coord2D<OSInt>(0, 0);
	this->sel1 = Math::Coord2DDbl(0, 0);
	this->sel2 = Math::Coord2DDbl(0, 0);
	this->stopDownload = false;

	NEW_CLASS(this->btnArea, UI::GUIButton(ui, this, CSTR("Select Area")));
	this->btnArea->SetRect(4, 4, 100, 23, false);
	this->btnArea->HandleButtonClick(OnAreaClicked, this);
	NEW_CLASS(this->btnSave, UI::GUIButton(ui, this, CSTR("Save As")));
	this->btnSave->SetRect(114, 4, 100, 23, false);
	this->btnSave->HandleButtonClick(OnSaveFileClicked, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, CSTR("Stop Downloading")));
	this->btnStop->SetRect(224, 4, 100, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);

	NEW_CLASS(this->lblLayer, UI::GUILabel(ui, this, CSTR("Curr Layer")));
	this->lblLayer->SetRect(4, 36, 100, 23, false);
	NEW_CLASS(this->txtLayer, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetRect(104, 36, 60, 23, false);
	NEW_CLASS(this->lblImages, UI::GUILabel(ui, this, CSTR("Images")));
	this->lblImages->SetRect(4, 60, 100, 23, false);
	NEW_CLASS(this->txtImages, UI::GUITextBox(ui, this, CSTR("")));
	this->txtImages->SetReadOnly(true);
	this->txtImages->SetRect(104, 60, 100, 23, false);
	NEW_CLASS(this->lblError, UI::GUILabel(ui, this, CSTR("Error")));
	this->lblError->SetRect(4, 84, 100, 23, false);
	NEW_CLASS(this->txtError, UI::GUITextBox(ui, this, CSTR("")));
	this->txtError->SetReadOnly(true);
	this->txtError->SetRect(104, 84, 100, 23, false);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);

	UOSInt i;
	this->threadCnt = this->lyr->GetTileMap()->GetConcurrentCount();
	if (this->threadCnt <= 0)
	{
		this->threadCnt = 1;
	}
	this->threadStat = MemAlloc(ThreadStat, this->threadCnt);
	i = 0;
	while (i < this->threadCnt)
	{
		this->threadStat[i].me = this;
		NEW_CLASS(this->threadStat[i].threadEvt, Sync::Event(true));
		this->threadStat[i].threadStat = 0;
		this->threadStat[i].lyrId = 0;
		this->threadStat[i].imageId = 0;
		this->threadStat[i].spkg = 0;
		this->threadStat[i].pkgMut = 0;
		this->threadStat[i].folderName = 0;
		this->threadStat[i].tileMap = this->lyr->GetTileMap();
		Sync::Thread::Create(ProcThread, &this->threadStat[i]);
		i++;
	}
	Bool running;
	while (true)
	{
		running = true;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStat[i].threadStat == 0)
			{
				running = false;
				break;
			}
		}
		if (running)
			break;
		this->mainEvt.Wait(100);
	}
}

SSWR::AVIRead::AVIRGISTileDownloadForm::~AVIRGISTileDownloadForm()
{
	this->navi->SetSelectedVector(0);
	this->navi->UnhandleMapMouse(this);

	UOSInt i;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threadStat[i].threadStat = 3;
		this->threadStat[i].threadEvt->Set();
	}
	Bool running;
	while (true)
	{
		running = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStat[i].threadStat != 0)
			{
				running = true;
				break;
			}
		}
		if (!running)
			break;
		this->mainEvt.Wait(100);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadStat[i].threadEvt);
	}
	MemFree(this->threadStat);
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
