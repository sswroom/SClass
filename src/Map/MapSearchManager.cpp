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
	UOSInt i = this->nameArr.GetCount();
	while (i-- > 0)
	{
		this->nameArr.RemoveAt(i)->Release();
		DEL_CLASS(this->layerArr.RemoveAt(i));
	}
}

Map::MapSearchLayer *Map::MapSearchManager::LoadLayer(Text::CStringNN fileName)
{
	OSInt i = this->nameArr.SortedIndexOfPtr(fileName.v, fileName.leng);
	if (i >= 0)
	{
		return this->layerArr.GetItem((UOSInt)i);
	}
	i = (OSInt)this->nameArr.SortedInsert(Text::String::New(fileName).Ptr());
	Map::MapLayerData *lyr;
	NEW_CLASS(lyr, Map::MapLayerData(fileName));
	this->layerArr.Insert((UOSInt)i, lyr);
	return lyr;
}
