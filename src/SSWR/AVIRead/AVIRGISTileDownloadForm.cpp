#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/Polygon.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (!me->selecting)
		return false;
	me->isDown = true;
	me->selecting = false;
	me->downX = x;
	me->downY = y;
	return true;
}

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseUp(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->isDown)
	{
		me->isDown = false;
		Double mapX1;
		Double mapY1;
		Double mapX2;
		Double mapY2;
		Double tmpV;
		me->navi->ScnXY2MapXY(me->downX, me->downY, &mapX1, &mapY1);
		me->navi->ScnXY2MapXY(x, y, &mapX2, &mapY2);
		if (mapX1 > mapX2)
		{
			tmpV = mapX1;
			mapX1 = mapX2;
			mapX2 = tmpV;
		}
		if (mapY1 > mapY2)
		{
			tmpV = mapY1;
			mapY1 = mapY2;
			mapY2 = tmpV;
		}
		me->selX1 = mapX1;
		me->selY1 = mapY1;
		me->selX2 = mapX2;
		me->selY2 = mapY2;

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Double *ptList = pg->GetPointList(&nPoints);
		ptList[0] = me->selX1;
		ptList[1] = me->selY1;
		ptList[2] = me->selX2;
		ptList[3] = me->selY1;
		ptList[4] = me->selX2;
		ptList[5] = me->selY2;
		ptList[6] = me->selX1;
		ptList[7] = me->selY2;
		ptList[8] = me->selX1;
		ptList[9] = me->selY1;
		me->navi->SetSelectedVector(pg);
		return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseMove(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	if (me->isDown)
	{
		Double mapX1;
		Double mapY1;
		Double mapX2;
		Double mapY2;
		Double tmpV;
		me->navi->ScnXY2MapXY(me->downX, me->downY, &mapX1, &mapY1);
		me->navi->ScnXY2MapXY(x, y, &mapX2, &mapY2);
		if (mapX1 > mapX2)
		{
			tmpV = mapX1;
			mapX1 = mapX2;
			mapX2 = tmpV;
		}
		if (mapY1 > mapY2)
		{
			tmpV = mapY1;
			mapY1 = mapY2;
			mapY2 = tmpV;
		}

		Math::Polygon *pg;
		NEW_CLASS(pg, Math::Polygon(me->navi->GetSRID(), 1, 5));
		UOSInt nPoints;
		Double *ptList = pg->GetPointList(&nPoints);
		ptList[0] = mapX1;
		ptList[1] = mapY1;
		ptList[2] = mapX2;
		ptList[3] = mapY1;
		ptList[4] = mapX2;
		ptList[5] = mapY2;
		ptList[6] = mapX1;
		ptList[7] = mapY2;
		ptList[8] = mapX1;
		ptList[9] = mapY1;
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
	UI::FolderDialog *dlg;
	if (me->selX1 != 0 || me->selY1 != 0 || me->selX2 != 0 || me->selY2 != 0)
	{
		NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"GISTileDown"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			me->SaveTilesDir(dlg->GetFolder());
		}
		DEL_CLASS(dlg);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	UI::FileDialog *dlg;
	if (me->selX1 != 0 || me->selY1 != 0 || me->selX2 != 0 || me->selY2 != 0)
	{
		NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"GISTileDownFile", true));
		dlg->AddFilter((const UTF8Char*)"*.spk", (const UTF8Char*)"SPackage File");
		if (dlg->ShowDialog(me->GetHandle()))
		{
			me->SaveTilesFile(dlg->GetFileName());
		}
		DEL_CLASS(dlg);
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
	WChar sbuff[32];
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
	IO::FileStream *fs;
	this->txtError->SetText(L"0");
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		Text::StrOSInt(sbuff, currLyr);
		this->txtLayer->SetText(sbuff);

		imgIdList.Clear();
		tileMap->GetImageIDs(currLyr, this->selX1, this->selY1, this->selX2, this->selY2, &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			Text::StrOSInt(Text::StrConcat(Text::StrOSInt(sbuff, cnt - i), L"/"), cnt);
			this->txtImages->SetText(sbuff);
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
						sb.Append((const UTF8Char*)".png");
					}
					else
					{
						sb.Append((const UTF8Char*)".jpg");
					}
					NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
					fs->Write(fileBuff, (OSInt)fileSize);
					DEL_CLASS(fs);
				}
				DEL_CLASS(fd);
			}
			else
			{
				err++;
				Text::StrOSInt(sbuff, err);
				this->txtError->SetText(sbuff);
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
	OSInt currLyr;
	OSInt lyrCnt = tileMap->GetLevelCount();
	Data::ArrayList<Int64> imgIdList;
	OSInt i;
	OSInt j;
	OSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText((const UTF8Char*)"0");
	this->stopDownload = false;
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		Text::StrOSInt(sbuff, currLyr);
		this->txtLayer->SetText(sbuff);

		imgIdList.Clear();
		tileMap->GetImageIDs(currLyr, this->selX1, this->selY1, this->selX2, this->selY2, &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			Text::StrOSInt(Text::StrConcat(Text::StrOSInt(sbuff, cnt - i), (const UTF8Char*)"/"), cnt);
			this->txtImages->SetText(sbuff);
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
				this->mainEvt->Wait(100);
			}
			if (err != this->errCnt)
			{
				err = this->errCnt;
				Text::StrOSInt(sbuff, err);
				this->txtError->SetText(sbuff);
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
		this->mainEvt->Wait(100);
	}
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesFile(const UTF8Char *fileName)
{
	Map::TileMap *tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	OSInt currLyr;
	UOSInt lyrCnt = tileMap->GetLevelCount();
	Data::ArrayList<Int64> imgIdList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText((const UTF8Char*)"0");
	this->stopDownload = false;

	IO::SPackageFile *spkg;
	Sync::Mutex *spkgMut;
	NEW_CLASS(spkg, IO::SPackageFile(fileName));
	NEW_CLASS(spkgMut, Sync::Mutex());
	
	currLyr = 0;
	while (currLyr < lyrCnt)
	{
		Text::StrOSInt(sbuff, currLyr);
		this->txtLayer->SetText(sbuff);

		imgIdList.Clear();
		tileMap->GetImageIDs(currLyr, this->selX1, this->selY1, this->selX2, this->selY2, &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			Text::StrOSInt(Text::StrConcat(Text::StrOSInt(sbuff, cnt - i), (const UTF8Char*)"/"), cnt);
			this->txtImages->SetText(sbuff);
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
						this->threadStat[j].spkg = spkg;
						this->threadStat[j].pkgMut = spkgMut;
						this->threadStat[j].folderName = 0;
						this->threadStat[j].threadStat = 2;
						this->threadStat[j].threadEvt->Set();

						found = true;
						break;
					}
				}
				if (found)
					break;
				this->mainEvt->Wait(100);
			}
			if (err != this->errCnt)
			{
				err = this->errCnt;
				Text::StrOSInt(sbuff, err);
				this->txtError->SetText(sbuff);
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
		this->mainEvt->Wait(100);
	}
	DEL_CLASS(spkgMut);
	DEL_CLASS(spkg);
}

UInt32 __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::ProcThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat*)userObj;
	UInt8 *fileBuff = 0;
	UOSInt fileBuffSize = 0;
	Map::TileMap::ImageType it;
	Int32 blockX;
	Int32 blockY;
	IO::IStreamData *fd;
	Double bounds[4];
	UInt64 fileSize;
	OSInt j;
	Text::StringBuilderUTF8 *sb;
	IO::FileStream *fs;
	Data::DateTime *dt;

	stat->threadStat = 1;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(dt, Data::DateTime());
	stat->me->mainEvt->Set();
	while (true)
	{
		if (stat->threadStat == 2)
		{
			fd = 0;
			j = 3;
			while (fd == 0 && j-- > 0)
			{
				fd = stat->tileMap->LoadTileImageData(stat->lyrId, stat->imageId, bounds, false, &blockX, &blockY, &it);
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
						sb->ClearStr();
						sb->AppendOSInt(stat->lyrId);
						sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb->AppendI32(blockX);
						sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb->AppendI32(blockY);
						if (it == Map::TileMap::IT_PNG)
						{
							sb->Append((const UTF8Char*)".png");
						}
						else
						{
							sb->Append((const UTF8Char*)".jpg");
						}
						dt->SetCurrTimeUTC();

//						stat->pkgMut->Lock();
						stat->spkg->AddFile(fileBuff, (OSInt)fileSize, sb->ToString(), dt->ToTicks());
//						stat->pkgMut->Unlock();
					}
					else
					{
						sb->ClearStr();
						sb->Append(stat->folderName);
						sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb->AppendOSInt(stat->lyrId);
						sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb->AppendI32(blockX);
						IO::Path::CreateDirectory(sb->ToString());
						sb->AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb->AppendI32(blockY);
						if (it == Map::TileMap::IT_PNG)
						{
							sb->Append((const UTF8Char*)".png");
						}
						else
						{
							sb->Append((const UTF8Char*)".jpg");
						}
						NEW_CLASS(fs, IO::FileStream(sb->ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
						fs->Write(fileBuff, (OSInt)fileSize);
						DEL_CLASS(fs);
					}
				}
				DEL_CLASS(fd);
			}
			else
			{
				Sync::Interlocked::Increment(&stat->me->errCnt);
			}
			stat->threadStat = 1;
			stat->me->mainEvt->Set();
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
	DEL_CLASS(dt);
	DEL_CLASS(sb);
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
	sb.Append((const UTF8Char*)"Tile Downloader - ");
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToString());
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);
	this->selecting = false;
	this->isDown = false;
	this->downX = 0;
	this->downY = 0;
	this->selX1 = 0;
	this->selY1 = 0;
	this->selX2 = 0;
	this->selY2 = 0;
	this->stopDownload = false;

	NEW_CLASS(this->btnArea, UI::GUIButton(ui, this, (const UTF8Char*)"Select Area"));
	this->btnArea->SetRect(4, 4, 100, 23, false);
	this->btnArea->HandleButtonClick(OnAreaClicked, this);
	NEW_CLASS(this->btnSave, UI::GUIButton(ui, this, (const UTF8Char*)"Save As"));
	this->btnSave->SetRect(114, 4, 100, 23, false);
	this->btnSave->HandleButtonClick(OnSaveFileClicked, this);
	NEW_CLASS(this->btnStop, UI::GUIButton(ui, this, (const UTF8Char*)"Stop Downloading"));
	this->btnStop->SetRect(224, 4, 100, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);

	NEW_CLASS(this->lblLayer, UI::GUILabel(ui, this, (const UTF8Char*)"Curr Layer"));
	this->lblLayer->SetRect(4, 36, 100, 23, false);
	NEW_CLASS(this->txtLayer, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetRect(104, 36, 60, 23, false);
	NEW_CLASS(this->lblImages, UI::GUILabel(ui, this, (const UTF8Char*)"Images"));
	this->lblImages->SetRect(4, 60, 100, 23, false);
	NEW_CLASS(this->txtImages, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtImages->SetReadOnly(true);
	this->txtImages->SetRect(104, 60, 100, 23, false);
	NEW_CLASS(this->lblError, UI::GUILabel(ui, this, (const UTF8Char*)"Error"));
	this->lblError->SetRect(4, 84, 100, 23, false);
	NEW_CLASS(this->txtError, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtError->SetReadOnly(true);
	this->txtError->SetRect(104, 84, 100, 23, false);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);

	OSInt i;
	this->threadCnt = this->lyr->GetTileMap()->GetConcurrentCount();
	if (this->threadCnt <= 0)
	{
		this->threadCnt = 1;
	}
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRGISTileDownloadForm.mainEvt"));
	this->threadStat = MemAlloc(ThreadStat, this->threadCnt);
	i = 0;
	while (i < this->threadCnt)
	{
		this->threadStat[i].me = this;
		NEW_CLASS(this->threadStat[i].threadEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRGISTileDownloadForm.threadStat.threadEvt"));
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
		this->mainEvt->Wait(100);
	}
}

SSWR::AVIRead::AVIRGISTileDownloadForm::~AVIRGISTileDownloadForm()
{
	this->navi->SetSelectedVector(0);
	this->navi->UnhandleMapMouse(this);

	OSInt i;
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
		this->mainEvt->Wait(100);
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadStat[i].threadEvt);
	}
	MemFree(this->threadStat);
	DEL_CLASS(this->mainEvt);
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
