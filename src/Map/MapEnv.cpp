#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ICaseStringMap.h"
#include "IO/Path.h"
#include "IO/ParsedObject.h"
#include "IO/StmData/FileData.h"
#include "Map/MapEnv.h"
#include "Map/ScaledMapView.h"
#include "Math/CoordinateSystemManager.h"
#include "Text/MyString.h"

UInt8 Map::MapEnv::GetRandomColor()
{
	Int32 i;
	i = this->random.NextInt15() & 0xff;
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

UOSInt Map::MapEnv::GetLayersInList(Data::ArrayList<Map::MapDrawLayer*> *layers, const Data::ArrayList<Map::MapEnv::MapItem*> *list, Map::DrawLayerType lyrType) const
{
	MapItem *item;
	UOSInt retCnt = 0;
	UOSInt i = list->GetCount();
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

void Map::MapEnv::AddGroupUpdatedHandler(Map::MapEnv::GroupItem *group, Map::MapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Data::ArrayList<Map::MapEnv::MapItem *> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	if (group)
	{
		objs = group->subitems;
	}
	else
	{
		objs = &this->mapLayers;
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

void Map::MapEnv::RemoveGroupUpdatedHandler(Map::MapEnv::GroupItem *group, Map::MapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Data::ArrayList<Map::MapEnv::MapItem *> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	if (group)
	{
		objs = group->subitems;
	}
	else
	{
		objs = &this->mapLayers;
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

Map::MapEnv::MapEnv(Text::CString fileName, UInt32 bgColor, Math::CoordinateSystem *csys) : IO::ParsedObject(fileName)
{
	this->bgColor = bgColor;
	this->nStr = 1000;
	this->csys = csys;
	if (this->csys == 0)
	{
		this->csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	}
	this->defFontStyle = 0;
	this->defLineStyle = 0;
	this->AddLineStyle();
	this->AddLineStyleLayer(0, 0xff000000, 1, 0, 0);
}

Map::MapEnv::~MapEnv()
{
	UOSInt i = this->mapLayers.GetCount();
	while (i-- > 0)
	{
		RemoveItem(0, i);
	}

	const Data::ArrayList<ImageInfo*> *imgs = this->images.GetValues();
	i = imgs->GetCount();
	while (i-- > 0)
	{
		ImageInfo *imgInfo = imgs->GetItem(i);
		imgInfo->fileName->Release();
		DEL_CLASS(imgInfo->imgs);
		MemFree(imgInfo);
	}

	i = this->lineStyles.GetCount();
	while (i-- > 0)
	{
		this->RemoveLineStyle(i);
	}

	i = this->fontStyles.GetCount();
	while (i-- > 0)
	{
		this->RemoveFontStyle(i);
	}
	SDEL_CLASS(this->csys);
}

IO::ParserType Map::MapEnv::GetParserType() const
{
	return IO::ParserType::MapEnv;
}

UInt32 Map::MapEnv::GetBGColor() const
{
	return this->bgColor;
}

UOSInt Map::MapEnv::GetDefLineStyle() const
{
	return this->defLineStyle;
}

void Map::MapEnv::SetDefLineStyle(UOSInt lineStyle)
{
	Sync::MutexUsage mutUsage(&this->mut);
	if (lineStyle < this->lineStyles.GetCount())
	{
		this->defLineStyle = lineStyle;
	}
}

UOSInt Map::MapEnv::GetDefFontStyle() const
{
	return this->defFontStyle;
}

void Map::MapEnv::SetDefFontStyle(UOSInt fontStyle)
{
	Sync::MutexUsage mutUsage(&this->mut);
	if (fontStyle < this->fontStyles.GetCount())
	{
		this->defFontStyle = fontStyle;
	}
}

UOSInt Map::MapEnv::AddLineStyle()
{
	Sync::MutexUsage mutUsage(&this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	Map::MapEnv::LineStyle *style;
	if (cnt == 0)
	{
		NEW_CLASS(style, Map::MapEnv::LineStyle());
		style->name = 0;
		return this->lineStyles.Add(style);
	}
	else
	{
		style = this->lineStyles.GetItem(cnt - 1);
		if (style->layers.GetCount() == 0)
		{
			return cnt - 1;
		}
		NEW_CLASS(style, Map::MapEnv::LineStyle());
		style->name = 0;
		return this->lineStyles.Add(style);
	}
}

Bool Map::MapEnv::SetLineStyleName(UOSInt index, Text::CString name)
{
	Sync::MutexUsage mutUsage(&this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	LineStyle *style;
	style = (LineStyle*)this->lineStyles.GetItem(index);
	SDEL_STRING(style->name);
	style->name = Text::String::NewOrNull(name);
	return true;
}

UTF8Char *Map::MapEnv::GetLineStyleName(UOSInt index, UTF8Char *buff) const
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return 0;
	}
	LineStyle *style;
	style = (LineStyle*)this->lineStyles.GetItem(index);
	if (style->name)
	{
		return style->name->ConcatTo(buff);
	}
	else
	{
		return 0;
	}
}

Bool Map::MapEnv::AddLineStyleLayer(UOSInt index, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern)
{
	Sync::MutexUsage mutUsage(&this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles.GetItem(index);
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
	style->layers.Add(layer);
	return true;
}

Bool Map::MapEnv::ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, UOSInt thick, const UInt8 *pattern, UOSInt npattern)
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles.GetItem(index);
	if (style->layers.GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers.GetItem(layerId);
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
	Sync::MutexUsage mutUsage(&this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles.GetItem(index);
	if (style->layers.GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers.RemoveAt(layerId);
	if (layer->pattern)
	{
		MemFree(layer->pattern);
	}
	MemFree(layer);
	return true;
}

Bool Map::MapEnv::RemoveLineStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(&this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	UOSInt i;
	style = this->lineStyles.RemoveAt(index);
	i = style->layers.GetCount();
	while (i-- > 0)
	{
		layer = style->layers.RemoveAt(i);
		if (layer->pattern)
		{
			MemFree(layer->pattern);
		}
		MemFree(layer);
	}
	SDEL_STRING(style->name);
	DEL_CLASS(style);
	return true;
}

UOSInt Map::MapEnv::GetLineStyleCount() const
{
	return this->lineStyles.GetCount();
}

Bool Map::MapEnv::GetLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 *color, UOSInt *thick, UInt8 **pattern, UOSInt *npattern) const
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	Map::MapEnv::LineStyleLayer *layer;
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles.GetItem(index);
	if (style->layers.GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers.GetItem(layerId);
	*color = layer->color;
	*thick = layer->thick;
	*pattern = layer->pattern;
	*npattern = layer->npattern;
	return true;
}

UOSInt Map::MapEnv::GetLineStyleLayerCnt(UOSInt index) const
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return 0;
	}
	Map::MapEnv::LineStyle *style;
	style = this->lineStyles.GetItem(index);
	return style->layers.GetCount();
}

UOSInt Map::MapEnv::AddFontStyle(Text::CString styleName, Text::CString fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	Map::MapEnv::FontStyle *style;
	if (fontName.leng == 0)
		return (UOSInt)-1;
	Sync::MutexUsage mutUsage(&this->mut);
	style = MemAlloc(Map::MapEnv::FontStyle, 1);
	style->styleName = Text::String::NewOrNull(styleName);
	style->fontName = Text::String::New(fontName);
	style->fontSizePt = fontSizePt;
	style->bold = bold;
	style->fontColor = fontColor;
	style->buffSize = buffSize;
	style->buffColor = buffColor;
	return this->fontStyles.Add(style);
}

Bool Map::MapEnv::SetFontStyleName(UOSInt index, Text::CString name)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::FontStyle *style = this->fontStyles.GetItem(index);
	if (style == 0)
		return false;
	SDEL_STRING(style->styleName);
	style->styleName = Text::String::NewOrNull(name);
	return true;
}

UTF8Char *Map::MapEnv::GetFontStyleName(UOSInt index, UTF8Char *buff) const
{
	Map::MapEnv::FontStyle *style = this->fontStyles.GetItem(index);
	if (style == 0)
		return 0;
	if (style->styleName)
		return style->styleName->ConcatTo(buff);
	return 0;
}

Bool Map::MapEnv::RemoveFontStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::FontStyle *style = this->fontStyles.RemoveAt(index);
	if (style == 0)
		return false;
	SDEL_STRING(style->styleName);
	style->fontName->Release();
	MemFree(style);
	return true;
}

UOSInt Map::MapEnv::GetFontStyleCount() const
{
	return this->fontStyles.GetCount();
}

Bool Map::MapEnv::GetFontStyle(UOSInt index, Text::String **fontName, Double *fontSizePt, Bool *bold, UInt32 *fontColor, UOSInt *buffSize, UInt32 *buffColor) const
{
	Map::MapEnv::FontStyle *style = this->fontStyles.GetItem(index);
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

Bool Map::MapEnv::ChgFontStyle(UOSInt index, Text::String *fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	if (fontName == 0)
		return false;
	Map::MapEnv::FontStyle *style = this->fontStyles.GetItem(index);
	if (style == 0)
		return false;

	if (!style->fontName->Equals(fontName))
	{
		style->fontName->Release();
		style->fontName = fontName->Clone();
	}
	style->fontSizePt = fontSizePt;
	style->bold = bold;
	style->fontColor = fontColor;
	style->buffSize = buffSize;
	style->buffColor = buffColor;
	return true;
}

UOSInt Map::MapEnv::AddLayer(Map::MapEnv::GroupItem *group, Map::MapDrawLayer *layer, Bool needRelease)
{
	Sync::MutexUsage mutUsage(&this->mut);
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)// && layer->GetLayerType() == Map::DRAW_LAYER_MIXED)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		UTF8Char *sptr2;
		Map::MapLayerCollection *layerColl = (Map::MapLayerCollection*)layer;
		sptr = layerColl->GetName()->ConcatTo(sbuff);
		sptr2 = &sbuff[Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\') + 1];
		Map::MapEnv::GroupItem *grp = this->AddGroup(group, CSTRP(sptr2, sptr));

		Map::MapDrawLayer *layer;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Data::ArrayList<Map::MapDrawLayer*> layers;
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
			k = layerColl->GetUpdatedHandlerCnt();
			while (k-- > 0)
			{
				layer->AddUpdatedHandler(layerColl->GetUpdatedHandler(k), layerColl->GetUpdatedObject(k));
			}
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
		lyr->lineStyle = this->defLineStyle;
		if (layer->HasLineStyle())
		{
			lyr->lineType = 1;
			lyr->lineThick = layer->GetLineStyleWidth();
			lyr->lineColor = layer->GetLineStyleColor();
		}
		else
		{
			if (this->defLineStyle >= this->lineStyles.GetCount())
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
				lyr->fillStyle = 0xff000000 | (UInt32)(this->GetRandomColor() << 16) | (UInt32)(this->GetRandomColor() << 8) | (UInt32)this->GetRandomColor();
			}
			else
			{
				lyr->fillStyle = 0xffc0c0c0;
			}
		}
		lyr->fontType = FontType::GlobalStyle;
		lyr->fontStyle = this->defFontStyle;
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
			return this->mapLayers.Add(lyr);
		}
	}
}

Bool Map::MapEnv::ReplaceLayer(Map::MapEnv::GroupItem *group, UOSInt index, Map::MapDrawLayer *layer, Bool needRelease)
{
	Map::MapEnv::MapItem *item;
	if (group)
	{
		item = group->subitems->GetItem(index);
	}
	else
	{
		item = this->mapLayers.GetItem(index);
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

Map::MapEnv::GroupItem *Map::MapEnv::AddGroup(Map::MapEnv::GroupItem *group, Text::String *subgroupName)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::GroupItem *newG;
	newG = MemAlloc(Map::MapEnv::GroupItem, 1);
	newG->itemType = Map::MapEnv::IT_GROUP;
	newG->groupName = subgroupName->Clone();
	newG->groupHide = false;
	NEW_CLASS(newG->subitems, Data::ArrayList<Map::MapEnv::MapItem*>());

	if (group)
	{
		group->subitems->Add(newG);
	}
	else
	{
		this->mapLayers.Add(newG);
	}
	return newG;
}

Map::MapEnv::GroupItem *Map::MapEnv::AddGroup(Map::MapEnv::GroupItem *group, Text::CString subgroupName)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::GroupItem *newG;
	newG = MemAlloc(Map::MapEnv::GroupItem, 1);
	newG->itemType = Map::MapEnv::IT_GROUP;
	newG->groupName = Text::String::New(subgroupName);
	newG->groupHide = false;
	NEW_CLASS(newG->subitems, Data::ArrayList<Map::MapEnv::MapItem*>());

	if (group)
	{
		group->subitems->Add(newG);
	}
	else
	{
		this->mapLayers.Add(newG);
	}
	return newG;
}

void Map::MapEnv::RemoveItem(Map::MapEnv::GroupItem *group, UOSInt index)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::MapItem *item;

	if (group)
	{
		item = group->subitems->RemoveAt(index);
	}
	else
	{
		item = this->mapLayers.RemoveAt(index);
	}
	if (item->itemType == Map::MapEnv::IT_LAYER)
	{
		Map::MapEnv::LayerItem *lyr = (Map::MapEnv::LayerItem*)item;
		if (lyr->needRelease)
		{
			DEL_CLASS(lyr->layer);
		}
		SDEL_STRING(lyr->fontName);
		MemFree(lyr);
	}
	else if (item->itemType == Map::MapEnv::IT_GROUP)
	{
		Map::MapEnv::GroupItem *g = (Map::MapEnv::GroupItem*)item;
		UOSInt i = g->subitems->GetCount();
		while (i-- > 0)
		{
			this->RemoveItem(g, i);
		}
		g->groupName->Release();
		DEL_CLASS(g->subitems);
		MemFree(g);
	}
}

void Map::MapEnv::MoveItem(Map::MapEnv::GroupItem *group, UOSInt fromIndex, UOSInt toIndex)
{
	Sync::MutexUsage mutUsage(&this->mut);
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
			this->mapLayers.Insert(toIndex - 1, this->mapLayers.RemoveAt(fromIndex));
		}
		else
		{
			this->mapLayers.Insert(toIndex, this->mapLayers.RemoveAt(fromIndex));
		}
	}
}

void Map::MapEnv::MoveItem(Map::MapEnv::GroupItem *fromGroup, UOSInt fromIndex, Map::MapEnv::GroupItem *toGroup, UOSInt toIndex)
{
	Map::MapEnv::MapItem *item;
	if (fromGroup == toGroup)
	{
		MoveItem(fromGroup, fromIndex, toIndex);
		return;
	}
	Sync::MutexUsage mutUsage(&this->mut);
	if (fromGroup)
	{
		item = fromGroup->subitems->RemoveAt(fromIndex);
	}
	else
	{
		item = this->mapLayers.RemoveAt(fromIndex);
	}
	if (toIndex == (UOSInt)-1)
	{
		if (toGroup)
		{
			toGroup->subitems->Add(item);
		}
		else
		{
			this->mapLayers.Add(item);
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
			this->mapLayers.Insert(toIndex, item);
		}
	}
}

UOSInt Map::MapEnv::GetItemCount(Map::MapEnv::GroupItem *group) const
{
	if (group == 0)
	{
		return this->mapLayers.GetCount();
	}
	else
	{
		return group->subitems->GetCount();
	}
}

Map::MapEnv::MapItem *Map::MapEnv::GetItem(Map::MapEnv::GroupItem *group, UOSInt index) const
{
	if (group == 0)
	{
		return this->mapLayers.GetItem(index);
	}
	else
	{
		return group->subitems->GetItem(index);
	}
}

Text::String *Map::MapEnv::GetGroupName(Map::MapEnv::GroupItem *group) const
{
	return group->groupName;
}

void Map::MapEnv::SetGroupName(Map::MapEnv::GroupItem *group, Text::CString name)
{
	Sync::MutexUsage mutUsage(&this->mut);
	group->groupName->Release();
	group->groupName = Text::String::New(name);
}

void Map::MapEnv::SetGroupHide(Map::MapEnv::GroupItem *group, Bool isHide)
{
	Sync::MutexUsage mutUsage(&this->mut);
	group->groupHide = isHide;
}

Bool Map::MapEnv::GetGroupHide(Map::MapEnv::GroupItem *group) const
{
	return group->groupHide;
}

Bool Map::MapEnv::GetLayerProp(Map::MapEnv::LayerItem *setting, Map::MapEnv::GroupItem *group, UOSInt index) const
{
	Map::MapEnv::MapItem *item;
	if (group == 0)
	{
		item = this->mapLayers.GetItem(index);
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

Bool Map::MapEnv::SetLayerProp(Map::MapEnv::LayerItem *setting, Map::MapEnv::GroupItem *group, UOSInt index)
{
	Sync::MutexUsage mutUsage(&this->mut);
	Map::MapEnv::MapItem *item;
	if (group == 0)
	{
		item = this->mapLayers.GetItem(index);
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
				SDEL_STRING(lyr->fontName);
				if (setting->fontName)
				{
					lyr->fontName = setting->fontName->Clone();
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

UOSInt Map::MapEnv::GetNString() const
{
	return this->nStr;
}

void Map::MapEnv::SetNString(UOSInt nStr)
{
	if (nStr <= 10 || nStr > 10000)
		return;
	Sync::MutexUsage mutUsage(&this->mut);
	this->nStr = nStr;
}

UOSInt Map::MapEnv::GetImageCnt() const
{
	ImageInfo *imgInfo = this->imgList.GetItem(this->imgList.GetCount() - 1);
	if (imgInfo)
	{
		return imgInfo->index + imgInfo->cnt;
	}
	else
	{
		return 0;
	}
}

Media::StaticImage *Map::MapEnv::GetImage(UOSInt index, UInt32 *imgDurMS) const
{
	UOSInt i;
	ImageInfo *imgInfo;
	i = this->imgList.GetCount();
	while (i-- > 0)
	{
		imgInfo = this->imgList.GetItem(i);
		if (index >= imgInfo->index && index < (imgInfo->index + imgInfo->cnt))
		{
			if (imgInfo->isAni)
			{
				UInt32 imgTimeMS;
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
					imgInfo->aniLastTimeTick = currTimeTick + (Int64)imgTimeMS;
				}
				else
				{
					simg = (Media::StaticImage*)imgInfo->imgs->GetImage(imgInfo->aniIndex, &imgTimeMS);
					imgTimeMS = (UInt32)(imgInfo->aniLastTimeTick - currTimeTick);
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

OSInt Map::MapEnv::AddImage(Text::CString fileName, Parser::ParserList *parserList)
{
	Sync::MutexUsage mutUsage(&this->mut);
	IO::StmData::FileData *fd;
	ImageInfo *imgInfo;
	if ((imgInfo = this->images.Get(fileName)) != 0)
	{
		return (OSInt)imgInfo->index;
	}
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	IO::ParserType pt;
	IO::ParsedObject *pobj = parserList->ParseFile(fd, &pt);
	DEL_CLASS(fd);
	if (pobj)
	{
		if (pt == IO::ParserType::ImageList)
		{
			UOSInt i;
			imgInfo = MemAlloc(ImageInfo, 1);
			Media::ImageList *imgList = (Media::ImageList*)pobj;
			imgInfo->fileName = Text::String::New(fileName.v, fileName.leng);
			imgInfo->index = this->GetImageCnt();
			imgInfo->cnt = imgList->GetCount();
			imgInfo->imgs = imgList;
			imgInfo->isAni = false;
			imgInfo->aniIndex = (UOSInt)-1;
			imgInfo->aniLastTimeTick = 0;
			i = imgInfo->cnt;
			while (i-- > 0)
			{
				UInt32 imgTime;
				imgList->ToStaticImage(i);
				((Media::StaticImage*)imgList->GetImage(i, &imgTime))->To32bpp();
			}
			if (imgInfo->isAni)
			{
				imgInfo->cnt = 1;
			}
			this->images.Put(fileName, imgInfo);
			this->imgList.Add(imgInfo);
			return (OSInt)imgInfo->index;
		}
		else
		{
			DEL_CLASS(pobj);
			return -1;
		}
	}
	return -1;
}

UOSInt Map::MapEnv::AddImage(Text::CString fileName, Media::ImageList *imgList)
{
	Sync::MutexUsage mutUsage(&this->mut);
	ImageInfo *imgInfo;
	if ((imgInfo = this->images.Get(fileName)) != 0)
	{
		DEL_CLASS(imgList);
		return imgInfo->index;
	}
	UOSInt i;
	imgInfo = MemAlloc(ImageInfo, 1);
	imgInfo->fileName = Text::String::New(fileName.v, fileName.leng);
	imgInfo->index = this->GetImageCnt();
	imgInfo->cnt = imgList->GetCount();
	imgInfo->imgs = imgList;
	imgInfo->isAni = false;
	imgInfo->aniIndex = (UOSInt)-1;
	imgInfo->aniLastTimeTick = 0;
	i = imgInfo->cnt;
	while (i-- > 0)
	{
		UInt32 imgTime;
		imgList->ToStaticImage(i);
		((Media::StaticImage*)imgList->GetImage(i, &imgTime))->To32bpp();
		if (imgTime != 0)
		{
			imgInfo->isAni = true;
		}
	}
	this->images.Put(imgInfo->fileName, imgInfo);
	this->imgList.Add(imgInfo);
	return imgInfo->index;
}

UOSInt Map::MapEnv::GetImageFileCnt() const
{
	return this->imgList.GetCount();
}

Bool Map::MapEnv::GetImageFileInfo(UOSInt index, Map::MapEnv::ImageInfo *info) const
{
	Map::MapEnv::ImageInfo *imgInfo;
	imgInfo = this->imgList.GetItem(index);
	if (imgInfo == 0)
		return false;
	MemCopyNO(info, imgInfo, sizeof(Map::MapEnv::ImageInfo));
	return true;
}

UOSInt Map::MapEnv::GetImageFileIndex(UOSInt index) const
{
	UOSInt i = this->imgList.GetCount();
	ImageInfo *info;
	while (i-- > 0)
	{
		info = this->imgList.GetItem(i);
		if (index >= info->index && index < info->index + info->cnt)
		{
			return i;
		}
	}
	return 0;
}

UOSInt Map::MapEnv::GetLayersOfType(Data::ArrayList<Map::MapDrawLayer *> *layers, Map::DrawLayerType lyrType) const
{
	return this->GetLayersInList(layers, &this->mapLayers, lyrType);
}

void Map::MapEnv::AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(&this->mut);
	this->AddGroupUpdatedHandler(0, hdlr, obj);
}

void Map::MapEnv::RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, void *obj)
{
	Sync::MutexUsage mutUsage(&this->mut);
	this->RemoveGroupUpdatedHandler(0, hdlr, obj);
}

Int64 Map::MapEnv::GetTimeEndTS(Map::MapEnv::GroupItem *group) const
{
	const Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	Int64 val = 0;
	Int64 val2 = 0;

	if (group == 0)
	{
		objs = &this->mapLayers;
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

Int64 Map::MapEnv::GetTimeStartTS(Map::MapEnv::GroupItem *group) const
{
	const Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	Int64 val = 0;
	Int64 val2 = 0;

	if (group == 0)
	{
		objs = &this->mapLayers;
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
	Sync::MutexUsage mutUsage(&this->mut);
	Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;

	if (group == 0)
	{
		objs = &this->mapLayers;
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

Map::MapDrawLayer *Map::MapEnv::GetFirstLayer(Map::MapEnv::GroupItem *group) const
{
	const Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	Map::MapDrawLayer *lyrObj;
	UOSInt i;
	UOSInt j;

	if (group == 0)
	{
		objs = &this->mapLayers;
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

UOSInt Map::MapEnv::GetLayersInGroup(Map::MapEnv::GroupItem *group, Data::ArrayList<Map::MapDrawLayer *> *layers) const
{
	const Data::ArrayList<Map::MapEnv::MapItem*> *objs;
	Map::MapEnv::MapItem *item;
	UOSInt i;
	UOSInt j;
	UOSInt ret = 0;

	if (group == 0)
	{
		objs = &this->mapLayers;
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

Bool Map::MapEnv::GetBounds(Map::MapEnv::GroupItem *group, Math::RectAreaDbl *bounds) const
{
	Data::ArrayList<Map::MapDrawLayer*> layers;
	UOSInt i = 0;
	UOSInt j = this->GetLayersInGroup(group, &layers);
	Math::RectAreaDbl minMax = Math::RectAreaDbl(0, 0, 0, 0);
	Math::RectAreaDbl thisBounds;
	Math::CoordinateSystem *lyrCSys;
	Bool isFirst = true;
	while (i < j)
	{
		Map::MapDrawLayer *lyr = layers.GetItem(i);
		if (lyr->GetBounds(&thisBounds))
		{
			lyrCSys = lyr->GetCoordinateSystem();
			if (this->csys != 0 && lyrCSys != 0)
			{
				if (!this->csys->Equals(lyrCSys))
				{
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, this->csys, thisBounds.tl.x, thisBounds.tl.y, 0, &thisBounds.tl.x, &thisBounds.tl.y, 0);
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, this->csys, thisBounds.br.x, thisBounds.br.y, 0, &thisBounds.br.x, &thisBounds.br.y, 0);
				}
			}
			if (isFirst)
			{
				isFirst = false;
				minMax = thisBounds;
			}
			else
			{
				minMax.tl = minMax.tl.Min(thisBounds.tl);
				minMax.br = minMax.br.Max(thisBounds.br);
			}
		}	
		i++;
	}
	*bounds = minMax;
	return !isFirst;
}

Map::MapView *Map::MapEnv::CreateMapView(Math::Size2D<Double> scnSize) const
{
	Map::MapDrawLayer *baseLayer = GetFirstLayer(0);
	if (baseLayer)
	{
		return baseLayer->CreateMapView(scnSize);
	}
	else if (csys == 0)
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ScaledMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), 10000, false));
		return view;
	}
	else if (csys->IsProjected())
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ScaledMapView(scnSize, Math::Coord2DDbl(835000, 820000), 10000, true));
		return view;
	}
	else
	{
		Map::MapView *view;
		NEW_CLASS(view, Map::ScaledMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), 10000, true));
		return view;
	}
}

Math::CoordinateSystem *Map::MapEnv::GetCoordinateSystem() const
{
	return this->csys;
}

UInt32 Map::MapEnv::GetSRID() const
{
	if (this->csys)
		return this->csys->GetSRID();
	return 0;
}

void Map::MapEnv::BeginUse(Sync::MutexUsage *mutUsage) const
{
	mutUsage->ReplaceMutex(&this->mut);
}