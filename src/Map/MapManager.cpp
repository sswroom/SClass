#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Text/MyString.h"
#include "Map/MapManager.h"

Map::MapManager::MapManager()
{
	NEW_CLASS(layerArr, Data::StringUTF8Map<Map::MapManager::MapLayerInfo*>());
}

Map::MapManager::~MapManager()
{
	Data::ArrayList<Map::MapManager::MapLayerInfo*> *arr = layerArr->GetValues();
	OSInt i = arr->GetCount();
	while (i-- > 0)
	{
		Map::MapManager::MapLayerInfo *info = arr->GetItem(i);
		DEL_CLASS(info->layer);
		DEL_CLASS(info->envList);
		MemFree(info);
	}
	DEL_CLASS(layerArr);
}

Map::IMapDrawLayer *Map::MapManager::LoadLayer(const UTF8Char *fileName, Parser::ParserList *parsers, Map::MapEnv *env)
{
	Map::MapManager::MapLayerInfo *info = this->layerArr->Get(fileName);
	if (info)
	{
		if (info->envList->IndexOf(env) == (UOSInt)-1)
		{
			info->envList->Add(env);
		}
		return info->layer;
	}
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	IO::ParsedObject::ParserType pt;
	IO::ParsedObject *pobj = parsers->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (pt != IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		if (pobj)
		{
			DEL_CLASS(pobj);
			return 0;
		}
	}
	Map::IMapDrawLayer *lyr;
	lyr = (Map::IMapDrawLayer*)pobj;
	info = MemAlloc(Map::MapManager::MapLayerInfo, 1);
	NEW_CLASS(info->envList, Data::ArrayList<Map::MapEnv*>());
	info->layer = lyr;
	info->envList->Add(env);
	this->layerArr->Put(fileName, info);
	return lyr;
}

void Map::MapManager::ClearMap(Map::MapEnv *env)
{
	Data::ArrayList<Map::MapManager::MapLayerInfo *> *infoArr = this->layerArr->GetValues();
	OSInt i = infoArr->GetCount();
	OSInt j;
	Map::MapManager::MapLayerInfo *info;
	while (i-- > 0)
	{
		info = infoArr->GetItem(i);
		j = info->envList->IndexOf(env);
		if (j >= 0)
		{
			info->envList->RemoveAt(j);
			if (info->envList->GetCount() == 0)
			{
				this->layerArr->Remove(this->layerArr->GetKey(i));
				DEL_CLASS(info->envList);
				DEL_CLASS(info->layer);
				MemFree(info);
			}
		}
	}
}
