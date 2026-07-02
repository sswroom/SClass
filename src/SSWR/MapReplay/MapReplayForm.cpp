#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/DrawMapRenderer.h"
#include "Map/MapDrawLayer.h"
#include "Map/ResizableTileMapRenderer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/CoordinateSystemManager.h"
#include "SSWR/MapReplay/MapReplayForm.h"
#include "UI/GUICore.h"

#define MNU_MAP 101

void SSWR::MapReplay::MapReplayForm::LoadMap(Int32 mapType)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->tileMap.Delete();
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);

	NN<Map::OSM::OSMTileMap> tileMap;
	switch (mapType)
	{
	case 0:
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTile"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.openstreetmap.org/"), CSTRP(sbuff, sptr), 0, 18, clif, nullptr));
		tileMap->AddAlternateURL(CSTR("http://b.tile.openstreetmap.org/"));
		tileMap->AddAlternateURL(CSTR("http://c.tile.openstreetmap.org/"));
		this->tileMap = tileMap;
		break;
	case 1:
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMOpenCycleMap"));
        NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile.opencyclemap.org/cycle/"), CSTRP(sbuff, sptr), 0, 18, clif, nullptr));
		tileMap->AddAlternateURL(CSTR("http://b.tile.opencyclemap.org/cycle/"));
		tileMap->AddAlternateURL(CSTR("http://c.tile.opencyclemap.org/cycle/"));
		this->tileMap = tileMap;
		break;
	case 2:
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMTransport"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://a.tile2.opencyclemap.org/transport/"), CSTRP(sbuff, sptr), 0, 18, clif, nullptr));
		tileMap->AddAlternateURL(CSTR("http://b.tile2.opencyclemap.org/transport/"));
		tileMap->AddAlternateURL(CSTR("http://c.tile2.opencyclemap.org/transport/"));
		this->tileMap = tileMap;
		break;
	default:
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("OSMMapQuest"));
		NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap(CSTR("http://otile1.mqcdn.com/tiles/1.0.0/osm/"), CSTRP(sbuff, sptr), 0, 18, clif, nullptr));
		tileMap->AddAlternateURL(CSTR("http://otile2.mqcdn.com/tiles/1.0.0/osm/"));
		tileMap->AddAlternateURL(CSTR("http://otile3.mqcdn.com/tiles/1.0.0/osm/"));
		tileMap->AddAlternateURL(CSTR("http://otile4.mqcdn.com/tiles/1.0.0/osm/"));
		this->tileMap = tileMap;
		break;
	}
}

void SSWR::MapReplay::MapReplayForm::UpdateList()
{
	UIntOS cnt = this->env->GetItemCount(nullptr);
	UIntOS i;
	Map::MapEnv::ItemType itemType;
	NN<Text::String> name;
	this->lbLayers->ClearItems();
	i = 0;
	while (i < cnt)
	{
		NN<Map::MapEnv::MapItem> item;
		if (this->env->GetItem(nullptr, i).SetTo(item) && item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem> layer = NN<Map::MapEnv::LayerItem>::ConvertFrom(item);
			name = layer->layer->GetName();
			this->lbLayers->AddItem(name->ToCString().Substring(name->LastIndexOf(IO::Path::PATH_SEPERATOR) + 1), layer->layer);
		}
		i++;
	}

}

void __stdcall SSWR::MapReplay::MapReplayForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::MapReplay::MapReplayForm> me = userObj.GetNN<SSWR::MapReplay::MapReplayForm>();
	NN<IO::StmData::FileData> fd;
	NN<IO::ParsedObject> pobj;
	IO::ParserType pt;

	UIntOS i = 0;
	while (i < files.GetCount())
	{
		NEW_CLASSNN(fd, IO::StmData::FileData(files[i], false));
		if (me->parsers->ParseFile(fd).SetTo(pobj))
		{
			if (pobj->GetParserType() == IO::ParserType::MapLayer)
			{
				me->env->AddLayer(nullptr, NN<Map::MapDrawLayer>::ConvertFrom(pobj), true);
			}
			else
			{
				pobj.Delete();
			}
		}
		fd.Delete();
		i++;
	}
	me->UpdateList();
	me->map->UpdateMap();
	me->Redraw();
}

void __stdcall SSWR::MapReplay::MapReplayForm::OnLayerDblClicked(AnyType userObj)
{
	NN<SSWR::MapReplay::MapReplayForm> me = userObj.GetNN<SSWR::MapReplay::MapReplayForm>();
	NN<Map::MapDrawLayer> layer;
	if (me->lbLayers->GetSelectedItem().GetOpt<Map::MapDrawLayer>().SetTo(layer))
	{
		if (layer->GetObjectClass() != Map::MapDrawLayer::OC_GPS_TRACK)
			return;

		NN<SSWR::AVIRead::AVIRGISReplayForm> replayForm;
		if (me->replayForm.SetTo(replayForm))
		{
			replayForm->Close();
		}

		NEW_CLASSNN(replayForm, SSWR::AVIRead::AVIRGISReplayForm(nullptr, me->ui, NN<Map::GPSTrack>::ConvertFrom(layer), me));
		me->replayForm = replayForm;
		replayForm->Show();
	}
}

SSWR::MapReplay::MapReplayForm::MapReplayForm(NN<UI::GUICore> ui, Optional<UI::GUIForm> parent, NN<Media::DrawEngine> eng, NN<Parser::ParserList> parsers, NN<Media::ColorManager> colorMgr, NN<Net::SocketFactory> sockf) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->parsers = parsers;
	this->colorMgr = colorMgr;
	this->sockf = sockf;
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(sockf));
	this->eng = eng;
	this->tileMap = nullptr;
	this->currMapType = 0;
	this->replayForm = nullptr;
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());

	this->SetText(CSTR("MapReplay"));
	this->SetFormState(FS_MAXIMIZED);
	this->HandleDropFiles(OnFileDrop, this);
	NEW_CLASSNN(this->mnu, UI::GUIMainMenu());
	this->mnu->AddItem(CSTR("Map"), MNU_MAP, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnu);
	this->SetFont(nullptr, 9, false);

	this->lbLayers = ui->NewListBox(*this, false);
	this->lbLayers->SetRect(0, 0, 200, 100, false);
	this->lbLayers->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbLayers->HandleDoubleClicked(OnLayerDblClicked, this);
	this->splitter = ui->NewHSplitter(*this, 2, false);

	IntOS i;
	NN<Math::CoordinateSystem> csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	NEW_CLASSNN(this->env, Map::MapEnv(CSTR("Untitled"), 0xffffffff, csys));
	csys.Delete();
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	i = this->env->AddLineStyle();
	this->env->AddLineStyleLayer(i, 0xff0000ff, 3, nullptr, 0);
	this->env->SetDefLineStyle(i);
	NEW_CLASSNN(this->envRenderer, Map::DrawMapRenderer(this->eng, this->env, color, this->colorSess, Map::DrawMapRenderer::DrawType::DT_PIXELDRAW));
	this->LoadMap(this->currMapType);

	this->mapView = this->env->CreateMapView(Math::Size2DDbl(1024, 768));
	NEW_CLASSNN(this->map, UI::GUIMapControl(ui, *this, eng, 0xffffffff, this->envRenderer, this->mapView, this->colorSess));
	this->map->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::MapReplay::MapReplayForm::~MapReplayForm()
{
	NN<SSWR::AVIRead::AVIRGISReplayForm> replayForm;
	if (this->replayForm.SetTo(replayForm))
	{
		replayForm->Close();
	}
	this->tileMap.Delete();
	this->envRenderer.Delete();
	this->env.Delete();
	this->ClearChildren();
	this->mapView.Delete();
	this->colorMgr->DeleteSess(this->colorSess);
	this->clif.Delete();
}

void SSWR::MapReplay::MapReplayForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_MAP:
		this->currMapType = (this->currMapType + 1) & 3;
		this->LoadMap(this->currMapType);
		break;
	}
}

Bool SSWR::MapReplay::MapReplayForm::InMap(Double lat, Double lon)
{
	return this->map->InMapMapXY(Math::Coord2DDbl(lon, lat));
}

void SSWR::MapReplay::MapReplayForm::PanToMap(Double lat, Double lon)
{
	this->map->PanToMapXY(Math::Coord2DDbl(lon, lat));
}

void SSWR::MapReplay::MapReplayForm::ShowMarker(Double lat, Double lon)
{
	this->map->ShowMarkerMapXY(Math::Coord2DDbl(lon, lat));
}

void SSWR::MapReplay::MapReplayForm::HideMarker()
{
	this->map->HideMarker();
}

void SSWR::MapReplay::MapReplayForm::AddLayer(NN<Map::MapDrawLayer> layer)
{
	this->env->AddLayer(nullptr, layer, true);
	this->UpdateList();
}

void SSWR::MapReplay::MapReplayForm::SetSelectedVector(Optional<Math::Geometry::Vector2D> vec)
{
	this->map->SetSelectedVector(vec);
}

void SSWR::MapReplay::MapReplayForm::SetKMapEnv(const WChar *kmapIP, Int32 kmapPort, Int32 lcid)
{
}

Bool SSWR::MapReplay::MapReplayForm::HasKMap()
{
	return false;
}

WChar *SSWR::MapReplay::MapReplayForm::ResolveAddress(WChar *wbuff, Double lat, Double lon)
{
	return 0;
}
