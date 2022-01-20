#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "UI/MSWindowUI.h"
#include "SSWR/MapReplay/MapReplayForm.h"
#include "Map/IMapDrawLayer.h"
#include "Map/DrawMapRenderer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Map/ResizableTileMapRenderer.h"

#define MNU_MAP 101

void SSWR::MapReplay::MapReplayForm::LoadMap(Int32 mapType)
{
	WChar sbuff[512];
	if (this->tileMap)
	{
		DEL_CLASS(this->tileMap);
	}
	
	IO::Path::GetProcessFileName(sbuff);

	switch (mapType)
	{
	case 0:
		IO::Path::AppendPath(sbuff, L"OSMTile");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.openstreetmap.org/", sbuff, 18, sockf));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.openstreetmap.org/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.openstreetmap.org/");
		break;
	case 1:
		IO::Path::AppendPath(sbuff, L"OSMOpenCycleMap");
        NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile.opencyclemap.org/cycle/", sbuff, 18, sockf));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile.opencyclemap.org/cycle/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile.opencyclemap.org/cycle/");
		break;
	case 2:
		IO::Path::AppendPath(sbuff, L"OSMTransport");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://a.tile2.opencyclemap.org/transport/", sbuff, 18, sockf));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://b.tile2.opencyclemap.org/transport/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://c.tile2.opencyclemap.org/transport/");
		break;
	default:
		IO::Path::AppendPath(sbuff, L"OSMMapQuest");
		NEW_CLASS(tileMap, Map::OSM::OSMTileMap(L"http://otile1.mqcdn.com/tiles/1.0.0/osm/", sbuff, 18, sockf));
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile2.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile3.mqcdn.com/tiles/1.0.0/osm/");
		((Map::OSM::OSMTileMap*)tileMap)->AddAlternateURL(L"http://otile4.mqcdn.com/tiles/1.0.0/osm/");
		break;
	}
}

void SSWR::MapReplay::MapReplayForm::UpdateList()
{
	UOSInt cnt = this->env->GetItemCount(0);
	UOSInt i;
	Map::MapEnv::ItemType itemType;
	const WChar *name;
	this->lbLayers->ClearItems();
	i = 0;
	while (i < cnt)
	{
		void *item = this->env->GetItem(0, i, &itemType);
		if (itemType == Map::MapEnv::IT_LAYER)
		{
			Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)item;
			name = layer->GetName();
			this->lbLayers->AddItem(&name[Text::StrLastIndexOfChar(name, '\\') + 1], layer);
		}
		i++;
	}

}

void __stdcall SSWR::MapReplay::MapReplayForm::OnFileDrop(void *userObj, const WChar **files, OSInt nFiles)
{
	SSWR::MapReplay::MapReplayForm *me = (SSWR::MapReplay::MapReplayForm*)userObj;
	IO::StmData::FileData *fd;
	IO::ParsedObject *pobj;
	IO::ParserType pt;

	OSInt i = 0;
	while (i < nFiles)
	{
		NEW_CLASS(fd, IO::StmData::FileData(files[i], false));
		pobj = me->parsers->ParseFile(fd, &pt);
		DEL_CLASS(fd);
		if (pobj)
		{
			if (pt == IO::ParserType::MapLayer)
			{
				me->env->AddLayer(0, (Map::IMapDrawLayer*)pobj, true);
			}
			else
			{
				DEL_CLASS(pobj);
			}
		}
		i++;
	}
	me->UpdateList();
	me->map->UpdateMap();
	me->Redraw();
}

void __stdcall SSWR::MapReplay::MapReplayForm::OnLayerDblClicked(void *userObj)
{
	SSWR::MapReplay::MapReplayForm *me = (SSWR::MapReplay::MapReplayForm*)userObj;
	void *obj = me->lbLayers->GetSelectedItem();
	if (obj)
	{
		Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)obj;
		if (layer->GetObjectClass() != Map::IMapDrawLayer::OC_GPS_TRACK)
			return;

		if (me->replayForm)
		{
			me->replayForm->Close();
		}

		NEW_CLASS(me->replayForm, SSWR::AVIRead::AVIRGISReplayForm(me->ui->GetHInst(), 0, me->ui, (Map::GPSTrack*)layer, me));
		me->replayForm->Show();
	}
}

SSWR::MapReplay::MapReplayForm::MapReplayForm(UI::MSWindowUI *ui, UI::MSWindowForm *parent, Media::GDIEngine *eng, Parser::ParserList *parsers, Media::ColorManager *colorMgr, Net::SocketFactory *sockf) : UI::MSWindowForm(ui->GetHInst(), parent, 1024, 768, ui)
{
	this->parsers = parsers;
	this->colorMgr = colorMgr;
	this->sockf = sockf;
	this->eng = eng;
	this->tileMap = 0;
	this->currMapType = 0;
	this->replayForm = 0;
	this->colorSess = this->colorMgr->CreateSess(this->GetHMonitor());

	this->SetText(L"MapReplay");
	this->SetFormState(FS_MAXIMIZED);
	this->HandleDropFiles(OnFileDrop, this);
	NEW_CLASS(this->mnu, UI::MSWindowMainMenu());
	this->mnu->AddItem(L"Map", MNU_MAP, 0, 0);
	this->SetMenu(this->mnu);
	this->SetFont(0, 9, false);

	NEW_CLASS(this->lbLayers, UI::MSWindowListBox(ui->GetHInst(), this));
	this->lbLayers->SetRect(0, 0, 200, 100, false);
	this->lbLayers->SetDockType(UI::MSWindowControl::DOCK_LEFT);
	this->lbLayers->HandleDoubleClicked(OnLayerDblClicked, this);
	NEW_CLASS(this->splitter, UI::MSWindowHSplitter(ui->GetHInst(), this, 2, false));

	OSInt i;
	Math::CoordinateSystem *csys = Math::GeographicCoordinateSystem::CreateCoordinateSystem(Math::GeographicCoordinateSystem::GCST_WGS84);
	NEW_CLASS(this->env, Map::MapEnv(L"Untitled", 0xffffffff, csys));
	DEL_CLASS(csys);
	Media::ColorProfile color(Media::ColorProfile::CPT_PDISPLAY);
	i = this->env->AddLineStyle();
	this->env->AddLineStyleLayer(i, 0xff0000ff, 3, 0, 0);
	this->env->SetDefLineStyle(i);
	NEW_CLASS(this->envRenderer, Map::DrawMapRenderer(this->eng, this->env, &color, this->colorSess));
	this->LoadMap(this->currMapType);

	this->mapView = this->env->CreateMapView(1024, 768);
	NEW_CLASS(this->map, UI::MSWindowMapControl(ui->GetHInst(), this, eng, 0xffffffff, this->envRenderer, this->mapView, this->colorSess));
	this->map->SetDockType(UI::MSWindowControl::DOCK_FILL);
}

SSWR::MapReplay::MapReplayForm::~MapReplayForm()
{
	if (this->replayForm)
	{
		this->replayForm->Close();
	}
	map->SetRenderer(0);
	if (tileMap)
	{
		DEL_CLASS(tileMap);
	}
	DEL_CLASS(this->envRenderer);
	DEL_CLASS(this->env);
	this->ClearChildren();
	DEL_CLASS(this->mapView);
	this->colorMgr->DeleteSess(this->colorSess);
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
	return this->map->InMapMapXY(lon, lat);
}

void SSWR::MapReplay::MapReplayForm::PanToMap(Double lat, Double lon)
{
	this->map->PanToMapXY(lon, lat);
}

void SSWR::MapReplay::MapReplayForm::ShowMarker(Double lat, Double lon)
{
	this->map->ShowMarkerMapXY(lon, lat);
}

void SSWR::MapReplay::MapReplayForm::HideMarker()
{
	this->map->HideMarker();
}

void SSWR::MapReplay::MapReplayForm::AddLayer(Map::IMapDrawLayer *layer)
{
	this->env->AddLayer(0, layer, true);
	this->UpdateList();
}

void SSWR::MapReplay::MapReplayForm::SetSelectedVector(Math::Vector2D *vec)
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

WChar *SSWR::MapReplay::MapReplayForm::ResolveAddress(WChar *sbuff, Double lat, Double lon)
{
	return 0;
}
