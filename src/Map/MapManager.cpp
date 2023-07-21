#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"
#include "Map/MapManager.h"

Map::MapManager::MapManager()
{
}

Map::MapManager::~MapManager()
{
	const Data::ArrayList<Map::MapManager::MapLayerInfo*> *arr = this->layerArr.GetValues();
	UOSInt i = arr->GetCount();
	while (i-- > 0)
	{
		Map::MapManager::MapLayerInfo *info = arr->GetItem(i);
		DEL_CLASS(info->layer);
		DEL_CLASS(info->envList);
		MemFree(info);
	}
}

Map::MapDrawLayer *Map::MapManager::LoadLayer(Text::CString fileName, Parser::ParserList *parsers, Map::MapEnv *env)
{
	Map::MapManager::MapLayerInfo *info = this->layerArr.Get(fileName.v);
	if (info)
	{
		if (info->envList->IndexOf(env) == INVALID_INDEX)
		{
			info->envList->Add(env);
		}
		return info->layer;
	}
	IO::ParsedObject *pobj;
	IO::ParserType pt;
	{
		IO::StmData::FileData fd(fileName, false);
		pobj = parsers->ParseFile(fd, &pt);
	}
	if (pt != IO::ParserType::MapLayer)
	{
		if (pobj)
		{
			DEL_CLASS(pobj);
			return 0;
		}
	}
	Map::MapDrawLayer *lyr;
	lyr = (Map::MapDrawLayer*)pobj;
	info = MemAlloc(Map::MapManager::MapLayerInfo, 1);
	NEW_CLASS(info->envList, Data::ArrayList<Map::MapEnv*>());
	info->layer = lyr;
	info->envList->Add(env);
	this->layerArr.Put(fileName.v, info);
	return lyr;
}

void Map::MapManager::ClearMap(Map::MapEnv *env)
{
	const Data::ArrayList<Map::MapManager::MapLayerInfo *> *infoArr = this->layerArr.GetValues();
	UOSInt i = infoArr->GetCount();
	UOSInt j;
	Map::MapManager::MapLayerInfo *info;
	while (i-- > 0)
	{
		info = infoArr->GetItem(i);
		j = info->envList->IndexOf(env);
		if (j != INVALID_INDEX)
		{
			info->envList->RemoveAt(j);
			if (info->envList->GetCount() == 0)
			{
				this->layerArr.Remove(this->layerArr.GetKey(i));
				DEL_CLASS(info->envList);
				DEL_CLASS(info->layer);
				MemFree(info);
			}
		}
	}
}
