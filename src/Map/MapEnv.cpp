#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ICaseStringUTF8Map.h"
#include "IO/Path.h"
#include "IO/ParsedObject.h"
#include "IO/StmData/FileData.h"
#include "Map/MapEnv.h"
#include "Map/ProjectedMapView.h"
#include "Map/ScaledMapView.h"
#include "Math/CoordinateSystemManager.h"
#include "Text/MyString.h"

UInt8 Map::MapEnv::GetRandomColor()
{
	Int32 i;
	i = random->NextInt15() & 0xff;
	if (i >= 0x80)
	{
		i = (i >> 1) + 0x80;
	}
	else if (i >= 0x40)
	{
		i += 0x40;
	}
	else
	{
		i = i << 1;
	}
	return (UInt8)i;
}

OSInt Map::MapEnv::GetLayersInList(Data::ArrayList<Map::IMapDrawLayer*> *layers, Data::ArrayList<Map::MapEnv::MapItem*> *list, Map::DrawLayerType lyrType)
{
	MapItem *item;
	OSInt retCnt = 0;
	OSInt i = list->GetCount();
	while (i-- > 0)
	{
		item = list->GetItem(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
			if (lyr->layer->GetLayerType() == lyrType)
			{
				retCnt++;
				layers->Add(lyr->layer);
			}
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			Map::MapEnv::GroupItem *grp = (Map::MapEnv::GroupItem*)item;
			retCnt += GetLayersInList(layers, grp->subitems, lyrType);
		}
	}
	return retCnt;
}

void Map::MapEnv::AddGroupUpdatedHandler(Map::MapEnv::GroupItem *group, Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Data::ArrayList<Map::MapEnv::MapItem *> *objs;
	Map::MapEnv::MapItem *item;
	OSInt i;
	if (group)
	{
		objs = group->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}

	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItem(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			((Map::MapEnv::LayerItem*)item)->layer->AddUpdatedHandler(hdlr, obj);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			this->AddGroupUpdatedHandler((Map::MapEnv::GroupItem*)item, hdlr, obj);
		}
	}
	
}

void Map::MapEnv::RemoveGroupUpdatedHandler(Map::MapEnv::GroupItem *group, Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Data::ArrayList<Map::MapEnv::MapItem *> *objs;
	Map::MapEnv::MapItem *item;
	OSInt i;
	if (group)
	{
		objs = group->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}

	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItem(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			((Map::MapEnv::LayerItem*)item)->layer->RemoveUpdatedHandler(hdlr, obj);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			this->RemoveGroupUpdatedHandler((Map::MapEnv::GroupItem*)item, hdlr, obj);
		}
	}
	
}

Map::MapEnv::MapEnv(const UTF8Char *fileName, UInt32 bgColor, Math::CoordinateSystem *csys) : IO::ParsedObject(fileName)
{
	this->bgColor = bgColor;
	this->nStr = 1000;
	this->csys = csys;
	if (this->csys == 0)
	{
		this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::GeographicCoordinateSystem::GCST_WGS84);
	}
	this->defFontStyle = 0;
	this->defLineStyle = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->mapLayers, Data::ArrayList<Map::MapEnv::MapItem*>());
	NEW_CLASS(this->lineStyles, Data::ArrayList<LineStyle*>());
	NEW_CLASS(this->fontStyles, Data::ArrayList<FontStyle*>());
	NEW_CLASS(this->images, Data::ICaseStringUTF8Map<ImageInfo*>());
	NEW_CLASS(this->imgList, Data::ArrayList<ImageInfo*>());
	NEW_CLASS(this->random, Data::RandomOS());
	this->AddLineStyle();
	this->AddLineStyleLayer(0, 0xff000000, 1, 0, 0);
}

Map::MapEnv::~MapEnv()
{
	OSInt i = this->mapLayers->GetCount();
	while (i-- > 0)
	{
		RemoveItem(0, i);
	}
	DEL_CLASS(this->mapLayers);

	Data::ArrayList<ImageInfo*> *imgs = this->images->GetValues();
	i = imgs->GetCount();
	while (i-- > 0)
	{
		ImageInfo *imgInfo = imgs->GetItem(i);
		Text::StrDelNew(imgInfo->fileName);
		DEL_CLASS(imgInfo->imgs);
		MemFree(imgInfo);
	}
	DEL_CLASS(this->images);
	DEL_CLASS(this->imgList);


	i = this->lineStyles->GetCount();
	while (i-- > 0)
	{
		this->RemoveLineStyle(i);
	}
	DEL_CLASS(this->lineStyles);

	i = this->fontStyles->GetCount();
	while (i-- > 0)
	{
		this->RemoveFontStyle(i);
	}
	DEL_CLASS(this->fontStyles);
	DEL_CLASS(this->random);
	SDEL_CLASS(this->csys);
	DEL_CLASS(this->mut);
}

IO::ParsedObject::ParserType Map::MapEnv::GetParserType()
{
	return IO::ParsedObject::PT_MAP_ENV_PARSER;
}

Int32 Map::MapEnv::GetBGColor()
{
	return this->bgColor;
}

UOSInt Map::MapEnv::GetDefLineStyle()
{
	return this->defLineStyle;
}

void Map::MapEnv::SetDefLineStyle(UOSInt lineStyle)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (lineStyle < this->lineStyles->GetCount())
	{
		this->defLineStyle = lineStyle;
	}
}

UOSInt Map::MapEnv::GetDefFontStyle()
{
	return this->defFontStyle;
}

void Map::MapEnv::SetDefFontStyle(UOSInt fontStyle)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (fontStyle < this->fontStyles->GetCount())
	{
		this->defFontStyle = fontStyle;
	}
}

UOSInt Map::MapEnv::AddLineStyle()
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles->GetCount();
	Map::MapEnv::LineStyle *style;
	if (cnt == 0)
	{
		style = MemAlloc(Map::MapEnv::LineStyle, 1);
		NEW_CLASS(style->layers, Data::ArrayList<Map::MapEnv::LineStyleLayer*>(4));
		style->name = 0;
		return this->lineStyles->Add(style);
	}
	else
	{
		style = this->lineStyles->GetItem(cnt - 1);
		if (style->layers->GetCount() == 0)
		{
			return cnt - 1;
		}
		style = MemAlloc(Map::MapEnv::LineStyle, 1);
		NEW_CLASS(style->layers, Data::ArrayList<Map::MapEnv::LineStyleLayer*>(4));
		style->name = 0;
		return this->lineStyles->Add(style);
	}
}

Bool Map::MapEnv::SetLineStyleName(UOSInt index, const UTF8Char *name)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	LineStyle *style;
	style = (LineStyle*)this->lineStyles->GetItem(index);
	if (style->name)
	{
		Text::StrDelNew(style->name);
		style->name = 0;
	}
	if (name)
	{
		style->name = Text::StrCopyNew(name);
	}
	return true;
}

UTF8Char *Map::MapEnv::GetLineStyleName(UOSInt index, UTF8Char *buff)
{
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return 0;
	}
	LineStyle *style;
	style = (LineStyle*)this->lineStyles->GetItem(index);
	if (style->name)
	{
		return Text::StrConcat(buff, style->name);
	}
	else
	{
		return 0;
	}
}

Bool Map::MapEnv::AddLineStyleLayer(UOSInt index, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles->GetItem(index);
	layer = MemAlloc(Map::MapEnv::LineStyleLayer, 1);
	layer->color = color;
	layer->thick = thick;
	if (npattern)
	{
		layer->pattern = MemAlloc(UInt8, npattern);
		layer->npattern = npattern;
		MemCopyNO(layer->pattern, pattern, npattern);
	}
	else
	{
		layer->pattern = 0;
		layer->npattern = 0;
	}
	style->layers->Add(layer);
	return true;
}

Bool Map::MapEnv::ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern)
{
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles->GetItem(index);
	if (style->layers->GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers->GetItem(layerId);
	if (layer->pattern)
	{
		MemFree(layer->pattern);
	}
	layer->color = color;
	layer->thick = thick;
	if (npattern)
	{
		layer->pattern = MemAlloc(UInt8, npattern);
		layer->npattern = npattern;
		MemCopyNO(layer->pattern, pattern, npattern);
	}
	else
	{
		layer->pattern = 0;
		layer->npattern = 0;
	}
	return true;
}

Bool Map::MapEnv::RemoveLineStyleLayer(UOSInt index, UOSInt layerId)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles->GetItem(index);
	if (style->layers->GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers->RemoveAt(layerId);
	if (layer->pattern)
	{
		MemFree(layer->pattern);
	}
	MemFree(layer);
	return true;
}

Bool Map::MapEnv::RemoveLineStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	UOSInt i;
	style = this->lineStyles->RemoveAt(index);
	i = style->layers->GetCount();
	while (i-- > 0)
	{
		layer = style->layers->RemoveAt(i);
		if (layer->pattern)
		{
			MemFree(layer->pattern);
		}
		MemFree(layer);
	}
	DEL_CLASS(style->layers);
	if (style->name)
	{
		Text::StrDelNew(style->name);
	}
	MemFree(style);
	return true;
}

UOSInt Map::MapEnv::GetLineStyleCount()
{
	return this->lineStyles->GetCount();
}

Bool Map::MapEnv::GetLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 *color, UOSInt *thick, UInt8 **pattern, UOSInt *npattern)
{
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles->GetItem(index);
	if (style->layers->GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers->GetItem(layerId);
	*color = layer->color;
	*thick = layer->thick;
	*pattern = layer->pattern;
	*npattern = layer->npattern;
	return true;
}

UOSInt Map::MapEnv::GetLineStyleLayerCnt(UOSInt index)
{
	UOSInt cnt = this->lineStyles->GetCount();
	if (index >= cnt)
	{
		return 0;
	}
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles->GetItem(index);
	return style->layers->GetCount();
}

OSInt Map::MapEnv::AddFontStyle(const UTF8Char *styleName, const UTF8Char *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	Map::MapEnv::FontStyle *style;
	if (fontName == 0)
		return -1;
	Sync::MutexUsage mutUsage(this->mut);
	style = MemAlloc(Map::MapEnv::FontStyle, 1);
	if (styleName)
	{
		style->styleName = Text::StrCopyNew(styleName);
	}
	else
	{
		style->styleName = 0;
	}
	style->fontName = Text::StrCopyNew(fontName);
	style->fontSizePt = fontSizePt;
	style->bold = bold;
	style->fontColor = fontColor;
	style->buffSize = buffSize;
	style->buffColor = buffColor;
	return this->fontStyles->Add(style);
}

Bool Map::MapEnv::SetFontStyleName(UOSInt index, const UTF8Char *name)
{
	Sync::MutexUsage mutUsage(this->mut);
	Map::MapEnv::FontStyle *style = this->fontStyles->GetItem(index);
	if (style == 0)
		return false;
	if (style->styleName)
		Text::StrDelNew(style->styleName);
	if (name)
	{
		style->styleName = Text::StrCopyNew(name);
	}
	else
	{
		style->styleName = 0;
	}
	return true;
}

UTF8Char *Map::MapEnv::GetFontStyleName(UOSInt index, UTF8Char *buff)
{
	Map::MapEnv::FontStyle *style = this->fontStyles->GetItem(index);
	if (style == 0)
		return 0;
	if (style->styleName)
		return Text::StrConcat(buff, style->styleName);
	return 0;
}

Bool Map::MapEnv::RemoveFontStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	Map::MapEnv::FontStyle *style = this->fontStyles->RemoveAt(index);
	if (style == 0)
		return false;
	if (style->styleName)
	{
		Text::StrDelNew(style->styleName);
	}
	Text::StrDelNew(style->fontName);
	MemFree(style);
	return true;
}

UOSInt Map::MapEnv::GetFontStyleCount()
{
	return this->fontStyles->GetCount();
}

Bool Map::MapEnv::GetFontStyle(UOSInt index, const UTF8Char **fontName, Double *fontSizePt, Bool *bold, UInt32 *fontColor, UOSInt *buffSize, UInt32 *buffColor)
{
	Map::MapEnv::FontStyle *style = this->fontStyles->GetItem(index);
	if (style == 0)
	{
		*fontName = 0;
		*fontSizePt = 0;
		*bold = false;
		*fontColor = 0xff000000;
		*buffSize = 0;
		*buffColor = 0xff000000;
		return false;
	}
	*fontName = style->fontName;
	*fontSizePt = style->fontSizePt;
	*bold = style->bold;
	*fontColor = style->fontColor;
	*buffSize = style->buffSize;
	*buffColor = style->buffColor;
	return true;
}

Bool Map::MapEnv::ChgFontStyle(UOSInt index, const UTF8Char *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	if (fontName == 0)
		return false;
	Map::MapEnv::FontStyle *style = this->fontStyles->GetItem(index);
	if (style == 0)
		return false;

	if (Text::StrCompare(style->fontName, fontName) != 0)
	{
		Text::StrDelNew(style->fontName);
		style->fontName = Text::StrCopyNew(fontName);
	}
	style->fontSizePt = fontSizePt;
	style->bold = bold;
	style->fontColor = fontColor;
	style->buffSize = buffSize;
	style->buffColor = buffColor;
	return true;
}

OSInt Map::MapEnv::AddLayer(Map::MapEnv::GroupItem *group, Map::IMapDrawLayer *layer, Bool needRelease)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (layer->GetObjectClass() == Map::IMapDrawLayer::OC_MAP_LAYER_COLL)// && layer->GetLayerType() == Map::DRAW_LAYER_MIXED)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Map::MapLayerCollection *layerColl = (Map::MapLayerCollection*)layer;
		Text::StrConcat(sbuff, layerColl->GetName());
		sptr = &sbuff[Text::StrLastIndexOf(sbuff, '\\') + 1];
		Map::MapEnv::GroupItem *grp = this->AddGroup(group, sptr);

		Map::IMapDrawLayer *layer;
		OSInt i;
		OSInt j;
		Data::ArrayList<Map::IMapDrawLayer*> layers;
		i = 0;
		j = layerColl->GetCount();
		while (i < j)
		{
			layers.Add(layerColl->GetItem(i));
			i++;
		}
		if (needRelease)
		{
			layerColl->Clear();
		}
		i = 0;
		j = layers.GetCount();
		while (i < j)
		{
			layer = layers.GetItem(i);
			this->AddLayer(grp, layer, needRelease);
			i++;
		}
		if (needRelease)
		{
			DEL_CLASS(layerColl);
		}
		return 1;
	}
	else
	{
		Map::MapEnv::LayerItem *lyr;
		lyr = MemAlloc(Map::MapEnv::LayerItem, 1);
		lyr->itemType = Map::MapEnv::IT_LAYER;
		lyr->layer = layer;
		lyr->needRelease = needRelease;
		lyr->lineStyle = (Int32)this->defLineStyle;
		if (layer->HasLineStyle())
		{
			lyr->lineType = 1;
			lyr->lineThick = layer->GetLineStyleWidth();
			lyr->lineColor = layer->GetLineStyleColor();
		}
		else
		{
			if (this->defLineStyle >= this->lineStyles->GetCount())
			{
				lyr->lineType = 1;
			}
			else
			{
				lyr->lineType = 0;
			}
			lyr->lineThick = 1;
			lyr->lineColor = 0xff000000;
		}
		if (layer->HasPGStyle())
		{
			lyr->fillStyle = layer->GetPGStyleColor();
		}
		else
		{
			if (layer->GetLayerType() == Map::DRAW_LAYER_POLYGON)
			{
				lyr->fillStyle = 0xff000000 | (this->GetRandomColor() << 16) | (this->GetRandomColor() << 8) | this->GetRandomColor();
			}
			else
			{
				lyr->fillStyle = 0xffc0c0c0;
			}
		}
		lyr->fontType = 0;
		lyr->fontStyle = (Int32)this->defFontStyle;
		lyr->fontName = 0;
		lyr->fontSizePt = 9.0;
		lyr->fontColor = 0xff000000;
		lyr->maxScale = 2000000000;
		lyr->minScale = -1;
		lyr->labelCol = 0;
		lyr->flags = Map::MapEnv::SFLG_NONE;
		if (layer->IsLabelVisible())
		{
			lyr->flags |= Map::MapEnv::SFLG_SHOWLABEL | Map::MapEnv::SFLG_SMART;
		}
		lyr->priority = 0;
		lyr->imgIndex = 0;
		
		if (group)
		{
			return group->subitems->Add(lyr);
		}
		else
		{
			return this->mapLayers->Add(lyr);
		}
	}
}

Bool Map::MapEnv::ReplaceLayer(Map::MapEnv::GroupItem *group, OSInt index, Map::IMapDrawLayer *layer, Bool needRelease)
{
	Map::MapEnv::MapItem *item;
	if (group)
	{
		item = group->subitems->GetItem(index);
	}
	else
	{
		item = this->mapLayers->GetItem(index);
	}
	if (item && item->itemType == Map::MapEnv::IT_LAYER)
	{
		Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
		if (lyr->needRelease)
		{
			DEL_CLASS(lyr->layer);
		}
		lyr->layer = layer;
		lyr->needRelease = needRelease;
		return true;
	}
	else
	{
		return false;
	}
}

Map::MapEnv::GroupItem *Map::MapEnv::AddGroup(Map::MapEnv::GroupItem *group, const UTF8Char *subgroupName)
{
	Sync::MutexUsage mutUsage(this->mut);
	Map::MapEnv::GroupItem *newG;
	newG = MemAlloc(Map::MapEnv::GroupItem, 1);
	newG->itemType = Map::MapEnv::IT_GROUP;
	newG->groupName = Text::StrCopyNew(subgroupName);
	newG->groupHide = false;
	NEW_CLASS(newG->subitems, Data::ArrayList<Map::MapEnv::MapItem*>());

	if (group)
	{
		group->subitems->Add(newG);
	}
	else
	{
		this->mapLayers->Add(newG);
	}
	return newG;
}

void Map::MapEnv::RemoveItem(Map::MapEnv::GroupItem *group, OSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	Map::MapEnv::MapItem *item;

	if (group)
	{
		item = group->subitems->RemoveAt(index);
	}
	else
	{
		item = this->mapLayers->RemoveAt(index);
	}
	if (item->itemType == Map::MapEnv::IT_LAYER)
	{
		Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
		if (lyr->needRelease)
		{
			DEL_CLASS(lyr->layer);
		}
		SDEL_TEXT(lyr->fontName);
		MemFree(lyr);
	}
	else if (item->itemType == Map::MapEnv::IT_GROUP)
	{
		Map::MapEnv::GroupItem *g = (Map::MapEnv::GroupItem*)item;
		OSInt i = g->subitems->GetCount();
		while (i-- > 0)
		{
			this->RemoveItem(g, i);
		}
		Text::StrDelNew(g->groupName);
		DEL_CLASS(g->subitems);
		MemFree(g);
	}
}

void Map::MapEnv::MoveItem(Map::MapEnv::GroupItem *group, OSInt fromIndex, OSInt toIndex)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (fromIndex == toIndex)
		return;

	if (group)
	{
		if (fromIndex < toIndex)
		{
			group->subitems->Insert(toIndex - 1, group->subitems->RemoveAt(fromIndex));
		}
		else
		{
			group->subitems->Insert(toIndex, group->subitems->RemoveAt(fromIndex));
		}
	}
	else
	{
		if (fromIndex < toIndex)
		{
			this->mapLayers->Insert(toIndex - 1, this->mapLayers->RemoveAt(fromIndex));
		}
		else
		{
			this->mapLayers->Insert(toIndex, this->mapLayers->RemoveAt(fromIndex));
		}
	}
}

void Map::MapEnv::MoveItem(Map::MapEnv::GroupItem *fromGroup, OSInt fromIndex, Map::MapEnv::GroupItem *toGroup, OSInt toIndex)
{
	Map::MapEnv::MapItem *item;
	if (fromGroup == toGroup)
	{
		MoveItem(fromGroup, fromIndex, toIndex);
		return;
	}
	Sync::MutexUsage mutUsage(this->mut);
	if (fromGroup)
	{
		item = fromGroup->subitems->RemoveAt(fromIndex);
	}
	else
	{
		item = this->mapLayers->RemoveAt(fromIndex);
	}
	if (toIndex == -1)
	{
		if (toGroup)
		{
			toGroup->subitems->Add(item);
		}
		else
		{
			this->mapLayers->Add(item);
		}
	}
	else
	{
		if (toGroup)
		{
			toGroup->subitems->Insert(toIndex, item);
		}
		else
		{
			this->mapLayers->Insert(toIndex, item);
		}
	}
}

OSInt Map::MapEnv::GetItemCount(Map::MapEnv::GroupItem *group)
{
	if (group == 0)
	{
		return this->mapLayers->GetCount();
	}
	else
	{
		return group->subitems->GetCount();
	}
}

Map::MapEnv::MapItem *Map::MapEnv::GetItem(Map::MapEnv::GroupItem *group, OSInt index)
{
	if (group == 0)
	{
		return this->mapLayers->GetItem(index);
	}
	else
	{
		return group->subitems->GetItem(index);
	}
}

const UTF8Char *Map::MapEnv::GetGroupName(Map::MapEnv::GroupItem *group)
{
	return group->groupName;
}

void Map::MapEnv::SetGroupName(Map::MapEnv::GroupItem *group, const UTF8Char *name)
{
	Sync::MutexUsage mutUsage(this->mut);
	Text::StrDelNew(group->groupName);
	group->groupName = Text::StrCopyNew(name);
}

void Map::MapEnv::SetGroupHide(Map::MapEnv::GroupItem *group, Bool isHide)
{
	Sync::MutexUsage mutUsage(this->mut);
	group->groupHide = isHide;
}

Bool Map::MapEnv::GetGroupHide(Map::MapEnv::GroupItem *group)
{
	return group->groupHide;
}

Bool Map::MapEnv::GetLayerProp(Map::MapEnv::LayerItem *setting, Map::MapEnv::GroupItem *group, OSInt index)
{
	Map::MapEnv::MapItem *item;
	if (group == 0)
	{
		item = this->mapLayers->GetItem(index);
	}
	else
	{
		item = group->subitems->GetItem(index);
	}
	if (item)
	{
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			MemCopyNO(setting, item, sizeof(Map::MapEnv::LayerItem));
			return true;
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool Map::MapEnv::SetLayerProp(Map::MapEnv::LayerItem *setting, Map::MapEnv::GroupItem *group, OSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	Map::MapEnv::MapItem *item;
	if (group == 0)
	{
		item = this->mapLayers->GetItem(index);
	}
	else
	{
		item = group->subitems->GetItem(index);
	}
	if (item)
	{
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
			Map::DrawLayerType layerType;
			layerType = lyr->layer->GetLayerType();
			if (setting->minScale > setting->maxScale)
				return false;
			

			lyr->fontType = setting->fontType;
			lyr->fontStyle = setting->fontStyle;
			if (lyr->fontName != setting->fontName)
			{
				SDEL_TEXT(lyr->fontName);
				if (setting->fontName)
				{
					lyr->fontName = Text::StrCopyNew(setting->fontName);
				}
			}
			lyr->fontSizePt = setting->fontSizePt;
			lyr->fontColor = setting->fontColor;


			if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
			{
				lyr->labelCol = setting->labelCol;
				lyr->flags = setting->flags;
				lyr->maxScale = setting->maxScale;
				lyr->minScale = setting->minScale;
				if (setting->imgIndex < this->GetImageCnt())
				{
					lyr->imgIndex = setting->imgIndex;
				}
				lyr->layer->SetNameCol(lyr->labelCol);
				return true;
			}
			else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
			{
				lyr->labelCol = setting->labelCol;
				lyr->flags = setting->flags;
				lyr->maxScale = setting->maxScale;
				lyr->minScale = setting->minScale;
				lyr->fontStyle = setting->fontStyle;
				lyr->lineType = setting->lineType;
				lyr->lineStyle = setting->lineStyle;
				lyr->lineThick = setting->lineThick;
				lyr->lineColor = setting->lineColor;
				lyr->layer->SetNameCol(lyr->labelCol);
				return true;
			}
			else if (layerType == Map::DRAW_LAYER_POLYGON)
			{
				lyr->labelCol = setting->labelCol;
				lyr->flags = setting->flags;
				lyr->maxScale = setting->maxScale;
				lyr->minScale = setting->minScale;
				lyr->fontStyle = setting->fontStyle;
				lyr->lineType = setting->lineType;
				lyr->lineStyle = setting->lineStyle;
				lyr->lineThick = setting->lineThick;
				lyr->lineColor = setting->lineColor;
				lyr->fillStyle = setting->fillStyle;
				lyr->layer->SetNameCol(lyr->labelCol);
				return true;
			}
			else if (layerType == Map::DRAW_LAYER_IMAGE)
			{
				lyr->flags = setting->flags;
				lyr->maxScale = setting->maxScale;
				lyr->minScale = setting->minScale;
				return true;
			}
			else if (layerType == Map::DRAW_LAYER_MIXED)
			{
				lyr->labelCol = setting->labelCol;
				lyr->flags = setting->flags;
				lyr->maxScale = setting->maxScale;
				lyr->minScale = setting->minScale;
				lyr->fontStyle = setting->fontStyle;
				lyr->lineType = setting->lineType;
				lyr->lineStyle = setting->lineStyle;
				lyr->lineThick = setting->lineThick;
				lyr->lineColor = setting->lineColor;
				lyr->fillStyle = setting->fillStyle;
				if (setting->imgIndex < this->GetImageCnt())
				{
					lyr->imgIndex = setting->imgIndex;
				}
				lyr->layer->SetNameCol(lyr->labelCol);
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			return false;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

UOSInt Map::MapEnv::GetNString()
{
	return this->nStr;
}

void Map::MapEnv::SetNString(UOSInt nStr)
{
	if (nStr <= 10 || nStr > 10000)
		return;
	Sync::MutexUsage mutUsage(this->mut);
	this->nStr = nStr;
}

UOSInt Map::MapEnv::GetImageCnt()
{
	ImageInfo *imgInfo = imgList->GetItem(imgList->GetCount() - 1);
	if (imgInfo)
	{
		return imgInfo->index + imgInfo->cnt;
	}
	else
	{
		return 0;
	}
}

Media::StaticImage *Map::MapEnv::GetImage(UOSInt index, Int32 *imgDurMS)
{
	UOSInt i;
	ImageInfo *imgInfo;
	i = imgList->GetCount();
	while (i-- > 0)
	{
		imgInfo = imgList->GetItem(i);
		if (index >= imgInfo->index && index < (imgInfo->index + imgInfo->cnt))
		{
			if (imgInfo->isAni)
			{
				Int32 imgTimeMS;
				Data::DateTime dt;
				Int64 currTimeTick;
				Media::StaticImage *simg;
				dt.SetCurrTimeUTC();
				currTimeTick = dt.ToTicks();
				if (currTimeTick >= imgInfo->aniLastTimeTick)
				{
					imgInfo->aniIndex++;
					if (imgInfo->aniIndex >= imgInfo->imgs->GetCount())
					{
						imgInfo->aniIndex = 0;
					}
					simg = (Media::StaticImage*)imgInfo->imgs->GetImage(imgInfo->aniIndex, &imgTimeMS);
					imgInfo->aniLastTimeTick = currTimeTick + imgTimeMS;
				}
				else
				{
					simg = (Media::StaticImage*)imgInfo->imgs->GetImage(imgInfo->aniIndex, &imgTimeMS);
					imgTimeMS = (Int32)(imgInfo->aniLastTimeTick - currTimeTick);
				}
				if (imgDurMS)
				{
					*imgDurMS = imgTimeMS;
				}
				return simg;
			}
			else
			{
				return (Media::StaticImage*)imgInfo->imgs->GetImage(index - imgInfo->index, imgDurMS);
			}
		}
	}
	return 0;
}

OSInt Map::MapEnv::AddImage(const UTF8Char *fileName, Parser::ParserList *parserList)
{
	Sync::MutexUsage mutUsage(this->mut);
	IO::StmData::FileData *fd;
	ImageInfo *imgInfo;
	if ((imgInfo = this->images->Get(fileName)) != 0)
	{
		return imgInfo->index;
	}
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	IO::ParsedObject::ParserType pt;
	IO::ParsedObject *pobj = parserList->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (pobj)
	{
		if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		{
			UOSInt i;
			imgInfo = MemAlloc(ImageInfo, 1);
			Media::ImageList *imgList = (Media::ImageList*)pobj;
			imgInfo->fileName = Text::StrCopyNew(fileName);
			imgInfo->index = this->GetImageCnt();
			imgInfo->cnt = imgList->GetCount();
			imgInfo->imgs = imgList;
			imgInfo->isAni = false;
			imgInfo->aniIndex = -1;
			imgInfo->aniLastTimeTick = 0;
			i = imgInfo->cnt;
			while (i-- > 0)
			{
				Int32 imgTime;
				imgList->ToStaticImage(i);
				((Media::StaticImage*)imgList->GetImage(i, &imgTime))->To32bpp();
			}
			if (imgInfo->isAni)
			{
				imgInfo->cnt = 1;
			}
			this->images->Put(fileName, imgInfo);
			this->imgList->Add(imgInfo);
			return imgInfo->index;
		}
		else
		{
			DEL_CLASS(pobj);
			return -1;
		}
	}
	return -1;
}

OSInt Map::MapEnv::AddImage(const UTF8Char *fileName, Media::ImageList *imgList)
{
	Sync::MutexUsage mutUsage(this->mut);
	ImageInfo *imgInfo;
	if ((imgInfo = this->images->Get(fileName)) != 0)
	{
		DEL_CLASS(imgList);
		return imgInfo->index;
	}
	OSInt i;
	imgInfo = MemAlloc(ImageInfo, 1);
	imgInfo->fileName = Text::StrCopyNew(fileName);
	imgInfo->index = this->GetImageCnt();
	imgInfo->cnt = imgList->GetCount();
	imgInfo->imgs = imgList;
	imgInfo->isAni = false;
	imgInfo->aniIndex = -1;
	imgInfo->aniLastTimeTick = 0;
	i = imgInfo->cnt;
	while (i-- > 0)
	{
		Int32 imgTime;
		imgList->ToStaticImage(i);
		((Media::StaticImage*)imgList->GetImage(i, &imgTime))->To32bpp();
		if (imgTime != 0)
		{
			imgInfo->isAni = true;
		}
	}
	this->images->Put(fileName, imgInfo);
	this->imgList->Add(imgInfo);
	return imgInfo->index;
}

UOSInt Map::MapEnv::GetImageFileCnt()
{
	return this->imgList->GetCount();
}

Bool Map::MapEnv::GetImageFileInfo(UOSInt index, Map::MapEnv::ImageInfo *info)
{
	Map::MapEnv::ImageInfo *imgInfo;
	imgInfo = this->imgList->GetItem(index);
	if (imgInfo == 0)
		return false;
	MemCopyNO(info, imgInfo, sizeof(Map::MapEnv::ImageInfo));
	return true;
}

UOSInt Map::MapEnv::GetImageFileIndex(UOSInt index)
{
	UOSInt i = this->imgList->GetCount();
	ImageInfo *info;
	while (i-- > 0)
	{
		info = this->imgList->GetItem(i);
		if (index >= info->index && index < info->index + info->cnt)
		{
			return i;
		}
	}
	return 0;
}

OSInt Map::MapEnv::GetLayersOfType(Data::ArrayList<Map::IMapDrawLayer *> *layers, Map::DrawLayerType lyrType)
{
	return this->GetLayersInList(layers, this->mapLayers, lyrType);
}

void Map::MapEnv::AddUpdatedHandler(Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->AddGroupUpdatedHandler(0, hdlr, obj);
}

void Map::MapEnv::RemoveUpdatedHandler(Map::IMapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->RemoveGroupUpdatedHandler(0, hdlr, obj);
}

Int64 Map::MapEnv::GetTimeEndTS(Map::MapEnv::GroupItem *group)
{
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	OSInt i;
	Int64 val = 0;
	Int64 val2 = 0;

	if (group == 0)
	{
		objs = this->mapLayers;
	}
	else
	{
		objs = group->subitems;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItem(i);
		if (item)
		{
			if (item->itemType == Map::MapEnv::IT_LAYER)
			{
				val2 = ((Map::MapEnv::LayerItem*)item)->layer->GetTimeEndTS();
			}
			else if (item->itemType == Map::MapEnv::IT_GROUP)
			{
				val2 = this->GetTimeEndTS((Map::MapEnv::GroupItem*)item);
			}
			if (val2 != 0)
			{
				if (val == 0)
					val = val2;
				else if (val2 > val)
					val = val2;
			}
		}
	}
	return val;
}

Int64 Map::MapEnv::GetTimeStartTS(Map::MapEnv::GroupItem *group)
{
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	Int64 val = 0;
	Int64 val2 = 0;

	if (group == 0)
	{
		objs = this->mapLayers;
	}
	else
	{
		objs = group->subitems;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItem(i);
		if (item)
		{
			if (item->itemType == Map::MapEnv::IT_LAYER)
			{
				val2 = ((Map::MapEnv::LayerItem*)item)->layer->GetTimeStartTS();
			}
			else if (item->itemType == Map::MapEnv::IT_GROUP)
			{
				val2 = GetTimeStartTS((Map::MapEnv::GroupItem*)item);
			}
			if (val2 != 0)
			{
				if (val == 0)
					val = val2;
				else if (val2 < val)
					val = val2;
			}
		}
	}
	return val;
}

void Map::MapEnv::SetCurrTimeTS(Map::MapEnv::GroupItem *group, Int64 timeStamp)
{
	Sync::MutexUsage mutUsage(this->mut);
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	OSInt i;

	if (group == 0)
	{
		objs = this->mapLayers;
	}
	else
	{
		objs = group->subitems;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItem(i);
		if (item)
		{
			if (item->itemType == Map::MapEnv::IT_LAYER)
			{
				((Map::MapEnv::LayerItem*)item)->layer->SetCurrTimeTS(timeStamp);
			}
			else if (item->itemType == Map::MapEnv::IT_GROUP)
			{
				mutUsage.EndUse();
				this->SetCurrTimeTS((Map::MapEnv::GroupItem*)item, timeStamp);
				mutUsage.BeginUse();
			}
		}
	}
}

Map::IMapDrawLayer *Map::MapEnv::GetFirstLayer(Map::MapEnv::GroupItem *group)
{
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	Map::IMapDrawLayer *lyrObj;
	OSInt i;
	OSInt j;

	if (group == 0)
	{
		objs = this->mapLayers;
	}
	else
	{
		objs = group->subitems;
	}
	i = 0;
	j = objs->GetCount();
	while (i < j)
	{
		item = objs->GetItem(i);
		if (item)
		{
			if (item->itemType == Map::MapEnv::IT_LAYER)
			{
				return ((Map::MapEnv::LayerItem*)item)->layer;
			}
			else if (item->itemType == Map::MapEnv::IT_GROUP)
			{
				lyrObj = this->GetFirstLayer((Map::MapEnv::GroupItem*)item);
				if (lyrObj)
					return lyrObj;
			}
		}
		i++;
	}
	return 0;
}

UOSInt Map::MapEnv::GetLayersInGroup(Map::MapEnv::GroupItem *group, Data::ArrayList<Map::IMapDrawLayer *> *layers)
{
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	UOSInt j;
	UOSInt ret = 0;

	if (group == 0)
	{
		objs = this->mapLayers;
	}
	else
	{
		objs = group->subitems;
	}
	i = 0;
	j = objs->GetCount();
	while (i < j)
	{
		item = objs->GetItem(i);
		if (item)
		{
			if (item->itemType == Map::MapEnv::IT_LAYER)
			{
				layers->Add(((Map::MapEnv::LayerItem*)item)->layer);
				ret++;
			}
			else if (item->itemType == Map::MapEnv::IT_GROUP)
			{
				ret += this->GetLayersInGroup((Map::MapEnv::GroupItem*)item, layers);
			}
		}
		i++;
	}
	return ret;
}

Bool Map::MapEnv::GetBoundsDbl(Map::MapEnv::GroupItem *group, Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	Data::ArrayList<Map::IMapDrawLayer*> layers;
	UOSInt i = 0;
	UOSInt j = this->GetLayersInGroup(group, &layers);
	Double currMinX = 0;
	Double currMinY = 0;
	Double currMaxX = 0;
	Double currMaxY = 0;
	Double thisMinX;
	Double thisMinY;
	Double thisMaxX;
	Double thisMaxY;
	Math::CoordinateSystem *lyrCSys;
	Bool isFirst = true;
	while (i < j)
	{
		Map::IMapDrawLayer *lyr = layers.GetItem(i);
		if (lyr->GetBoundsDbl(&thisMinX, &thisMinY, &thisMaxX, &thisMaxY))
		{
			lyrCSys = lyr->GetCoordinateSystem();
			if (this->csys != 0 && lyrCSys != 0)
			{
				if (!this->csys->Equals(lyrCSys))
				{
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, this->csys, thisMinX, thisMinY, 0, &thisMinX, &thisMinY, 0);
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, this->csys, thisMaxX, thisMaxY, 0, &thisMaxX, &thisMaxY, 0);
				}
			}
			if (isFirst)
			{
				isFirst = false;
				currMinX = thisMinX;
				currMinY = thisMinY;
				currMaxX = thisMaxX;
				currMaxY = thisMaxY;
			}
			else
			{
				if (currMinX > thisMinX) currMinX = thisMinX;
				if (currMinY > thisMinY) currMinY = thisMinY;
				if (currMaxX < thisMaxX) currMaxX = thisMaxX;
				if (currMaxY < thisMaxY) currMaxY = thisMaxY;
			}
		}	
		i++;
	}
	*minX = currMinX;
	*minY = currMinY;
	*maxX = currMaxX;
	*maxY = currMaxY;
	return !isFirst;
}

Map::MapView *Map::MapEnv::CreateMapView(OSInt width, OSInt height)
{
	Map::IMapDrawLayer *baseLayer = GetFirstLayer(0);
	if (baseLayer)
	{
		return baseLayer->CreateMapView(width, height);
	}
	else if (csys == 0)
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ScaledMapView(width, height, 22.4, 114.2, 10000));
		return view;
	}
	else if (csys->IsProjected())
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ProjectedMapView(width, height, 835000, 820000, 10000));
		return view;
	}
	else
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ScaledMapView(width, height, 22.4, 114.2, 10000));
		return view;
	}
}

Math::CoordinateSystem *Map::MapEnv::GetCoordinateSystem()
{
	return this->csys;
}

Int32 Map::MapEnv::GetSRID()
{
	if (this->csys)
		return this->csys->GetSRID();
	return 0;
}

void Map::MapEnv::BeginUse(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
}