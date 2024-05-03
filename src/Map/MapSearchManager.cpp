#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Map/MapLayerData.h"
#include "Map/MapSearchManager.h"

Map::MapSearchManager::MapSearchManager()
{
}

Map::MapSearchManager::~MapSearchManager()
{
	this->nameArr.FreeAll();
	this->layerArr.DeleteAll();
}

NN<Map::MapSearchLayer> Map::MapSearchManager::LoadLayer(Text::CStringNN fileName)
{
	OSInt i = this->nameArr.SortedIndexOfC(fileName);
	if (i >= 0)
	{
		return this->layerArr.GetItemNoCheck((UOSInt)i);
	}
	i = (OSInt)this->nameArr.SortedInsert(Text::String::New(fileName));
	NN<Map::MapLayerData> lyr;
	NEW_CLASSNN(lyr, Map::MapLayerData(fileName));
	this->layerArr.Insert((UOSInt)i, lyr);
	return lyr;
}
