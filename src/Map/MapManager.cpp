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
	UOSInt i = this->layerArr.GetCount();
	while (i-- > 0)
	{
		NN<Map::MapManager::MapLayerInfo> info = this->layerArr.GetItemNoCheck(i);
		info->layer.Delete();
		info.Delete();
	}
}

Optional<Map::MapDrawLayer> Map::MapManager::LoadLayer(Text::CStringNN fileName, NN<Parser::ParserList> parsers, NN<Map::MapEnv> env)
{
	NN<Map::MapManager::MapLayerInfo> info;
	if (this->layerArr.GetC(fileName).SetTo(info))
	{
		if (info->envList.IndexOf(env) == INVALID_INDEX)
		{
			info->envList.Add(env);
		}
		return info->layer;
	}
	NN<IO::ParsedObject> pobj;
	{
		IO::StmData::FileData fd(fileName, false);
		if (!parsers->ParseFile(fd).SetTo(pobj))
			return nullptr;
	}
	if (pobj->GetParserType() != IO::ParserType::MapLayer)
	{
		pobj.Delete();
		return nullptr;
	}
	NN<Map::MapDrawLayer> lyr;
	lyr = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
	NEW_CLASSNN(info, Map::MapManager::MapLayerInfo());
	info->layer = lyr;
	info->envList.Add(env);
	this->layerArr.PutC(fileName, info);
	return lyr;
}

void Map::MapManager::ClearMap(NN<Map::MapEnv> env)
{
	UOSInt i = this->layerArr.GetCount();
	UOSInt j;
	NN<Map::MapManager::MapLayerInfo> info;
	while (i-- > 0)
	{
		info = this->layerArr.GetItemNoCheck(i);
		j = info->envList.IndexOf(env);
		if (j != INVALID_INDEX)
		{
			info->envList.RemoveAt(j);
			if (info->envList.GetCount() == 0)
			{
				this->layerArr.Remove(this->layerArr.GetKey(i));
				info->layer.Delete();
				info.Delete();
			}
		}
	}
}
