#include "Stdafx.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteBuffer.h"
#include "IO/DirectoryPackage.h"
#include "IO/Path.h"
#include "IO/SerialPort.h"
#include "IO/StreamReader.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "IO/StmData/FileData.h"
#include "Map/CSVMapParser.h"
#include "Map/DrawMapRenderer.h"
#include "Map/DrawMapServiceLayer.h"
#include "Map/GeoPackageLayer.h"
#include "Map/HKParkingVacancy.h"
#include "Map/VectorLayer.h"
#include "Map/ESRI/ESRITileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/Math_C.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/UTMGridConvertDbl.h"
#include "Math/Geometry/VectorImage.h"
#include "Media/ImagePreviewTool.h"
#include "Media/SharedImage.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebBrowser.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRESRIMapForm.h"
#include "SSWR/AVIRead/AVIRGISCombineForm.h"
#include "SSWR/AVIRead/AVIRGISCSysForm.h"
#include "SSWR/AVIRead/AVIRGISDistanceForm.h"
#include "SSWR/AVIRead/AVIRGISEditImageForm.h"
#include "SSWR/AVIRead/AVIRGISEditPointForm.h"
#include "SSWR/AVIRead/AVIRGISEditVectorForm.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRGISGroupQueryForm.h"
#include "SSWR/AVIRead/AVIRGISHKTDTonnesForm.h"
#include "SSWR/AVIRead/AVIRGISHKTrafficForm.h"
#include "SSWR/AVIRead/AVIRGISPropForm.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "SSWR/AVIRead/AVIRGISRandomLocForm.h"
#include "SSWR/AVIRead/AVIRGISReplayForm.h"
#include "SSWR/AVIRead/AVIRGISSearchForm.h"
#include "SSWR/AVIRead/AVIRGISShortestPathForm.h"
#include "SSWR/AVIRead/AVIRGPSSimulatorForm.h"
#include "SSWR/AVIRead/AVIRGISTileDownloadForm.h"
#include "SSWR/AVIRead/AVIRGooglePolylineForm.h"
#include "SSWR/AVIRead/AVIRGPSTrackerForm.h"
#include "SSWR/AVIRead/AVIROpenFileForm.h"
#include "SSWR/AVIRead/AVIRRegionalMapForm.h"
#include "SSWR/AVIRead/AVIRSelStreamForm.h"
#include "SSWR/AVIRead/AVIRTMSForm.h"
#include "SSWR/AVIRead/AVIRWFSForm.h"
#include "SSWR/AVIRead/AVIRWMSForm.h"
#include "SSWR/AVIRead/AVIRWMTSForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"
#include "UtilUI/TextInputDialog.h"

#include <stdio.h>

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
	MNU_LAYER_FIT,
	MNU_LAYER_PROP,
	MNU_LAYER_PATH,
	MNU_LAYER_SEARCH,
	MNU_LAYER_SHORTESTPATH,
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
	MNU_LAYER_MUL_COORD,
	MNU_MTK_GPS,
	MNU_MTK_FILE,
	MNU_GPS_TRACKER,
	MNU_MTKGPS_TRACKER,
	MNU_GOOGLE_POLYLINE,
	MNU_OPEN_FILE,
	MNU_TMS,
	MNU_WMTS,
	MNU_WMS,
	MNU_WFS,
	MNU_ESRI_MAP,
	MNU_REGIONAL_MAP,
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
	MNU_RANDOMLOC,
	MNU_HK_WASTELESS,
	MNU_HK_HKE_EV_CHARGING_EN,
	MNU_HK_PARKING_VACANCY,
	MNU_HK_GEODATA_WMS
} MenuItems;

void __stdcall SSWR::AVIRead::AVIRGISForm::FileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRGISForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISForm>();
	NN<Parser::ParserList> parsers = me->core->GetParserList();
	Optional<IO::ParsedObject> pobj;
	NN<IO::ParsedObject> nnpobj;
	NN<Data::ArrayListNN<Map::MapDrawLayer>> layers;
	Math::Size2D<UOSInt> sz;
	Math::Coord2D<OSInt> mousePos = me->ui->GetCursorPos();
	Math::Coord2D<OSInt> scnPos = me->mapCtrl->GetScreenPosP();
	mousePos.x = mousePos.x - scnPos.x;
	mousePos.y = mousePos.y - scnPos.y;
	sz = me->mapCtrl->GetSizeP();
	if (mousePos.x >= 0 && mousePos.y >= 0 && mousePos.x < (OSInt)sz.x && mousePos.y < (OSInt)sz.y)
	{
	}
	else
	{
		mousePos.x = (OSInt)(sz.x >> 1);
		mousePos.y = (OSInt)(sz.y >> 1);
	}

	NEW_CLASSNN(layers, Data::ArrayListNN<Map::MapDrawLayer>());
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		IO::Path::PathType pathType = IO::Path::GetPathType(files[i]->ToCString());
		pobj = 0;
		if (pathType == IO::Path::PathType::File)
		{
			IO::StmData::FileData fd(files[i], false);
			pobj = parsers->ParseFile(fd);
		}
		else if (pathType == IO::Path::PathType::Directory)
		{
			IO::DirectoryPackage dpkg(files[i]);
			pobj = parsers->ParseObject(dpkg);
		}
		if (pobj.SetTo(nnpobj))
		{
			IO::ParserType pt = nnpobj->GetParserType();
			if (pt == IO::ParserType::MapLayer)
			{
				layers->Add(NN<Map::MapDrawLayer>::ConvertFrom(nnpobj));
			}
			else if (pt == IO::ParserType::MapEnv)
			{
				////////////////////////////
				pobj.Delete();
			}
			else if (pt == IO::ParserType::ImageList)
			{
				Math::Coord2DDbl pt1;
				Math::Coord2DDbl pt2;
				NN<Map::VectorLayer> lyr;
				NN<Media::SharedImage> simg;
				NN<Math::Geometry::VectorImage> vimg;
				NN<Media::RasterImage> stimg;
				if (NN<Media::ImageList>::ConvertFrom(nnpobj)->GetImage(0, 0).SetTo(stimg))
				{
					NEW_CLASSNN(lyr, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, files[i]->ToCString(), Math::CoordinateSystemManager::CreateWGS84Csys(), nullptr));
					Double calcImgW;
					Double calcImgH;
					if (stimg->HasHotSpot())
					{
						Double hsX;
						Double hsY;
						if (stimg->info.par2 > 1)
						{
							hsX = OSInt2Double(stimg->GetHotSpotX());
							hsY = OSInt2Double(stimg->GetHotSpotY()) * stimg->info.par2;
							calcImgW = UOSInt2Double(stimg->info.dispSize.x);
							calcImgH = UOSInt2Double(stimg->info.dispSize.y) * stimg->info.par2;
						}
						else
						{
							hsX = OSInt2Double(stimg->GetHotSpotX()) / stimg->info.par2;
							hsY = OSInt2Double(stimg->GetHotSpotY());
							calcImgW = UOSInt2Double(stimg->info.dispSize.x) / stimg->info.par2;
							calcImgH = UOSInt2Double(stimg->info.dispSize.y);
						}
						pt1 = me->mapCtrl->ScnXYD2MapXY(Math::Coord2DDbl(OSInt2Double(mousePos.x) - hsX, OSInt2Double(mousePos.y) - hsY));
						pt2 = me->mapCtrl->ScnXYD2MapXY(Math::Coord2DDbl(OSInt2Double(mousePos.x) + calcImgW - hsX, OSInt2Double(mousePos.y) + calcImgH - hsY));
					}
					else
					{
						if (stimg->info.par2 > 1)
						{
							calcImgW = UOSInt2Double(stimg->info.dispSize.x);
							calcImgH = UOSInt2Double(stimg->info.dispSize.y) * stimg->info.par2;
						}
						else
						{
							calcImgW = UOSInt2Double(stimg->info.dispSize.x) / stimg->info.par2;
							calcImgH = UOSInt2Double(stimg->info.dispSize.y);
						}
						pt1 = me->mapCtrl->ScnXYD2MapXY(Math::Coord2DDbl(OSInt2Double(mousePos.x) - calcImgW * 0.5, OSInt2Double(mousePos.y) - calcImgH * 0.5));
						pt2 = me->mapCtrl->ScnXYD2MapXY(Math::Coord2DDbl(OSInt2Double(mousePos.x) + calcImgW * 0.5, OSInt2Double(mousePos.y) + calcImgH * 0.5));
					}
					Data::ArrayListNN<Media::StaticImage> prevList;
					Media::ImagePreviewTool::CreatePreviews(NN<Media::ImageList>::ConvertFrom(nnpobj), prevList, 640);
					NEW_CLASSNN(simg, Media::SharedImage(NN<Media::ImageList>::ConvertFrom(nnpobj), prevList));
					NEW_CLASSNN(vimg, Math::Geometry::VectorImage(me->env->GetSRID(), simg, pt1, pt2, pt2 - pt1, false, files[i].Ptr(), 0, 0));
					simg.Delete();
					lyr->AddVector2(vimg, (Text::String**)0);
					layers->Add(lyr);
				}
				else
				{
					pobj.Delete();
				}
			}
			else
			{
				if (me->ParseObject(nnpobj))
				{
					pobj.Delete();
				}
				else
				{
					me->core->OpenObject(nnpobj);
				}
			}
		}
		i++;
	}
	if (layers->GetCount() > 0)
	{
		me->AddLayers(layers);
	}
	layers.Delete();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Math::Coord2DDbl latLon;
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	Math::Coord2DDbl mapPos = me->mapCtrl->ScnXY2MapXY(scnPos);

	sptr = Text::StrDouble(Text::StrConcatC(Text::StrDouble(sbuff, mapPos.x), UTF8STRC(", ")), mapPos.y);
	me->txtLatLon->SetText(CSTRP(sbuff, sptr));
	
	NN<Math::CoordinateSystem> csys = me->env->GetCoordinateSystem();
	latLon = Math::CoordinateSystem::Convert(csys, me->wgs84CSys, mapPos);

	Math::UTMGridConvertDbl conv;
	sptr = conv.WGS84_Grid(sbuff, 5, 0, 0, 0, 0, latLon.GetLat(), latLon.GetLon());
	me->txtUTMGrid->SetText(CSTRP(sbuff, sptr));

	UOSInt i;
	i = me->mouseMoveHdlrs.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<MouseEvent> cb = me->mouseMoveHdlrs.GetItem(i);
		cb.func(cb.userObj, scnPos);
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	Bool ret = false;
	UOSInt i;
	if (button == MouseButton::MBTN_LEFT)
	{
		i = me->mouseLDownHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MouseEvent> cb = me->mouseLDownHdlrs.GetItem(i);
			ret = cb.func(cb.userObj, scnPos);
			if (ret)
				return true;
		}
	}
	else if (button == MouseButton::MBTN_RIGHT)
	{
		i = me->mouseRDownHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MouseEvent> cb = me->mouseRDownHdlrs.GetItem(i);
			ret = cb.func(cb.userObj, scnPos);
			if (ret)
				return true;
		}
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISForm::OnMapMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	Bool ret = false;
	UOSInt i;
	if (button == MouseButton::MBTN_LEFT)
	{
		i = me->mouseLUpHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MouseEvent> cb = me->mouseLUpHdlrs.GetItem(i);
			ret = cb.func(cb.userObj, scnPos);
			if (ret)
				return true;
		}
	}
	else if (button == MouseButton::MBTN_RIGHT)
	{
		i = me->mouseRUpHdlrs.GetCount();
		while (i-- > 0)
		{
			Data::CallbackStorage<MouseEvent> cb = me->mouseRUpHdlrs.GetItem(i);
			ret = cb.func(cb.userObj, scnPos);
			if (ret)
				return true;
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapScaleChanged(AnyType userObj, Double newScale)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (me->scaleChanging)
	{
		sptr = Text::StrDoubleFmt(Text::StrConcatC(sbuff, UTF8STRC("1:")), me->mapCtrl->GetViewScale(), "0.#");
		me->txtScale->SetText(CSTRP(sbuff, sptr));
		return;
	}
	me->scaleChanging = true;
	me->tbScale->SetPos((UOSInt)Double2OSInt(Math_Log10(newScale / me->env->GetMinScale()) * 65536.0 / Math_Log10(me->env->GetMaxScale() / me->env->GetMinScale())));
	me->scaleChanging = false;
	sptr = Text::StrDoubleFmt(Text::StrConcatC(sbuff, UTF8STRC("1:")), me->mapCtrl->GetViewScale(), "0.#");
	me->txtScale->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapUpdated(AnyType userObj, Math::Coord2DDbl center, Double timeUsed)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	me->mapUpdT = timeUsed;
	me->mapUpdTChanged = true;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnScaleScrolled(AnyType userObj, UOSInt newVal)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	if (me->scaleChanging)
		return;
	me->scaleChanging = true;
	Int32 scale = Double2Int32(Math_Pow(10, Math_Log10((me->env->GetMaxScale() / me->env->GetMinScale())) * UOSInt2Double(newVal) / 65536.0) * me->env->GetMinScale());
	me->mapCtrl->SetMapScale(scale);
	me->scaleChanging = false;
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTreeRightClick(AnyType userObj)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	NN<UI::GUITreeView::TreeItem> item;
	if (me->mapTree->GetHighlightItem().SetTo(item))
	{
		Math::Coord2D<OSInt> cursorPos = me->ui->GetCursorPos();

		NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
		NN<Map::MapEnv::LayerItem> lyr;
		if (ind->itemType == Map::MapEnv::IT_LAYER && Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(lyr))
		{
			Bool canImport = false;
			if (lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
			{
				NN<Map::TileMapLayer> layer = NN<Map::TileMapLayer>::ConvertFrom(lyr->layer);
				NN<Map::TileMap> tileMap = layer->GetTileMap();
				if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
				{
					NN<Map::OSM::OSMTileMap> osm = NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
					canImport = osm->HasSPackageFile();
				}
			}
			me->popNode = item;
			me->mnuLayer->SetItemEnabled(MNU_LAYER_REPLAY, lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_CONV, lyr->layer->GetObjectClass() != Map::MapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_EDIT, lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_CONV_CSYS, lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_VECTOR_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_EXPAND, lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_TILEDOWN, lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_IMPORT_TILES, canImport);
			me->mnuLayer->SetItemEnabled(MNU_LAYER_OPTIMIZE_FILE, canImport);
			me->mnuLayer->ShowMenu(me, cursorPos);
		}
		else if (ind->itemType == Map::MapEnv::IT_GROUP)
		{
			me->popNode = item;
			me->mnuGroup->SetItemEnabled(MNU_GROUP_REMOVE, (ind->item != 0));
			me->mnuGroup->ShowMenu(me, cursorPos);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnCtrlFormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRGISForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISForm>();
	me->ctrlForm = 0;
	if (!me->pauseUpdate)
	{
		me->mapCtrl->HideMarker();
		me->mapCtrl->SetSelectedVector(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnSubFormClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<SSWR::AVIRead::AVIRGISForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISForm>();
	me->subForms.RemoveAt(me->subForms.IndexOf(frm));
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnMapLayerUpdated(AnyType userObj)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	me->mapLyrUpdated = true;
	me->mapCtrl->UpdateMap();
	me->mapCtrl->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimeScrolled(AnyType userObj, UOSInt newVal)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	if (me->useTime)
	{
		me->currTime = (OSInt)newVal + me->timeRangeStart;
		me->env->SetCurrTimeTS(0, me->currTime);
		OnMapLayerUpdated(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimeChecked(AnyType userObj, Bool newState)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
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
		OnMapLayerUpdated(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTreeDrag(AnyType userObj, NN<UI::GUIMapTreeView::ItemIndex> dragItem, NN<UI::GUIMapTreeView::ItemIndex> dropItem)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	if (dragItem->group.IsNull() && dragItem->index == INVALID_INDEX)
		return;
	if (dropItem->itemType == Map::MapEnv::IT_GROUP)
	{
		me->env->MoveItem(dragItem->group, dragItem->index, Optional<Map::MapEnv::GroupItem>::ConvertFrom(dropItem->item), me->env->GetItemCount(Optional<Map::MapEnv::GroupItem>::ConvertFrom(dropItem->item)));
	}
	else
	{
		me->env->MoveItem(dragItem->group, dragItem->index, dropItem->group, dropItem->index);
	}
	me->mapTree->UpdateTree();
	me->mapCtrl->UpdateMap();
	me->mapCtrl->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnVAngleScrolled(AnyType userObj, UOSInt newVal)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	me->mapCtrl->SetVAngle(UOSInt2Double(newVal) * Math::PI / 180);
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnHAngleScrolled(AnyType userObj, UOSInt newVal)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	me->mapCtrl->SetHAngle(UOSInt2Double(newVal) * Math::PI / 180);
	me->mapCtrl->UpdateMap();
	me->mapCtrl->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRGISForm::OnTimerTick(AnyType userObj)
{
	NN<AVIRead::AVIRGISForm> me = userObj.GetNN<AVIRead::AVIRGISForm>();
	if (me->mapUpdTChanged)
	{
		me->mapUpdTChanged = false;
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrDouble(sbuff, me->mapUpdT);
		me->txtTimeUsed->SetText(CSTRP(sbuff, sptr));
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
	UnsafeArray<UTF8Char> sptr;
	sptr = this->env->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("GISForm - ")));
	this->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRGISForm::CloseCtrlForm(Bool closing)
{
	NN<UI::GUIForm> ctrlForm;
	if (this->ctrlForm.SetTo(ctrlForm))
	{
		ctrlForm->Close();
		if (!closing)
		{
			this->HideMarker();
			this->SetSelectedVector(0);
		}
	}
}

void SSWR::AVIRead::AVIRGISForm::SetCtrlForm(NN<UI::GUIForm> frm, Optional<UI::GUITreeView::TreeItem> item)
{
	this->CloseCtrlForm(false);
	this->ctrlItem = item;
	this->ctrlForm = frm;
	frm->HandleFormClosed(OnCtrlFormClosed, this);
	frm->Show();
}

Bool SSWR::AVIRead::AVIRGISForm::ParseObject(NN<IO::ParsedObject> pobj)
{
	NN<Parser::ParserList> parsers = this->core->GetParserList();
	NN<IO::ParsedObject> nnpobj;
	if (parsers->ParseObject(pobj).SetTo(nnpobj))
	{
		if (nnpobj->GetParserType() == IO::ParserType::MapLayer)
		{
			this->AddLayer(NN<Map::MapDrawLayer>::ConvertFrom(nnpobj));
			return true;
		}
		else
		{
			Bool succ = ParseObject(nnpobj);
			nnpobj.Delete();
			return succ;
		}
	}
	return false;
}

void SSWR::AVIRead::AVIRGISForm::OpenURL(Text::CStringNN url, Text::CString customName)
{
	NN<Net::WebBrowser> browser = this->core->GetWebBrowser();
	NN<IO::StreamData> fd;
	Text::CStringNN nncustomeName;
	if (browser->GetData(url, false, 0).SetTo(fd))
	{
		if (customName.SetTo(nncustomeName) && nncustomeName.leng > 0)
		{
			fd->SetFullName(nncustomeName);
		}
		NN<IO::ParsedObject> pobj;
		if (this->core->GetParserList()->ParseFile(fd).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::MapLayer)
			{
				this->AddLayer(NN<Map::MapDrawLayer>::ConvertFrom(pobj));
			}
			else
			{
				this->core->OpenObject(pobj);
			}
		}
		fd.Delete();
	}
}

void SSWR::AVIRead::AVIRGISForm::HKOPortal(Text::CStringNN listFile, Text::CStringNN filePath)
{
	Data::DateTime dt;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sarr[3];
	UnsafeArrayOpt<UTF8Char> dateStr;
	UnsafeArrayOpt<UTF8Char> dateStrEnd;
	UnsafeArrayOpt<UTF8Char> timeStr;
	UnsafeArrayOpt<UTF8Char> timeStrEnd;
	Text::StringBuilderUTF8 sb;
	NN<Net::HTTPClient> cli;
	dt.SetCurrTimeUTC();
	sb.AppendC(UTF8STRC("https://maps.weather.gov.hk/gis-portal/web/data/dirList/"));
	sb.Append(listFile);
	sb.AppendC(UTF8STRC("?t="));
	sb.AppendI64(dt.ToTicks());
	cli = Net::HTTPClient::CreateConnect(this->core->GetTCPClientFactory(), this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	{
		Text::UTF8Reader reader(cli);
		dateStr = 0;
		dateStrEnd = 0;
		timeStr = 0;
		timeStrEnd = 0;
		while (true)
		{
			sb.ClearStr();
			if (!reader.ReadLine(sb, 4096))
				break;
			if (sb.GetLength() < 30 && Text::StrSplit(sarr, 3, sb.v, ',') == 2)
			{
				dateStr = sbuff;
				dateStrEnd = Text::StrConcat(dateStr.Ptr(), sarr[0]);
				timeStr = dateStrEnd.Ptr() + 1;
				timeStrEnd = Text::StrConcat(timeStr.Ptr(), sarr[1]);
			}
		}
	}
	cli.Delete();
	if (dateStr.NotNull() && timeStr.NotNull())
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("https://maps.weather.gov.hk/gis-portal/web/data/"));
		sb.AppendP(dateStr.Ptr(), dateStrEnd.Ptr());
		sb.Append(filePath);
		sb.AppendP(timeStr.Ptr(), timeStrEnd.Ptr());
		sb.AppendC(UTF8STRC("/index.kml?t="));
		sb.AppendI64(dt.ToTicks());
		this->OpenURL(sb.ToCString(), CSTR("https://maps.weather.gov.hk/gis-portal/web/index.kml"));
	}
}

void SSWR::AVIRead::AVIRGISForm::OpenCSV(Text::CStringNN url, UInt32 codePage, Text::CStringNN name, Text::CStringNN nameCol, Text::CStringNN latCol, Text::CStringNN lonCol)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->core->GetTCPClientFactory(), this->ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() == Net::WebStatus::SC_OK)
	{
		NN<Map::MapDrawLayer> lyr;
		if (lyr.Set(Map::CSVMapParser::ParseAsPoint(cli, codePage, name, nameCol, latCol, lonCol, Math::CoordinateSystemManager::CreateWGS84Csys())))
		{
			this->AddLayer(lyr);
		}
	}
	cli.Delete();
}

SSWR::AVIRead::AVIRGISForm::AVIRGISForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Map::MapView> view) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
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
	this->mapUpdTChanged = false;
	this->mapUpdT = 0;
	this->pauseUpdate = false;
	this->mapLyrUpdated = false;

	this->wgs84CSys = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->UpdateTitle();
	this->SetFont(0, 0, 8.25, false);

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetArea(0, 0, 100, 24, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlStatus = ui->NewPanel(*this);
	this->pnlStatus->SetArea(0, 0, 100, 19, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtScale = ui->NewTextBox(pnlStatus, CSTR(""));
	this->txtScale->SetRect(0, 0, 100, 19, false);
	this->txtScale->SetReadOnly(true);
	this->txtScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->txtLatLon = ui->NewTextBox(pnlStatus, CSTR(""));
	this->txtLatLon->SetRect(0, 0, 250, 19, false);
	this->txtLatLon->SetReadOnly(true);
	this->txtLatLon->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->txtUTMGrid = ui->NewTextBox(pnlStatus, CSTR(""));
	this->txtUTMGrid->SetRect(0, 0, 150, 19, false);
	this->txtUTMGrid->SetReadOnly(true);
	this->txtUTMGrid->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->txtTimeUsed = ui->NewTextBox(pnlStatus, CSTR(""));
	this->txtTimeUsed->SetRect(0, 0, 100, 19, false);
	this->txtTimeUsed->SetReadOnly(true);
	this->txtTimeUsed->SetDockType(UI::GUIControl::DOCK_RIGHT);

	this->tbScale = ui->NewTrackBar(this->pnlControl, 0, 65535, 0);
	this->tbScale->SetRect(0, 0, 192, 24, false);
	this->tbScale->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->tbScale->HandleScrolled(OnScaleScrolled, this);
	this->tbTimeRange = ui->NewTrackBar(this->pnlControl, 0, 0, 0);
	this->tbTimeRange->SetRect(150, 0, 100, 24, false);
	this->tbTimeRange->HandleScrolled(OnTimeScrolled, this);
	this->chkTime = ui->NewCheckBox(this->pnlControl, CSTR("Unavailable"), false);
	this->chkTime->SetEnabled(false);
	this->chkTime->SetRect(0, 0, 150, 24, false);
	this->chkTime->HandleCheckedChange(OnTimeChecked, this);
	this->lblVAngle = ui->NewLabel(this->pnlControl, CSTR("VAngle"));
	this->lblVAngle->SetRect(250, 0, 50, 23, false);
	this->tbVAngle = ui->NewTrackBar(this->pnlControl, 0, 90, 0);
	this->tbVAngle->SetRect(300, 0, 100, 23, false);
	this->tbVAngle->HandleScrolled(OnVAngleScrolled, this);
	this->lblHAngle = ui->NewLabel(this->pnlControl, CSTR("HAngle"));
	this->lblHAngle->SetRect(400, 0, 50, 23, false);
	this->tbHAngle = ui->NewTrackBar(this->pnlControl, 0, 360, 0);
	this->tbHAngle->SetRect(450, 0, 100, 23, false);
	this->tbHAngle->HandleScrolled(OnHAngleScrolled, this);

	NEW_CLASSNN(this->mapTree, UI::GUIMapTreeView(ui, *this, env));
	this->mapTree->SetRect(0, 0, 200, 10, false);
	this->mapTree->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->mapTree->HandleRightClick(OnTreeRightClick, this);
	this->mapTree->SetDragHandler(OnTreeDrag, this);
	this->splitter = ui->NewHSplitter(*this, 3, false);
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->envRenderer, Map::DrawMapRenderer(core->GetDrawEngine(), env, color, this->colorSess, Map::DrawMapRenderer::DT_PIXELDRAW));
	NEW_CLASSNN(this->mapCtrl, UI::GUIMapControl(ui, *this, this->core->GetDrawEngine(), env->GetBGColor(), this->envRenderer, view, this->colorSess));
	this->mapCtrl->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapCtrl->HandleScaleChanged(OnMapScaleChanged, this);
	this->mapCtrl->HandleMouseMove(OnMapMouseMove, this);
	this->mapCtrl->HandleMouseDown(OnMapMouseDown, this);
	this->mapCtrl->HandleMouseUp(OnMapMouseUp, this);
	this->mapCtrl->HandleMapUpdated(OnMapUpdated, this);

	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	NN<UI::GUIMenu> mnu = this->mnuMain->AddSubMenu(CSTR("&Map"));
	mnu->AddItem(CSTR("&Save"), MNU_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem(CSTR("&Copy Lat/Lon"), MNU_COPY_LATLON, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_L);
	mnu->AddItem(CSTR("&Print..."), MNU_PRINT, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_P);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Add Layer"));
	NN<UI::GUIMenu> mnu2 = mnu->AddSubMenu(CSTR("&MTK GPS Tracker"));
	mnu2->AddItem(CSTR("From &Device"), MNU_MTK_GPS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("From &File"), MNU_MTK_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("From Google Polyline String"), MNU_GOOGLE_POLYLINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("From &File"), MNU_OPEN_FILE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu->AddItem(CSTR("Tile Map Service"), MNU_TMS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Web Map Tile Service"), MNU_WMTS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Web Map Service"), MNU_WMS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Web Feature Service"), MNU_WFS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("ESRI MapServer"), MNU_ESRI_MAP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Regional Map Source"), MNU_REGIONAL_MAP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddSeperator();
	mnu2 = mnu->AddSubMenu(CSTR("HK Data"));
	mnu2->AddItem(CSTR("Recyclable Collection Points"), MNU_HK_WASTELESS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HKE EV Charging Station (EN)"), MNU_HK_HKE_EV_CHARGING_EN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HK Parking Vacancy"), MNU_HK_PARKING_VACANCY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("HK Geodata WMS"), MNU_HK_GEODATA_WMS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	NN<UI::GUIMenu> mnu3 = mnu2->AddSubMenu(CSTR("HKTD"));
	mnu3->AddItem(CSTR("Traffic"), MNU_HKTD_TRAFFIC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Tonnes Sign"), MNU_HKTD_TONNES_SIGN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3 = mnu2->AddSubMenu(CSTR("HKO"));
	NN<UI::GUIMenu> mnu4 = mnu3->AddSubMenu(CSTR("Radar"));
	mnu4->AddItem(CSTR("64km"), MNU_HKO_RADAR_64, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("128km"), MNU_HKO_RADAR_128, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("256km"), MNU_HKO_RADAR_256, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu3->AddItem(CSTR("Tropical Cyclone"), MNU_HKO_CYCLONE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4 = mnu3->AddSubMenu(CSTR("Current"));
	mnu4->AddItem(CSTR("Radar 64km"), MNU_HKO_CURR_RADAR_64, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Radar 128km"), MNU_HKO_CURR_RADAR_128, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Radar 256km"), MNU_HKO_CURR_RADAR_256, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Satellite"), MNU_HKO_CURR_SATELLITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Satellite (IR1-L1B-10)"), MNU_HKO_CURR_SATELLITE_IR1_L1B_10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Satellite (DC-10)"), MNU_HKO_CURR_SATELLITE_DC_10, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Sand"), MNU_HKO_CURR_SAND, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu4->AddItem(CSTR("Weather Chart"), MNU_HKO_CURR_WEATHER_CHART, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Device"));
	mnu->AddItem(CSTR("GPS Tracker"), MNU_GPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("MTK GPS Tracker"), MNU_MTKGPS_TRACKER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Tools"));
	mnu->AddItem(CSTR("GPS Simulator"), MNU_GPS_SIMULATOR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Distance Tool"), MNU_DISTANCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Random Loc"), MNU_RANDOMLOC, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	NEW_CLASSNN(this->mnuLayer, UI::GUIPopupMenu());
	this->mnuLayer->SetMenuForm(this);
	this->mnuLayer->AddItem(CSTR("&Add Group"), MNU_LAYER_ADD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("&Remove"), MNU_LAYER_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Make Center"), MNU_LAYER_CENTER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Fit to Bounds"), MNU_LAYER_FIT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Properties"), MNU_LAYER_PROP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Show Full Path"), MNU_LAYER_PATH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Search"), MNU_LAYER_SEARCH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Shortest Path"), MNU_LAYER_SHORTESTPATH, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Assign Coord Sys"), MNU_LAYER_ASSIGN_CSYS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Convert to Coord Sys"), MNU_LAYER_CONV_CSYS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddSeperator();
	this->mnuLayer->AddItem(CSTR("&Save"), MNU_LAYER_SAVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Combine Layer"), MNU_LAYER_COMBINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Expand Layer"), MNU_LAYER_EXPAND, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Convert to editable layer"), MNU_LAYER_CONV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Edit Layer"), MNU_LAYER_EDIT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Multiply Coordinates"), MNU_LAYER_MUL_COORD, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddSeperator();
	this->mnuLayer->AddItem(CSTR("Replay"), MNU_LAYER_REPLAY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Open DB"), MNU_LAYER_OPENDB, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Tile Downloader"), MNU_LAYER_TILEDOWN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Import Tiles"), MNU_LAYER_IMPORT_TILES, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Optimize File"), MNU_LAYER_OPTIMIZE_FILE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuLayer->AddItem(CSTR("Query"), MNU_LAYER_QUERY, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASSNN(this->mnuGroup, UI::GUIPopupMenu());
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
	this->CloseCtrlForm(true);
	NN<UI::GUIForm> frm;
	i = this->subForms.GetCount();
	while (i-- > 0)
	{
		if (this->subForms.GetItem(i).SetTo(frm))
		{
			frm->Close();
		}
	}
	this->ui->ProcessMessages();
	this->mnuLayer.Delete();
	this->mnuGroup.Delete();
	this->envRenderer.Delete();
	this->env.Delete();
	this->wgs84CSys.Delete();
	this->ssl.Delete();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UTF8Char sbuff2[32];
	UnsafeArray<UTF8Char> sptr2;
	NN<UI::GUIForm> frm;
	NN<Math::CoordinateSystem> csys;
	NN<UI::GUITreeView::TreeItem> item;
	switch (cmdId)
	{
	case MNU_SAVE:
		this->core->SaveData(*this, this->env, L"SaveMapEnv");
		break;
	case MNU_COPY_LATLON:
		sbuff[0] = 0;
		sptr = this->txtUTMGrid->GetText(sbuff).Or(sbuff);
		UI::Clipboard::SetString(this->hwnd, CSTRP(sbuff, sptr));
		break;
	case MNU_PRINT:
		{
			NN<Media::Printer> printer;
			if (this->core->SelectPrinter(this).SetTo(printer))
			{
				this->printer = printer;
				NN<Media::PrintDocument> doc;
				if (!printer->StartPrint(*this, this->core->GetDrawEngine()).SetTo(doc))
				{
					printer.Delete();
					this->ui->ShowMsgOK(CSTR("Error in printing the map"), CSTR("GISForm"), this);
				}
				else
				{
					doc->WaitForEnd();
					printer->EndPrint(doc);
				}
//				DEL_CLASS(printer);
			}
		}
		break;
	case MNU_GROUP_ADD:
		if (this->popNode.SetTo(item))
		{
			this->mapTree->AddSubGroup(item);
		}
		break;
	case MNU_GROUP_REMOVE:
		if (this->popNode.SetTo(item))
		{
			if (this->ctrlItem == item.Ptr()) this->CloseCtrlForm(false);
			this->mapTree->RemoveItem(item);
			this->UpdateTimeRange();
			this->mapCtrl->UpdateMap();
			this->mapCtrl->Redraw();
		}
		break;
	case MNU_GROUP_NEW_IMAGE:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::VectorLayer> layer;
			UnsafeArrayOpt<const UTF8Char> cols = (const UTF8Char*)"Name";
			NEW_CLASSNN(layer, Map::VectorLayer(Map::DRAW_LAYER_IMAGE, CSTR("Image Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, nullptr));
			this->env->AddLayer(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POINT:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::VectorLayer> layer;
			UnsafeArrayOpt<const UTF8Char> cols = (const UTF8Char*)"Name";
			NEW_CLASSNN(layer, Map::VectorLayer(Map::DRAW_LAYER_POINT, CSTR("Point Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, nullptr));
			this->env->AddLayer(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POLYLINE:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::VectorLayer> layer;
			UnsafeArrayOpt<const UTF8Char> cols = (const UTF8Char*)"Name";
			NEW_CLASSNN(layer, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, CSTR("Polyline Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, nullptr));
			this->env->AddLayer(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_NEW_POLYGON:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::VectorLayer> layer;
			UnsafeArrayOpt<const UTF8Char> cols = (const UTF8Char*)"Name";
			NEW_CLASSNN(layer, Map::VectorLayer(Map::DRAW_LAYER_POLYGON, CSTR("Polygon Layer"), 1, &cols, this->env->GetCoordinateSystem()->Clone(), 0, nullptr));
			this->env->AddLayer(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), layer, true);
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->mapTree->UpdateTree();
		}
		break;
	case MNU_GROUP_CENTER:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			Math::RectAreaDbl bounds;
			if (this->env->GetBounds(Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item), bounds))
			{
				this->mapCtrl->PanToMapXY(bounds.GetCenter());
			}
		}
		break;
	case MNU_GROUP_QUERY:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<UI::GUIForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISGroupQueryForm(0, this->ui, this->core, *this, this->env, Optional<Map::MapEnv::GroupItem>::ConvertFrom(ind->item)));
			this->SetCtrlForm(frm, item);
		}
		break;
	case MNU_LAYER_ADD:
		if (this->popNode.SetTo(item))
		{
			this->mapTree->AddSubGroup(item);
		}
		break;
	case MNU_LAYER_REMOVE:
		if (this->popNode.SetTo(item))
		{
			if (this->ctrlItem == this->popNode) this->CloseCtrlForm(false);
			this->mapTree->RemoveItem(item);
			this->UpdateTimeRange();
			this->mapCtrl->UpdateMap();
			this->mapCtrl->Redraw();
		}
		break;
	case MNU_LAYER_CENTER:
		if (this->popNode.SetTo(item))
		{
			Math::RectAreaDbl bounds;
			NN<Math::CoordinateSystem> envCSys;
			NN<Math::CoordinateSystem> lyrCSys;
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(this->env->GetItem(ind->group, ind->index)).SetTo(lyr))
			{
				if (lyr->layer->GetBounds(bounds))
				{
					Math::Coord2DDbl center = bounds.GetCenter();
					envCSys = this->env->GetCoordinateSystem();
					lyrCSys = lyr->layer->GetCoordinateSystem();
					if (!envCSys->Equals(lyrCSys))
					{
						center = Math::CoordinateSystem::Convert(lyrCSys, envCSys, center);
					}
					this->mapCtrl->PanToMapXY(center);
				}
			}
		}
		break;
	case MNU_LAYER_FIT:
		if (this->popNode.SetTo(item))
		{
			Math::RectAreaDbl bounds;
			NN<Math::CoordinateSystem> envCSys;
			NN<Math::CoordinateSystem> lyrCSys;
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(this->env->GetItem(ind->group, ind->index)).SetTo(lyr))
			{
				if (lyr->layer->GetBounds(bounds))
				{
					envCSys = this->env->GetCoordinateSystem();
					lyrCSys = lyr->layer->GetCoordinateSystem();
					if (!envCSys->Equals(lyrCSys))
					{
						bounds.min = Math::CoordinateSystem::Convert(lyrCSys, envCSys, bounds.min);
						bounds.max = Math::CoordinateSystem::Convert(lyrCSys, envCSys, bounds.max);
					}
					this->mapCtrl->ZoomToRect(bounds);
				}
			}
		}
		break;
	case MNU_LAYER_PROP:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			AVIRGISPropForm frm(0, this->ui, this->core, this->env, ind->group, ind->index);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->mapCtrl->UpdateMap();
				this->mapCtrl->Redraw();
			}
		}
		break;
	case MNU_LAYER_PATH:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(lyr))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(lyr->layer->GetName());
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("Layer Path"), this);
			}
		}
		break;
	case MNU_LAYER_SEARCH:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			Map::MapEnv::LayerItem setting;
			this->env->GetLayerProp(setting, ind->group, ind->index);
			NN<Map::MapEnv::LayerItem> layer;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(layer))
			{
				NN<Text::SearchIndexer> searching;
				if (layer->layer->CreateSearchIndexer(this->ta, setting.labelCol).SetTo(searching))
				{
					NN<AVIRGISSearchForm> frm;
					NEW_CLASSNN(frm, AVIRGISSearchForm(0, ui, this->core, *this, layer->layer, searching, setting.labelCol, setting.flags));
					this->SetCtrlForm(frm, item);
				}
			}
		}
		break;
	case MNU_LAYER_SHORTESTPATH:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			Map::MapEnv::LayerItem setting;
			this->env->GetLayerProp(setting, ind->group, ind->index);
			NN<Map::MapEnv::LayerItem> layer;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(layer))
			{
				if (layer->layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE || layer->layer->GetLayerType() == Map::DRAW_LAYER_POLYLINE3D)
				{
					NN<AVIRGISShortestPathForm> frm;
					NEW_CLASSNN(frm, AVIRGISShortestPathForm(0, ui, this->core, *this, layer->layer));
					this->SetCtrlForm(frm, item);
				}
			}
		}
		break;
	case MNU_LAYER_SAVE:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (ind->itemType == Map::MapEnv::IT_LAYER && Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(lyr))
			{
				this->core->SaveData(*this, lyr->layer, L"SaveMapLayer");
			}
		}
		break;
	case MNU_LAYER_COMBINE:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(this->env->GetItem(ind->group, ind->index)).SetTo(lyr))
			{
				Data::ArrayListNN<Map::MapDrawLayer> layers;
				this->env->GetLayersOfType(layers, lyr->layer->GetLayerType());
				AVIRGISCombineForm frm(0, this->ui, this->core, layers);
				if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
				{
					NN<Map::MapDrawLayer> newLyr;
					if (frm.GetCombinedLayer().SetTo(newLyr))
					{
						this->env->AddLayer(0, newLyr, true);
						newLyr->AddUpdatedHandler(OnMapLayerUpdated, this);
						this->mapTree->UpdateTree();
						this->mapCtrl->UpdateMap();
						this->mapCtrl->Redraw();
					}
				}
			}
		}
		break;
	case MNU_LAYER_CONV:
		if (this->popNode.SetTo(item))
		{
			NN<UI::GUIMapTreeView::ItemIndex> ind = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
			NN<Map::MapEnv::LayerItem> lyr;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(ind->item).SetTo(lyr))
			{
				NN<Map::VectorLayer> nlyr = lyr->layer->CreateEditableLayer();
				this->env->ReplaceLayer(ind->group, ind->index, nlyr, true);
				this->mapTree->UpdateTree();
			}
		}
		break;
	case MNU_LAYER_EDIT:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				Map::DrawLayerType lyrType = lyr->GetLayerType();
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_VECTOR_LAYER)
				{
					NN<UI::GUIForm> frm;
					if (lyrType == Map::DRAW_LAYER_IMAGE)
					{
						NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISEditImageForm(0, this->ui, this->core, NN<Map::VectorLayer>::ConvertFrom(lyr), *this));
						this->SetCtrlForm(frm, item);
					}
					else if (lyrType == Map::DRAW_LAYER_MIXED || lyrType == Map::DRAW_LAYER_POLYGON || lyrType == Map::DRAW_LAYER_POLYLINE)
					{
						NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISEditVectorForm(0, this->ui, this->core, NN<Map::VectorLayer>::ConvertFrom(lyr), *this));
						this->SetCtrlForm(frm, item);
					}
					else if (lyrType == Map::DRAW_LAYER_POINT3D || lyrType == Map::DRAW_LAYER_POINT)
					{
						NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISEditPointForm(0, this->ui, this->core, NN<Map::VectorLayer>::ConvertFrom(lyr), *this));
						this->SetCtrlForm(frm, item);
					}
				}
			}
		}
		break;
	case MNU_LAYER_MUL_COORD:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GEOPACKAGE)
				{
					UtilUI::TextInputDialog dlg(0, this->ui, this->core->GetMonitorMgr(), CSTR("Multiply Coordinate"), CSTR("Please input value floating point value to multiply"));
					dlg.SetInputString(CSTR("1.0"));
					if (dlg.ShowDialog(this))
					{
						Text::StringBuilderUTF8 sb;
						if (dlg.GetInputString(sb))
						{
							Double v = sb.ToDoubleOr(0);
							if (v > 0)
							{
								((Map::GeoPackageLayer*)lyr.Ptr())->MultiplyCoordinates(v);
							}
							else
							{
								this->ui->ShowMsgOK(CSTR("Input is not valid floating point number"), CSTR("Multiply Coordinate"), this);
							}
						}
						else
						{
							this->ui->ShowMsgOK(CSTR("Error in getting input string"), CSTR("Multiply Coordinate"), this);
						}
					}
				}
			}
		}
		break;
	case MNU_LAYER_REPLAY:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				NN<UI::GUIForm> frm;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_GPS_TRACK)
				{
					NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISReplayForm(0, this->ui, this->core, NN<Map::GPSTrack>::ConvertFrom(lyr), *this));
					this->SetCtrlForm(frm, item);
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("This layer does not support Replay"), CSTR("GIS Form"), this);
				}
			}
		}
		break;
	case MNU_LAYER_OPENDB:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				NEW_CLASSNN(frm, AVIRDBForm(0, ui, this->core, lyr, false));
				this->AddSubForm(frm);
			}
		}
		break;
	case MNU_LAYER_ASSIGN_CSYS:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				Math::RectAreaDbl bounds;
				if (!lyr->GetBounds(bounds)) bounds = Math::RectAreaDbl(0, 0, 0, 0);
				AVIRGISCSysForm frm(0, this->ui, this->core, bounds, lyr->GetCoordinateSystem());
				frm.SetText(CSTR("Assign Coordinate System"));
				if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetCSys().SetTo(csys))
				{
					lyr->SetCoordinateSystem(csys);
					this->mapCtrl->UpdateMap();
					this->mapCtrl->Redraw();
				}
			}
		}
		break;
	case MNU_LAYER_CONV_CSYS:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_VECTOR_LAYER)
				{
					NN<Map::VectorLayer> vec = NN<Map::VectorLayer>::ConvertFrom(lyr);
					Math::RectAreaDbl bounds;
					if (!lyr->GetBounds(bounds)) bounds = Math::RectAreaDbl(0, 0, 0, 0);
					AVIRGISCSysForm frm(0, this->ui, this->core, bounds, lyr->GetCoordinateSystem());
					frm.SetText(CSTR("Convert Coordinate System"));
					if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetCSys().SetTo(csys))
					{
						vec->ConvCoordinateSystem(csys);
						this->mapCtrl->UpdateMap();
						this->mapCtrl->Redraw();
					}
				}
			}
		}
		break;
	case MNU_LAYER_EXPAND:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)
				{
					NN<UI::GUIMapTreeView::ItemIndex> itemInd = item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>();
					this->mapTree->ExpandColl(itemInd);
				}
			}
		}
		break;
	case MNU_LAYER_TILEDOWN:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<UI::GUIForm> frm;
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
				{
					NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISTileDownloadForm(0, this->ui, this->core, NN<Map::TileMapLayer>::ConvertFrom(lyr), *this));
					this->SetCtrlForm(frm, item);
				}
			}
		}
		break;
	case MNU_LAYER_IMPORT_TILES:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
				{
					NN<Map::TileMapLayer> layer = NN<Map::TileMapLayer>::ConvertFrom(lyr);
					NN<Map::TileMap> tileMap = layer->GetTileMap();
					if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
					{
						NN<Map::OSM::OSMTileMap> osm = NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
						if (osm->HasSPackageFile())
						{
							NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISImportTiles", false);
							this->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::PackageFile);
							if (dlg->ShowDialog(this->GetHandle()))
							{
								IO::StmData::FileData fd(dlg->GetFileName(), false);
								NN<IO::PackageFile> pkg;
								if (Optional<IO::PackageFile>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::PackageFile)).SetTo(pkg))
								{
									osm->ImportTiles(pkg);
									pkg.Delete();
								}
							}
							dlg.Delete();
						}
					}
				}
			}
		}
		break;
	case MNU_LAYER_OPTIMIZE_FILE:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<Map::MapDrawLayer> lyr = litem->layer;
				if (lyr->GetObjectClass() == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
				{
					NN<Map::TileMapLayer> layer = NN<Map::TileMapLayer>::ConvertFrom(lyr);
					NN<Map::TileMap> tileMap = layer->GetTileMap();
					if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
					{
						NN<Map::OSM::OSMTileMap> osm = NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
						if (osm->HasSPackageFile())
						{
							NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISOptimizeFile", true);
							dlg->AddFilter(CSTR("*.spk"), CSTR("SPackage File"));
							if (dlg->ShowDialog(this->GetHandle()))
							{
								osm->OptimizeToFile(dlg->GetFileName()->ToCString());
							}
							dlg.Delete();
						}
					}
				}
			}
		}
		break;
	case MNU_LAYER_QUERY:
		if (this->popNode.SetTo(item))
		{
			NN<Map::MapEnv::LayerItem> litem;
			if (Optional<Map::MapEnv::LayerItem>::ConvertFrom(item->GetItemObj().GetNN<UI::GUIMapTreeView::ItemIndex>()->item).SetTo(litem))
			{
				NN<UI::GUIForm> frm;
				NN<Map::MapDrawLayer> lyr = litem->layer;
				NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISQueryForm(0, this->ui, this->core, lyr, *this));
				this->SetCtrlForm(frm, item);
			}
		}
		break;
	case MNU_MTK_GPS:
		{
			NN<IO::SerialPort> port;
			NEW_CLASSNN(port, IO::SerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort(), 115200, IO::SerialPort::PARITY_NONE, true));
			if (!port->IsError())
			{
				IO::Device::MTKGPSNMEA *mtk;
				NN<Map::GPSTrack> trk;
				UnsafeArray<UTF8Char> sptr;
				NEW_CLASS(mtk, IO::Device::MTKGPSNMEA(port, true));
				if (mtk->IsMTKDevice())
				{
					NEW_CLASSNN(trk, Map::GPSTrack(CSTR("MTK_Tracker"), true, 0, nullptr));
					if (mtk->ParseLog(trk))
					{
						Data::DateTime dt;
						trk->GetTrackStartTime(0, dt);
						dt.ToLocalTime();
						if (mtk->QueryFirmware())
						{
							sptr = Text::String::OrEmpty(mtk->GetProductMode())->ConcatTo(sbuff);
							*sptr++ = '_';
						}
						else
						{
							sptr = Text::StrConcatC(sbuff, UTF8STRC("MTK_"));
						}
						sptr = dt.ToString(sptr, "yyyyMMdd");
						trk->SetSourceName(CSTRP(sbuff, sptr));
						this->env->AddLayer(0, trk, true);
						this->mapTree->UpdateTree();
						this->mapCtrl->UpdateMap();
						this->mapCtrl->Redraw();
					}
					else
					{
						this->ui->ShowMsgOK(CSTR("Error in parsing log"), CSTR("MTK Tracker"), this);
						trk.Delete();
					}
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("MTK Tracker not found"), CSTR("MTK Tracker"), this);
				}
				DEL_CLASS(mtk);
			}
			else
			{
				port.Delete();
				this->ui->ShowMsgOK(CSTR("MTK GPS Tracker not found"), CSTR("MTK Tracker"), this);
			}
		}
		break;
	case MNU_MTK_FILE:
		{
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISMTKFile", false);
			dlg->AddFilter(CSTR("*.bin"), CSTR("MTK Binary File"));
			if (dlg->ShowDialog(this->GetHandle()))
			{
				UInt64 fileSize;
				IO::FileStream fs(dlg->GetFileName(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				fileSize = fs.GetLength();
				if (fileSize > 0 && fileSize <= 8388608 && (fileSize & 0xffff) == 0)
				{
					UOSInt i;
					NN<Map::GPSTrack> trk;
					Data::ByteBuffer fileBuff((UOSInt)fileSize);
					fs.Read(fileBuff);
					NEW_CLASSNN(trk, Map::GPSTrack(dlg->GetFileName(), true, 0, 0));
					i = 0;
					while (i < fileSize)
					{
						if (!IO::Device::MTKGPSNMEA::ParseBlock(&fileBuff[i], trk))
						{
							break;
						}
						i += 65536;
					}
					this->AddLayer(trk);
				}
			}
			dlg.Delete();
		}
		break;
	case MNU_GPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(0, this->ui, this->core, true, this->ssl, this->core->GetLog());
			frm.SetText(CSTR("Select GPS Tracker"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<SSWR::AVIRead::AVIRGPSTrackerForm> gpsFrm;
				NN<IO::GPSNMEA> gps;
				NN<Map::GPSTrack> trk;
				NEW_CLASSNN(gps, IO::GPSNMEA(frm.GetStream(), true));
				NEW_CLASSNN(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(0, this->ui, this->core, gps, true));
				this->AddSubForm(gpsFrm);
				NEW_CLASSNN(trk, Map::GPSTrack(CSTR("GPS_Tracker"), true, 0, nullptr));
				gpsFrm->SetGPSTrack(trk.Ptr());
				gpsFrm->SetMapNavigator(this);
				this->AddLayer(trk);
			}
		}
		break;
	case MNU_MTKGPS_TRACKER:
		{
			SSWR::AVIRead::AVIRSelStreamForm frm(0, this->ui, this->core, false, this->ssl, this->core->GetLog());
			frm.SetText(CSTR("Select MTK GPS Tracker"));
			frm.SetInitSerialPort(IO::Device::MTKGPSNMEA::GetMTKSerialPort());
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<SSWR::AVIRead::AVIRGPSTrackerForm> gpsFrm;
				NN<IO::GPSNMEA> gps;
				NN<Map::GPSTrack> trk;
				NEW_CLASSNN(gps, IO::Device::MTKGPSNMEA(frm.GetStream(), true));
				NEW_CLASSNN(gpsFrm, SSWR::AVIRead::AVIRGPSTrackerForm(0, this->ui, this->core, gps, true));
				this->AddSubForm(gpsFrm);
				NEW_CLASSNN(trk, Map::GPSTrack(CSTR("MTK_GPS_Tracker"), true, 0, nullptr));
				gpsFrm->SetGPSTrack(trk.Ptr());
				gpsFrm->SetMapNavigator(this);
				this->AddLayer(trk);
			}
		}
		break;
	case MNU_GOOGLE_POLYLINE:
		{
			SSWR::AVIRead::AVIRGooglePolylineForm frm(0, this->ui, this->core);
			NN<Math::Geometry::LineString> pl;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetPolyline().SetTo(pl))
			{
				NN<Map::VectorLayer> lyr;
				NEW_CLASSNN(lyr, Map::VectorLayer(Map::DRAW_LAYER_POLYLINE, CSTR("Google Polyline"), Math::CoordinateSystemManager::CreateWGS84Csys(), nullptr));
				lyr->AddVector2(pl, (Text::String**)0);
				this->AddLayer(lyr);
			}
		}
		break;
	case MNU_OPEN_FILE:
		{
			SSWR::AVIRead::AVIROpenFileForm frm(0, this->ui, this->core, IO::ParserType::Unknown);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Text::String> fname = frm.GetFileName();
				UOSInt i = fname->IndexOf(':');
				if (i == INVALID_INDEX || i == 1)
				{
					IO::StmData::FileData fd(fname, false);
					if (fd.GetDataSize() == 0)
					{
						this->ui->ShowMsgOK(CSTR("Error in loading file"), CSTR("AVIRead"), this);
					}
					else
					{
						this->core->LoadDataType(fd, 0, frm.GetParserType());
					}
				}
				else
				{
					this->OpenURL(fname->ToCString(), nullptr);
				}
			}
		}
		break;
	case MNU_HKO_RADAR_64:
		this->OpenURL(CSTR("http://www.weather.gov.hk/wxinfo/radars/radar_064_kml/Radar_064k.kml"), nullptr);
		break;
	case MNU_HKO_RADAR_128:
		this->OpenURL(CSTR("http://www.weather.gov.hk/wxinfo/radars/radar_128_kml/Radar_128k.kml"), nullptr);
		break;
	case MNU_HKO_RADAR_256:
		this->OpenURL(CSTR("http://www.weather.gov.hk/wxinfo/radars/radar_256_kml/Radar_256k.kml"), nullptr);
		break;
	case MNU_HKO_CYCLONE:
		{
			NN<Net::HTTPClient> cli;
			Text::UTF8Reader *reader;
			Text::StringBuilderUTF8 sb;
			UnsafeArray<UTF8Char> sptr;
			UTF8Char sbuff[10];
			UOSInt i;
			Data::DateTime dt;
			cli = Net::HTTPClient::CreateConnect(this->core->GetTCPClientFactory(), this->ssl, CSTR("https://www.weather.gov.hk/wxinfo/currwx/tc_gis_list.xml"), Net::WebUtil::RequestMethod::HTTP_GET, false);
			NEW_CLASS(reader, Text::UTF8Reader(cli));
			reader->ReadLine(sb, 4096);
			while (true)
			{
				sb.ClearStr();
				if (!reader->ReadLine(sb, 4096))
					break;
				sptr = sb.v;
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
						this->OpenURL(sb.ToCString(), nullptr);
						sb.ClearStr();
						sb.AppendC(UTF8STRC("https://www.weather.gov.hk/wxinfo/currwx/tc_gis_track_15a_e_"));
						sb.AppendC(sbuff, 4);
						sb.AppendC(UTF8STRC(".xml?rand="));
						sb.AppendI64(dt.ToTicks());
						this->OpenURL(sb.ToCString(), nullptr);
					}
				}
			}
			DEL_CLASS(reader);
			cli.Delete();
		}
		break;

	case MNU_HKO_CURR_RADAR_64:
		this->HKOPortal(CSTR("radar_merge_064.txt"), CSTR("/radar2/064/"));
		break;
	case MNU_HKO_CURR_RADAR_128:
		this->HKOPortal(CSTR("radar_merge_128.txt"), CSTR("/radar2/128/"));
		break;
	case MNU_HKO_CURR_RADAR_256:
		this->HKOPortal(CSTR("radar_merge_256.txt"), CSTR("/radar2/256/"));
		break;
	case MNU_HKO_CURR_SATELLITE:
		this->HKOPortal(CSTR("satellite_TC.txt"), CSTR("/satellite/TC/"));
		break;
	case MNU_HKO_CURR_SATELLITE_IR1_L1B_10:
		this->HKOPortal(CSTR("satellite_IR1-L1B-10.txt"), CSTR("/satellite/IR1-L1B-10/"));
		break;
	case MNU_HKO_CURR_SATELLITE_DC_10:
		this->HKOPortal(CSTR("satellite_DC-10.txt"), CSTR("/satellite/DC-10/"));
		break;
	case MNU_HKO_CURR_SAND:
		this->HKOPortal(CSTR("sand.txt"), CSTR("/sand/"));
		break;
	case MNU_HKO_CURR_WEATHER_CHART:
		this->HKOPortal(CSTR("weather_chart.txt"), CSTR("/weather_chart/"));
		break;
	case MNU_GPS_SIMULATOR:
		{
			NN<UI::GUIForm> frm;
			NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGPSSimulatorForm(0, this->ui, this->core, *this));
			this->SetCtrlForm(frm, 0);
		}
		break;
	case MNU_DISTANCE:
		NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISDistanceForm(0, this->ui, this->core, *this));
		this->AddSubForm(frm);
		break;
	case MNU_HKTD_TRAFFIC:
		{
			SSWR::AVIRead::AVIRGISHKTrafficForm frm(0, this->ui, this->core);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::MapDrawLayer> layer;
				if (frm.GetMapLayer().SetTo(layer))
				{
					this->AddLayer(layer);
				}
			}
		}
		break;
	case MNU_HKTD_TONNES_SIGN:
		{
			SSWR::AVIRead::AVIRGISHKTDTonnesForm frm(0, this->ui, this->core);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::MapDrawLayer> layer;
				if (frm.GetMapLayer().SetTo(layer))
				{
					this->AddLayer(layer);
				}
			}
		}
		break;
	case MNU_RANDOMLOC:
		NEW_CLASSNN(frm, SSWR::AVIRead::AVIRGISRandomLocForm(0, this->ui, this->core, *this));
		this->AddSubForm(frm);
		break;
	case MNU_HK_WASTELESS:
		this->OpenCSV(CSTR("https://www.wastereduction.gov.hk/sites/default/files/wasteless.csv"),
			65001, CSTR("Waste Less"), CSTR("address_en"), CSTR("lat"), CSTR("lgt"));
		break;
	case MNU_HK_HKE_EV_CHARGING_EN:
		this->OpenCSV(CSTR("https://www.hkelectric.com/en/ElectricLiving/ElectricVehicles/Documents/Locations%20of%20HK%20Electric%20EV%20charging%20stations_eng.csv"),
			65001, CSTR("HK Electric EV Charging Station"), CSTR("HK Electric EV Charging Station_Car Park"), CSTR("Latitude"), CSTR("Longitude"));
		break;
	case MNU_HK_PARKING_VACANCY:
		{
			NN<Map::HKParkingVacancy> parking;
			NEW_CLASSNN(parking, Map::HKParkingVacancy(this->core->GetTCPClientFactory(), this->ssl));
			this->AddLayer(parking);
			break;
		}
	case MNU_TMS:
		{
			SSWR::AVIRead::AVIRTMSForm frm(0, this->ui, this->core, this->ssl);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::TileMap> tileMap;
				if (frm.GetTileMap().SetTo(tileMap))
				{
					NN<Map::TileMapLayer> layer;
					NEW_CLASSNN(layer, Map::TileMapLayer(tileMap, this->core->GetParserList()));
					this->AddLayer(layer);
				}
			}
			break;
		}
	case MNU_WMTS:
		{
			SSWR::AVIRead::AVIRWMTSForm frm(0, this->ui, this->core, this->ssl);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::TileMap> tileMap;
				if (frm.GetTileMap().SetTo(tileMap))
				{
					NN<Map::TileMapLayer> layer;
					NEW_CLASSNN(layer, Map::TileMapLayer(tileMap, this->core->GetParserList()));
					this->AddLayer(layer);
				}
			}
			break;
		}
	case MNU_WMS:
		{
			SSWR::AVIRead::AVIRWMSForm frm(0, this->ui, this->core, this->ssl, this->env->GetCoordinateSystem());
			NN<Map::DrawMapService> mapService;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetDrawMapService().SetTo(mapService))
			{
				NN<Map::DrawMapServiceLayer> layer;
				NEW_CLASSNN(layer, Map::DrawMapServiceLayer(mapService));
				this->AddLayer(layer);
			}
			break;
		}
	case MNU_WFS:
		{
			SSWR::AVIRead::AVIRWFSForm frm(0, this->ui, this->core);
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::MapDrawLayer> layer;
				if (frm.LoadLayer().SetTo(layer))
				{
					this->AddLayer(layer);
				}
			}
			break;
		}
	case MNU_ESRI_MAP:
		{
			SSWR::AVIRead::AVIRESRIMapForm frm(0, this->ui, this->core, this->ssl);
			NN<Map::ESRI::ESRIMapServer> esriMap;
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetSelectedMap().SetTo(esriMap))
			{
				Crypto::Hash::CRC32R crc(Crypto::Hash::CRC32::GetPolynormialIEEE());
				UInt8 crcVal[4];
				NN<Map::MapDrawLayer> mapLyr;
				if (esriMap->HasTile())
				{
					NN<Map::ESRI::ESRITileMap> map;
					NN<Text::String> url = esriMap->GetURL();
					crc.Calc(url->v, url->leng);
					crc.GetValue(crcVal);
					sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
					sptr2 = Text::StrInt32(sbuff2, ReadMInt32(crcVal));
					sptr = IO::Path::AppendPath(sbuff, sptr, CSTRP(sbuff2, sptr2));
					*sptr++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
					*sptr = 0;
					NEW_CLASSNN(map, Map::ESRI::ESRITileMap(esriMap, true, CSTRP(sbuff, sptr)));
					NEW_CLASSNN(mapLyr, Map::TileMapLayer(map, this->core->GetParserList()));
					this->AddLayer(mapLyr);
				}
				else
				{
					NEW_CLASSNN(mapLyr, Map::DrawMapServiceLayer(esriMap));
					this->AddLayer(mapLyr);
				}
			}
			break;
		}
	case MNU_REGIONAL_MAP:
		{
			SSWR::AVIRead::AVIRRegionalMapForm frm(0, this->ui, this->core, this->ssl, this->env->GetCoordinateSystem());
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				NN<Map::MapDrawLayer> layer;
				if (frm.GetMapLayer().SetTo(layer))
				{
					this->AddLayer(layer);
				}
			}
			break;
		}
	case MNU_HK_GEODATA_WMS:
		{
			SSWR::AVIRead::AVIRWMSForm frm(0, this->ui, this->core, this->ssl, this->env->GetCoordinateSystem());
			NN<Map::DrawMapService> mapService;
			frm.SetURL(CSTR("https://geodata.gov.hk/geoserver/geodatastore/wms"));
			if (frm.ShowDialog(this) == UI::GUIForm::DR_OK && frm.GetDrawMapService().SetTo(mapService))
			{
				NN<Map::DrawMapServiceLayer> layer;
				NEW_CLASSNN(layer, Map::DrawMapServiceLayer(mapService));
				this->AddLayer(layer);
			}
			break;
		}
	}
}

void SSWR::AVIRead::AVIRGISForm::OnMonitorChanged()
{
	Optional<MonitorHandle> hMonitor = this->GetHMonitor();
	this->colorSess->ChangeMonitor(hMonitor);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	OnMapScaleChanged(this, this->mapCtrl->GetMapScale());
}

void SSWR::AVIRead::AVIRGISForm::OnFocus()
{
	this->ResumeUpdate();
}

void SSWR::AVIRead::AVIRGISForm::AddLayerFromFile(Text::CStringNN fileName)
{
	IO::Path::PathType pathType = IO::Path::GetPathType(fileName);
	NN<Parser::ParserList> parsers = this->core->GetParserList();
	Optional<IO::ParsedObject> pobj = 0;
	NN<IO::ParsedObject> nnpobj;
	if (pathType == IO::Path::PathType::File)
	{
		IO::StmData::FileData fd(fileName, false);
		pobj = parsers->ParseFile(fd);
	}
	else if (pathType == IO::Path::PathType::Directory)
	{
		IO::DirectoryPackage dpkg(fileName);
		pobj = parsers->ParseObject(dpkg);
	}
	if (pobj.SetTo(nnpobj))
	{
		IO::ParserType pt = nnpobj->GetParserType();
		if (pt == IO::ParserType::MapLayer)
		{
			this->AddLayer(NN<Map::MapDrawLayer>::ConvertFrom(nnpobj));
		}
		else if (pt == IO::ParserType::MapEnv)
		{
			////////////////////////////
			pobj.Delete();
		}
		else if (pt == IO::ParserType::ImageList)
		{
			pobj.Delete();
		}
		else
		{
			if (this->ParseObject(nnpobj))
			{
				pobj.Delete();
			}
			else
			{
				pobj.Delete();
			}
		}
	}
}

void SSWR::AVIRead::AVIRGISForm::AddLayer(NN<Map::MapDrawLayer> layer)
{
	layer->AddUpdatedHandler(OnMapLayerUpdated, this);
	if (this->env->GetItemCount(0) == 0)
	{
		Math::RectAreaDbl bounds;
//		OSInt w;
//		OSInt h;
		layer->GetBounds(bounds);
//		this->mapCtrl->GetSize(&w, &h);
//		this->mapCtrl->UpdateMapView(layer->CreateMapView(w, h));
		this->env->AddLayer(0, layer, true);
		this->mapCtrl->PanToMapXY(bounds.GetCenter());
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

void SSWR::AVIRead::AVIRGISForm::AddLayers(NN<::Data::ArrayListNN<Map::MapDrawLayer>> layers)
{
	NN<Map::MapDrawLayer> layer;
	Bool needPan = this->env->GetItemCount(0) == 0;
	UOSInt i;
	UOSInt j;
	Math::RectAreaDbl bounds;

	if (needPan)
	{
		if (layers->GetItem(0).SetTo(layer))
		{
			layer->GetBounds(bounds);
		}
	}

	i = 0;
	j = layers->GetCount();
	while (i < j)
	{
		if (layers->GetItem(i).SetTo(layer))
		{
			layer->AddUpdatedHandler(OnMapLayerUpdated, this);
			this->env->AddLayer(0, layer, true);
		}
		i++;
	}
	this->UpdateTimeRange();
	this->mapTree->UpdateTree();
	if (needPan)
	{
		this->mapCtrl->PanToMapXY(bounds.GetCenter());
	}
	else
	{
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
}

/*void SSWR::AVIRead::AVIRGISForm::AddLayerColl(Map::MapLayerCollection *lyrColl)
{
	WChar wbuff[512];
	WChar *wptr;
	lyrColl->GetSourceName(wbuff);
	wptr = &wbuff[Text::StrLastIndexOfCharC(wbuff, '\\') + 1];
	void *grp = this->env->AddGroup(0, wptr);

	Map::MapDrawLayer *layer;
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

void SSWR::AVIRead::AVIRGISForm::AddSubForm(NN<UI::GUIForm> frm)
{
	frm->HandleFormClosed(OnSubFormClosed, this);
	this->subForms.Add(frm);
	frm->Show();
}

UInt32 SSWR::AVIRead::AVIRGISForm::GetSRID()
{
	return this->env->GetSRID();
}

Bool SSWR::AVIRead::AVIRGISForm::InMap(Math::Coord2DDbl pos)
{
	return this->mapCtrl->InMapMapXY(pos);
}

void SSWR::AVIRead::AVIRGISForm::PanToMap(Math::Coord2DDbl pos)
{
	this->mapCtrl->PanToMapXY(pos);
}

void SSWR::AVIRead::AVIRGISForm::ShowMarker(Math::Coord2DDbl pos)
{
	this->mapCtrl->ShowMarkerMapXY(pos);
}

void SSWR::AVIRead::AVIRGISForm::ShowMarkerDir(Math::Coord2DDbl pos, Double dir, Math::Unit::Angle::AngleUnit unit)
{
	this->mapCtrl->ShowMarkerMapXYDir(pos, dir, unit);
}

void SSWR::AVIRead::AVIRGISForm::HideMarker()
{
	if (!this->pauseUpdate)
	{
		this->mapCtrl->HideMarker();
	}
}

void SSWR::AVIRead::AVIRGISForm::SetSelectedVector(Optional<Math::Geometry::Vector2D> vec)
{
	this->mapCtrl->SetSelectedVector(vec);
}

void SSWR::AVIRead::AVIRGISForm::SetSelectedVectors(NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList)
{
	this->mapCtrl->SetSelectedVectors(vecList);
}

void SSWR::AVIRead::AVIRGISForm::RedrawMap()
{
	if (!this->pauseUpdate)
	{
		this->mapCtrl->UpdateMap();
		this->mapCtrl->Redraw();
	}
}

NN<Math::CoordinateSystem> SSWR::AVIRead::AVIRGISForm::GetCoordinateSystem() const
{
	return this->env->GetCoordinateSystem();
}

Math::Coord2DDbl SSWR::AVIRead::AVIRGISForm::ScnXY2MapXY(Math::Coord2D<OSInt> scnPos)
{
	return this->mapCtrl->ScnXY2MapXY(scnPos);
}

Math::Coord2D<OSInt> SSWR::AVIRead::AVIRGISForm::MapXY2ScnXY(Math::Coord2DDbl mapPos)
{
	return this->mapCtrl->MapXY2ScnXY(mapPos);
}

void SSWR::AVIRead::AVIRGISForm::SetMapCursor(UI::GUIControl::CursorType curType)
{
	if (this->currCursor != curType)
	{
		this->currCursor = curType;
		this->mapCtrl->SetCursor(curType);
	}
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseLDown(MouseEvent evt, AnyType userObj)
{
	this->mouseLDownHdlrs.Add({evt, userObj});
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseLUp(MouseEvent evt, AnyType userObj)
{
	this->mouseLUpHdlrs.Add({evt, userObj});
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseRDown(MouseEvent evt, AnyType userObj)
{
	this->mouseRDownHdlrs.Add({evt, userObj});
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseRUp(MouseEvent evt, AnyType userObj)
{
	this->mouseRUpHdlrs.Add({evt, userObj});
}

void SSWR::AVIRead::AVIRGISForm::HandleMapMouseMove(MouseEvent evt, AnyType userObj)
{
	this->mouseMoveHdlrs.Add({evt, userObj});
}

void SSWR::AVIRead::AVIRGISForm::UnhandleMapMouse(AnyType userObj)
{
	UOSInt i;
	i = this->mouseLDownHdlrs.GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseLDownHdlrs.GetItem(i).userObj)
		{
			this->mouseLDownHdlrs.RemoveAt(i);
		}
	}
	i = this->mouseLUpHdlrs.GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseLUpHdlrs.GetItem(i).userObj)
		{
			this->mouseLUpHdlrs.RemoveAt(i);
		}
	}
	i = this->mouseRDownHdlrs.GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseRDownHdlrs.GetItem(i).userObj)
		{
			this->mouseRDownHdlrs.RemoveAt(i);
		}
	}
	i = this->mouseRUpHdlrs.GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseRUpHdlrs.GetItem(i).userObj)
		{
			this->mouseRUpHdlrs.RemoveAt(i);
		}
	}
	i = this->mouseMoveHdlrs.GetCount();
	while (i-- > 0)
	{
		if (userObj == this->mouseMoveHdlrs.GetItem(i).userObj)
		{
			this->mouseMoveHdlrs.RemoveAt(i);
		}
	}
	if (this->currCursor != UI::GUIControl::CT_ARROW)
	{
		this->currCursor = UI::GUIControl::CT_ARROW;
		this->mapCtrl->SetCursor(this->currCursor);
	}
}

void SSWR::AVIRead::AVIRGISForm::SetKMapEnv(UnsafeArray<const UTF8Char> kmapIP, Int32 kmapPort, Int32 lcid)
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

UnsafeArrayOpt<UTF8Char> SSWR::AVIRead::AVIRGISForm::ResolveAddress(UnsafeArray<UTF8Char> sbuff, Math::Coord2DDbl pos)
{
/*	if (this->kmap == 0)
	{
		return 0;
	}
	return this->kmap->ResolveAddress(sbuff, this->lcid, pos, L"~@#");*/
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
		this->chkTime->SetText(CSTR("Unavailable"));
	}
	else
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
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
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->chkTime->SetText(CSTRP(sbuff, sptr));
	}
}

Bool SSWR::AVIRead::AVIRGISForm::BeginPrint(NN<Media::PrintDocument> doc)
{
	doc->SetDocName(this->env->GetSourceNameObj().Ptr());
	return true;
}

Bool SSWR::AVIRead::AVIRGISForm::PrintPage(NN<Media::DrawImage> printPage)
{
	NN<Map::MapView> view = this->mapCtrl->CloneMapView();
	Map::DrawMapRenderer *renderer;
	NEW_CLASS(renderer, Map::DrawMapRenderer(this->core->GetDrawEngine(), this->env, printPage->GetColorProfile(), 0, Map::DrawMapRenderer::DT_VECTORDRAW));
	view->SetDestImage(printPage);
	renderer->DrawMap(printPage, view, 0);
	DEL_CLASS(renderer);
	view.Delete();
	return false;
}

Bool SSWR::AVIRead::AVIRGISForm::EndPrint(NN<Media::PrintDocument> doc)
{
	this->printer.Delete();
	return true;
}
