#include "Stdafx.h"
#include "Map/TileMapServiceSource.h"
#include "Math/Unit/Distance.h"
#include "Net/HTTPClient.h"
#include "Text/XMLReader.h"

void Map::TileMapServiceSource::LoadXML()
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, 0, this->tmsURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli == 0)
		return;
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return;
	}
	{
		Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);

	}
	DEL_CLASS(cli);
}

Map::TileMapServiceSource::TileMapServiceSource(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, Text::CString tmsURL)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->tmsURL = Text::String::New(tmsURL);
	this->title = 0;
	this->csys = 0;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->LoadXML();
}

Map::TileMapServiceSource::~TileMapServiceSource()
{
	this->tmsURL->Release();
	SDEL_STRING(this->title);
	TileLayer *layer;
	UOSInt i = this->layers.GetCount();
	while (i-- > 0)
	{
		layer = this->layers.GetItem(i);
		layer->url->Release();
		MemFree(layer);
	}
}

Text::CString Map::TileMapServiceSource::GetName()
{
	return STR_CSTR(this->title);
}

Bool Map::TileMapServiceSource::IsError()
{
	return this->layers.GetCount() == 0;
}

Map::TileMap::TileType Map::TileMapServiceSource::GetTileType()
{
	return Map::TileMap::TT_TMS;
}

UOSInt Map::TileMapServiceSource::GetLevelCount()
{
	return this->layers.GetCount();
}

Double Map::TileMapServiceSource::GetLevelScale(UOSInt level)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer == 0)
	{
		return 0;
	}
	return layer->unitPerPixel / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 1);
}

UOSInt Map::TileMapServiceSource::GetNearestLevel(Double scale)
{
	//////////////////////////
	return 0;
}

UOSInt Map::TileMapServiceSource::GetConcurrentCount()
{
	return 2;
}

Bool Map::TileMapServiceSource::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = this->bounds;
	return true;
}

Math::CoordinateSystem *Map::TileMapServiceSource::GetCoordinateSystem()
{
	return this->csys;
}

Bool Map::TileMapServiceSource::IsMercatorProj()
{
	return false;
}

UOSInt Map::TileMapServiceSource::GetTileSize()
{
	return this->tileWidth;
}

UOSInt Map::TileMapServiceSource::GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids)
{
	///////////////////////////////////////
	return 0;
}

Media::ImageList *Map::TileMapServiceSource::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly)
{
	///////////////////////////////////////
	return 0;
}

UTF8Char *Map::TileMapServiceSource::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	///////////////////////////////////////
	return 0;
}

IO::IStreamData *Map::TileMapServiceSource::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	///////////////////////////////////////
	return 0;
}
