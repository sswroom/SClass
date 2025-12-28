#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ICaseStringMap.hpp"
#include "IO/Path.h"
#include "IO/ParsedObject.h"
#include "IO/StmData/FileData.h"
#include "Map/MapEnv.h"
#include "Map/ScaledMapView.h"
#include "Math/CoordinateSystemManager.h"
#include "Media/ImageUtil_C.h"
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

UOSInt Map::MapEnv::GetLayersInList(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> list, Map::DrawLayerType lyrType) const
{
	NN<MapItem> item;
	UOSInt retCnt = 0;
	UOSInt i = list->GetCount();
	while (i-- > 0)
	{
		item = list->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem> lyr = NN<Map::MapEnv::LayerItem>::ConvertFrom(item);
			if (lyr->layer->GetLayerType() == lyrType)
			{
				retCnt++;
				layers->Add(lyr->layer);
			}
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			NN<Map::MapEnv::GroupItem> grp = NN<Map::MapEnv::GroupItem>::ConvertFrom(item);
			retCnt += GetLayersInList(layers, grp->subitems, lyrType);
		}
	}
	return retCnt;
}

void Map::MapEnv::AddGroupUpdatedHandler(Optional<Map::MapEnv::GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	NN<Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	NN<GroupItem> nngroup;
	UOSInt i;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}

	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->AddUpdatedHandler(hdlr, obj);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			this->AddGroupUpdatedHandler(NN<Map::MapEnv::GroupItem>::ConvertFrom(item), hdlr, obj);
		}
	}
	
}

void Map::MapEnv::RemoveGroupUpdatedHandler(Optional<Map::MapEnv::GroupItem> group, Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	NN<Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	NN<GroupItem> nngroup;
	UOSInt i;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}

	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->RemoveUpdatedHandler(hdlr, obj);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			this->RemoveGroupUpdatedHandler(NN<Map::MapEnv::GroupItem>::ConvertFrom(item), hdlr, obj);
		}
	}
	
}

Map::MapEnv::MapEnv(Text::CStringNN fileName, UInt32 bgColor, NN<Math::CoordinateSystem> csys) : IO::ParsedObject(fileName)
{
	this->bgColor = bgColor;
	this->nStr = 1000;
	this->csys = csys;
	this->defFontStyle = 0;
	this->defLineStyle = 0;
	this->maxScale = 200000000;
	this->minScale = 10;

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

	NN<const Data::ArrayListNN<ImageInfo>> imgs = this->images.GetValues();
	i = imgs->GetCount();
	while (i-- > 0)
	{
		NN<ImageInfo> imgInfo = imgs->GetItemNoCheck(i);
		imgInfo->fileName->Release();
		imgInfo->imgs.Delete();
		MemFreeNN(imgInfo);
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
	this->csys.Delete();
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
	Sync::MutexUsage mutUsage(this->mut);
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
	Sync::MutexUsage mutUsage(this->mut);
	if (fontStyle < this->fontStyles.GetCount())
	{
		this->defFontStyle = fontStyle;
	}
}

Double Map::MapEnv::GetMaxScale() const
{
	return this->maxScale;
}

Double Map::MapEnv::GetMinScale() const
{
	return this->minScale;
}

UOSInt Map::MapEnv::AddLineStyle()
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	NN<Map::MapEnv::LineStyle> style;
	if (cnt == 0)
	{
		NEW_CLASSNN(style, Map::MapEnv::LineStyle());
		style->name = 0;
		return this->lineStyles.Add(style);
	}
	else
	{
		style = this->lineStyles.GetItemNoCheck(cnt - 1);
		if (style->layers.GetCount() == 0)
		{
			return cnt - 1;
		}
		NEW_CLASSNN(style, Map::MapEnv::LineStyle());
		style->name = 0;
		return this->lineStyles.Add(style);
	}
}

Bool Map::MapEnv::SetLineStyleName(UOSInt index, Text::CString name)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	NN<LineStyle> style;
	style = this->lineStyles.GetItemNoCheck(index);
	OPTSTR_DEL(style->name);
	style->name = Text::String::NewOrNull(name);
	return true;
}

UnsafeArrayOpt<UTF8Char> Map::MapEnv::GetLineStyleName(UOSInt index, UnsafeArray<UTF8Char> buff) const
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return 0;
	}
	NN<LineStyle> style;
	style = this->lineStyles.GetItemNoCheck(index);
	NN<Text::String> s;
	if (style->name.SetTo(s))
	{
		return s->ConcatTo(buff);
	}
	else
	{
		return 0;
	}
}

Bool Map::MapEnv::AddLineStyleLayer(UOSInt index, UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UOSInt npattern)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	NN<Map::MapEnv::LineStyleLayer> layer;
	NN<Map::MapEnv::LineStyle> style;
	UnsafeArray<const UInt8> nnpattern;
	UnsafeArray<UInt8> lpattern;
	style = this->lineStyles.GetItemNoCheck(index);
	layer = MemAllocNN(Map::MapEnv::LineStyleLayer);
	layer->color = color;
	layer->thick = thick;
	if (npattern && pattern.SetTo(nnpattern))
	{
		layer->pattern = lpattern = MemAllocArr(UInt8, npattern);
		layer->npattern = npattern;
		MemCopyNO(lpattern.Ptr(), nnpattern.Ptr(), npattern);
	}
	else
	{
		layer->pattern = 0;
		layer->npattern = 0;
	}
	style->layers.Add(layer);
	return true;
}

Bool Map::MapEnv::ChgLineStyleLayer(UOSInt index, UOSInt layerId, UInt32 color, Double thick, UnsafeArrayOpt<const UInt8> pattern, UOSInt npattern)
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	NN<Map::MapEnv::LineStyleLayer> layer;
	NN<Map::MapEnv::LineStyle> style;
	UnsafeArray<const UInt8> nnpattern;
	UnsafeArray<UInt8> lpattern;
	style = this->lineStyles.GetItemNoCheck(index);
	if (style->layers.GetCount() <= layerId)
	{
		return false;
	}
	layer = style->layers.GetItemNoCheck(layerId);
	if (layer->pattern.SetTo(lpattern))
	{
		MemFreeArr(lpattern);
	}
	layer->color = color;
	layer->thick = thick;
	if (npattern && pattern.SetTo(nnpattern))
	{
		layer->pattern = lpattern = MemAllocArr(UInt8, npattern);
		layer->npattern = npattern;
		MemCopyNO(lpattern.Ptr(), nnpattern.Ptr(), npattern);
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
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	NN<Map::MapEnv::LineStyleLayer> layer;
	NN<Map::MapEnv::LineStyle> style;
	UnsafeArray<UInt8> lpattern;
	style = this->lineStyles.GetItemNoCheck(index);
	if (!style->layers.RemoveAt(layerId).SetTo(layer))
	{
		return false;
	}
	if (layer->pattern.SetTo(lpattern))
	{
		MemFreeArr(lpattern);
	}
	MemFreeNN(layer);
	return true;
}

Bool Map::MapEnv::RemoveLineStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::LineStyle> style;
	if (!this->lineStyles.RemoveAt(index).SetTo(style))
	{
		return false;
	}
	NN<Map::MapEnv::LineStyleLayer> layer;
	UnsafeArray<UInt8> lpattern;
	UOSInt i;
	i = style->layers.GetCount();
	while (i-- > 0)
	{
		layer = style->layers.GetItemNoCheck(i);
		if (layer->pattern.SetTo(lpattern))
		{
			MemFreeArr(lpattern);
		}
		MemFreeNN(layer);
	}
	OPTSTR_DEL(style->name);
	style.Delete();
	return true;
}

UOSInt Map::MapEnv::GetLineStyleCount() const
{
	return this->lineStyles.GetCount();
}

Bool Map::MapEnv::GetLineStyleLayer(UOSInt index, UOSInt layerId, OutParam<UInt32> color, OutParam<Double> thick, OutParam<UnsafeArrayOpt<UInt8>> pattern, OutParam<UOSInt> npattern) const
{
	UOSInt cnt = this->lineStyles.GetCount();
	if (index >= cnt)
	{
		return false;
	}
	NN<Map::MapEnv::LineStyleLayer> layer;
	NN<Map::MapEnv::LineStyle> style;
	style = this->lineStyles.GetItemNoCheck(index);
	if (!style->layers.GetItem(layerId).SetTo(layer))
	{
		return false;
	}
	color.Set(layer->color);
	thick.Set(layer->thick);
	pattern.Set(layer->pattern);
	npattern.Set(layer->npattern);
	return true;
}

UOSInt Map::MapEnv::GetLineStyleLayerCnt(UOSInt index) const
{
	NN<Map::MapEnv::LineStyle> style;
	if (this->lineStyles.GetItem(index).SetTo(style))
	{
		return style->layers.GetCount();
	}
	return 0;
}

UOSInt Map::MapEnv::AddFontStyle(Text::CStringNN styleName, Text::CStringNN fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	NN<Map::MapEnv::FontStyle> style;
	if (fontName.leng == 0)
		return INVALID_INDEX;
	Sync::MutexUsage mutUsage(this->mut);
	style = MemAllocNN(Map::MapEnv::FontStyle);
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
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::FontStyle> style;
	if (!this->fontStyles.GetItem(index).SetTo(style))
		return false;
	OPTSTR_DEL(style->styleName);
	style->styleName = Text::String::NewOrNull(name);
	return true;
}

UnsafeArrayOpt<UTF8Char> Map::MapEnv::GetFontStyleName(UOSInt index, UnsafeArray<UTF8Char> buff) const
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::FontStyle> style;
	if (!this->fontStyles.GetItem(index).SetTo(style))
		return 0;
	NN<Text::String> s;
	if (style->styleName.SetTo(s))
		return s->ConcatTo(buff);
	return 0;
}

Bool Map::MapEnv::RemoveFontStyle(UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::FontStyle> style;
	if (!this->fontStyles.RemoveAt(index).SetTo(style))
		return false;
	OPTSTR_DEL(style->styleName);
	style->fontName->Release();
	MemFreeNN(style);
	return true;
}

UOSInt Map::MapEnv::GetFontStyleCount() const
{
	return this->fontStyles.GetCount();
}

Bool Map::MapEnv::GetFontStyle(UOSInt index, OutParam<NN<Text::String>> fontName, OutParam<Double> fontSizePt, OutParam<Bool> bold, OutParam<UInt32> fontColor, OutParam<UOSInt> buffSize, OutParam<UInt32> buffColor) const
{
	NN<Map::MapEnv::FontStyle> style;
	if (!this->fontStyles.GetItem(index).SetTo(style))
	{
		fontSizePt.Set(0);
		bold.Set(false);
		fontColor.Set(0xff000000);
		buffSize.Set(0);
		buffColor.Set(0xff000000);
		return false;
	}
	fontName.Set(style->fontName);
	fontSizePt.Set(style->fontSizePt);
	bold.Set(style->bold);
	fontColor.Set(style->fontColor);
	buffSize.Set(style->buffSize);
	buffColor.Set(style->buffColor);
	return true;
}

Bool Map::MapEnv::ChgFontStyle(UOSInt index, NN<Text::String> fontName, Double fontSizePt, Bool bold, UInt32 fontColor, UOSInt buffSize, UInt32 buffColor)
{
	NN<Map::MapEnv::FontStyle> style;
	if (!this->fontStyles.GetItem(index).SetTo(style))
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

UOSInt Map::MapEnv::AddLayer(Optional<Map::MapEnv::GroupItem> group, NN<Map::MapDrawLayer> layer, Bool needRelease)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (layer->GetObjectClass() == Map::MapDrawLayer::OC_MAP_LAYER_COLL)// && layer->GetLayerType() == Map::DRAW_LAYER_MIXED)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UnsafeArray<UTF8Char> sptr2;
		NN<Map::MapLayerCollection> layerColl = NN<Map::MapLayerCollection>::ConvertFrom(layer);
		sptr = layerColl->GetName()->ConcatTo(sbuff);
		sptr2 = &sbuff[Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '\\') + 1];
		NN<Map::MapEnv::GroupItem> grp = this->AddGroup(group, CSTRP(sptr2, sptr));

		NN<Map::MapDrawLayer> layer;
		UOSInt k;
		Data::ArrayListNN<Map::MapDrawLayer> layers;
		Sync::RWMutexUsage mutUsage;
		Data::ArrayIterator<NN<Map::MapDrawLayer>> it = layerColl->Iterator(mutUsage);
		while (it.HasNext())
		{
			layers.Add(it.Next());
		}
		mutUsage.EndUse();
		if (needRelease)
		{
			layerColl->Clear();
		}
		it = layers.Iterator();
		while (it.HasNext())
		{
			layer = it.Next();
			k = layerColl->GetUpdatedHandlerCnt();
			while (k-- > 0)
			{
				layer->AddUpdatedHandler(layerColl->GetUpdatedHandler(k), layerColl->GetUpdatedObject(k));
			}
			this->AddLayer(grp, layer, needRelease);
		}
		if (needRelease)
		{
			layerColl.Delete();
		}
		return 1;
	}
	else
	{
		NN<Map::MapEnv::LayerItem> lyr;
		lyr = MemAllocNN(Map::MapEnv::LayerItem);
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
		
		Map::DrawLayerType layerType = layer->GetLayerType();
		if (layerType == Map::DRAW_LAYER_MIXED || layerType == Map::DRAW_LAYER_POINT3D || layerType == Map::DRAW_LAYER_POINT)
		{
			if (this->GetImageCnt() == 0)
			{
				this->AddImageSquare(0xff22b14c, 16);
			}
		}

		NN<GroupItem> nngroup;
		if (group.SetTo(nngroup))
		{
			return nngroup->subitems.Add(lyr);
		}
		else
		{
			return this->mapLayers.Add(lyr);
		}
	}
}

Bool Map::MapEnv::ReplaceLayer(Optional<Map::MapEnv::GroupItem> group, UOSInt index, NN<Map::MapDrawLayer> layer, Bool needRelease)
{
	Optional<Map::MapEnv::MapItem> item;
	NN<MapItem> nnitem;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		item = nngroup->subitems.GetItem(index);
	}
	else
	{
		item = this->mapLayers.GetItem(index);
	}
	if (item.SetTo(nnitem) && nnitem->itemType == Map::MapEnv::IT_LAYER)
	{
		NN<Map::MapEnv::LayerItem> lyr = NN<Map::MapEnv::LayerItem>::ConvertFrom(nnitem);
		if (lyr->needRelease)
		{
			lyr->layer.Delete();
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

NN<Map::MapEnv::GroupItem> Map::MapEnv::AddGroup(Optional<Map::MapEnv::GroupItem> group, NN<Text::String> subgroupName)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::GroupItem> newG;
	NEW_CLASSNN(newG, Map::MapEnv::GroupItem());
	newG->itemType = Map::MapEnv::IT_GROUP;
	newG->groupName = subgroupName->Clone();
	newG->groupHide = false;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		nngroup->subitems.Add(newG);
	}
	else
	{
		this->mapLayers.Add(newG);
	}
	return newG;
}

NN<Map::MapEnv::GroupItem> Map::MapEnv::AddGroup(Optional<Map::MapEnv::GroupItem> group, Text::CStringNN subgroupName)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Map::MapEnv::GroupItem> newG;
	NEW_CLASSNN(newG, Map::MapEnv::GroupItem());
	newG->itemType = Map::MapEnv::IT_GROUP;
	newG->groupName = Text::String::New(subgroupName);
	newG->groupHide = false;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		nngroup->subitems.Add(newG);
	}
	else
	{
		this->mapLayers.Add(newG);
	}
	return newG;
}

void Map::MapEnv::RemoveItem(Optional<Map::MapEnv::GroupItem> group, UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	Optional<Map::MapEnv::MapItem> item;
	NN<Map::MapEnv::MapItem> nnitem;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		item = nngroup->subitems.RemoveAt(index);
	}
	else
	{
		item = this->mapLayers.RemoveAt(index);
	}
	if (item.SetTo(nnitem))
	{
		if (nnitem->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem> lyr = NN<Map::MapEnv::LayerItem>::ConvertFrom(nnitem);
			if (lyr->needRelease)
			{
				lyr->layer.Delete();
			}
			OPTSTR_DEL(lyr->fontName);
			MemFreeNN(lyr);
		}
		else if (nnitem->itemType == Map::MapEnv::IT_GROUP)
		{
			NN<Map::MapEnv::GroupItem> g = NN<Map::MapEnv::GroupItem>::ConvertFrom(nnitem);
			UOSInt i = g->subitems.GetCount();
			while (i-- > 0)
			{
				this->RemoveItem(g, i);
			}
			g->groupName->Release();
			g.Delete();
		}
	}
}

void Map::MapEnv::MoveItem(Optional<Map::MapEnv::GroupItem> group, UOSInt fromIndex, UOSInt toIndex)
{
	Sync::MutexUsage mutUsage(this->mut);
	if (fromIndex == toIndex)
		return;

	NN<MapItem> item;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		if (!nngroup->subitems.RemoveAt(fromIndex).SetTo(item))
			return;
		if (fromIndex < toIndex)
		{
			nngroup->subitems.Insert(toIndex - 1, item);
		}
		else
		{
			nngroup->subitems.Insert(toIndex, item);
		}
	}
	else
	{
		if (!this->mapLayers.RemoveAt(fromIndex).SetTo(item))
			return;
		if (fromIndex < toIndex)
		{
			this->mapLayers.Insert(toIndex - 1, item);
		}
		else
		{
			this->mapLayers.Insert(toIndex, item);
		}
	}
}

void Map::MapEnv::MoveItem(Optional<Map::MapEnv::GroupItem> fromGroup, UOSInt fromIndex, Optional<Map::MapEnv::GroupItem> toGroup, UOSInt toIndex)
{
	NN<Map::MapEnv::MapItem> item;
	if (fromGroup == toGroup)
	{
		MoveItem(fromGroup, fromIndex, toIndex);
		return;
	}
	Sync::MutexUsage mutUsage(this->mut);
	NN<GroupItem> nngroup;
	if (fromGroup.SetTo(nngroup))
	{
		if (!nngroup->subitems.RemoveAt(fromIndex).SetTo(item))
			return;
	}
	else
	{
		if (!this->mapLayers.RemoveAt(fromIndex).SetTo(item))
			return;
	}
	if (toIndex == INVALID_INDEX)
	{
		if (toGroup.SetTo(nngroup))
		{
			nngroup->subitems.Add(item);
		}
		else
		{
			this->mapLayers.Add(item);
		}
	}
	else
	{
		if (toGroup.SetTo(nngroup))
		{
			nngroup->subitems.Insert(toIndex, item);
		}
		else
		{
			this->mapLayers.Insert(toIndex, item);
		}
	}
}

UOSInt Map::MapEnv::GetItemCount(Optional<Map::MapEnv::GroupItem> group) const
{
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		return nngroup->subitems.GetCount();
	}
	else
	{
		return this->mapLayers.GetCount();
	}
}

Optional<Map::MapEnv::MapItem> Map::MapEnv::GetItem(Optional<Map::MapEnv::GroupItem> group, UOSInt index) const
{
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		return nngroup->subitems.GetItem(index);
	}
	else
	{
		return this->mapLayers.GetItem(index);
	}
}

NN<Text::String> Map::MapEnv::GetGroupName(NN<Map::MapEnv::GroupItem> group) const
{
	return group->groupName;
}

void Map::MapEnv::SetGroupName(NN<Map::MapEnv::GroupItem> group, Text::CStringNN name)
{
	Sync::MutexUsage mutUsage(this->mut);
	group->groupName->Release();
	group->groupName = Text::String::New(name);
}

void Map::MapEnv::SetGroupHide(NN<Map::MapEnv::GroupItem> group, Bool isHide)
{
	Sync::MutexUsage mutUsage(this->mut);
	group->groupHide = isHide;
}

Bool Map::MapEnv::GetGroupHide(NN<Map::MapEnv::GroupItem> group) const
{
	return group->groupHide;
}

Bool Map::MapEnv::GetLayerProp(NN<Map::MapEnv::LayerItem> setting, Optional<Map::MapEnv::GroupItem> group, UOSInt index) const
{
	Optional<Map::MapEnv::MapItem> item;
	NN<MapItem> nnitem;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		item = nngroup->subitems.GetItem(index);
	}
	else
	{
		item = this->mapLayers.GetItem(index);
	}
	if (item.SetTo(nnitem))
	{
		if (nnitem->itemType == Map::MapEnv::IT_LAYER)
		{
			MemCopyNO(setting.Ptr(), nnitem.Ptr(), sizeof(Map::MapEnv::LayerItem));
			return true;
		}
		else if (nnitem->itemType == Map::MapEnv::IT_GROUP)
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

Bool Map::MapEnv::SetLayerProp(NN<Map::MapEnv::LayerItem> setting, Optional<Map::MapEnv::GroupItem> group, UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	Optional<Map::MapEnv::MapItem> item;
	NN<MapItem> nnitem;
	NN<GroupItem> nngroup;
	NN<Text::String> s;
	if (group.SetTo(nngroup))
	{
		item = nngroup->subitems.GetItem(index);
	}
	else
	{
		item = this->mapLayers.GetItem(index);
	}
	if (item.SetTo(nnitem))
	{
		if (nnitem->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem> lyr = NN<Map::MapEnv::LayerItem>::ConvertFrom(nnitem);
			Map::DrawLayerType layerType;
			layerType = lyr->layer->GetLayerType();
			if (setting->minScale > setting->maxScale)
				return false;
			

			lyr->fontType = setting->fontType;
			lyr->fontStyle = setting->fontStyle;
			if (lyr->fontName != setting->fontName)
			{
				OPTSTR_DEL(lyr->fontName);
				if (setting->fontName.SetTo(s))
				{
					lyr->fontName = s->Clone();
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
		else if (nnitem->itemType == Map::MapEnv::IT_GROUP)
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
	Sync::MutexUsage mutUsage(this->mut);
	this->nStr = nStr;
}

UOSInt Map::MapEnv::GetImageCnt() const
{
	NN<ImageInfo> imgInfo;
	if (this->imgList.GetItem(this->imgList.GetCount() - 1).SetTo(imgInfo))
	{
		return imgInfo->index + imgInfo->cnt;
	}
	else
	{
		return 0;
	}
}

Optional<Media::StaticImage> Map::MapEnv::GetImage(UOSInt index, OptOut<UInt32> imgDurMS) const
{
	UOSInt i;
	NN<ImageInfo> imgInfo;
	i = this->imgList.GetCount();
	while (i-- > 0)
	{
		imgInfo = this->imgList.GetItemNoCheck(i);
		if (index >= imgInfo->index && index < (imgInfo->index + imgInfo->cnt))
		{
			if (imgInfo->isAni)
			{
				UInt32 imgTimeMS;
				Int64 currTimeTick;
				Optional<Media::StaticImage> simg;
				currTimeTick = Data::DateTimeUtil::GetCurrTimeMillis();
				if (currTimeTick >= imgInfo->aniLastTimeTick)
				{
					imgInfo->aniIndex++;
					if (imgInfo->aniIndex >= imgInfo->imgs->GetCount())
					{
						imgInfo->aniIndex = 0;
					}
					simg = Optional<Media::StaticImage>::ConvertFrom(imgInfo->imgs->GetImage(imgInfo->aniIndex, imgTimeMS));
					imgInfo->aniLastTimeTick = currTimeTick + (Int64)imgTimeMS;
				}
				else
				{
					simg = Optional<Media::StaticImage>::ConvertFrom(imgInfo->imgs->GetImage(imgInfo->aniIndex, 0));
					imgTimeMS = (UInt32)(imgInfo->aniLastTimeTick - currTimeTick);
				}
				imgDurMS.Set(imgTimeMS);
				return simg;
			}
			else
			{
				return Optional<Media::StaticImage>::ConvertFrom(imgInfo->imgs->GetImage(index - imgInfo->index, imgDurMS));
			}
		}
	}
	return 0;
}

OSInt Map::MapEnv::AddImage(Text::CStringNN fileName, NN<Parser::ParserList> parserList)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::StaticImage> simg;
	NN<ImageInfo> imgInfo;
	if (this->images.GetC(fileName).SetTo(imgInfo))
	{
		return (OSInt)imgInfo->index;
	}
	NN<IO::ParsedObject> pobj;
	IO::StmData::FileData fd(fileName, false);
	if (parserList->ParseFile(fd).SetTo(pobj))
	{
		if (pobj->GetParserType() == IO::ParserType::ImageList)
		{
			UOSInt i;
			imgInfo = MemAllocNN(ImageInfo);
			NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(pobj);
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
				if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(i, imgTime)).SetTo(simg))
					simg->ToB8G8R8A8();
			}
			if (imgInfo->isAni)
			{
				imgInfo->cnt = 1;
			}
			this->images.PutC(fileName, imgInfo);
			this->imgList.Add(imgInfo);
			return (OSInt)imgInfo->index;
		}
		else
		{
			pobj.Delete();
			return -1;
		}
	}
	return -1;
}

UOSInt Map::MapEnv::AddImage(Text::CStringNN fileName, NN<Media::ImageList> imgList)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Media::StaticImage> simg;
	NN<ImageInfo> imgInfo;
	if (this->images.GetC(fileName).SetTo(imgInfo))
	{
		imgList.Delete();
		return imgInfo->index;
	}
	UOSInt i;
	imgInfo = MemAllocNN(ImageInfo);
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
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(i, imgTime)).SetTo(simg))
		{
			simg->ToB8G8R8A8();
			if (imgTime != 0)
			{
				imgInfo->isAni = true;
			}
		}
	}
	this->images.PutNN(imgInfo->fileName, imgInfo);
	this->imgList.Add(imgInfo);
	return imgInfo->index;
}

UOSInt Map::MapEnv::AddImageSquare(UInt32 color, UOSInt size)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	NN<ImageInfo> imgInfo;
	Sync::MutexUsage mutUsage(this->mut);
	imgInfo = MemAllocNN(ImageInfo);
	imgInfo->index = this->GetImageCnt();
	sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Image")), imgInfo->index), UTF8STRC(".png"));
	imgInfo->fileName = Text::String::NewP(sbuff, UnsafeArray<const UTF8Char>(sptr));
	imgInfo->cnt = 1;
	NEW_CLASSNN(imgInfo->imgs, Media::ImageList(imgInfo->fileName));
	imgInfo->isAni = false;
	imgInfo->aniIndex = (UOSInt)-1;
	imgInfo->aniLastTimeTick = 0;
	NN<Media::StaticImage> simg;
	Media::AlphaType atype;
	if ((color & 0xff000000) == 0xff000000)
		atype = Media::AlphaType::AT_ALPHA_ALL_FF;
	else
		atype = Media::AlphaType::AT_ALPHA;
	NEW_CLASSNN(simg, Media::StaticImage(Math::Size2D<UOSInt>(size, size), 0, 32, Media::PixelFormat::PF_B8G8R8A8, size * size * 4, Media::ColorProfile(Media::ColorProfile::CPT_SRGB), Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT));
	ImageUtil_ColorFill32(simg->data.Ptr(), size * size, color);
	imgInfo->imgs->AddImage(simg, 0);
	this->images.PutNN(imgInfo->fileName, imgInfo);
	this->imgList.Add(imgInfo);
	return imgInfo->index;
}

UOSInt Map::MapEnv::GetImageFileCnt() const
{
	return this->imgList.GetCount();
}

Bool Map::MapEnv::GetImageFileInfo(UOSInt index, NN<Map::MapEnv::ImageInfo> info) const
{
	NN<Map::MapEnv::ImageInfo> imgInfo;
	if (!this->imgList.GetItem(index).SetTo(imgInfo))
		return false;
	info.CopyFrom(imgInfo);
	return true;
}

UOSInt Map::MapEnv::GetImageFileIndex(UOSInt index) const
{
	UOSInt i = this->imgList.GetCount();
	NN<ImageInfo> info;
	while (i-- > 0)
	{
		info = this->imgList.GetItemNoCheck(i);
		if (index >= info->index && index < info->index + info->cnt)
		{
			return i;
		}
	}
	return 0;
}

UOSInt Map::MapEnv::GetLayersOfType(NN<Data::ArrayListNN<Map::MapDrawLayer>> layers, Map::DrawLayerType lyrType) const
{
	return this->GetLayersInList(layers, this->mapLayers, lyrType);
}

void Map::MapEnv::AddUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->AddGroupUpdatedHandler(0, hdlr, obj);
}

void Map::MapEnv::RemoveUpdatedHandler(Map::MapDrawLayer::UpdatedHandler hdlr, AnyType obj)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->RemoveGroupUpdatedHandler(0, hdlr, obj);
}

Int64 Map::MapEnv::GetTimeEndTS(Optional<Map::MapEnv::GroupItem> group) const
{
	NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	UOSInt i;
	Int64 val = 0;
	Int64 val2 = 0;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			val2 = NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->GetTimeEndTS();
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			val2 = this->GetTimeEndTS(NN<Map::MapEnv::GroupItem>::ConvertFrom(item));
		}
		if (val2 != 0)
		{
			if (val == 0)
				val = val2;
			else if (val2 > val)
				val = val2;
		}
	}
	return val;
}

Int64 Map::MapEnv::GetTimeStartTS(Optional<Map::MapEnv::GroupItem> group) const
{
	NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	UOSInt i;
	Int64 val = 0;
	Int64 val2 = 0;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			val2 = NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->GetTimeStartTS();
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			val2 = GetTimeStartTS(NN<Map::MapEnv::GroupItem>::ConvertFrom(item));
		}
		if (val2 != 0)
		{
			if (val == 0)
				val = val2;
			else if (val2 < val)
				val = val2;
		}
	}
	return val;
}

void Map::MapEnv::SetCurrTimeTS(Optional<Map::MapEnv::GroupItem> group, Int64 timeStamp)
{
	Sync::MutexUsage mutUsage(this->mut);
	NN<Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	UOSInt i;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	i = objs->GetCount();
	while (i-- > 0)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->SetCurrTimeTS(timeStamp);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			mutUsage.EndUse();
			this->SetCurrTimeTS(NN<Map::MapEnv::GroupItem>::ConvertFrom(item), timeStamp);
			mutUsage.BeginUse();
		}
	}
}

Optional<Map::MapDrawLayer> Map::MapEnv::GetFirstLayer(Optional<Map::MapEnv::GroupItem> group) const
{
	NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	Optional<Map::MapDrawLayer> lyrObj;
	UOSInt i;
	UOSInt j;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	i = 0;
	j = objs->GetCount();
	while (i < j)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			return NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer;
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			lyrObj = this->GetFirstLayer(NN<Map::MapEnv::GroupItem>::ConvertFrom(item));
			if (lyrObj.NotNull())
				return lyrObj;
		}
		i++;
	}
	return 0;
}

UOSInt Map::MapEnv::GetLayersInGroup(Optional<Map::MapEnv::GroupItem> group, NN<Data::ArrayListNN<Map::MapDrawLayer>> layers) const
{
	NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	UOSInt i;
	UOSInt j;
	UOSInt ret = 0;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	i = 0;
	j = objs->GetCount();
	while (i < j)
	{
		item = objs->GetItemNoCheck(i);
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			layers->Add(NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer);
			ret++;
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			ret += this->GetLayersInGroup(NN<Map::MapEnv::GroupItem>::ConvertFrom(item), layers);
		}
		i++;
	}
	return ret;
}

Bool Map::MapEnv::GetBounds(Optional<Map::MapEnv::GroupItem> group, OutParam<Math::RectAreaDbl> bounds) const
{
	Data::ArrayListNN<Map::MapDrawLayer> layers;
	this->GetLayersInGroup(group, layers);
	Math::RectAreaDbl minMax = Math::RectAreaDbl(0, 0, 0, 0);
	Math::RectAreaDbl thisBounds;
	NN<Math::CoordinateSystem> lyrCSys;
	Bool isFirst = true;
	Data::ArrayIterator<NN<Map::MapDrawLayer>> it = layers.Iterator();
	while (it.HasNext())
	{
		NN<Map::MapDrawLayer> lyr = it.Next();
		if (lyr->GetBounds(thisBounds))
		{
			lyrCSys = lyr->GetCoordinateSystem();
			if (!this->csys->Equals(lyrCSys))
			{
				thisBounds.min = Math::CoordinateSystem::Convert(lyrCSys, this->csys, thisBounds.min);
				thisBounds.max = Math::CoordinateSystem::Convert(lyrCSys, this->csys, thisBounds.max);
			}
			if (isFirst)
			{
				isFirst = false;
				minMax = thisBounds;
			}
			else
			{
				minMax.min = minMax.min.Min(thisBounds.min);
				minMax.max = minMax.max.Max(thisBounds.max);
			}
		}	
	}
	bounds.Set(minMax);
	return !isFirst;
}

Bool Map::MapEnv::GetLayerBounds(Optional<Map::MapEnv::GroupItem> group, UOSInt index, OutParam<Math::RectAreaDbl> bounds) const
{
	NN<const Data::ArrayListNN<Map::MapEnv::MapItem>> objs;
	NN<Map::MapEnv::MapItem> item;
	NN<GroupItem> nngroup;
	if (group.SetTo(nngroup))
	{
		objs = nngroup->subitems;
	}
	else
	{
		objs = this->mapLayers;
	}
	if (objs->GetItem(index).SetTo(item))
	{
		if (item->itemType == Map::MapEnv::IT_LAYER)
		{
			return NN<Map::MapEnv::LayerItem>::ConvertFrom(item)->layer->GetBounds(bounds);
		}
		else if (item->itemType == Map::MapEnv::IT_GROUP)
		{
			return this->GetBounds(NN<Map::MapEnv::GroupItem>::ConvertFrom(item), bounds);
		}
	}
	return false;
}

NN<Map::MapView> Map::MapEnv::CreateMapView(Math::Size2DDbl scnSize) const
{
	NN<Map::MapDrawLayer> baseLayer;
	if (GetFirstLayer(0).SetTo(baseLayer))
	{
		return baseLayer->CreateMapView(scnSize);
	}
	if (csys->IsProjected())
	{
		NN<Map::MapView> view;
		NEW_CLASSNN(view, Map::ScaledMapView(scnSize, Math::Coord2DDbl(835000, 820000), 10000, true));
		return view;
	}
	else
	{
		NN<Map::MapView> view;
		NEW_CLASSNN(view, Map::ScaledMapView(scnSize, Math::Coord2DDbl(114.2, 22.4), 10000, true));
		return view;
	}
}

NN<Math::CoordinateSystem> Map::MapEnv::GetCoordinateSystem() const
{
	return this->csys;
}

UInt32 Map::MapEnv::GetSRID() const
{
	return this->csys->GetSRID();
}

void Map::MapEnv::BeginUse(NN<Sync::MutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut);
}