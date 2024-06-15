#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/OWSFeatureParser.h"
#include "Map/TileMapUtil.h"
#include "Map/WebMapTileServiceSource.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Math/Unit/Distance.h"
#include "Net/HTTPClient.h"
#include "Parser/FileParser/JSONParser.h"
#include "Parser/FileParser/XMLParser.h"
#include "Text/JSON.h"
#include "Text/XMLReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void Map::WebMapTileServiceSource::LoadXML()
{
	Text::StringBuilderUTF8 sb;
	sb.Append(this->wmtsURL);
	sb.AppendC(UTF8STRC("?REQUEST=GetCapabilities"));
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		cli.Delete();
		return;
	}
	{
		Bool valid = false;
		Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);
		NN<Text::String> nodeText;
		if (reader.NextElementName().SetTo(nodeText))
		{
			if (nodeText->Equals(UTF8STRC("Capabilities")))
			{
				valid = true;
			}
		}

		if (valid)
		{
			Text::StringBuilderUTF8 sb;
			while (reader.NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("Contents")))
				{
					while (reader.NextElementName().SetTo(nodeText))
					{
						if (nodeText->Equals(UTF8STRC("Layer")))
						{
							ReadLayer(reader);
						}
						else if (nodeText->Equals(UTF8STRC("TileMatrixSet")))
						{
							NN<TileMatrixDefSet> set;
							if (ReadTileMatrixSet(reader).SetTo(set))
							{
								this->matrixDef.Put(set->id, set);
							}
						}
						else
						{
							reader.SkipElement();
						}
					}
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
	}
	cli.Delete();
	this->SetLayer(0);
}

void Map::WebMapTileServiceSource::ReadLayer(NN<Text::XMLReader> reader)
{
	Text::StringBuilderUTF8 sb;
	NN<Text::String> name;
	NN<TileLayer> layer;
	UOSInt i;
	Text::PString sarr[3];
	NEW_CLASSNN(layer, TileLayer());
	layer->title = 0;
	layer->id = 0;
	while (reader->NextElementName().SetTo(name))
	{
		if (name->Equals(UTF8STRC("ows:Title")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				OPTSTR_DEL(layer->title);
				layer->title = Text::String::New(sb.ToCString());
			}
		}
		else if (name->Equals(UTF8STRC("ows:WGS84BoundingBox")))
		{
			while (reader->NextElementName().SetTo(name))
			{
				if (name->Equals(UTF8STRC("ows:LowerCorner")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (Text::StrSplitP(sarr, 3, sb, ' ') == 2)
					{
						layer->wgs84Bounds.min.x = sarr[0].ToDouble();
						layer->wgs84Bounds.min.y = sarr[1].ToDouble();
					}
				}
				else if (name->Equals(UTF8STRC("ows:UpperCorner")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (Text::StrSplitP(sarr, 3, sb, ' ') == 2)
					{
						layer->wgs84Bounds.max.x = sarr[0].ToDouble();
						layer->wgs84Bounds.max.y = sarr[1].ToDouble();
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		else if (name->Equals(UTF8STRC("ows:Identifier")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				OPTSTR_DEL(layer->id);
				layer->id = Text::String::New(sb.ToCString());
			}
		}
		else if (name->Equals(UTF8STRC("Style")))
		{
			//////////////////////////////////////////
			reader->SkipElement();
		}
		else if (name->Equals(UTF8STRC("Format")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				layer->format.Add(Text::String::New(sb.ToCString()));
			}
		}
		else if (name->Equals(UTF8STRC("InfoFormat")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				layer->infoFormat.Add(Text::String::New(sb.ToCString()));
			}
		}
		else if (name->Equals(UTF8STRC("TileMatrixSetLink")))
		{
			NN<TileMatrixSet> set;
			if (ReadTileMatrixSetLink(reader).SetTo(set))
			{
				set->bounds.min = Math::CoordinateSystem::Convert(this->wgs84, set->csys, layer->wgs84Bounds.min);
				set->bounds.max = Math::CoordinateSystem::Convert(this->wgs84, set->csys, layer->wgs84Bounds.max);
				layer->tileMatrixes.Add(set);
			}
		}
		else if (name->Equals(UTF8STRC("ResourceURL")))
		{
			Optional<Text::XMLAttrib> formatAttr = 0;
			Optional<Text::XMLAttrib> resourceTypeAttr = 0;
			Optional<Text::XMLAttrib> templateAttr = 0;
			NN<Text::XMLAttrib> nnformatAttr;
			NN<Text::XMLAttrib> nnresourceTypeAttr;
			NN<Text::XMLAttrib> nntemplateAttr;
			NN<Text::XMLAttrib> attr;
			i = reader->GetAttribCount();
			while (i-- > 0)
			{
				attr = reader->GetAttribNoCheck(i);
				if (attr->name->Equals(UTF8STRC("format")))
				{
					formatAttr = attr;
				}
				else if (attr->name->Equals(UTF8STRC("resourceType")))
				{
					resourceTypeAttr = attr;
				}
				else if (attr->name->Equals(UTF8STRC("template")))
				{
					templateAttr = attr;
				}
			}
			if (formatAttr.SetTo(nnformatAttr) && resourceTypeAttr.SetTo(nnresourceTypeAttr) && templateAttr.SetTo(nntemplateAttr) && nnformatAttr->value && nnresourceTypeAttr->value && nntemplateAttr->value)
			{
				NN<ResourceURL> resource = MemAllocNN(ResourceURL);
				resource->format = nnformatAttr->value->Clone();
				resource->templateURL = nntemplateAttr->value->Clone();
				if (nnresourceTypeAttr->value->Equals(UTF8STRC("tile")))
				{
					resource->resourceType = ResourceType::Tile;
				}
				else if (nnresourceTypeAttr->value->Equals(UTF8STRC("FeatureInfo")))
				{
					resource->resourceType = ResourceType::FeatureInfo;
				}
				else
				{
					resource->resourceType = ResourceType::Unknown;
				}
				if (resource->format->Equals(UTF8STRC("image/png")))
				{
					resource->imgType = Map::TileMap::ImageType::IT_PNG;
				}
				else if (resource->format->Equals(UTF8STRC("image/jpeg")))
				{
					resource->imgType = Map::TileMap::ImageType::IT_JPG;
				}
				else
				{
					resource->imgType = Map::TileMap::ImageType::IT_PNG;
				}
				layer->resourceURLs.Add(resource);
			}
			reader->SkipElement();
		}
		else
		{
			reader->SkipElement();
		}
	}
	NN<Text::String> id;
	if (layer->title.IsNull() || !layer->id.SetTo(id) || layer->format.GetCount() == 0 || layer->tileMatrixes.GetCount() == 0 || layer->resourceURLs.GetCount() == 0)
	{
		this->ReleaseLayer(layer);
	}
	else
	{
		this->layers.PutNN(id, layer);
	}
}

Optional<Map::WebMapTileServiceSource::TileMatrixSet> Map::WebMapTileServiceSource::ReadTileMatrixSetLink(NN<Text::XMLReader> reader)
{
	NN<Text::String> name;
	NN<TileMatrixSet> set;
	Text::StringBuilderUTF8 sb;
	NEW_CLASSNN(set, TileMatrixSet());
	set->id = Text::String::NewEmpty();
	set->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	while (reader->NextElementName().SetTo(name))
	{
		if (name->Equals(UTF8STRC("TileMatrixSet")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				set->id->Release();
				set->id = Text::String::New(sb.ToCString());
			}
		}
		else if (name->Equals(UTF8STRC("TileMatrixSetLimits")))
		{
			while (reader->NextElementName().SetTo(name))
			{
				if (name->Equals(UTF8STRC("TileMatrixLimits")))
				{
					Text::String *id = 0;
					Int32 minRow = 0x7fffffff;
					Int32 maxRow = 0x7fffffff;
					Int32 minCol = 0x7fffffff;
					Int32 maxCol = 0x7fffffff;
					while (reader->NextElementName().SetTo(name))
					{
						if (name->Equals(UTF8STRC("TileMatrix")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								SDEL_STRING(id);
								id = Text::String::New(sb.ToCString()).Ptr();
							}
						}
						else if (name->Equals(UTF8STRC("MinTileRow")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								minRow = sb.ToInt32();
							}
						}
						else if (name->Equals(UTF8STRC("MaxTileRow")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								maxRow = sb.ToInt32();
							}
						}
						else if (name->Equals(UTF8STRC("MinTileCol")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								minCol = sb.ToInt32();
							}
						}
						else if (name->Equals(UTF8STRC("MaxTileCol")))
						{
							sb.ClearStr();
							reader->ReadNodeText(sb);
							if (sb.GetLength() > 0)
							{
								maxCol = sb.ToInt32();
							}
						}
						else
						{
							reader->SkipElement();
						}
					}
					if (id && minRow != 0x7fffffff && maxRow != 0x7fffffff && minCol != 0x7fffffff && maxCol != 0x7fffffff)
					{
						NN<TileMatrix> tile = MemAllocNN(TileMatrix);
						tile->id = id->Clone();
						tile->minRow = minRow;
						tile->maxRow = maxRow;
						tile->minCol = minCol;
						tile->maxCol = maxCol;
						set->tiles.Add(tile);
					}
					SDEL_STRING(id);
				}
				else
				{
					reader->SkipElement();
				}
			}
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (set->id->leng > 0 && set->tiles.GetCount() > 0)
	{
		NN<Math::CoordinateSystem> csys;
		if (Math::CoordinateSystemManager::CreateFromName(set->id->ToCString()).SetTo(csys))
		{
			set->csys.Delete();
			set->csys = csys;
		}
		return set;
	}
	else
	{
		this->ReleaseTileMatrixSet(set);
		return 0;
	}
}

Optional<Map::WebMapTileServiceSource::TileMatrixDefSet> Map::WebMapTileServiceSource::ReadTileMatrixSet(NN<Text::XMLReader> reader)
{
	NN<TileMatrixDefSet> set;
	Text::StringBuilderUTF8 sb;
	NEW_CLASSNN(set, TileMatrixDefSet());
	set->id = 0;
	set->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	NN<Text::String> name;
	while (reader->NextElementName().SetTo(name))
	{
		if (name->Equals(UTF8STRC("ows:Identifier")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				SDEL_STRING(set->id);
				set->id = Text::String::New(sb.ToCString()).Ptr();
			}
		}
		else if (name->Equals(UTF8STRC("ows:SupportedCRS")))
		{
			sb.ClearStr();
			reader->ReadNodeText(sb);
			if (sb.GetLength() > 0)
			{
				set->csys.Delete();
				set->csys = Math::CoordinateSystemManager::CreateFromNameOrDef(sb.ToCString());
			}
		}
		else if (name->Equals(UTF8STRC("TileMatrix")))
		{
			Text::String *id = 0;
			Double scaleDenom = 0;
			Double topPos = -1;
			Double leftPos = -1;
			UInt32 tileWidth = 0;
			UInt32 tileHeight = 0;
			UInt32 matrixWidth = 0;
			UInt32 matrixHeight = 0;
			while (reader->NextElementName().SetTo(name))
			{
				if (name->Equals(UTF8STRC("ows:Identifier")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						SDEL_STRING(id);
						id = Text::String::New(sb.ToCString()).Ptr();
					}
				}
				else if (name->Equals(UTF8STRC("ScaleDenominator")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						scaleDenom = sb.ToDouble();
					}
				}
				else if (name->Equals(UTF8STRC("TopLeftCorner")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						Text::PString sarr[3];
						if (Text::StrSplitP(sarr, 3, sb, ' ') == 2)
						{
							leftPos = sarr[0].ToDouble();
							topPos = sarr[1].ToDouble();
						}
					}
				}
				else if (name->Equals(UTF8STRC("TileWidth")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						tileWidth = sb.ToUInt32();
					}
				}
				else if (name->Equals(UTF8STRC("TileHeight")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						tileHeight = sb.ToUInt32();
					}
				}
				else if (name->Equals(UTF8STRC("MatrixWidth")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						matrixWidth = sb.ToUInt32();
					}
				}
				else if (name->Equals(UTF8STRC("MatrixHeight")))
				{
					sb.ClearStr();
					reader->ReadNodeText(sb);
					if (sb.GetLength() > 0)
					{
						matrixHeight = sb.ToUInt32();
					}
				}
				else
				{
					reader->SkipElement();
				}
			}
			if (id && scaleDenom != 0 && topPos != -1 && leftPos != -1 && tileWidth != 0 && tileHeight != 0 && matrixWidth != 0 && matrixHeight != 0)
			{
				NN<TileMatrixDef> tile = MemAllocNN(TileMatrixDef);
				tile->id = id->Clone();
				tile->scaleDenom = scaleDenom;
				tile->unitPerPixel = scaleDenom / 3571.428571;
				tile->origin.x = leftPos;
				tile->origin.y = topPos;
				tile->tileWidth = tileWidth;
				tile->tileHeight = tileHeight;
				tile->matrixWidth = matrixWidth;
				tile->matrixHeight = matrixHeight;
				set->tiles.Add(tile);
			}
			SDEL_STRING(id);
		}
		else
		{
			reader->SkipElement();
		}
	}
	if (set->id && set->tiles.GetCount() > 0)
	{
		UOSInt i = 0;
		UOSInt j = set->tiles.GetCount();
		Double unitPerPixel;
		if (!set->csys->IsProjected())
		{
			Double tmp;
			unitPerPixel = 0.703125;
			while (i < j)
			{
				NN<TileMatrixDef> tile = set->tiles.GetItemNoCheck(i);
				tmp = tile->origin.x;
				tile->origin.x = tile->origin.y;
				tile->origin.y = tmp;
				tile->unitPerPixel = unitPerPixel;
				unitPerPixel *= 0.5;
				i++;
			}
		}
		else
		{
			unitPerPixel = 156543.03390625;
			while (i < j)
			{
				NN<TileMatrixDef> tile = set->tiles.GetItemNoCheck(i);
				tile->unitPerPixel = unitPerPixel;
				unitPerPixel *= 0.5;
				i++;
			}
		}
		return set;
	}
	else
	{
		this->ReleaseTileMatrixDefSet(set);
		return 0;
	}
}

Optional<Map::WebMapTileServiceSource::TileMatrix> Map::WebMapTileServiceSource::GetTileMatrix(UOSInt level) const
{
	NN<TileMatrixSet> currSet;
	if (!this->currSet.SetTo(currSet))
		return 0;
	return currSet->tiles.GetItem(level);
}

void Map::WebMapTileServiceSource::ReleaseLayer(NN<TileLayer> layer)
{
	OPTSTR_DEL(layer->id);
	OPTSTR_DEL(layer->title);
	layer->tileMatrixes.FreeAll(ReleaseTileMatrixSet);
	layer->format.FreeAll();
	layer->infoFormat.FreeAll();
	layer->resourceURLs.FreeAll(ReleaseResourceURL);
	layer.Delete();
}

void Map::WebMapTileServiceSource::ReleaseTileMatrix(NN<TileMatrix> tileMatrix)
{
	tileMatrix->id->Release();
	MemFreeNN(tileMatrix);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixSet(NN<TileMatrixSet> set)
{
	set->id->Release();
	set->csys.Delete();
	set->tiles.FreeAll(ReleaseTileMatrix);
	set.Delete();
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixDef(NN<TileMatrixDef> tileMatrix)
{
	tileMatrix->id->Release();
	MemFreeNN(tileMatrix);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixDefSet(NN<TileMatrixDefSet> set)
{
	set->id->Release();
	set->csys.Delete();
	set->tiles.FreeAll(ReleaseTileMatrixDef);
	set.Delete();
}

void Map::WebMapTileServiceSource::ReleaseResourceURL(NN<ResourceURL> resourceURL)
{
	resourceURL->templateURL->Release();
	resourceURL->format->Release();
	MemFreeNN(resourceURL);
}

Map::WebMapTileServiceSource::WebMapTileServiceSource(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN wmtsURL)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->wmtsURL = Text::String::New(wmtsURL);
	this->currLayer = 0;
	this->currDef = 0;
	this->currResource = 0;
	this->currResourceInfo = 0;
	this->currSet = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->LoadXML();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("wmts"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Crypto::Hash::CRC32RC crc;
	sptr = Text::StrHexVal32(sptr, crc.CalcDirect(wmtsURL.v, wmtsURL.leng));
	this->cacheDir = Text::String::NewP(sbuff, sptr);
}

Map::WebMapTileServiceSource::~WebMapTileServiceSource()
{
	this->wmtsURL->Release();
	this->cacheDir->Release();
	this->layers.FreeAll(ReleaseLayer);
	this->matrixDef.FreeAll(ReleaseTileMatrixDefSet);
	this->wgs84.Delete();
}

Text::CStringNN Map::WebMapTileServiceSource::GetName() const
{
	NN<TileLayer> currLayer;
	if (this->currLayer.SetTo(currLayer))
		return Text::String::OrEmpty(currLayer->id)->ToCString();
	return CSTR("");
}

Bool Map::WebMapTileServiceSource::IsError() const
{
	return this->layers.GetCount() == 0 || this->currLayer.IsNull();
}

Map::TileMap::TileType Map::WebMapTileServiceSource::GetTileType() const
{
	return Map::TileMap::TT_WMTS;
}

UOSInt Map::WebMapTileServiceSource::GetMinLevel() const
{
	return 0;
}

UOSInt Map::WebMapTileServiceSource::GetMaxLevel() const
{
	NN<TileMatrixSet> currSet;
	if (!this->currSet.SetTo(currSet))
		return 0;
	return currSet->tiles.GetCount() - 1;
}

Double Map::WebMapTileServiceSource::GetLevelScale(UOSInt level) const
{
	NN<TileMatrixDefSet> currDef;
	if (!this->currDef.SetTo(currDef))
		return 0;
	NN<TileMatrixDef> tileMatrixDef;
	if (!currDef->tiles.GetItem(level).SetTo(tileMatrixDef))
	{
		return 0;
	}
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(currDef->csys);
	return tileMatrixDef->unitPerPixel / scaleDiv;
}

UOSInt Map::WebMapTileServiceSource::GetNearestLevel(Double scale) const
{
	NN<TileMatrixDefSet> currDef;
	NN<TileMatrixSet> currSet;
	if (!this->currSet.SetTo(currSet) || !this->currDef.SetTo(currDef))
		return 0;
	Double minDiff = 1.0E+100;
	UOSInt minLevel = 0;
	UOSInt i = currSet->tiles.GetCount();
	NN<TileMatrixDef> tileMatrixDef;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(currSet->csys);
	while (i-- > 0)
	{
		tileMatrixDef = currDef->tiles.GetItemNoCheck(i);
		layerScale = tileMatrixDef->unitPerPixel / scaleDiv;
		thisDiff = Math_Ln(scale / layerScale);
		if (thisDiff < 0)
		{
			thisDiff = -thisDiff;
		}
		if (thisDiff < minDiff)
		{
			minDiff = thisDiff;
			minLevel = i;
		}
	}
	return minLevel;
}

UOSInt Map::WebMapTileServiceSource::GetConcurrentCount() const
{
	return 2;
}

Bool Map::WebMapTileServiceSource::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	NN<TileMatrixSet> currSet;
	if (this->currSet.SetTo(currSet))
	{
		bounds.Set(currSet->bounds);
		return true;
	}
	return false;
}

NN<Math::CoordinateSystem> Map::WebMapTileServiceSource::GetCoordinateSystem() const
{
	NN<TileMatrixSet> currSet;
	if (this->currSet.SetTo(currSet))
	{
		return currSet->csys;
	}
	else
	{
		return this->wgs84;
	}
}

Bool Map::WebMapTileServiceSource::IsMercatorProj() const
{
	return false;
}

UOSInt Map::WebMapTileServiceSource::GetTileSize() const
{
	NN<TileMatrixDefSet> currDef;
	if (this->currDef.SetTo(currDef) && currDef->tiles.GetCount() > 0)
	{
		return currDef->tiles.GetItemNoCheck(0)->tileWidth;
	}
	else
	{
		return 0;
	}
}

Map::TileMap::ImageType Map::WebMapTileServiceSource::GetImageType() const
{
	NN<ResourceURL> currResource;
	if (this->currResource.SetTo(currResource))
		return currResource->imgType;
	return IT_PNG;
}

Bool Map::WebMapTileServiceSource::CanQuery() const
{
	return this->currResourceInfo != 0;
}

Bool Map::WebMapTileServiceSource::QueryInfos(Math::Coord2DDbl coord, UOSInt level, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList) const
{
	NN<TileMatrixDefSet> currDef;
	NN<TileMatrixSet> currSet;
	NN<ResourceURL> currResourceInfo;
	if (!this->currResourceInfo.SetTo(currResourceInfo) || !this->currDef.SetTo(currDef) || !this->currSet.SetTo(currSet))
		return false;

	NN<TileMatrix> tileMatrix;
	NN<TileMatrixDef> tileMatrixDef;
	if (!this->GetTileMatrix(level).SetTo(tileMatrix) || !currDef->tiles.GetItem(level).SetTo(tileMatrixDef))
	{
		return false;
	}
	UTF8Char tmpBuff[1024];
	UnsafeArray<UTF8Char> tmpPtr;

	Int32 pxX = (Int32)((coord.x - tileMatrixDef->origin.x) / tileMatrixDef->unitPerPixel);
	Int32 pxY = (Int32)((tileMatrixDef->origin.y - coord.y) / tileMatrixDef->unitPerPixel);
	Int32 imgX = pxX / (Int32)tileMatrixDef->tileWidth;
	Int32 imgY = pxY / (Int32)tileMatrixDef->tileHeight;
	Int32 tileX = pxX % (Int32)tileMatrixDef->tileWidth;
	Int32 tileY = pxY % (Int32)tileMatrixDef->tileHeight;

	Text::StringBuilderUTF8 urlSb;
	urlSb.Append(currResourceInfo->templateURL);
	tmpPtr = Text::StrInt32(tmpBuff, imgX);
	urlSb.ReplaceStr(UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, imgY);
	urlSb.ReplaceStr(UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, tileX);
	urlSb.ReplaceStr(UTF8STRC("{I}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, tileY);
	urlSb.ReplaceStr(UTF8STRC("{J}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	urlSb.ReplaceStr(UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{TileMatrixSet}"), currSet->id->v, currSet->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{style}"), UTF8STRC(""));

#if defined(VERBOSE)
	printf("Info URL: %s\r\n", urlSb.ToString());
#endif

	Text::StringBuilderUTF8 sb;
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("WMTS/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	UOSInt readSize;
	while ((readSize = cli->Read(BYTEARR(tmpBuff))) > 0)
	{
		sb.AppendC(tmpBuff, readSize);
	}
	cli.Delete();

	if (currResourceInfo->format->Equals(UTF8STRC("text/plain")))
	{
		return Map::OWSFeatureParser::ParseText(sb, currDef->csys->GetSRID(), coord, vecList, valueOfstList, nameList, valueList);
	}
	else if (currResourceInfo->format->Equals(UTF8STRC("application/json")))
	{
		return Map::OWSFeatureParser::ParseJSON(sb.ToCString(), currDef->csys->GetSRID(), vecList, valueOfstList, nameList, valueList, coord);
	}
	else if (currResourceInfo->format->StartsWith(UTF8STRC("application/vnd.ogc.gml")) || currResourceInfo->format->Equals(UTF8STRC("text/xml")))
	{
		return Map::OWSFeatureParser::ParseGML(sb.ToCString(), currDef->csys->GetSRID(), false, this->encFact, vecList, valueOfstList, nameList, valueList);
	}
	else
	{
#if defined(VERBOSE)
		printf("%s\r\n", sb.ToString());
#endif
	}
	return false;
}

UOSInt Map::WebMapTileServiceSource::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids)
{
	NN<TileMatrix> tileMatrix;
	NN<TileMatrixDef> tileMatrixDef;
	NN<TileMatrixDefSet> currDef;
	NN<TileMatrixSet> currSet;
	if (!this->GetTileMatrix(level).SetTo(tileMatrix) || !this->currDef.SetTo(currDef) || !this->currSet.SetTo(currSet) || !currDef->tiles.GetItem(level).SetTo(tileMatrixDef))
	{
		return 0;
	}
	
	rect = rect.OverlapArea(currSet->bounds);
	UOSInt ret = 0;
	Int32 minX = (Int32)((rect.min.x - tileMatrixDef->origin.x) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth)));
	Int32 maxX = (Int32)((rect.max.x - tileMatrixDef->origin.x) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth)));
	Int32 minY = (Int32)((tileMatrixDef->origin.y - rect.max.y) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight)));
	Int32 maxY = (Int32)((tileMatrixDef->origin.y - rect.min.y) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight)));
	Int32 i = minY;
	Int32 j;
	while (i <= maxY)
	{
		j = minX;
		while (j <= maxX)
		{
			ids->Add(Math::Coord2D<Int32>(j, i));
			ret++;
			j++;
		}
		i++;
	}
	return ret;
}

Media::ImageList *Map::WebMapTileServiceSource::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	ImageType it;
	NN<IO::StreamData> fd;
	NN<IO::ParsedObject> pobj;
	if (this->LoadTileImageData(level, tileId, bounds, localOnly, it).SetTo(fd))
	{
		if (parsers->ParseFile(fd).SetTo(pobj))
		{
			fd.Delete();
			if (pobj->GetParserType() == IO::ParserType::ImageList)
			{
				return NN<Media::ImageList>::ConvertFrom(pobj).Ptr();
			}
			pobj.Delete();
		}
		else
		{
			fd.Delete();
		}
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> Map::WebMapTileServiceSource::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UTF8Char tmpBuff[32];
	UnsafeArray<UTF8Char> tmpPtr;
	UnsafeArray<UTF8Char> sptrEnd;
	NN<TileMatrix> tileMatrix;
	NN<ResourceURL> currResource;
	NN<TileMatrixSet> currSet;
	if (this->GetTileMatrix(level).SetTo(tileMatrix) && this->currResource.SetTo(currResource) && this->currSet.SetTo(currSet))
	{
		sptrEnd = currResource->templateURL->ConcatTo(sbuff);
		tmpPtr = Text::StrInt32(tmpBuff, tileId.x);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		tmpPtr = Text::StrInt32(tmpBuff, tileId.y);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileMatrixSet}"), currSet->id->v, currSet->id->leng);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{style}"), UTF8STRC("generic"));
		return sbuff;
	}
	return 0;
}

Bool Map::WebMapTileServiceSource::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UTF8Char tmpBuff[32];
	UnsafeArray<UTF8Char> tmpPtr;
	NN<TileMatrix> tileMatrix;
	NN<ResourceURL> currResource;
	NN<TileMatrixSet> currSet;
	if (this->GetTileMatrix(level).SetTo(tileMatrix) && this->currResource.SetTo(currResource) && this->currSet.SetTo(currSet))
	{
		sb->Append(currResource->templateURL);
		tmpPtr = Text::StrInt32(tmpBuff, tileId.x);
		sb->ReplaceStr(UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		tmpPtr = Text::StrInt32(tmpBuff, tileId.y);
		sb->ReplaceStr(UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		sb->ReplaceStr(UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
		sb->ReplaceStr(UTF8STRC("{TileMatrixSet}"), currSet->id->v, currSet->id->leng);
		sb->ReplaceStr(UTF8STRC("{style}"), UTF8STRC("generic"));
		return true;
	}
	return false;
}

Optional<IO::StreamData> Map::WebMapTileServiceSource::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 urlSb;
	UnsafeArray<UTF8Char> sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	NN<Net::HTTPClient> cli;
	NN<IO::StreamData> fd;
	NN<TileMatrix> tileMatrix;
	NN<TileMatrixDef> tileMatrixDef;
	NN<TileMatrixDefSet> currDef;
	NN<TileMatrixSet> currSet;
	NN<ResourceURL> currResource;
	NN<TileLayer> currLayer;
	if (!this->GetTileMatrix(level).SetTo(tileMatrix) || !this->currDef.SetTo(currDef) || !this->currSet.SetTo(currSet) || !this->currResource.SetTo(currResource) || !this->currLayer.SetTo(currLayer) || !currDef->tiles.GetItem(level).SetTo(tileMatrixDef))
		return 0;
	Double x1 = tileId.x * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth) + tileMatrixDef->origin.x;
	Double y1 = tileMatrixDef->origin.y - tileId.y * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight);
	Double x2 = (tileId.x + 1) * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth) + tileMatrixDef->origin.x;
	Double y2 = tileMatrixDef->origin.y - (tileId.y + 1) * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight);

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y2), Math::Coord2DDbl(x2, y1));
	bounds.Set(b);
	if (!currSet->bounds.OverlapOrTouch(b))
		return 0;

#if defined(VERBOSE)
	printf("Loading Tile %d %d %d\r\n", (UInt32)level, imgX, imgY);
#endif

	{
		sptru = this->cacheDir->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::String::OrEmpty(currLayer->id)->ConcatTo(sptru);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.x);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, tileId.y);
		*sptru++ = '.';
		sptru = GetExt(currResource->imgType).ConcatTo(sptru);
		NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd.Ptr())->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(currTime) > 0)
			{
				it.Set(currResource->imgType);
				return fd.Ptr();
			}
			else
			{
				hasTime = true;
			}
		}
		fd.Delete();
	}

	if (localOnly)
		return 0;

	UTF8Char tmpBuff[32];
	UnsafeArray<UTF8Char> tmpPtr;

	urlSb.ClearStr();
	urlSb.Append(currResource->templateURL);
	tmpPtr = Text::StrInt32(tmpBuff, tileId.x);
	urlSb.ReplaceStr(UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, tileId.y);
	urlSb.ReplaceStr(UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	urlSb.ReplaceStr(UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{TileMatrixSet}"), currSet->id->v, currSet->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{style}"), UTF8STRC(""));

#if defined(VERBOSE)
	printf("URL: %s\r\n", urlSb.ToString());
#endif

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("WMTS/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), CSTRP(sbuff, sptr));
	}
	Net::WebStatus::StatusCode status = cli->GetRespStatus();
	if (status == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, 0, 0);
	}
	else if (status >= 200 && status < 300)
	{
		IO::MemoryStream mstm;
		if (cli->ReadAllContent(mstm, 16384, 10485760))
		{
			IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
			fs.Write(mstm.GetArray());
			if (cli->GetLastModified(dt))
			{
				currTime.SetCurrTimeUTC();
				fs.SetFileTimes(&currTime, 0, &dt);
			}
			else
			{
				currTime.SetCurrTimeUTC();
				fs.SetFileTimes(&currTime, 0, 0);
			}
		}
	}
	cli.Delete();

	NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	if (fd->GetDataSize() > 0)
	{
		it.Set(IT_PNG);
		return fd.Ptr();
	}
	fd.Delete();
	return 0;
}

Bool Map::WebMapTileServiceSource::SetLayer(UOSInt index)
{
	if (index >= this->layers.GetCount())
	{
		return false;
	}
	this->currLayer = this->layers.GetItem(index);
	this->SetResourceTileType(0);
	this->SetResourceInfoType(0);
	this->SetResourceInfoType(CSTR("application/json"));
	this->SetMatrixSet(0);
	return true;
}

Bool Map::WebMapTileServiceSource::SetMatrixSet(UOSInt index)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return false;
	this->currSet = currLayer->tileMatrixes.GetItem(index);
	NN<TileMatrixSet> currSet;
	if (!this->currSet.SetTo(currSet))
		return false;
	this->currDef = matrixDef.GetNN(currSet->id);
	return true;
}

Bool Map::WebMapTileServiceSource::SetResourceTileType(UOSInt index)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return false;
	this->currResource = 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::Tile)
		{
			if (index == 0)
			{
				this->currResource = resource;
				return true;
			}
			index--;
		}
		i++;
	}
	return false;
}

Bool Map::WebMapTileServiceSource::SetResourceInfoType(UOSInt index)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return false;
	this->currResourceInfo = 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::FeatureInfo)
		{
			if (index == 0)
			{
				this->currResourceInfo = resource;
				return true;
			}
			index--;
		}
		i++;
	}
	return false;
}

Bool Map::WebMapTileServiceSource::SetResourceInfoType(Text::CStringNN name)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return false;
	this->currResourceInfo = 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::FeatureInfo && resource->format->Equals(name.v, name.leng))
		{
			this->currResourceInfo = resource;
			return true;
		}
		i++;
	}
	return false;
}

UOSInt Map::WebMapTileServiceSource::GetResourceInfoType()
{
	NN<TileLayer> currLayer;
	NN<ResourceURL> currResourceInfo;
	if (!this->currLayer.SetTo(currLayer))
		return INVALID_INDEX;
	if (!this->currResourceInfo.SetTo(currResourceInfo))
		return INVALID_INDEX;
	UOSInt index = 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::FeatureInfo)
		{
			if (resource.Ptr() == this->currResourceInfo.OrNull())
			{
				return index;
			}
			index++;
		}
		i++;
	}
	return INVALID_INDEX;
}

UOSInt Map::WebMapTileServiceSource::GetLayerNames(NN<Data::ArrayListNN<Text::String>> layerNames)
{
	UOSInt i = 0;
	UOSInt j = this->layers.GetCount();
	while (i < j)
	{
		NN<Text::String> s;
		if (this->layers.GetItemNoCheck(i)->title.SetTo(s))
		{
			layerNames->Add(s);
		}
		i++;
	}
	return j;
}

UOSInt Map::WebMapTileServiceSource::GetMatrixSetNames(NN<Data::ArrayListStringNN> matrixSetNames)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
	{
		return 0;
	}
	UOSInt i = 0;
	UOSInt j = currLayer->tileMatrixes.GetCount();
	while (i < j)
	{
		matrixSetNames->Add(currLayer->tileMatrixes.GetItemNoCheck(i)->id);
		i++;
	}
	return j;
}

UOSInt Map::WebMapTileServiceSource::GetResourceTileTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	UOSInt initCnt = resourceTypeNames->GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::Tile)
		{
			resourceTypeNames->Add(resource->format);
		}
		i++;
	}
	return resourceTypeNames->GetCount() - initCnt;
}

UOSInt Map::WebMapTileServiceSource::GetResourceInfoTypeNames(NN<Data::ArrayListStringNN> resourceTypeNames)
{
	NN<TileLayer> currLayer;
	if (!this->currLayer.SetTo(currLayer))
		return 0;
	NN<ResourceURL> resource;
	UOSInt i = 0;
	UOSInt j = currLayer->resourceURLs.GetCount();
	UOSInt initCnt = resourceTypeNames->GetCount();
	while (i < j)
	{
		resource = currLayer->resourceURLs.GetItemNoCheck(i);
		if (resource->resourceType == ResourceType::FeatureInfo)
		{
			resourceTypeNames->Add(resource->format);
		}
		i++;
	}
	return resourceTypeNames->GetCount() - initCnt;
}

Text::CStringNN Map::WebMapTileServiceSource::GetExt(Map::TileMap::ImageType imgType)
{
	switch (imgType)
	{
	case ImageType::IT_JPG:
		return CSTR("jpg");
	case ImageType::IT_PNG:
		return CSTR("png");
	case ImageType::IT_WEBP:
		return CSTR("webp");
	default:
		return CSTR("");
	}
}
