#include "Stdafx.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/StreamReader.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "IO/StmData/FileData.h"
#include "Map/DrawMapRenderer.h"
#include "Map/VectorLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Math.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/UTMGridConvertDbl.h"
#include "Math/VectorImage.h"
#include "Media/SharedImage.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebBrowser.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRGISCombineForm.h"
#include "SSWR/AVIRead/AVIRGISCSysForm.h"
#include "SSWR/AVIRead/AVIRGISDistanceForm.h"
#include "SSWR/AVIRead/AVIRGISEditImageForm.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGISGroupQueryForm.h"
#include "SSWR/AVIRead/AVIRGISHKTDTonnesForm.h"
#include "SSWR/AVIRead/AVIRGISHKTrafficForm.h"
#include "SSWR/AVIRead/AVIRGISPropForm.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "SSWR/AVIRead/AVIRGISRandomLocForm.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "SSWR/AVIRead/AVIRGISSearchForm.h"
#include "SSWR/AVIRead/AVIRGPSSimulatorForm.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "SSWR/AVIRead/AVIRGooglePolylineForm.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

typedef enum
{
	MNU_SAVE = 101,
	MNU_COPY_LATLON,
	MNU_PRINT,
	MNU_GROUP_ADD,
	MNU_GROUP_REMOVE,
	MNU_GROUP_NEW_IMAGE,
	MNU_GROUP_NEW_POINT,
	MNU_GROUP_NEW_POLYLINE,
	MNU_GROUP_NEW_POLYGON,
	MNU_GROUP_CENTER,
	MNU_GROUP_QUERY,
	MNU_LAYER_ADD,
	MNU_LAYER_REMOVE,
	MNU_LAYER_CENTER,
	MNU_LAYER_PROP,
	MNU_LAYER_PATH,
	MNU_LAYER_SEARCH,
	MNU_LAYER_SAVE,
	MNU_LAYER_COMBINE,
	MNU_LAYER_REPLAY,
	MNU_LAYER_CONV,
	MNU_LAYER_EDIT,
	MNU_LAYER_CONV_CSYS,
	MNU_LAYER_OPENDB,
	MNU_LAYER_ASSIGN_CSYS,
	MNU_LAYER_EXPAND,
	MNU_LAYER_TILEDOWN,
	MNU_LAYER_IMPORT_TILES,
	MNU_LAYER_OPTIMIZE_FILE,
	MNU_LAYER_QUERY,
	MNU_MTK_GPS,
	MNU_MTK_FILE,
	MNU_GPS_TRACKER,
	MNU_MTKGPS_TRACKER,
	MNU_GOOGLE_POLYLINE,
	MNU_OPEN_FILE,
	MNU_HKO_RADAR_64,
	MNU_HKO_RADAR_128,
	MNU_HKO_RADAR_256,
	MNU_HKO_CYCLONE,
	MNU_HKO_CURR_RADAR_64,
	MNU_HKO_CURR_RADAR_128,
	MNU_HKO_CURR_RADAR_256,
	MNU_HKO_CURR_SATELLITE,
	MNU_HKO_CURR_SATELLITE_IR1_L1B_10,
	MNU_HKO_CURR_SATELLITE_DC_10,
	MNU_HKO_CURR_SAND,
	MNU_HKO_CURR_WEATHER_CHART,
	MNU_GPS_SIMULATOR,
	MNU_DISTANCE,
	MNU_HKTD_TRAFFIC,
	MNU_HKTD_TONNES_SIGN,
	MNU_RANDOMLOC
} MenuItems;

#define MAX_SCALE 200000000
#define MIN_SCALE 400

void __stdcall SSWR::AVIRead::AVIRGISForm::FileHandler(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	Parser::ParserList *parsers = me->core->GetParserList();
	IO::StmData::FileData *fd;
	IO::ParsedObject *pobj;
	IO::ParserType pt;
	Data::ArrayList<Map::IMapDrawLayer *> *layers;
	OSInt mousePosX;
	OSInt mousePosY;
	OSInt scnX;
	OSInt scnY;
	UOSInt w;
	UOSInt h;
	me->ui->GetCursorPos(&mousePosX, &mousePosY);
	me->mapCtrl->GetScreenPosP(&scnX, &scnY);
	mousePosX = mousePosX - scnX;
	mousePosY = mousePosY - scnY;
	me->mapCtrl->GetSizeP(&w, &h);
	if (mousePosX >= 0 && mousePosY >= 0 && mousePosX < (OSInt)w && mousePosY < (OSInt)h)
	{
	}
	else
	{
		mousePosX = (OSInt)(w >> 1);
		mousePosY = (OSInt)(h >> 1);
	}

	NEW_CLASS(layers, Data::ArrayList<Map::IMapDrawLayer*>());
	UOSInt i = 0;
	while (i < nFiles)
	{
		UOSInt fileNameLen = Text::StrCharCnt(files[i]);
		IO::Path::PathType pathType = IO::Path::GetPathType(files[i], fileNameLen);
		pobj = 0;
		if (pathType == IO::Path::PathType::File)
		{
			NEW_CLASS(fd, IO::StmData::FileData({files[i], fileNameLen}, false));
			pobj = parsers->ParseFile(fd, &pt);
			DEL_CLASS(fd);
		}
		else if (pathType == IO::Path::PathType::Directory)
		{
			IO::DirectoryPackage *dpkg;
			NEW_CLASS(dpkg, IO::DirectoryPackage({files[i], fileNameLen}));
			pobj = parsers->ParseObject(dpkg, &pt);
			DEL_CLASS(dpkg);
		}
		if (pobj)
		{
			if (pt == IO::ParserType::MapLayer)
			{
				layers->Add((Map::IMapDrawLayer*)pobj);
			}
			else if (pt == IO::ParserType::MapEnv)
			{
				////////////////////////////
				DEL_CLASS(pobj);
			}
			else if (pt == IO::ParserType::ImageList)
			{
				Double x1;
				Double y1;
				Double x2;
				Double y2;
				Map::VectorLayer *lyr;
				Media::SharedImage *simg;
				Math::VectorImage *vimg;
				Media::Image *stimg;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, {files[i], fileNameLen}, 0, 0, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, 0, 0, 0, 0));
				stimg = ((Media::ImageList*)pobj)->GetImage(0, 0);
				Double calcImgW;
				Double calcImgH;
				if (stimg->HasHotSpot())
				{
					Double hsX;
					Double hsY;
					if (stimg->info->par2 > 1)
					{
						hsX = OSInt2Double(stimg->GetHotSpotX());
						hsY = OSInt2Double(stimg->GetHotSpotY()) * stimg->info->par2;
						calcImgW = UOSInt2Double(stimg->info->dispWidth);
						calcImgH = UOSInt2Double(stimg->info->dispHeight) * stimg->info->par2;
					}
					else
					{
						hsX = OSInt2Double(stimg->GetHotSpotX()) / stimg->info->par2;
						hsY = OSInt2Double(stimg->GetHotSpotY());
						calcImgW = UOSInt2Double(stimg->info->dispWidth) / stimg->info->par2;
						calcImgH = UOSInt2Double(stimg->info->dispHeight);
					}
					me->mapCtrl->ScnXYD2MapXY(OSInt2Double(mousePosX) - hsX, OSInt2Double(mousePosY) - hsY, &x1, &y1);
					me->mapCtrl->ScnXYD2MapXY(OSInt2Double(mousePosX) + calcImgW - hsX, OSInt2Double(mousePosY) + calcImgH - hsY, &x2, &y2);
				}
				else
				{
					if (stimg->info->par2 > 1)
					{
						calcImgW = UOSInt2Double(stimg->info->dispWidth);
						calcImgH = UOSInt2Double(stimg->info->dispHeight) * stimg->info->par2;
					}
					else
					{
						calcImgW = UOSInt2Double(stimg->info->dispWidth) / stimg->info->par2;
						calcImgH = UOSInt2Double(stimg->info->dispHeight);
					}
					me->mapCtrl->ScnXYD2MapXY(OSInt2Double(mousePosX) - calcImgW * 0.5, OSInt2Double(mousePosY) - calcImgH * 0.5, &x1, &y1);
					me->mapCtrl->ScnXYD2MapXY(OSInt2Double(mousePosX) + calcImgW * 0.5, OSInt2Double(mousePosY) + calcImgH * 0.5, &x2, &y2);
				}
				NEW_CLASS(simg, Media::SharedImage((Media::ImageList*)pobj, true));
				NEW_CLASS(vimg, Math::VectorImage(me->env->GetSRID(), simg, x1, y1, x2, y2, x2 - x1, y1 - y2, false, files[i], 0, 0));
				DEL_CLASS(simg);
				lyr->AddVector(vimg, (const UTF8Char**)0);
				layers->Add(lyr);
			}
			else
			{
				if (me->ParseObject(pobj))
				{
					DEL_CLASS(pobj);
				}
				else
				{
					me->core->OpenObject(pobj);
				}
			}
		}
		i++;
	}
	if (layers->GetCount() > 0)
	{
		me->AddLayers(layers);
	}
	DEL_CLASS(layers);
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseMove(void *userObj, OSInt x, OSInt y)
{
	UTF8Char sbuff[64];
	Double mapX;
	Double mapY;
	Double lat;
	Double lon;
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	me->mapCtrl->ScnXY2MapXY(x, y, &mapX, &mapY);

	Text::StrDouble(Text::StrConcatC(Text::StrDouble(sbuff, mapX), UTF8STRC(", ")), mapY);
	me->txtLatLon->SetText(sbuff);
	
	Math::CoordinateSystem *csys = me->env->GetCoordinateSystem();
	if (csys)
	{
		Math::CoordinateSystem::ConvertXYZ(csys, me->wgs84CSys, mapX, mapY, 0, &lon, &lat, 0);
	}
	else
	{
		lat = mapY;
		lon = mapX;
	}

	Math::UTMGridConvertDbl conv;
	conv.WGS84_Grid(sbuff, 5, 0, 0, 0, 0, lat, lon);
	me->txtUTMGrid->SetText(sbuff);

	UOSInt i;
	i = me->mouseMoveHdlrs->GetCount();
	while (i-- > 0)
	{
		me->mouseMoveHdlrs->GetItem(i)(me->mouseMoveObjs->GetItem(i), x, y);
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseDown(void *userObj, OSInt x, OSInt y, MouseButton button)
{
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	Bool ret = false;
	UOSInt i;
	i = me->mouseDownHdlrs->GetCount();
	while (i-- > 0)
	{
		ret = me->mouseDownHdlrs->GetItem(i)(me->mouseDownObjs->GetItem(i), x, y);
		if (ret)
			return true;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseUp(void *userObj, OSInt x, OSInt y, MouseButton button)
{
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	Bool ret = false;
	UOSInt i;
	i = me->mouseUpHdlrs->GetCount();
	while (i-- > 0)
	{
		ret = me->mouseUpHdlrs->GetItem(i)(me->mouseUpObjs->GetItem(i), x, y);
		if (ret)
			return true;
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapScaleChanged(void *userObj, Double newScale)
{
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	UTF8Char sbuff[32];
	if (me->scaleChanging)
	{
		Text::StrDoubleFmt(Text::StrConcatC(sbuff, UTF8STRC("1:")), me->mapCtrl->GetViewScale(), "0.#");
		me->txtScale->SetText(sbuff);
		return;
	}
	me->scaleChanging = true;
	me->tbScale->SetPos((UOSInt)Double2OSInt(Math_Log10(newScale / MIN_SCALE) * 65536.0 / Math_Log10(MAX_SCALE / (Double)MIN_SCALE)));
	me->scaleChanging = false;
	Text::StrDoubleFmt(Text::StrConcatC(sbuff, UTF8STRC("1:")), me->mapCtrl->GetViewScale(), "0.#");
	me->txtScale->SetText(sbuff);
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapUpdated(void *userObj, Double centerX, Double centerY, Double timeUsed)
{
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	me->mapUpdT = timeUsed;
	me->mapUpdTChanged = true;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnScaleScrolled(void *userObj, UOSInt newVal)
{
	AVIRead::AVIRGISForm *me = (AVIRead::AVIRGISForm*)userObj;
	if (me->scaleChanging)
		return;
	me->scaleChanging = true;
	Int32 scale = Double2Int32(Math_Pow(10, Math_Log10((MAX_SCALE / (Double)MIN_SCALE)) * UOSInt2Double(newVal) / 65536.0) * MIN_SCALE);
	me->mapCtrl->SetMapScale(scale);
	me->scaleChanging = false;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTreeRightClick(void *userObj)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	UI::GUITreeView::TreeItem *item = me->mapTree->GetHighlightItem();
	if (item)
	{
		OSInt x;
		OSInt y;
		me->ui->GetCursorPos(&x, &y);

		UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)item->GetItemObj();
		if (ind->itemType == Map::MapEnv::IT_LAYER)
		{
			Bool canImport = false;
			if (((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
			{
				Map::TileMapLayer *layer = (Map::TileMapLayer*)((Map::MapEnv::LayerItem*)ind->item)->layer;
				Map::TileMap *tileMap = layer->GetTileMap();
				if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
				{
					Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
					canImport = osm->HasSPackageFile();
				}
			}
			me->popNode = item;
			me->mnuLayer->SetItemEnabled(MNU_LAYER_REPLAY, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_GPS_TRACK);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_CONV, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() != Map::IMapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_EDIT, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_CONV_CSYS, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_EXPAND, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_MAP_LAYER_COLL);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_TILEDOWN, ((Map::MapEnv::LayerItem*)ind->item)->layer->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_IMPORT_TILES, canImport);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_OPTIMIZE_FILE, canImport);
			me->mnuLayer->ShowMenu(me, x, y);
		}
		else if (ind->itemType == Map::MapEnv::IT_GROUP)
		{
			me->popNode = item;
			me->mnuGroup->SetItemEnabled(MNU_GROUP_REMOVE, (ind->item != 0));
			me->mnuGroup->ShowMenu(me, x, y);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnCtrlFormClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	me->ctrlForm = 0;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnSubFormClosed(void *userObj, UI::GUIForm *frm)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	me->subForms->RemoveAt(me->subForms->IndexOf(frm));
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapLayerUpdated(void *userObj)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	me->mapLyrUpdated = true;
	me->mapCtrl->UpdateMap();
	me->mapCtrl->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimeScrolled(void *userObj, UOSInt newVal)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	if (me->useTime)
	{
		me->currTime = (OSInt)newVal + me->timeRangeStart;
		me->env->SetCurrTimeTS(0, me->currTime);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimeChecked(void *userObj, Bool newState)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	if (newState != me->useTime)
	{
		me->useTime = newState;
		if (me->useTime)
		{
			me->env->SetCurrTimeTS(0, me->currTime);
		}
		else
		{
			me->env->SetCurrTimeTS(0, 0);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTreeDrag(void *userObj, UI::GUIMapTreeView::ItemIndex *dragItem, UI::GUIMapTreeView::ItemIndex *dropItem)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	if (dragItem->group == 0 && dragItem->index == (UOSInt)-1)
		return;
	if (dropItem->itemType == Map::MapEnv::IT_GROUP)
	{
		me->env->MoveItem(dragItem->group, dragItem->index, (Map::MapEnv::GroupItem*)dropItem->item, me->env->GetItemCount((Map::MapEnv::GroupItem*)dropItem->item));
	}
	else
	{
		me->env->MoveItem(dragItem->group, dragItem->index, dropItem->group, dropItem->index);
	}
	me->mapTree->UpdateTree();
	me->mapCtrl->UpdateMap();
	me->mapCtrl->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRGISForm *me = (SSWR::AVIRead::AVIRGISForm*)userObj;
	if (me->mapUpdTChanged)
	{
		me->mapUpdTChanged = false;
		UTF8Char sbuff[64];
		Text::StrDouble(sbuff, me->mapUpdT);
		me->txtTimeUsed->SetText(sbuff);
	}
	if (me->mapLyrUpdated)
	{
		me->mapLyrUpdated = false;
		me->UpdateTimeRange();
	}
}

void SSWR::AVIRead::AVIRGISForm::UpdateTitle()
{
	UTF8Char sbuff[512];
	this->env->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("GISForm - ")));
	this->SetText(sbuff);
}

void SSWR::AVIRead::AVIRGISForm::CloseCtrlForm()
{
	if (this->ctrlForm)
	{
		this->ctrlForm->Close();
		this->HideMarker();
		this->SetSelectedVector(0);
	}
}

void SSWR::AVIRead::AVIRGISForm::SetCtrlForm(UI::GUIForm *frm, UI::GUITreeView::TreeItem *item)
{
	this->CloseCtrlForm();
	this->ctrlItem = item;
	this->ctrlForm = frm;
	this->ctrlForm->HandleFormClosed(OnCtrlFormClosed, this);
	this->ctrlForm->Show();
}

Bool SSWR::AVIRead::AVIRGISForm::ParseObject(IO::ParsedObject *pobj)
{
	Parser::ParserList *parsers = this->core->GetParserList();
	IO::ParsedObject *npobj;
	IO::ParserType pt;
	npobj = parsers->ParseObject(pobj, &pt);
	if (npobj)
	{
		if (pt == IO::ParserType::MapLayer)
		{
			this->AddLayer((Map::IMapDrawLayer*)npobj);;
			return true;
		}
		else
		{
			Bool succ = ParseObject(npobj);
			DEL_CLASS(npobj);
			return succ;
		}
	}
	return false;
}

void SSWR::AVIRead::AVIRGISForm::OpenURL(const UTF8Char *url, UOSInt urlLen, const UTF8Char *customName)
{
	Net::WebBrowser *browser = this->core->GetWebBrowser();
	IO::IStreamData *fd = browser->GetData(url, urlLen, false, 0);
	if (fd)
	{
		if (customName)
		{
			fd->SetFullName(customName);
		}
		IO::ParserType pt;
		IO::ParsedObject *pobj = this->core->GetParserList()->ParseFile(fd, &pt);
		DEL_CLASS(fd);
		if (pobj)
		{
			if (pt == IO::ParserType::MapLayer)
			{
				this->AddLayer((Map::IMapDrawLayer*)pobj);
			}
			else
			{
				this->core->OpenObject(pobj);
			}
		}
	}
}

void SSWR::AVIRead::AVIRGISForm::HKOPortal(const UTF8Char *listFile, const UTF8Char *filePath)
{
	Data::DateTime dt;
	UTF8Char sbuff[512];
	UTF8Char *sarr[3];
	UTF8Char *dateStr;
	UTF8Char *timeStr;
	Text::StringBuilderUTF8 sb;
	Net::HTTPClient *cli;
	Text::UTF8Reader *reader;
	dt.SetCurrTimeUTC();
	sb.AppendC(UTF8STRC("https://maps.weather.gov.hk/gis-portal/web/data/dirList/"));
	sb.AppendSlow(listFile);
	sb.AppendC(UTF8STRC("?t="));
	sb.AppendI64(dt.ToTicks());
	cli = Net::HTTPClient::CreateConnect(this->core->GetSocketFactory(), this->ssl, sb.ToString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	NEW_CLASS(reader, Text::UTF8Reader(cli));
	dateStr = 0;
	timeStr = 0;
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 4096))
			break;
		if (sb.GetLength() < 30 && Text::StrSplit(sarr, 3, sb.ToString(), ',') == 2)
		{
			dateStr = sbuff;
			timeStr = Text::StrConcat(dateStr, sarr[0]) + 1;
			Text::StrConcat(timeStr, sarr[1]);
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(cli);
	if (dateStr && timeStr)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://maps.weather.gov.hk/gis-portal/web/data/"));
		sb.AppendSlow(dateStr);
		sb.AppendSlow(filePath);
		sb.AppendSlow(timeStr);
		sb.AppendC(UTF8STRC("/index.kml?t="));
		sb.AppendI64(dt.ToTicks());
		this->OpenURL(sb.ToString(), sb.GetLength(), (const UTF8Char*)"https://maps.weather.gov.hk/gis-portal/web/index.kml");
	}
}

SSWR::AVIRead::AVIRGISForm::AVIRGISForm(UI::GUIClientControl *parent, UI::GUICore *ui, AVIRead::AVIRCore *core, Map::MapEnv *env, Map::MapView *view) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	this->env = env;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->scaleChanging = false;
	this->ctrlForm = 0;
	this->ctrlItem = 0;
	this->timeRangeStart = 0;
	this->timeRangeEnd = 0;
	this->useTime = false;
	this->currTime = 0;
	this->printer = 0;
	this->currCursor = UI::GUIControl::CT_ARROW;
	NEW_CLASS(this->mouseDownHdlrs, Data::ArrayList<MouseEvent>());
	NEW_CLASS(this->mouseDownObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->mouseUpHdlrs, Data::ArrayList<MouseEvent>());
	NEW_CLASS(this->mouseUpObjs, Data::ArrayList<void*>());
	NEW_CLASS(this->mouseMoveHdlrs, Data::ArrayList<MouseEvent>());
	NEW_CLASS(this->mouseMoveObjs, Data::ArrayList<void*>());
	this->mapUpdTChanged = false;
	this->mapUpdT = 0;
	this->pauseUpdate = false;
	this->mapLyrUpdated = false;

	this->wgs84CSys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	NEW_CLASS(this->ta, Text::TextAnalyzer());
	NEW_CLASS(this->subForms, Data::ArrayList<UI::GUIForm*>());
	UpdateTitle();
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetArea(0, 0, 100, 24, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this));
	this->pnlStatus->SetArea(0, 0, 100, 19, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->txtScale, UI::GUITextBox(ui, pnlStatus, (const UTF8Char*)""));
	this->txtScale->SetRect(0, 0, 100, 19, false);
	this->txtScale->SetReadOnly(true);
	this->txtScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->txtLatLon, UI::GUITextBox(ui, pnlStatus, (const UTF8Char*)""));
	this->txtLatLon->SetRect(0, 0, 250, 19, false);
	this->txtLatLon->SetReadOnly(true);
	this->txtLatLon->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->txtUTMGrid, UI::GUITextBox(ui, pnlStatus, (const UTF8Char*)""));
	this->txtUTMGrid->SetRect(0, 0, 150, 19, false);
	this->txtUTMGrid->SetReadOnly(true);
	this->txtUTMGrid->SetDockType(UI::GUIControl::DOCK_RIGHT);
	NEW_CLASS(this->txtTimeUsed, UI::GUITextBox(ui, pnlStatus, (const UTF8Char*)""));
	this->txtTimeUsed->SetRect(0, 0, 100, 19, false);
	this->txtTimeUsed->SetReadOnly(true);
	this->txtTimeUsed->SetDockType(UI::GUIControl::DOCK_RIGHT);

	NEW_CLASS(this->tbScale, UI::GUITrackBar(ui, this->pnlControl, 0, 65535, 0));
	this->tbScale->SetRect(0, 0, 192, 24, false);
	this->tbScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->tbScale->HandleScrolled(OnScaleScrolled, this);
	NEW_CLASS(this->tbTimeRange, UI::GUITrackBar(ui, this->pnlControl, 0, 0, 0));
	this->tbTimeRange->SetRect(150, 0, 100, 24, false);
	this->tbTimeRange->HandleScrolled(OnTimeScrolled, this);
	NEW_CLASS(this->chkTime, UI::GUICheckBox(ui, this->pnlControl, (const UTF8Char*)"Unavailable", false));
	this->chkTime->SetEnabled(false);
	this->chkTime->SetRect(0, 0, 150, 24, false);
	this->chkTime->HandleCheckedChange(OnTimeChecked, this);

	NEW_CLASS(this->mapTree, UI::GUIMapTreeView(ui, this, env));
	this->mapTree->SetRect(0, 0, 200, 10, false);
	this->mapTree->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->mapTree->HandleRightClick(OnTreeRightClick, this);
	this->mapTree->SetDragHandler(OnTreeDrag, this);
	NEW_CLASS(this->splitter, UI::GUIHSplitter(ui, this, 3, false));
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->envRenderer, Map::DrawMapRenderer(core->GetDrawEngine(), env, &color, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	NEW_CLASS(this->mapCtrl, UI::GUIMapControl(ui, this, this->core->GetDrawEngine(), env->GetBGColor(), this->envRenderer, view, this->colorSess));
	this->mapCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapCtrl->HandleScaleChanged(OnMapScaleChanged, this);
	this->mapCtrl->HandleMouseMove(OnMapMouseMove, this);
	this->mapCtrl->HandleMouseDown(OnMapMouseDown, this);
	this->mapCtrl->HandleMouseUp(OnMapMouseUp, this);
	this->mapCtrl->HandleMapUpdated(OnMapUpdated, this);

	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	UI::GUIMenu *mnu = this->mnuMain->AddSubMenu(CSTR("&Map"));
	mnu->AddItem(CSTR("&Save"), MNU_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("&Copy Lat/Lon"), MNU_COPY_LATLON, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);
	mnu->AddItem(CSTR("&Print..."), MNU_PRINT, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_P);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Add Layer"));
	UI::GUIMenu *mnu2 = mnu->AddSubMenu(CSTR("&MTK GPS Tracker"));
	mnu2->AddItem(CSTR("From &Device"), MNU_MTK_GPS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("From &File"), MNU_MTK_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("From Google Polyline String"), MNU_GOOGLE_POLYLINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("From &File"), MNU_OPEN_FILE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu2 = mnu->AddSubMenu(CSTR("HKO"));
	UI::GUIMenu *mnu3 = mnu2->AddSubMenu(CSTR("Radar"));
	mnu3->AddItem(CSTR("64km"), MNU_HKO_RADAR_64, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("128km"), MNU_HKO_RADAR_128, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("256km"), MNU_HKO_RADAR_256, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Tropical Cyclone"), MNU_HKO_CYCLONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("Current"));
	mnu3->AddItem(CSTR("Radar 64km"), MNU_HKO_CURR_RADAR_64, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Radar 128km"), MNU_HKO_CURR_RADAR_128, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Radar 256km"), MNU_HKO_CURR_RADAR_256, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Satellite"), MNU_HKO_CURR_SATELLITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Satellite (IR1-L1B-10)"), MNU_HKO_CURR_SATELLITE_IR1_L1B_10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Satellite (DC-10)"), MNU_HKO_CURR_SATELLITE_DC_10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Sand"), MNU_HKO_CURR_SAND, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Weather Chart"), MNU_HKO_CURR_WEATHER_CHART, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2 = mnu->AddSubMenu(CSTR("HKTD"));
	mnu2->AddItem(CSTR("Traffic"), MNU_HKTD_TRAFFIC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Tonnes Sign"), MNU_HKTD_TONNES_SIGN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Device"));
	mnu->AddItem(CSTR("GPS Tracker"), MNU_GPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("MTK GPS Tracker"), MNU_MTKGPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Tools"));
	mnu->AddItem(CSTR("GPS Simulator"), MNU_GPS_SIMULATOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Distance Tool"), MNU_DISTANCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Random Loc"), MNU_RANDOMLOC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->mnuLayer, UI::GUIPopupMenu());
	this->mnuLayer->SetMenuForm(this);
	this->mnuLayer->AddItem(CSTR("&Add Group"), MNU_LAYER_ADD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("&Remove"), MNU_LAYER_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Make Center"), MNU_LAYER_CENTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Properties"), MNU_LAYER_PROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Show Full Path"), MNU_LAYER_PATH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Search"), MNU_LAYER_SEARCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Assign Coord Sys"), MNU_LAYER_ASSIGN_CSYS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Convert to Coord Sys"), MNU_LAYER_CONV_CSYS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddSeperator();
	this->mnuLayer->AddItem(CSTR("&Save"), MNU_LAYER_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Combine Layer"), MNU_LAYER_COMBINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Expand Layer"), MNU_LAYER_EXPAND, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Convert to editable layer"), MNU_LAYER_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Edit Layer"), MNU_LAYER_EDIT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddSeperator();
	this->mnuLayer->AddItem(CSTR("Replay"), MNU_LAYER_REPLAY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Open DB"), MNU_LAYER_OPENDB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Tile Downloader"), MNU_LAYER_TILEDOWN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Import Tiles"), MNU_LAYER_IMPORT_TILES, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Optimize File"), MNU_LAYER_OPTIMIZE_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Query"), MNU_LAYER_QUERY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuGroup, UI::GUIPopupMenu());
	this->mnuGroup->AddItem(CSTR("&Add Group"), MNU_GROUP_ADD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuGroup->AddItem(CSTR("&Remove"), MNU_GROUP_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuGroup->SetMenuForm(this);
	mnu = this->mnuGroup->AddSubMenu(CSTR("&New Layer"));
	mnu->AddItem(CSTR("&Point Layer"), MNU_GROUP_NEW_POINT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Poly&line Layer"), MNU_GROUP_NEW_POLYLINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Poly&gon Layer"), MNU_GROUP_NEW_POLYGON, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("&Image Layer"), MNU_GROUP_NEW_IMAGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuGroup->AddItem(CSTR("Make &Center"), MNU_GROUP_CENTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuGroup->AddItem(CSTR("Query"), MNU_GROUP_QUERY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	this->HandleDropFiles(FileHandler, this);
	this->SetFormState(FS_MAXIMIZED);
	this->env->AddUpdatedHandler(OnMapLayerUpdated, this);
	this->AddTimer(1000, OnTimerTick, this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->OnMapScaleChanged(this, this->mapCtrl->GetMapScale());
	this->mapCtrl->Focus();
	
	this->core->SetGISForm(this);
}

SSWR::AVIRead::AVIRGISForm::~AVIRGISForm()
{
	//this->mapCtrl->SetRenderer(0);
	UOSInt i;
	this->pauseUpdate = true;
	this->CloseCtrlForm();
	i = this->subForms->GetCount();
	while (i-- > 0)
	{
		this->subForms->GetItem(i)->Close();
	}
	DEL_CLASS(this->subForms);
	DEL_CLASS(this->mouseDownHdlrs);
	DEL_CLASS(this->mouseDownObjs);
	DEL_CLASS(this->mouseUpHdlrs);
	DEL_CLASS(this->mouseUpObjs);
	DEL_CLASS(this->mouseMoveHdlrs);
	DEL_CLASS(this->mouseMoveObjs);
	DEL_CLASS(this->mnuLayer);
	DEL_CLASS(this->mnuGroup);
	DEL_CLASS(this->envRenderer);
	DEL_CLASS(this->env);
	DEL_CLASS(this->ta);
	DEL_CLASS(this->wgs84CSys);
	SDEL_CLASS(this->ssl);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[32];
	switch (cmdId)
	{
	case MNU_SAVE:
		this->core->SaveData(this, this->env, L"SaveMapEnv");
		break;
	case MNU_COPY_LATLON:
		this->txtUTMGrid->GetText(sbuff);
		Win32::Clipboard::SetString(this->hwnd, sbuff);
		break;
	case MNU_PRINT:
		{
			Media::Printer *printer = this->core->SelectPrinter(this);
			if (printer)
			{
				this->printer = printer;
				Media::IPrintDocument *doc;
				doc = printer->StartPrint(this, this->core->GetDrawEngine());
				if (doc == 0)
				{
					DEL_CLASS(printer);
					UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in printing the map", (const UTF8Char*)"GISForm", this);
				}
				doc->WaitForEnd();
				printer->EndPrint(doc);
//				DEL_CLASS(printer);
			}
		}
		break;
	case MNU_GROUP_ADD:
		{
			this->mapTree->AddSubGroup(this->popNode);
		}
		break;
	case MNU_GROUP_REMOVE:
		{
			if (this->ctrlItem == this->popNode) this->CloseCtrlForm();
			this->mapTree->RemoveItem(this->popNode);
			this->UpdateTimeRange();
			this->mapCtrl->UpdateMap();
			this->mapCtrl->Redraw();
		}
		break;
	case MNU_GROUP_NEW_IMAGE:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Map::VectorLayer *layer;
			const UTF8Char *cols = (const UTF8Char*)"Name";
			NEW_CLASS(layer, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, CSTR("Image Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, 0));
			this->env->AddLayer((Map::MapEnv::GroupItem*)ind->item, layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POINT:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Map::VectorLayer *layer;
			const UTF8Char *cols = (const UTF8Char*)"Name";
			NEW_CLASS(layer, Map::VectorLayer(Map::DRAW_LAYER_POINT, CSTR("Point Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, 0));
			this->env->AddLayer((Map::MapEnv::GroupItem*)ind->item, layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POLYLINE:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Map::VectorLayer *layer;
			const UTF8Char *cols = (const UTF8Char*)"Name";
			NEW_CLASS(layer, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, CSTR("Polyline Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, 0));
			this->env->AddLayer((Map::MapEnv::GroupItem*)ind->item, layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POLYGON:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Map::VectorLayer *layer;
			const UTF8Char *cols = (const UTF8Char*)"Name";
			NEW_CLASS(layer, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, CSTR("Polygon Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, 0));
			this->env->AddLayer((Map::MapEnv::GroupItem*)ind->item, layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_CENTER:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			if (this->env->GetBoundsDbl((Map::MapEnv::GroupItem*)ind->item, &minX, &minY, &maxX, &maxY))
			{
				this->mapCtrl->PanToMapXY((minX + maxX) * 0.5, (minY + maxY) * 0.5);
			}
		}
		break;
	case MNU_GROUP_QUERY:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISGroupQueryForm(0, this->ui, this->core, this, this->env, (Map::MapEnv::GroupItem*)ind->item)), this->popNode);
		}
		break;
	case MNU_LAYER_ADD:
		{
			this->mapTree->AddSubGroup(this->popNode);
		}
		break;
	case MNU_LAYER_REMOVE:
		{
			if (this->ctrlItem == this->popNode) this->CloseCtrlForm();
			this->mapTree->RemoveItem(this->popNode);
			this->UpdateTimeRange();
			this->mapCtrl->UpdateMap();
			this->mapCtrl->Redraw();
		}
		break;
	case MNU_LAYER_CENTER:
		{
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			Math::CoordinateSystem *envCSys;
			Math::CoordinateSystem *lyrCSys;
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)this->env->GetItem(ind->group, ind->index);
			if (lyr->layer->GetBoundsDbl(&minX, &minY, &maxX, &maxY))
			{
				minX = (minX + maxX) * 0.5;
				minY = (minY + maxY) * 0.5;
				envCSys = this->env->GetCoordinateSystem();
				lyrCSys = lyr->layer->GetCoordinateSystem();
				if (envCSys != 0 && lyrCSys != 0)
				{
					if (!envCSys->Equals(lyrCSys))
					{
						Math::CoordinateSystem::ConvertXYZ(lyrCSys, envCSys, minX, minY, 0, &minX, &minY, 0);
					}
				}
				this->mapCtrl->PanToMapXY(minX, minY);
			}
		}
		break;
	case MNU_LAYER_PROP:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			AVIRGISPropForm *frm;
			NEW_CLASS(frm, AVIRGISPropForm(0, this->ui, this->core, this->env, ind->group, ind->index));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->mapCtrl->UpdateMap();
				this->mapCtrl->Redraw();
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_LAYER_PATH:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			Text::StringBuilderUTF8 sb;
			sb.Append(((Map::MapEnv::LayerItem*)ind->item)->layer->GetName());
			UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"Layer Path", this);
		}
		break;
	case MNU_LAYER_SEARCH:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			Map::MapEnv::LayerItem setting;
			this->env->GetLayerProp(&setting, ind->group, ind->index);
			Map::MapEnv::LayerItem *layer = (Map::MapEnv::LayerItem*)ind->item;
			Text::SearchIndexer *searching = layer->layer->CreateSearchIndexer(ta, setting.labelCol);
			if (searching)
			{
				this->AddSubForm(NEW_CLASS_D(AVIRGISSearchForm(0, ui, this->core, this, layer->layer, searching, setting.labelCol, setting.flags)));
			}
		}
		break;
	case MNU_LAYER_SAVE:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			if (ind->itemType == Map::MapEnv::IT_LAYER)
			{
				Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)ind->item;
				this->core->SaveData(this, lyr->layer, L"SaveMapLayer");
			}
		}
		break;
	case MNU_LAYER_COMBINE:
		{
			UI::GUIMapTreeView::ItemIndex *ind = (UI::GUIMapTreeView::ItemIndex *)this->popNode->GetItemObj();
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)this->env->GetItem(ind->group, ind->index);

			Data::ArrayList<Map::IMapDrawLayer *> *layers;
			NEW_CLASS(layers, Data::ArrayList<Map::IMapDrawLayer*>());
			this->env->GetLayersOfType(layers, lyr->layer->GetLayerType());
			AVIRGISCombineForm *frm;
			NEW_CLASS(frm, AVIRGISCombineForm(0, this->ui, this->core, layers));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Map::IMapDrawLayer *newLyr;
				this->env->AddLayer(0, newLyr = frm->GetCombinedLayer(), true);
				newLyr->AddUpdatedHandler(OnMapLayerUpdated, this);
				this->mapTree->UpdateTree();
				this->mapCtrl->UpdateMap();
				this->mapCtrl->Redraw();
			}
			DEL_CLASS(frm);
			DEL_CLASS(layers);
		}
		break;
	case MNU_LAYER_CONV:
		{
			UI::GUIMapTreeView::ItemIndex *itmInd = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)itmInd->item)->layer;
			Map::VectorLayer *nlyr = lyr->CreateEditableLayer();
			this->env->ReplaceLayer(itmInd->group, itmInd->index, nlyr, true);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_LAYER_EDIT:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			Map::DrawLayerType lyrType = lyr->GetLayerType();
			if (lyrType == Map::DRAW_LAYER_IMAGE && lyr->GetObjectClass() == Map::IMapDrawLayer::OC_VECTOR_LAYER)
			{
				this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISEditImageForm(0, this->ui, this->core, (Map::VectorLayer*)lyr, this)), this->popNode);
			}
		}
		break;
	case MNU_LAYER_REPLAY:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_GPS_TRACK)
			{
				this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISReplayForm(0, this->ui, this->core, (Map::GPSTrack*)lyr, this)), this->popNode);
			}
			else
			{
				UI::MessageDialog::ShowDialog((const UTF8Char*)"This layer does not support Replay", (const UTF8Char*)"GIS Form", this);
			}
		}
		break;
	case MNU_LAYER_OPENDB:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			this->AddSubForm(NEW_CLASS_D(AVIRDBForm(0, ui, this->core, lyr, false)));
		}
		break;
	case MNU_LAYER_ASSIGN_CSYS:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			AVIRGISCSysForm *frm;
			NEW_CLASS(frm, AVIRGISCSysForm(0, this->ui, this->core, lyr->GetCoordinateSystem()));
			frm->SetText((const UTF8Char*)"Assign Coordinate System");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				lyr->SetCoordinateSystem(frm->GetCSys());
				this->mapCtrl->UpdateMap();
				this->mapCtrl->Redraw();
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_LAYER_CONV_CSYS:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			AVIRGISCSysForm *frm;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_VECTOR_LAYER)
			{
				Map::VectorLayer *vec = (Map::VectorLayer*)lyr;
				NEW_CLASS(frm, AVIRGISCSysForm(0, this->ui, this->core, lyr->GetCoordinateSystem()));
				frm->SetText((const UTF8Char*)"Convert Coordinate System");
				if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
				{
					vec->ConvCoordinateSystem(frm->GetCSys());
					this->mapCtrl->UpdateMap();
					this->mapCtrl->Redraw();
				}
				DEL_CLASS(frm);
			}
		}
		break;
	case MNU_LAYER_EXPAND:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_MAP_LAYER_COLL)
			{
				UI::GUIMapTreeView::ItemIndex *itemInd = (UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj();
				this->mapTree->ExpandColl(itemInd);
			}
		}
		break;
	case MNU_LAYER_TILEDOWN:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
			{
				this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISTileDownloadForm(0, this->ui, this->core, (Map::TileMapLayer*)lyr, this)), this->popNode);
			}
		}
		break;
	case MNU_LAYER_IMPORT_TILES:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
			{
				Map::TileMapLayer *layer = (Map::TileMapLayer*)lyr;
				Map::TileMap *tileMap = layer->GetTileMap();
				if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
				{
					Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
					if (osm->HasSPackageFile())
					{
						UI::FileDialog *dlg;
						NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"GISImportTiles", false));
						this->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::PackageFile);
						if (dlg->ShowDialog(this->GetHandle()))
						{
							IO::StmData::FileData *fd;
							NEW_CLASS(fd, IO::StmData::FileData(dlg->GetFileName(), false));
							IO::PackageFile *pkg = (IO::PackageFile*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile);
							DEL_CLASS(fd);
							if (pkg)
							{
								osm->ImportTiles(pkg);
								DEL_CLASS(pkg);
							}
						}
						DEL_CLASS(dlg);
					}
				}
			}
		}
		break;
	case MNU_LAYER_OPTIMIZE_FILE:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			if (lyr->GetObjectClass() == Map::IMapDrawLayer::OC_TILE_MAP_LAYER)
			{
				Map::TileMapLayer *layer = (Map::TileMapLayer*)lyr;
				Map::TileMap *tileMap = layer->GetTileMap();
				if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
				{
					Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
					if (osm->HasSPackageFile())
					{
						UI::FileDialog *dlg;
						NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"GISOptimizeFile", true));
						dlg->AddFilter((const UTF8Char*)"*.spk", (const UTF8Char*)"SPackage File");
						if (dlg->ShowDialog(this->GetHandle()))
						{
							osm->OptimizeToFile(dlg->GetFileName()->ToCString());
						}
						DEL_CLASS(dlg);
					}
				}
			}
		}
		break;
	case MNU_LAYER_QUERY:
		{
			Map::IMapDrawLayer *lyr = ((Map::MapEnv::LayerItem*)((UI::GUIMapTreeView::ItemIndex*)this->popNode->GetItemObj())->item)->layer;
			this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISQueryForm(0, this->ui, this->core, lyr, this)), this->popNode);
		}
		break;
	case MNU_MTK_GPS:
		{
			IO::SerialPort *port;
			IO::Device::MTKGPSNMEA *mtk;
			Map::GPSTrack *trk;
			UTF8Char *sptr;

			NEW_CLASS(port, IO::SerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort(), 115200, IO::SerialPort::PARITY_NONE, true));
			if (!port->IsError())
			{
				NEW_CLASS(mtk, IO::Device::MTKGPSNMEA(port, true));
				if (mtk->IsMTKDevice())
				{
					NEW_CLASS(trk, Map::GPSTrack(CSTR("MTK_Tracker"), true, 0, 0));
					if (mtk->ParseLog(trk))
					{
						Data::DateTime dt;
						trk->GetTrackStartTime(0, &dt);
						dt.ToLocalTime();
						if (mtk->QueryFirmware())
						{
							sptr = Text::StrConcat(sbuff, mtk->GetProductMode());
							*sptr++ = '_';
						}
						else
						{
							sptr = Text::StrConcatC(sbuff, UTF8STRC("MTK_"));
						}
						sptr = dt.ToString(sptr, "yyyyMMdd");
						trk->SetSourceName(sbuff, (UOSInt)(sptr - sbuff));
						this->env->AddLayer(0, trk, true);
						this->mapTree->UpdateTree();
						this->mapCtrl->UpdateMap();
						this->mapCtrl->Redraw();
					}
					else
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing log", (const UTF8Char*)"MTK Tracker", this);
						DEL_CLASS(trk);
					}
				}
				else
				{
					UI::MessageDialog::ShowDialog((const UTF8Char*)"MTK Tracker not found", (const UTF8Char*)"MTK Tracker", this);
				}
				DEL_CLASS(mtk);
			}
			else
			{
				DEL_CLASS(port);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"MTK GPS Tracker not found", (const UTF8Char*)"MTK Tracker", this);
			}
		}
		break;
	case MNU_MTK_FILE:
		{
			UI::FileDialog *dlg;
			NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"GISMTKFile", false));
			dlg->AddFilter((const UTF8Char*)"*.bin", (const UTF8Char*)"MTK Binary File");
			if (dlg->ShowDialog(this->GetHandle()))
			{
				IO::FileStream *fs;
				UInt64 fileSize;
				NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				fileSize = fs->GetLength();
				if (fileSize > 0 && fileSize <= 8388608 && (fileSize & 0xffff) == 0)
				{
					UOSInt i;
					Map::GPSTrack *trk;
					UInt8 *fileBuff = MemAlloc(UInt8, (UOSInt)fileSize);
					fs->Read(fileBuff, (UOSInt)fileSize);
					NEW_CLASS(trk, Map::GPSTrack(dlg->GetFileName(), true, 0, 0));
					i = 0;
					while (i < fileSize)
					{
						if (!IO::Device::MTKGPSNMEA::ParseBlock(&fileBuff[i], trk))
						{
							break;
						}
						i += 65536;
					}
					MemFree(fileBuff);
					this->AddLayer(trk);
				}
				DEL_CLASS(fs);
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, true));
			frm->SetText((const UTF8Char*)"Select GPS Tracker");
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRGPSTrackerForm *gpsFrm;
				IO::GPSNMEA *gps;
				Map::GPSTrack *trk;
				NEW_CLASS(gps, IO::GPSNMEA(frm->stm, true));
				NEW_CLASS(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(0, this->ui, this->core, gps, true));
				this->AddSubForm(gpsFrm);
				NEW_CLASS(trk, Map::GPSTrack(CSTR("GPS_Tracker"), true, 0, 0));
				gpsFrm->SetGPSTrack(trk);
				gpsFrm->SetMapNavigator(this);
				this->AddLayer(trk);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_MTKGPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRSelStreamForm(0, this->ui, this->core, false));
			frm->SetText((const UTF8Char*)"Select MTK GPS Tracker");
			frm->SetInitSerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort());
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				SSWR::AVIRead::AVIRGPSTrackerForm *gpsFrm;
				IO::GPSNMEA *gps;
				Map::GPSTrack *trk;
				NEW_CLASS(gps, IO::Device::MTKGPSNMEA(frm->stm, true));
				NEW_CLASS(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(0, this->ui, this->core, gps, true));
				this->AddSubForm(gpsFrm);
				NEW_CLASS(trk, Map::GPSTrack(CSTR("MTK_GPS_Tracker"), true, 0, 0));
				gpsFrm->SetGPSTrack(trk);
				gpsFrm->SetMapNavigator(this);
				this->AddLayer(trk);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_GOOGLE_POLYLINE:
		{
			SSWR::AVIRead::AVIRGooglePolylineForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGooglePolylineForm(0, this->ui, this->core));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Map::VectorLayer *lyr;
				NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, CSTR("Google Polyline"), 0, 0, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), 0, 0, 0, 0, 0));
				lyr->AddVector(frm->GetPolyline(), (const UTF8Char**)0);
				this->AddLayer(lyr);
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_OPEN_FILE:
		{
			SSWR::AVIRead::AVIROpenFileForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIROpenFileForm(0, this->ui, this->core, IO::ParserType::Unknown));
			if (frm->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				Text::String *fname = frm->GetFileName();
				UOSInt i = fname->IndexOf(':');
				if (i == INVALID_INDEX || i == 1)
				{
					IO::StmData::FileData *fd;
					NEW_CLASS(fd, IO::StmData::FileData(fname, false));
					if (fd->GetDataSize() == 0)
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in loading file", (const UTF8Char*)"AVIRead", this);
					}
					else
					{
						this->core->LoadData(fd, 0);
					}
					DEL_CLASS(fd);
				}
				else
				{
					this->OpenURL(fname->v, fname->leng, 0);
				}
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_HKO_RADAR_64:
		this->OpenURL(UTF8STRC("http://www.weather.gov.hk/wxinfo/radars/radar_064_kml/Radar_064k.kml"), 0);
		break;
	case MNU_HKO_RADAR_128:
		this->OpenURL(UTF8STRC("http://www.weather.gov.hk/wxinfo/radars/radar_128_kml/Radar_128k.kml"), 0);
		break;
	case MNU_HKO_RADAR_256:
		this->OpenURL(UTF8STRC("http://www.weather.gov.hk/wxinfo/radars/radar_256_kml/Radar_256k.kml"), 0);
		break;
	case MNU_HKO_CYCLONE:
		{
			Net::HTTPClient *cli;
			Text::UTF8Reader *reader;
			Text::StringBuilderUTF8 sb;
			UTF8Char *sptr;
			UTF8Char sbuff[10];
			UOSInt i;
			Data::DateTime dt;
			cli = Net::HTTPClient::CreateConnect(this->core->GetSocketFactory(), this->ssl, (const UTF8Char*)"https://www.weather.gov.hk/wxinfo/currwx/tc_gis_list.xml", Net::WebUtil::RequestMethod::HTTP_GET, false);
			NEW_CLASS(reader, Text::UTF8Reader(cli));
			reader->ReadLine(&sb, 4096);
			while (true)
			{
				sb.ClearStr();
				if (!reader->ReadLine(&sb, 4096))
					break;
				sptr = sb.ToString();
				i = Text::StrIndexOfC(sptr, sb.GetLength(), UTF8STRC("=\""));
				if (i != INVALID_INDEX)
				{
					sptr = sptr + i + 2;
					i = Text::StrIndexOfChar(sptr, ',');
					if (i == 4)
					{
						sptr[i] = 0;
						Text::StrConcat(sbuff, sptr);
						dt.SetCurrTimeUTC();
						sb.ClearStr();
						sb.AppendC(UTF8STRC("https://www.weather.gov.hk/wxinfo/currwx/tc_gis_cone_15a_"));
						sb.AppendC(sbuff, 4);
						sb.AppendC(UTF8STRC(".kml?rand="));
						sb.AppendI64(dt.ToTicks());
						this->OpenURL(sb.ToString(), sb.GetLength(), 0);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("https://www.weather.gov.hk/wxinfo/currwx/tc_gis_track_15a_e_"));
						sb.AppendC(sbuff, 4);
						sb.AppendC(UTF8STRC(".xml?rand="));
						sb.AppendI64(dt.ToTicks());
						this->OpenURL(sb.ToString(), sb.GetLength(), 0);
					}
				}
			}
			DEL_CLASS(reader);
			DEL_CLASS(cli);
		}
		break;

	case MNU_HKO_CURR_RADAR_64:
		this->HKOPortal((const UTF8Char*)"radar_merge_064.txt", (const UTF8Char*)"/radar2/064/");
		break;
	case MNU_HKO_CURR_RADAR_128:
		this->HKOPortal((const UTF8Char*)"radar_merge_128.txt", (const UTF8Char*)"/radar2/128/");
		break;
	case MNU_HKO_CURR_RADAR_256:
		this->HKOPortal((const UTF8Char*)"radar_merge_256.txt", (const UTF8Char*)"/radar2/256/");
		break;
	case MNU_HKO_CURR_SATELLITE:
		this->HKOPortal((const UTF8Char*)"satellite_TC.txt", (const UTF8Char*)"/satellite/TC/");
		break;
	case MNU_HKO_CURR_SATELLITE_IR1_L1B_10:
		this->HKOPortal((const UTF8Char*)"satellite_IR1-L1B-10.txt", (const UTF8Char*)"/satellite/IR1-L1B-10/");
		break;
	case MNU_HKO_CURR_SATELLITE_DC_10:
		this->HKOPortal((const UTF8Char*)"satellite_DC-10.txt", (const UTF8Char*)"/satellite/DC-10/");
		break;
	case MNU_HKO_CURR_SAND:
		this->HKOPortal((const UTF8Char*)"sand.txt", (const UTF8Char*)"/sand/");
		break;
	case MNU_HKO_CURR_WEATHER_CHART:
		this->HKOPortal((const UTF8Char*)"weather_chart.txt", (const UTF8Char*)"/weather_chart/");
		break;
	case MNU_GPS_SIMULATOR:
		this->SetCtrlForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGPSSimulatorForm(0, this->ui, this->core, this)), 0);
		break;
	case MNU_DISTANCE:
		this->AddSubForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISDistanceForm(0, this->ui, this->core, this)));
		break;
	case MNU_HKTD_TRAFFIC:
		{
			SSWR::AVIRead::AVIRGISHKTrafficForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGISHKTrafficForm(0, this->ui, this->core));
			if (frm->ShowDialog(this))
			{
				if (frm->GetMapLayer())
				{
					this->AddLayer(frm->GetMapLayer());
				}
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_HKTD_TONNES_SIGN:
		{
			SSWR::AVIRead::AVIRGISHKTDTonnesForm *frm;
			NEW_CLASS(frm, SSWR::AVIRead::AVIRGISHKTDTonnesForm(0, this->ui, this->core));
			if (frm->ShowDialog(this))
			{
				this->AddLayer(frm->GetMapLayer());
			}
			DEL_CLASS(frm);
		}
		break;
	case MNU_RANDOMLOC:
		this->AddSubForm(NEW_CLASS_D(SSWR::AVIRead::AVIRGISRandomLocForm(0, this->ui, this->core, this)));
		break;
	}
}

void SSWR::AVIRead::AVIRGISForm::OnMonitorChanged()
{
	MonitorHandle *hMonitor = this->GetHMonitor();
	this->colorSess->ChangeMonitor(hMonitor);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	OnMapScaleChanged(this, this->mapCtrl->GetMapScale());
}

void SSWR::AVIRead::AVIRGISForm::OnFocus()
{
	this->ResumeUpdate();
}

void SSWR::AVIRead::AVIRGISForm::AddLayer(Map::IMapDrawLayer *layer)
{
	layer->AddUpdatedHandler(OnMapLayerUpdated, this);
	if (this->env->GetItemCount(0) == 0)
	{
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
//		OSInt w;
//		OSInt h;
		layer->GetBoundsDbl(&minX, &minY, &maxX, &maxY);
//		this->mapCtrl->GetSize(&w, &h);
//		this->mapCtrl->UpdateMapView(layer->CreateMapView(w, h));
		this->env->AddLayer(0, layer, true);
		this->mapCtrl->PanToMapXY((minX + maxX) * 0.5, (minY + maxY) * 0.5);
	}
	else
	{
		this->env->AddLayer(0, layer, true);
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
	this->mapTree->UpdateTree();
	this->UpdateTimeRange();
}

void SSWR::AVIRead::AVIRGISForm::AddLayers(::Data::ArrayList<Map::IMapDrawLayer*> *layers)
{
	Map::IMapDrawLayer *layer;
	Bool needPan = this->env->GetItemCount(0) == 0;
	UOSInt i;
	UOSInt j;
	Double minX;
	Double minY;
	Double maxX;
	Double maxY;

	if (needPan)
	{
		layer = layers->GetItem(0);
		layer->GetBoundsDbl(&minX, &minY, &maxX, &maxY);
	}

	i = 0;
	j = layers->GetCount();
	while (i < j)
	{
		layer = layers->GetItem(i);
		layer->AddUpdatedHandler(OnMapLayerUpdated, this);
		this->env->AddLayer(0, layer, true);
		i++;
	}
	this->UpdateTimeRange();
	this->mapTree->UpdateTree();
	if (needPan)
	{
		this->mapCtrl->PanToMapXY((minX + maxX) * 0.5, (minY + maxY) * 0.5);
	}
	else
	{
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
}

/*void SSWR::AVIRead::AVIRGISForm::AddLayerColl(Map::MapLayerCollection *lyrColl)
{
	WChar sbuff[512];
	WChar *sptr;
	lyrColl->GetSourceName(sbuff);
	sptr = &sbuff[Text::StrLastIndexOfCharC(sbuff, '\\') + 1];
	void *grp = this->env->AddGroup(0, sptr);

	Map::IMapDrawLayer *layer;
	Bool needPan = this->env->GetItemCount(0) == 0;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = lyrColl->GetCount();
	while (i < j)
	{
		layer = lyrColl->GetItem(i);
		layer->SetUpdatedHandler(OnMapLayerUpdated, this);
		this->env->AddLayer(grp, layer, true);
		i++;
	}
	this->UpdateTimeRange();
	this->mapTree->UpdateTree();
	if (needPan)
	{
		Double minX;
		Double minY;
		Double maxX;
		Double maxY;
		layer = lyrColl->GetItem(0);
		layer->GetBoundsDbl(&minX, &minY, &maxX, &maxY);
		this->mapCtrl->PanToMapXY((minX + maxX) * 0.5, (minY + maxY) * 0.5);
	}
	else
	{
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
	lyrColl->Clear();
	DEL_CLASS(lyrColl);
}*/

void SSWR::AVIRead::AVIRGISForm::AddSubForm(UI::GUIForm *frm)
{
	frm->HandleFormClosed(OnSubFormClosed, this);
	this->subForms->Add(frm);
	frm->Show();
}

UInt32 SSWR::AVIRead::AVIRGISForm::GetSRID()
{
	return this->env->GetSRID();
}

Bool SSWR::AVIRead::AVIRGISForm::InMap(Double lat, Double lon)
{
	return this->mapCtrl->InMapMapXY(lon, lat);
}

void SSWR::AVIRead::AVIRGISForm::PanToMap(Double lat, Double lon)
{
	this->mapCtrl->PanToMapXY(lon, lat);
}

void SSWR::AVIRead::AVIRGISForm::ShowMarker(Double lat, Double lon)
{
	this->mapCtrl->ShowMarkerMapXY(lon, lat);
}

void SSWR::AVIRead::AVIRGISForm::ShowMarkerDir(Double lat, Double lon, Double dir, Math::Unit::Angle::AngleUnit unit)
{
	this->mapCtrl->ShowMarkerMapXYDir(lon, lat, dir, unit);
}

void SSWR::AVIRead::AVIRGISForm::HideMarker()
{
	if (!this->pauseUpdate)
	{
		this->mapCtrl->HideMarker();
	}
}

void SSWR::AVIRead::AVIRGISForm::SetSelectedVector(Math::Vector2D *vec)
{
	this->mapCtrl->SetSelectedVector(vec);
}

void SSWR::AVIRead::AVIRGISForm::RedrawMap()
{
	if (!this->pauseUpdate)
	{
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
}

Math::CoordinateSystem *SSWR::AVIRead::AVIRGISForm::GetCoordinateSystem()
{
	return this->env->GetCoordinateSystem();
}

void SSWR::AVIRead::AVIRGISForm::ScnXY2MapXY(OSInt scnX, OSInt scnY, Double *mapX, Double *mapY)
{
	this->mapCtrl->ScnXY2MapXY(scnX, scnY, mapX, mapY);
}

void SSWR::AVIRead::AVIRGISForm::MapXY2ScnXY(Double mapX, Double mapY, OSInt *scnX, OSInt *scnY)
{
	this->mapCtrl->MapXY2ScnXY(mapX, mapY, scnX, scnY);
}

void SSWR::AVIRead::AVIRGISForm::SetMapCursor(UI::GUIControl::CursorType curType)
{
	if (this->currCursor != curType)
	{
		this->currCursor = curType;
		this->mapCtrl->SetCursor(curType);
	}
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseDown(MouseEvent evt, void *userObj)
{
	this->mouseDownHdlrs->Add(evt);
	this->mouseDownObjs->Add(userObj);
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseUp(MouseEvent evt, void *userObj)
{
	this->mouseUpHdlrs->Add(evt);
	this->mouseUpObjs->Add(userObj);
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseMove(MouseEvent evt, void *userObj)
{
	this->mouseMoveHdlrs->Add(evt);
	this->mouseMoveObjs->Add(userObj);
}

void SSWR::AVIRead::AVIRGISForm::UnhandleMapMouse(void *userObj)
{
	UOSInt i;
	i = this->mouseDownObjs->GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseDownObjs->GetItem(i))
		{
			this->mouseDownHdlrs->RemoveAt(i);
			this->mouseDownObjs->RemoveAt(i);
		}
	}
	i = this->mouseUpObjs->GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseUpObjs->GetItem(i))
		{
			this->mouseUpHdlrs->RemoveAt(i);
			this->mouseUpObjs->RemoveAt(i);
		}
	}
	i = this->mouseMoveObjs->GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseMoveObjs->GetItem(i))
		{
			this->mouseMoveHdlrs->RemoveAt(i);
			this->mouseMoveObjs->RemoveAt(i);
		}
	}
	if (this->currCursor != UI::GUIControl::CT_ARROW)
	{
		this->currCursor = UI::GUIControl::CT_ARROW;
		this->mapCtrl->SetCursor(this->currCursor);
	}
}

void SSWR::AVIRead::AVIRGISForm::SetKMapEnv(const UTF8Char *kmapIP, Int32 kmapPort, Int32 lcid)
{
/*	if (kmapIP != 0 && kmapPort != 0 && lcid != 0)
	{
		NEW_CLASS(this->kmap, SP::KMap::KMapConn(this->core->GetSocketFactory(), kmapIP, (::UInt16)kmapPort));
		this->lcid = lcid;
	}*/
}

Bool SSWR::AVIRead::AVIRGISForm::HasKMap()
{
	return false; //return this->kmap != 0;
}

UTF8Char *SSWR::AVIRead::AVIRGISForm::ResolveAddress(UTF8Char *sbuff, Double lat, Double lon)
{
/*	if (this->kmap == 0)
	{
		return 0;
	}
	return this->kmap->ResolveAddress(sbuff, this->lcid, lat, lon, L"~@#");*/
	return 0;
}

void SSWR::AVIRead::AVIRGISForm::PauseUpdate()
{
	this->pauseUpdate = true;
	this->mapCtrl->PauseUpdate(true);
}

void SSWR::AVIRead::AVIRGISForm::ResumeUpdate()
{
	if (this->pauseUpdate)
	{
		this->pauseUpdate = false;
		this->mapCtrl->PauseUpdate(false);
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
}

void SSWR::AVIRead::AVIRGISForm::UpdateTimeRange()
{
	Int64 timeStart = this->env->GetTimeStartTS(0);
	Int64 timeEnd = this->env->GetTimeEndTS(0);
	if (this->timeRangeStart == timeStart && this->timeRangeEnd == timeEnd)
	{
		return;
	}
	if (timeStart == 0)
	{
		this->timeRangeStart = 0;
		this->timeRangeEnd = 0;
		this->tbTimeRange->SetEnabled(false);
		this->chkTime->SetChecked(false);
		this->chkTime->SetEnabled(false);
		this->chkTime->SetText((const UTF8Char*)"Unavailable");
	}
	else
	{
		UTF8Char sbuff[64];
		Data::DateTime dt;
		if (timeEnd == 0)
		{
			dt.SetCurrTimeUTC();
			timeEnd = dt.ToUnixTimestamp();
		}

		if (!this->useTime)
		{
			this->currTime = timeStart;
		}

		this->timeRangeStart = timeStart;
		this->timeRangeEnd = timeEnd;
		this->tbTimeRange->SetRange(0, (UOSInt)(timeEnd - timeStart));
		this->tbTimeRange->SetPos((UOSInt)(this->currTime - timeStart));
		this->tbTimeRange->SetEnabled(true);
		this->chkTime->SetEnabled(true);
		this->chkTime->SetChecked(this->useTime);
		dt.SetUnixTimestamp(timeStart);
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->chkTime->SetText(sbuff);
	}
}

Bool SSWR::AVIRead::AVIRGISForm::BeginPrint(Media::IPrintDocument *doc)
{
	doc->SetDocName(this->env->GetSourceNameObj());
	return true;
}

Bool SSWR::AVIRead::AVIRGISForm::PrintPage(Media::DrawImage *printPage)
{
	Map::MapView *view = this->mapCtrl->CloneMapView();
	Map::DrawMapRenderer *renderer;
	NEW_CLASS(renderer, Map::DrawMapRenderer(this->core->GetDrawEngine(), this->env, printPage->GetColorProfile(), 0, Map::DrawMapRenderer::DT_VECTORDRAW));
	view->SetDestImage(printPage);
	renderer->DrawMap(printPage, view, 0);
	DEL_CLASS(renderer);
	DEL_CLASS(view);
	return false;
}

Bool SSWR::AVIRead::AVIRGISForm::EndPrint(Media::IPrintDocument *doc)
{
	SDEL_CLASS(this->printer);
	return true;
}
