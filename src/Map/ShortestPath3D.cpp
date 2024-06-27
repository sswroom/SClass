#include "Stdafx.h"
#include "Map/ShortestPath3D.h"

Map::ShortestPath3D::ShortestPath3D(NN<Math::CoordinateSystem> csys, Double searchDist)
{
	this->csys = csys;
	this->searchDist = searchDist;
}

Map::ShortestPath3D::~ShortestPath3D()
{
	this->csys.Delete();
}

void Map::ShortestPath3D::AddLayer(NN<Map::MapDrawLayer> layer)
{
	NN<Map::GetObjectSess> sess = layer->BeginGetObject();
	layer->EndGetObject(sess);
}
