#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Math/Geometry/Polygon.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/JSONBuilder.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"

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

		Math::Geometry::LinearRing *lr;
		NEW_CLASS(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false));
		UOSInt nPoints;
		Math::Coord2DDbl *ptList = lr->GetPointList(nPoints);
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
		me->navi->SetSelectedVector(lr);

		UTF8Char sbuff[32];
		UTF8Char *sptr;
		NotNullPtr<Map::TileMap> tileMap = me->lyr->GetTileMap();
		UOSInt maxLevel = tileMap->GetMaxLevel();
		UOSInt currLyr = tileMap->GetMinLevel();
		Data::ArrayList<Math::Coord2D<Int32>> imgIdList;
		UOSInt cnt = 0;
		while (currLyr <= maxLevel)
		{
			imgIdList.Clear();
			tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(me->sel1, me->sel2), &imgIdList);
			cnt += imgIdList.GetCount();
			currLyr++;
		}
		sptr = Text::StrUOSInt(sbuff, cnt);
		me->txtTotalImages->SetText(CSTRP(sbuff, sptr));

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

		Math::Geometry::LinearRing *lr;
		NEW_CLASS(lr, Math::Geometry::LinearRing(me->navi->GetSRID(), 5, false, false));
		UOSInt nPoints;
		Math::Coord2DDbl *ptList = lr->GetPointList(nPoints);
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
		me->navi->SetSelectedVector(lr);
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
	UOSInt minLevel;
	UOSInt maxLevel;
	if (!me->GetLevels(minLevel, maxLevel))
		return;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"GISTileDown");
		if (dlg.ShowDialog(me->GetHandle()))
		{
			me->SaveTilesDir(dlg.GetFolder()->v, minLevel, maxLevel);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	UOSInt minLevel;
	UOSInt maxLevel;
	if (!me->GetLevels(minLevel, maxLevel))
		return;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"GISTileDownFile", true);
		dlg.AddFilter(CSTR("*.spk"), CSTR("SPackage File"));
		dlg.AddFilter(CSTR("*.zip"), CSTR("ZIP File"));
		if (dlg.ShowDialog(me->GetHandle()))
		{
			me->SaveTilesFile(dlg.GetFileName()->ToCString(), dlg.GetFilterIndex(), minLevel, maxLevel);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnStopClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISTileDownloadForm *me = (SSWR::AVIRead::AVIRGISTileDownloadForm*)userObj;
	me->stopDownload = true;
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesDir(const UTF8Char *folderName, UOSInt userMinLevel, UOSInt userMaxLevel)
{
	NotNullPtr<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt currLyr;
	UOSInt tileMinLevel = tileMap->GetMinLevel();
	UOSInt tileMaxLevel = tileMap->GetMaxLevel();
	if (userMinLevel > tileMaxLevel || userMaxLevel < tileMinLevel)
		return;
	if (userMinLevel < tileMinLevel)
		userMinLevel = tileMinLevel;
	if (userMaxLevel > tileMaxLevel)
		userMaxLevel = tileMaxLevel;
	Data::ArrayList<Math::Coord2D<Int32>> imgIdList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText(CSTR("0"));
	this->stopDownload = false;
	currLyr = tileMinLevel;
	while (currLyr <= tileMaxLevel)
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
						this->threadStat[j].zip = 0;
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

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesFile(Text::CStringNN fileName, UOSInt fileType, UOSInt userMinLevel, UOSInt userMaxLevel)
{
	NotNullPtr<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt currLyr;
	UOSInt tileMinLevel = tileMap->GetMinLevel();
	UOSInt tileMaxLevel = tileMap->GetMaxLevel();
	if (userMinLevel > tileMaxLevel || userMaxLevel < tileMinLevel)
		return;
	if (userMinLevel < tileMinLevel)
		userMinLevel = tileMinLevel;
	if (userMaxLevel > tileMaxLevel)
		userMaxLevel = tileMaxLevel;
	Data::ArrayList<Math::Coord2D<Int32>> imgIdList;
	Data::ArrayListInt32 imgXList;
	UOSInt i;
	UOSInt j;
	UOSInt cnt;
	OSInt err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText(CSTR("0"));
	this->stopDownload = false;

	Sync::Mutex *spkgMut = 0;
	IO::SPackageFile *spkg = 0;
	IO::FileStream *zipFS = 0;
	IO::ZIPBuilder *zip = 0;
	if (fileType == 1)
	{
		NotNullPtr<IO::FileStream> fs;
		NEW_CLASSNN(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		zipFS = fs.Ptr();
		NEW_CLASS(zip, IO::ZIPBuilder(fs, IO::ZIPOS::UNIX));
	}
	else
	{
		NEW_CLASS(spkgMut, Sync::Mutex());
		NEW_CLASS(spkg, IO::SPackageFile(fileName));
	}
	
	currLyr = userMinLevel;
	while (currLyr <= userMaxLevel)
	{
		sptr = Text::StrUOSInt(sbuff, currLyr);
		this->txtLayer->SetText(CSTRP(sbuff, sptr));

		if (zip)
		{
			*sptr++ = '/';
			*sptr = 0;
			Data::Timestamp t = Data::Timestamp::UtcNow();
			zip->AddDir(CSTRP(sbuff, sptr), t, t, t, 0);
			imgXList.Clear();
		}

		imgIdList.Clear();
		tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(this->sel1, this->sel2), &imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(Text::StrUOSInt(sbuff, cnt - i), UTF8STRC("/")), cnt);
			this->txtImages->SetText(CSTRP(sbuff, sptr));
			this->ui->ProcessMessages();
			Math::Coord2D<Int32> imageId = imgIdList.GetItem(i);
			if (zip)
			{
				OSInt k = imgXList.SortedIndexOf(imageId.x);
				if (k < 0)
				{
					imgXList.Insert((UOSInt)~k, imageId.x);
					sptr = Text::StrUOSInt(sbuff, currLyr);
					*sptr++ = '/';
					sptr = Text::StrInt32(sptr, imageId.x);
					*sptr++ = '/';
					*sptr = 0;
					Data::Timestamp t = Data::Timestamp::UtcNow();
					zip->AddDir(CSTRP(sbuff, sptr), t, t, t, 0);
				}
			}

			found = false;
			while (!this->stopDownload)
			{
				j = this->threadCnt;
				while (j-- > 0)
				{
					if (this->threadStat[j].threadStat == 1)
					{
						this->threadStat[j].lyrId = currLyr;
						this->threadStat[j].imageId = imageId;
						this->threadStat[j].spkg = spkg;
						this->threadStat[j].pkgMut = spkgMut;
						this->threadStat[j].zip = zip;
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
	if (fileType == 1)
	{
		if (!this->stopDownload)
		{
			Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
			json.ObjectAddStr(CSTR("name"), this->lyr->GetName());
			json.ObjectAddStr(CSTR("version"), CSTR("1.0"));
			json.ObjectAddStr(CSTR("description"), CSTR(""));
			json.ObjectAddStr(CSTR("attribution"), CSTR("Generated by AVIRead (sswroom)"));
			json.ObjectAddStr(CSTR("type"), CSTR("overlay"));
			switch (this->lyr->GetTileMap()->GetImageType())
			{
			case Map::TileMap::IT_WEBP:
				json.ObjectAddStr(CSTR("format"), CSTR("webp"));
				break;
			case Map::TileMap::IT_JPG:
				json.ObjectAddStr(CSTR("format"), CSTR("jpg"));
				break;
			case Map::TileMap::IT_PNG:
				json.ObjectAddStr(CSTR("format"), CSTR("png"));
				break;
			}
			sptr = Text::StrUOSInt(sbuff, userMinLevel);
			json.ObjectAddStr(CSTR("minzoom"), CSTRP(sbuff, sptr));
			sptr = Text::StrUOSInt(sbuff, userMaxLevel);
			json.ObjectAddStr(CSTR("maxzoom"), CSTRP(sbuff, sptr));
			json.ObjectAddStr(CSTR("scale"), CSTR("1.000000"));
			json.ObjectBeginArray(CSTR("bounds"));
			json.ArrayAddFloat64(this->sel1.x);
			json.ArrayAddFloat64(this->sel1.y);
			json.ArrayAddFloat64(this->sel2.x);
			json.ArrayAddFloat64(this->sel2.y);
			json.ArrayEnd();
			json.ObjectAddStr(CSTR("profile"), CSTR("mercator"));
			Text::CStringNN metadata = json.Build();
			Data::Timestamp t = Data::Timestamp::UtcNow();
			zip->AddFile(CSTR("metadata.json"), metadata.v, metadata.leng, t, t, t, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
		}
		DEL_CLASS(zip);
		DEL_CLASS(zipFS);
	}
	else
	{
		DEL_CLASS(spkg);
		DEL_CLASS(spkgMut);
	}
}

Bool SSWR::AVIRead::AVIRGISTileDownloadForm::GetLevels(OutParam<UOSInt> minLevel, OutParam<UOSInt> maxLevel)
{
	NotNullPtr<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UInt32 userMinLevel;
	UInt32 userMaxLevel;
	Text::StringBuilderUTF8 sb;
	this->txtMinLevel->GetText(sb);
	if (!sb.ToUInt32(userMinLevel))
	{
		UI::MessageDialog::ShowDialog(CSTR("Min Level invalid"), CSTR("Tile Downloader"), this);
		return false;
	}
	sb.ClearStr();
	this->txtMaxLevel->GetText(sb);
	if (!sb.ToUInt32(userMaxLevel))
	{
		UI::MessageDialog::ShowDialog(CSTR("Max Level invalid"), CSTR("Tile Downloader"), this);
		return false;
	}
	if (userMinLevel > tileMap->GetMaxLevel() || userMaxLevel < tileMap->GetMinLevel())
	{
		UI::MessageDialog::ShowDialog(CSTR("Input level is not within tile map range"), CSTR("Tile Downloader"), this);
		return false;
	}

	minLevel.Set(userMinLevel);
	maxLevel.Set(userMaxLevel);
	return true;
}

UInt32 __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::ProcThread(void *userObj)
{
	ThreadStat *stat = (ThreadStat*)userObj;
	Map::TileMap::ImageType it;
	IO::StreamData *fd;
	Math::RectAreaDbl bounds;
	UInt64 fileSize;
	OSInt j;

	stat->threadStat = 1;
	{
		Data::ByteBuffer fileBuff;
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
					fd = stat->tileMap->LoadTileImageData(stat->lyrId, stat->imageId, bounds, false, it).OrNull();
				}
				if (fd)
				{
					fileSize = fd->GetDataSize();
					if (fileSize > fileBuff.GetSize())
					{
						fileBuff.ChangeSize((UOSInt)fileSize);
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
							switch (it)
							{
							default:
							case Map::TileMap::IT_PNG:
								sb.AppendC(UTF8STRC(".png"));
								break;
							case Map::TileMap::IT_WEBP:
								sb.AppendC(UTF8STRC(".webp"));
								break;
							case Map::TileMap::IT_JPG:
								sb.AppendC(UTF8STRC(".jpg"));
								break;
							}
	//						stat->pkgMut->Lock();
							stat->spkg->AddFile(fileBuff.Ptr(), (UOSInt)fileSize, sb.ToCString(), Data::Timestamp::UtcNow());
	//						stat->pkgMut->Unlock();
						}
						else if (stat->zip)
						{
							sb.ClearStr();
							sb.AppendUOSInt(stat->lyrId);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.x);
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendI32(stat->imageId.y);
							switch (it)
							{
							default:
							case Map::TileMap::IT_PNG:
								sb.AppendC(UTF8STRC(".png"));
								break;
							case Map::TileMap::IT_WEBP:
								sb.AppendC(UTF8STRC(".webp"));
								break;
							case Map::TileMap::IT_JPG:
								sb.AppendC(UTF8STRC(".jpg"));
								break;
							}
							Data::Timestamp t = Data::Timestamp::UtcNow();
							stat->zip->AddFile(sb.ToCString(), fileBuff.Ptr(), (UOSInt)fileSize, t, t, t, Data::Compress::Inflate::CompressionLevel::BestCompression, 0);
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
							switch (it)
							{
							default:
							case Map::TileMap::IT_PNG:
								sb.AppendC(UTF8STRC(".png"));
								break;
							case Map::TileMap::IT_WEBP:
								sb.AppendC(UTF8STRC(".webp"));
								break;
							case Map::TileMap::IT_JPG:
								sb.AppendC(UTF8STRC(".jpg"));
								break;
							}
							IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
							fs.Write(fileBuff.Ptr(), (UOSInt)fileSize);
						}
					}
					DEL_CLASS(fd);
				}
				else
				{
					Sync::Interlocked::IncrementI32(stat->me->errCnt);
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
	}
	stat->threadStat = 0;
	return 0;
}

SSWR::AVIRead::AVIRGISTileDownloadForm::AVIRGISTileDownloadForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::TileMapLayer> lyr, IMapNavigator *navi) : UI::GUIForm(parent, 360, 216, ui)
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

	NEW_CLASSNN(this->lblMinLevel, UI::GUILabel(ui, *this, CSTR("Min. Level")));
	this->lblMinLevel->SetRect(4, 4, 100, 23, false);
	NEW_CLASSNN(this->txtMinLevel, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtMinLevel->SetRect(104, 4, 60, 23, false);
	NEW_CLASSNN(this->lblMaxLevel, UI::GUILabel(ui, *this, CSTR("Min. Level")));
	this->lblMaxLevel->SetRect(4, 28, 100, 23, false);
	NEW_CLASSNN(this->txtMaxLevel, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtMaxLevel->SetRect(104, 28, 60, 23, false);
	NEW_CLASSNN(this->btnArea, UI::GUIButton(ui, *this, CSTR("Select Area")));
	this->btnArea->SetRect(4, 52, 100, 23, false);
	this->btnArea->HandleButtonClick(OnAreaClicked, this);
	NEW_CLASSNN(this->btnSave, UI::GUIButton(ui, *this, CSTR("Save As")));
	this->btnSave->SetRect(114, 52, 100, 23, false);
	this->btnSave->HandleButtonClick(OnSaveFileClicked, this);
	NEW_CLASSNN(this->btnStop, UI::GUIButton(ui, *this, CSTR("Stop Downloading")));
	this->btnStop->SetRect(224, 52, 100, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);

	NEW_CLASSNN(this->lblTotalImages, UI::GUILabel(ui, *this, CSTR("Total Images")));
	this->lblTotalImages->SetRect(4, 84, 100, 23, false);
	NEW_CLASSNN(this->txtTotalImages, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtTotalImages->SetReadOnly(true);
	this->txtTotalImages->SetRect(104, 84, 60, 23, false);
	NEW_CLASSNN(this->lblLayer, UI::GUILabel(ui, *this, CSTR("Curr Layer")));
	this->lblLayer->SetRect(4, 108, 100, 23, false);
	NEW_CLASSNN(this->txtLayer, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetRect(104, 108, 60, 23, false);
	NEW_CLASSNN(this->lblImages, UI::GUILabel(ui, *this, CSTR("Images")));
	this->lblImages->SetRect(4, 132, 100, 23, false);
	NEW_CLASSNN(this->txtImages, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtImages->SetReadOnly(true);
	this->txtImages->SetRect(104, 132, 100, 23, false);
	NEW_CLASSNN(this->lblError, UI::GUILabel(ui, *this, CSTR("Error")));
	this->lblError->SetRect(4, 156, 100, 23, false);
	NEW_CLASSNN(this->txtError, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtError->SetReadOnly(true);
	this->txtError->SetRect(104, 156, 100, 23, false);

	sb.ClearStr();
	sb.AppendUOSInt(this->lyr->GetTileMap()->GetMinLevel());
	this->txtMinLevel->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendUOSInt(this->lyr->GetTileMap()->GetMaxLevel());
	this->txtMaxLevel->SetText(sb.ToCString());

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
		this->threadStat[i].zip = 0;
		this->threadStat[i].pkgMut = 0;
		this->threadStat[i].folderName = 0;
		this->threadStat[i].tileMap = this->lyr->GetTileMap();
		Sync::ThreadUtil::Create(ProcThread, &this->threadStat[i]);
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
