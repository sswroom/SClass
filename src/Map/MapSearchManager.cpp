#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"
#include "Map/MapLayerData.h"
#include "Map/MapSearchManager.h"

Map::MapSearchManager::MapSearchManager()
{
	NEW_CLASS(this->nameArr, Data::ArrayListICaseStrUTF8());
	NEW_CLASS(this->layerArr, Data::ArrayList<Map::IMapSearchLayer*>());
}

Map::MapSearchManager::~MapSearchManager()
{
	UOSInt i = this->nameArr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->nameArr->RemoveAt(i));
		DEL_CLASS(this->layerArr->RemoveAt(i));
	}
	DEL_CLASS(this->nameArr);
	DEL_CLASS(this->layerArr);
}

Map::IMapSearchLayer *Map::MapSearchManager::LoadLayer(Text::CString fileName)
{
	OSInt i = this->nameArr->SortedIndexOf(fileName.v);
	if (i >= 0)
	{
		return this->layerArr->GetItem((UOSInt)i);
	}
	i = (OSInt)this->nameArr->SortedInsert(Text::StrCopyNewC(fileName.v, fileName.leng));
	Map::MapLayerData *lyr;
	NEW_CLASS(lyr, Map::MapLayerData(fileName));
	this->layerArr->Insert((UOSInt)i, lyr);
	return lyr;
}
