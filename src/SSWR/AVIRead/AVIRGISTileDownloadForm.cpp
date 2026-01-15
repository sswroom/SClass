#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/TileMapFolderWriter.h"
#include "Map/TileMapOruxWriter.h"
#include "Map/TileMapSPKWriter.h"
#include "Map/TileMapZipWriter.h"
#include "Math/Geometry/Polygon.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/JSONBuilder.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

UI::EventState __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseDown(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
	if (!me->selecting)
		return UI::EventState::ContinueEvent;
	me->isDown = true;
	me->selecting = false;
	me->downPos = scnPos;
	return UI::EventState::StopEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseUp(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
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
		UIntOS nPoints;
		UnsafeArray<Math::Coord2DDbl> ptList = lr->GetPointList(nPoints);
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
		UnsafeArray<UTF8Char> sptr;
		NN<Map::TileMap> tileMap = me->lyr->GetTileMap();
		UIntOS maxLevel = tileMap->GetMaxLevel();
		UIntOS currLyr = tileMap->GetMinLevel();
		Data::ArrayListT<Math::Coord2D<Int32>> imgIdList;
		UIntOS cnt = 0;
		while (currLyr <= maxLevel)
		{
			imgIdList.Clear();
			tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(me->sel1, me->sel2), imgIdList);
			cnt += imgIdList.GetCount();
			currLyr++;
		}
		sptr = Text::StrUIntOS(sbuff, cnt);
		me->txtTotalImages->SetText(CSTRP(sbuff, sptr));

		return UI::EventState::StopEvent;
	}
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnMouseMove(AnyType userObj, Math::Coord2D<IntOS> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
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
		UIntOS nPoints;
		UnsafeArray<Math::Coord2DDbl> ptList = lr->GetPointList(nPoints);
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
		return UI::EventState::StopEvent;
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnAreaClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
	me->selecting = true;
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveDirClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
	UIntOS minLevel;
	UIntOS maxLevel;
	if (!me->GetLevels(minLevel, maxLevel))
		return;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
		if (dlg->ShowDialog(me->GetHandle()))
		{
			me->SaveTilesDir(dlg->GetFolder()->ToCString(), minLevel, maxLevel);
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnSaveFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
	UIntOS minLevel;
	UIntOS maxLevel;
	if (!me->GetLevels(minLevel, maxLevel))
		return;
	if (me->sel1.x != 0 || me->sel1.y != 0 || me->sel2.x != 0 || me->sel2.y != 0)
	{
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISTileDownFile", true);
		dlg->AddFilter(CSTR("*.spk"), CSTR("SPackage File"));
		dlg->AddFilter(CSTR("*.zip"), CSTR("ZIP File"));
		dlg->AddFilter(CSTR("*.otrk2.xml"), CSTR("Orux Map Tile"));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			me->SaveTilesFile(dlg->GetFileName()->ToCString(), dlg->GetFilterIndex(), minLevel, maxLevel);
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::OnStopClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISTileDownloadForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISTileDownloadForm>();
	me->stopDownload = true;
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesDir(Text::CStringNN folderName, UIntOS userMinLevel, UIntOS userMaxLevel)
{
	NN<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UIntOS tileMinLevel = tileMap->GetMinLevel();
	UIntOS tileMaxLevel = tileMap->GetMaxLevel();
	if (userMinLevel > tileMaxLevel || userMaxLevel < tileMinLevel)
		return;
	if (userMinLevel < tileMinLevel)
		userMinLevel = tileMinLevel;
	if (userMaxLevel > tileMaxLevel)
		userMaxLevel = tileMaxLevel;
	Map::TileMapFolderWriter writer(folderName, tileMap->GetImageType(), userMinLevel, userMaxLevel, Math::RectAreaDbl(this->sel1, this->sel2));
	WriteTiles(writer, userMinLevel, userMaxLevel);
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::SaveTilesFile(Text::CStringNN fileName, UIntOS fileType, UIntOS userMinLevel, UIntOS userMaxLevel)
{
	NN<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UIntOS tileMinLevel = tileMap->GetMinLevel();
	UIntOS tileMaxLevel = tileMap->GetMaxLevel();
	if (userMinLevel > tileMaxLevel || userMaxLevel < tileMinLevel)
		return;
	if (userMinLevel < tileMinLevel)
		userMinLevel = tileMinLevel;
	if (userMaxLevel > tileMaxLevel)
		userMaxLevel = tileMaxLevel;
	NN<Map::TileMapWriter> writer;
	if (fileType == 1)
	{
		NEW_CLASSNN(writer, Map::TileMapZipWriter(fileName, tileMap->GetImageType(), userMinLevel, userMaxLevel, Math::RectAreaDbl(this->sel1, this->sel2)));
	}
	else if (fileType == 2)
	{
		NEW_CLASSNN(writer, Map::TileMapOruxWriter(fileName, userMinLevel, userMaxLevel, Math::RectAreaDbl(this->sel1, this->sel2)));
	}
	else
	{
		NEW_CLASSNN(writer, Map::TileMapSPKWriter(fileName));
	}
	WriteTiles(writer, userMinLevel, userMaxLevel);
	writer.Delete();
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::WriteTiles(NN<Map::TileMapWriter> writer, UIntOS userMinLevel, UIntOS userMaxLevel)
{
	NN<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UIntOS currLyr;
	Data::ArrayListT<Math::Coord2D<Int32>> imgIdList;
	UIntOS i;
	UIntOS j;
	UIntOS cnt;
	IntOS err = 0;
	Bool found;
	this->errCnt = 0;
	this->txtError->SetText(CSTR("0"));
	this->stopDownload = false;

	currLyr = userMinLevel;
	while (currLyr <= userMaxLevel)
	{
		sptr = Text::StrUIntOS(sbuff, currLyr);
		this->txtLayer->SetText(CSTRP(sbuff, sptr));
		writer->BeginLevel(currLyr);

		imgIdList.Clear();
		tileMap->GetTileImageIDs(currLyr, Math::RectAreaDbl(this->sel1, this->sel2), imgIdList);
		cnt = imgIdList.GetCount();
		i = cnt;
		while (i-- > 0)
		{
			sptr = Text::StrUIntOS(Text::StrConcatC(Text::StrUIntOS(sbuff, cnt - i), UTF8STRC("/")), cnt);
			this->txtImages->SetText(CSTRP(sbuff, sptr));
			this->ui->ProcessMessages();
			Math::Coord2D<Int32> imageId = imgIdList.GetItem(i);
			writer->AddX(imageId.x);

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
						this->threadStat[j].writer = writer.Ptr();
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
				sptr = Text::StrIntOS(sbuff, err);
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

Bool SSWR::AVIRead::AVIRGISTileDownloadForm::GetLevels(OutParam<UIntOS> minLevel, OutParam<UIntOS> maxLevel)
{
	NN<Map::TileMap> tileMap = this->lyr->GetTileMap();
	UInt32 userMinLevel;
	UInt32 userMaxLevel;
	Text::StringBuilderUTF8 sb;
	this->txtMinLevel->GetText(sb);
	if (!sb.ToUInt32(userMinLevel))
	{
		this->ui->ShowMsgOK(CSTR("Min Level invalid"), CSTR("Tile Downloader"), this);
		return false;
	}
	sb.ClearStr();
	this->txtMaxLevel->GetText(sb);
	if (!sb.ToUInt32(userMaxLevel))
	{
		this->ui->ShowMsgOK(CSTR("Max Level invalid"), CSTR("Tile Downloader"), this);
		return false;
	}
	if (userMinLevel > tileMap->GetMaxLevel() || userMaxLevel < tileMap->GetMinLevel())
	{
		this->ui->ShowMsgOK(CSTR("Input level is not within tile map range"), CSTR("Tile Downloader"), this);
		return false;
	}

	minLevel.Set(userMinLevel);
	maxLevel.Set(userMaxLevel);
	return true;
}

UInt32 __stdcall SSWR::AVIRead::AVIRGISTileDownloadForm::ProcThread(AnyType userObj)
{
	NN<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	Map::TileMap::ImageType it;
	IO::StreamData *fd;
	Math::RectAreaDbl bounds;
	UInt64 fileSize;
	IntOS j;

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
						fileBuff.ChangeSizeAndClear((UIntOS)fileSize);
					}
					if (fd->GetRealData(0, (UIntOS)fileSize, fileBuff) == fileSize)
					{
						NN<Map::TileMapWriter> writer;
						if (stat->writer.SetTo(writer))
						{
							writer->AddImage(stat->lyrId, stat->imageId.x, stat->imageId.y, fileBuff.SubArray(0, (UIntOS)fileSize), it);
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

SSWR::AVIRead::AVIRGISTileDownloadForm::AVIRGISTileDownloadForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::TileMapLayer> lyr, NN<AVIRMapNavigator> navi) : UI::GUIForm(parent, 360, 216, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Tile Downloader - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->selecting = false;
	this->isDown = false;
	this->downPos = Math::Coord2D<IntOS>(0, 0);
	this->sel1 = Math::Coord2DDbl(0, 0);
	this->sel2 = Math::Coord2DDbl(0, 0);
	this->stopDownload = false;

	this->lblMinLevel = ui->NewLabel(*this, CSTR("Min. Level"));
	this->lblMinLevel->SetRect(4, 4, 100, 23, false);
	this->txtMinLevel = ui->NewTextBox(*this, CSTR(""));
	this->txtMinLevel->SetRect(104, 4, 60, 23, false);
	this->lblMaxLevel = ui->NewLabel(*this, CSTR("Min. Level"));
	this->lblMaxLevel->SetRect(4, 28, 100, 23, false);
	this->txtMaxLevel = ui->NewTextBox(*this, CSTR(""));
	this->txtMaxLevel->SetRect(104, 28, 60, 23, false);
	this->btnArea = ui->NewButton(*this, CSTR("Select Area"));
	this->btnArea->SetRect(4, 52, 100, 23, false);
	this->btnArea->HandleButtonClick(OnAreaClicked, this);
	this->btnSave = ui->NewButton(*this, CSTR("Save As"));
	this->btnSave->SetRect(114, 52, 100, 23, false);
	this->btnSave->HandleButtonClick(OnSaveFileClicked, this);
	this->btnStop = ui->NewButton(*this, CSTR("Stop Downloading"));
	this->btnStop->SetRect(224, 52, 100, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);

	this->lblTotalImages = ui->NewLabel(*this, CSTR("Total Images"));
	this->lblTotalImages->SetRect(4, 84, 100, 23, false);
	this->txtTotalImages = ui->NewTextBox(*this, CSTR(""));
	this->txtTotalImages->SetReadOnly(true);
	this->txtTotalImages->SetRect(104, 84, 60, 23, false);
	this->lblLayer = ui->NewLabel(*this, CSTR("Curr Layer"));
	this->lblLayer->SetRect(4, 108, 100, 23, false);
	this->txtLayer = ui->NewTextBox(*this, CSTR(""));
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetRect(104, 108, 60, 23, false);
	this->lblImages = ui->NewLabel(*this, CSTR("Images"));
	this->lblImages->SetRect(4, 132, 100, 23, false);
	this->txtImages = ui->NewTextBox(*this, CSTR(""));
	this->txtImages->SetReadOnly(true);
	this->txtImages->SetRect(104, 132, 100, 23, false);
	this->lblError = ui->NewLabel(*this, CSTR("Error"));
	this->lblError->SetRect(4, 156, 100, 23, false);
	this->txtError = ui->NewTextBox(*this, CSTR(""));
	this->txtError->SetReadOnly(true);
	this->txtError->SetRect(104, 156, 100, 23, false);

	sb.ClearStr();
	sb.AppendUIntOS(this->lyr->GetTileMap()->GetMinLevel());
	this->txtMinLevel->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendUIntOS(this->lyr->GetTileMap()->GetMaxLevel());
	this->txtMaxLevel->SetText(sb.ToCString());

	this->navi->HandleMapMouseLDown(OnMouseDown, this);
	this->navi->HandleMapMouseLUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);

	UIntOS i;
	this->threadCnt = this->lyr->GetTileMap()->GetConcurrentCount();
	if (this->threadCnt <= 0)
	{
		this->threadCnt = 1;
	}
	this->threadStat = MemAllocArr(ThreadStat, this->threadCnt);
	i = 0;
	while (i < this->threadCnt)
	{
		this->threadStat[i].me = *this;
		NEW_CLASSNN(this->threadStat[i].threadEvt, Sync::Event(true));
		this->threadStat[i].threadStat = 0;
		this->threadStat[i].lyrId = 0;
		this->threadStat[i].imageId = 0;
		this->threadStat[i].writer = nullptr;
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

	UIntOS i;
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
		this->threadStat[i].threadEvt.Delete();
	}
	MemFreeArr(this->threadStat);
}

void SSWR::AVIRead::AVIRGISTileDownloadForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
