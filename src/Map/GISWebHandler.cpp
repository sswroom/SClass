#include "Stdafx.h"
#include "Map/GISWebHandler.h"

Map::GISWebHandler::GISWebHandler()
{

}

Map::GISWebHandler::~GISWebHandler()
{
	this->assets.DeleteAll();
}

UOSInt Map::GISWebHandler::AddAsset(NN<Map::MapDrawLayer> layer)
{
	return this->assets.Add(layer);
}
