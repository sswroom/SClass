#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, this->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli == 0)
		return;
	if (cli->IsError())
	{
		DEL_CLASS(cli);
		return;
	}
	{
		Bool valid = false;
		Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader.GetNodeText()->Equals(UTF8STRC("Capabilities")))
				{
					valid = true;
				}
				break;
			}
		}

		if (valid)
		{
			Text::StringBuilderUTF8 sb;
			Text::String *s;
			while (reader.ReadNext())
			{
				if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
				{
					s = reader.GetNodeText();
					if (s->Equals(UTF8STRC("Contents")))
					{
						while (reader.ReadNext())
						{
							if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
							{
								s = reader.GetNodeText();
								if (s->Equals(UTF8STRC("Layer")))
								{
									ReadLayer(&reader);
								}
								else if (s->Equals(UTF8STRC("TileMatrixSet")))
								{
									TileMatrixDefSet *set = ReadTileMatrixSet(&reader);
									if (set)
									{
										this->matrixDef.Put(set->id, set);
									}
								}
								else
								{
									reader.SkipElement();
								}
							}
							else if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
							{
								break;
							}
						}
					}
					else
					{
						reader.SkipElement();
					}
				}
				else if (reader.GetNodeType() == Text::XMLNode::NodeType::ElementEnd)
				{
					break;
				}
			}
		}
	}
	DEL_CLASS(cli);
	this->SetLayer(0);
}

void Map::WebMapTileServiceSource::ReadLayer(Text::XMLReader *reader)
{
	Text::StringBuilderUTF8 sb;
	Text::String *name;
	TileLayer *layer;
	UOSInt i;
	Text::PString sarr[3];
	NEW_CLASS(layer, TileLayer());
	while (reader->ReadNext())
	{
		Text::XMLNode::NodeType nt = reader->GetNodeType();
		if (nt == Text::XMLNode::NodeType::Element)
		{
			name = reader->GetNodeText();
			if (name->Equals(UTF8STRC("ows:Title")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					SDEL_STRING(layer->title);
					layer->title = Text::String::New(sb.ToCString());
				}
			}
			else if (name->Equals(UTF8STRC("ows:WGS84BoundingBox")))
			{
				while (reader->ReadNext())
				{
					nt = reader->GetNodeType();
					if (nt == Text::XMLNode::NodeType::Element)
					{
						name = reader->GetNodeText();
						if (name->Equals(UTF8STRC("ows:LowerCorner")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (Text::StrSplitP(sarr, 3, sb, ' ') == 2)
							{
								layer->wgs84Bounds.tl.x = sarr[0].ToDouble();
								layer->wgs84Bounds.tl.y = sarr[1].ToDouble();
							}
						}
						else if (name->Equals(UTF8STRC("ows:UpperCorner")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (Text::StrSplitP(sarr, 3, sb, ' ') == 2)
							{
								layer->wgs84Bounds.br.x = sarr[0].ToDouble();
								layer->wgs84Bounds.br.y = sarr[1].ToDouble();
							}
						}
						else
						{
							reader->SkipElement();
						}
					}
					else if (nt == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
				}
			}
			else if (name->Equals(UTF8STRC("ows:Identifier")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					SDEL_STRING(layer->id);
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
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					layer->format.Add(Text::String::New(sb.ToCString()));
				}
			}
			else if (name->Equals(UTF8STRC("InfoFormat")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					layer->infoFormat.Add(Text::String::New(sb.ToCString()));
				}
			}
			else if (name->Equals(UTF8STRC("TileMatrixSetLink")))
			{
				TileMatrixSet *set = ReadTileMatrixSetLink(reader);
				if (set)
				{
					Double tmp;
					Math::CoordinateSystem::ConvertXYZ(this->wgs84, set->csys, layer->wgs84Bounds.tl.x, layer->wgs84Bounds.tl.y, 0, &set->bounds.tl.x, &set->bounds.tl.y, &tmp);
					Math::CoordinateSystem::ConvertXYZ(this->wgs84, set->csys, layer->wgs84Bounds.br.x, layer->wgs84Bounds.br.y, 0, &set->bounds.br.x, &set->bounds.br.y, &tmp);
					layer->tileMatrixes.Add(set);
				}
			}
			else if (name->Equals(UTF8STRC("ResourceURL")))
			{
				Text::XMLAttrib *formatAttr = 0;
				Text::XMLAttrib *resourceTypeAttr = 0;
				Text::XMLAttrib *templateAttr = 0;
				Text::XMLAttrib *attr;
				i = reader->GetAttribCount();
				while (i-- > 0)
				{
					attr = reader->GetAttrib(i);
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
				if (formatAttr && resourceTypeAttr && templateAttr && formatAttr->value && resourceTypeAttr->value && templateAttr->value)
				{
					ResourceURL *resource = MemAlloc(ResourceURL, 1);
					resource->format = formatAttr->value->Clone();
					resource->templateURL = templateAttr->value->Clone();
					if (resourceTypeAttr->value->Equals(UTF8STRC("tile")))
					{
						resource->resourceType = ResourceType::Tile;
					}
					else if (resourceTypeAttr->value->Equals(UTF8STRC("FeatureInfo")))
					{
						resource->resourceType = ResourceType::FeatureInfo;
					}
					else
					{
						resource->resourceType = ResourceType::Unknown;
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
		else if (nt == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
	}
	if (layer->title == 0 || layer->id == 0 || layer->format.GetCount() == 0 || layer->tileMatrixes.GetCount() == 0 || layer->resourceURLs.GetCount() == 0)
	{
		this->ReleaseLayer(layer);
	}
	else
	{
		this->layers.Put(layer->id, layer);
	}
}

Map::WebMapTileServiceSource::TileMatrixSet *Map::WebMapTileServiceSource::ReadTileMatrixSetLink(Text::XMLReader *reader)
{
	Text::XMLNode::NodeType nt;
	Text::String *name;
	TileMatrixSet *set;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(set, TileMatrixSet());
	set->id = 0;
	set->csys = 0;
	while (reader->ReadNext())
	{
		nt = reader->GetNodeType();
		if (nt == Text::XMLNode::NodeType::Element)
		{
			name = reader->GetNodeText();
			if (name->Equals(UTF8STRC("TileMatrixSet")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					SDEL_STRING(set->id);
					set->id = Text::String::New(sb.ToCString());
				}
			}
			else if (name->Equals(UTF8STRC("TileMatrixSetLimits")))
			{
				while (reader->ReadNext())
				{
					nt = reader->GetNodeType();
					if (nt == Text::XMLNode::NodeType::Element)
					{
						name = reader->GetNodeText();
						if (name->Equals(UTF8STRC("TileMatrixLimits")))
						{
							Text::String *id = 0;
							Int32 minRow = 0x7fffffff;
							Int32 maxRow = 0x7fffffff;
							Int32 minCol = 0x7fffffff;
							Int32 maxCol = 0x7fffffff;
							while (reader->ReadNext())
							{
								nt = reader->GetNodeType();
								if (nt == Text::XMLNode::NodeType::Element)
								{
									name = reader->GetNodeText();
									if (name->Equals(UTF8STRC("TileMatrix")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											SDEL_STRING(id);
											id = Text::String::New(sb.ToCString());
										}
									}
									else if (name->Equals(UTF8STRC("MinTileRow")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											minRow = sb.ToInt32();
										}
									}
									else if (name->Equals(UTF8STRC("MaxTileRow")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											maxRow = sb.ToInt32();
										}
									}
									else if (name->Equals(UTF8STRC("MinTileCol")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
										if (sb.GetLength() > 0)
										{
											minCol = sb.ToInt32();
										}
									}
									else if (name->Equals(UTF8STRC("MaxTileCol")))
									{
										sb.ClearStr();
										reader->ReadNodeText(&sb);
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
								else if (nt == Text::XMLNode::NodeType::ElementEnd)
								{
									break;
								}
							}
							if (id && minRow != 0x7fffffff && maxRow != 0x7fffffff && minCol != 0x7fffffff && maxCol != 0x7fffffff)
							{
								TileMatrix *tile = MemAlloc(TileMatrix, 1);
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
					else if (nt == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
				}
			}
			else
			{
				reader->SkipElement();
			}
		}
		else if (nt == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
		}
	}
	if (set->id && set->tiles.GetCount() > 0)
	{
		set->csys = Math::CoordinateSystemManager::CreateFromName(set->id->ToCString());
		return set;
	}
	else
	{
		this->ReleaseTileMatrixSet(set);
		return 0;
	}
}

Map::WebMapTileServiceSource::TileMatrixDefSet *Map::WebMapTileServiceSource::ReadTileMatrixSet(Text::XMLReader *reader)
{
	Text::XMLNode::NodeType nt;
	Text::String *name;
	TileMatrixDefSet *set;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(set, TileMatrixDefSet());
	set->id = 0;
	set->csys = 0;
	while (reader->ReadNext())
	{
		nt = reader->GetNodeType();
		if (nt == Text::XMLNode::NodeType::Element)
		{
			name = reader->GetNodeText();
			if (name->Equals(UTF8STRC("ows:Identifier")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					SDEL_STRING(set->id);
					set->id = Text::String::New(sb.ToCString());
				}
			}
			else if (name->Equals(UTF8STRC("ows:SupportedCRS")))
			{
				sb.ClearStr();
				reader->ReadNodeText(&sb);
				if (sb.GetLength() > 0)
				{
					SDEL_CLASS(set->csys);
					set->csys = Math::CoordinateSystemManager::CreateFromName(sb.ToCString());
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
				while (reader->ReadNext())
				{
					nt = reader->GetNodeType();
					if (nt == Text::XMLNode::NodeType::Element)
					{
						name = reader->GetNodeText();
						if (name->Equals(UTF8STRC("ows:Identifier")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (sb.GetLength() > 0)
							{
								SDEL_STRING(id);
								id = Text::String::New(sb.ToCString());
							}
						}
						else if (name->Equals(UTF8STRC("ScaleDenominator")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (sb.GetLength() > 0)
							{
								scaleDenom = sb.ToDouble();
							}
						}
						else if (name->Equals(UTF8STRC("TopLeftCorner")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
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
							reader->ReadNodeText(&sb);
							if (sb.GetLength() > 0)
							{
								tileWidth = sb.ToUInt32();
							}
						}
						else if (name->Equals(UTF8STRC("TileHeight")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (sb.GetLength() > 0)
							{
								tileHeight = sb.ToUInt32();
							}
						}
						else if (name->Equals(UTF8STRC("MatrixWidth")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
							if (sb.GetLength() > 0)
							{
								matrixWidth = sb.ToUInt32();
							}
						}
						else if (name->Equals(UTF8STRC("MatrixHeight")))
						{
							sb.ClearStr();
							reader->ReadNodeText(&sb);
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
					else if (nt == Text::XMLNode::NodeType::ElementEnd)
					{
						break;
					}
				}
				if (id && scaleDenom != 0 && topPos != -1 && leftPos != -1 && tileWidth != 0 && tileHeight != 0 && matrixWidth != 0 && matrixHeight != 0)
				{
					TileMatrixDef *tile = MemAlloc(TileMatrixDef, 1);
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
		else if (nt == Text::XMLNode::NodeType::ElementEnd)
		{
			break;
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
				TileMatrixDef *tile = set->tiles.GetItem(i);
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
				TileMatrixDef *tile = set->tiles.GetItem(i);
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

Map::WebMapTileServiceSource::TileMatrix *Map::WebMapTileServiceSource::GetTileMatrix(UOSInt level) const
{
	if (this->currSet == 0)
		return 0;
	return this->currSet->tiles.GetItem(level);
}

void Map::WebMapTileServiceSource::ReleaseLayer(TileLayer *layer)
{
	SDEL_STRING(layer->id);
	SDEL_STRING(layer->title);
	UOSInt i = layer->tileMatrixes.GetCount();
	while (i-- > 0)
	{
		this->ReleaseTileMatrixSet(layer->tileMatrixes.GetItem(i));
	}
	i = layer->format.GetCount();
	while (i-- > 0)
	{
		layer->format.GetItem(i)->Release();
	}
	i = layer->infoFormat.GetCount();
	while (i-- > 0)
	{
		layer->infoFormat.GetItem(i)->Release();
	}
	i = layer->resourceURLs.GetCount();
	while (i-- > 0)
	{
		this->ReleaseResourceURL(layer->resourceURLs.GetItem(i));
	}
	DEL_CLASS(layer);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrix(TileMatrix *tileMatrix)
{
	tileMatrix->id->Release();
	MemFree(tileMatrix);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixSet(TileMatrixSet *set)
{
	UOSInt i;
	set->id->Release();
	DEL_CLASS(set->csys);
	i = set->tiles.GetCount();
	while (i-- > 0)
	{
		this->ReleaseTileMatrix(set->tiles.GetItem(i));
	}
	DEL_CLASS(set);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixDef(TileMatrixDef *tileMatrix)
{
	tileMatrix->id->Release();
	MemFree(tileMatrix);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrixDefSet(TileMatrixDefSet *set)
{
	UOSInt i;
	set->id->Release();
	SDEL_CLASS(set->csys);
	i = set->tiles.GetCount();
	while (i-- > 0)
	{
		this->ReleaseTileMatrixDef(set->tiles.GetItem(i));
	}
	DEL_CLASS(set);
}

void Map::WebMapTileServiceSource::ReleaseResourceURL(ResourceURL *resourceURL)
{
	resourceURL->templateURL->Release();
	resourceURL->format->Release();
	MemFree(resourceURL);
}

Map::WebMapTileServiceSource::WebMapTileServiceSource(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString wmtsURL)
{
	this->cacheDir = 0;
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->wmtsURL = Text::String::New(wmtsURL);
	this->currLayer = 0;
	this->currDef = 0;
	this->currResource = 0;
	this->currResourceInfo = 0;
	this->currSet = 0;
	this->wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
	this->LoadXML();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("wmts"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Crypto::Hash::CRC32RC crc;
	sptr = Text::StrHexVal32(sptr, crc.CalcDirect(wmtsURL.v, wmtsURL.leng));
	this->cacheDir = Text::String::NewP(sbuff, sptr);
}

Map::WebMapTileServiceSource::~WebMapTileServiceSource()
{
	this->wmtsURL->Release();
	SDEL_STRING(this->cacheDir);
	UOSInt i = this->layers.GetCount();
	while (i-- > 0)
	{
		this->ReleaseLayer(this->layers.GetItem(i));
	}
	i = this->matrixDef.GetCount();
	while (i-- > 0)
	{
		this->ReleaseTileMatrixDefSet(this->matrixDef.GetItem(i));
	}
	SDEL_CLASS(this->wgs84);
}

Text::CString Map::WebMapTileServiceSource::GetName()
{
	return this->currLayer->id->ToCString();
}

Bool Map::WebMapTileServiceSource::IsError()
{
	return this->layers.GetCount() == 0 || this->currLayer == 0;
}

Map::TileMap::TileType Map::WebMapTileServiceSource::GetTileType()
{
	return Map::TileMap::TT_WMTS;
}

UOSInt Map::WebMapTileServiceSource::GetLevelCount()
{
	if (this->currSet == 0)
		return 0;
	return this->currSet->tiles.GetCount();
}

Double Map::WebMapTileServiceSource::GetLevelScale(UOSInt level)
{
	if (this->currDef == 0)
		return 0;
	TileMatrixDef *tileMatrixDef = this->currDef->tiles.GetItem(level);
	if (tileMatrixDef == 0)
	{
		return 0;
	}
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->currDef->csys);
	return tileMatrixDef->unitPerPixel / scaleDiv;
}

UOSInt Map::WebMapTileServiceSource::GetNearestLevel(Double scale)
{
	if (this->currSet == 0)
		return 0;
	Double minDiff = 1.0E+100;
	UOSInt minLevel = 0;
	UOSInt i = this->currSet->tiles.GetCount();
	TileMatrixDef *tileMatrixDef;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->currSet->csys);
	while (i-- > 0)
	{
		tileMatrixDef = this->currDef->tiles.GetItem(i);
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

UOSInt Map::WebMapTileServiceSource::GetConcurrentCount()
{
	return 2;
}

Bool Map::WebMapTileServiceSource::GetBounds(Math::RectAreaDbl *bounds)
{
	if (this->currSet)
	{
		*bounds = this->currSet->bounds;
		return true;
	}
	return false;
}

Math::CoordinateSystem *Map::WebMapTileServiceSource::GetCoordinateSystem()
{
	if (this->currSet)
	{
		return this->currSet->csys;
	}
	else
	{
		return 0;
	}
}

Bool Map::WebMapTileServiceSource::IsMercatorProj()
{
	return false;
}

UOSInt Map::WebMapTileServiceSource::GetTileSize()
{
	if (this->currDef)
	{
		return this->currDef->tiles.GetItem(0)->tileWidth;
	}
	else
	{
		return 0;
	}
}

Bool Map::WebMapTileServiceSource::CanQuery() const
{
	return this->currResourceInfo != 0;
}

Math::Geometry::Vector2D *Map::WebMapTileServiceSource::QueryInfo(Math::Coord2DDbl coord, UOSInt level, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList) const
{
	if (this->currResourceInfo == 0)
		return 0;

	TileMatrix *tileMatrix = this->GetTileMatrix(level);
	if (tileMatrix == 0)
	{
		return 0;
	}
	TileMatrixDef *tileMatrixDef = this->currDef->tiles.GetItem(level);

	UTF8Char tmpBuff[1024];
	UTF8Char *tmpPtr;

	Int32 pxX = (Int32)((coord.x - tileMatrixDef->origin.x) / tileMatrixDef->unitPerPixel);
	Int32 pxY = (Int32)((tileMatrixDef->origin.y - coord.y) / tileMatrixDef->unitPerPixel);
	Int32 imgX = pxX / (Int32)tileMatrixDef->tileWidth;
	Int32 imgY = pxY / (Int32)tileMatrixDef->tileHeight;
	Int32 tileX = pxX % (Int32)tileMatrixDef->tileWidth;
	Int32 tileY = pxY % (Int32)tileMatrixDef->tileHeight;

	Text::StringBuilderUTF8 urlSb;
	urlSb.Append(this->currResourceInfo->templateURL);
	tmpPtr = Text::StrInt32(tmpBuff, imgX);
	urlSb.ReplaceStr(UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, imgY);
	urlSb.ReplaceStr(UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, tileX);
	urlSb.ReplaceStr(UTF8STRC("{I}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, tileY);
	urlSb.ReplaceStr(UTF8STRC("{J}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	urlSb.ReplaceStr(UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{TileMatrixSet}"), this->currSet->id->v, this->currSet->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{style}"), UTF8STRC(""));

#if defined(VERBOSE)
	printf("Info URL: %s\r\n", urlSb.ToString());
#endif

	Text::StringBuilderUTF8 sb;
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("WMTS/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	UOSInt readSize;
	while ((readSize = cli->Read(tmpBuff, 1024)) > 0)
	{
		sb.AppendC(tmpBuff, readSize);
	}
	DEL_CLASS(cli);

	if (this->currResourceInfo->format->Equals(UTF8STRC("text/plain")))
	{
		Text::PString lineArr[2];
		Text::PString sarr[2];
		UOSInt lineCnt;
		lineArr[1] = sb;
		while (true)
		{
			lineCnt = Text::StrSplitLineP(lineArr, 2, lineArr[1]);
			if (Text::StrSplitTrimP(sarr, 2, lineArr[0], '=') == 2)
			{
				nameList->Add(Text::String::New(sarr[0].ToCString()));
				valueList->Add(Text::String::New(sarr[1].ToCString()));
			}

			if (lineCnt != 2)
				break;
		}
		return NEW_CLASS_D(Math::Geometry::Point(this->currDef->csys->GetSRID(), coord));
	}
	else if (this->currResourceInfo->format->Equals(UTF8STRC("application/json")))
	{
		Text::JSONBase *json = Text::JSONBase::ParseJSONStr(sb.ToCString());
		if (json)
		{
			UInt32 srid = this->currDef->csys->GetSRID();
			if (json->GetType() == Text::JSONType::Object)
			{
				Text::String *crsName = json->GetString(UTF8STRC("crs.properties.name"));
				if (crsName)
				{
					Math::CoordinateSystem *csys = Math::CoordinateSystemManager::CreateFromName(crsName->ToCString());
					if (csys)
					{
						srid = csys->GetSRID();
						DEL_CLASS(csys);
					}
				}
			}
			Text::JSONBase *feature = json->GetValue(UTF8STRC("features[0]"));
			if (feature)
			{
				Text::JSONBase *geometry = feature->GetValue(UTF8STRC("geometry"));
				if (geometry && geometry->GetType() == Text::JSONType::Object)
				{
					Math::Geometry::Vector2D *vec = Parser::FileParser::JSONParser::ParseGeomJSON((Text::JSONObject*)geometry, srid);
					if (vec)
					{
						Text::JSONBase *properties = feature->GetValue(UTF8STRC("properties"));
						if (properties && properties->GetType() == Text::JSONType::Object)
						{
							Data::ArrayList<Text::String*> names;
							Text::String *name;
							Text::StringBuilderUTF8 sb;
							Text::JSONObject *obj = (Text::JSONObject*)properties;
							obj->GetObjectNames(&names);
							UOSInt i = 0;
							UOSInt j = names.GetCount();
							while (i < j)
							{
								name = names.GetItem(i);
								nameList->Add(name->Clone());
								sb.ClearStr();
								obj->GetValue(name->v, name->leng)->ToString(&sb);
								valueList->Add(Text::String::New(sb.ToCString()));
								i++;
							}
						}
						json->EndUse();
						return vec;
					}
				}
			}
			json->EndUse();
		}
	}
	else if (this->currResourceInfo->format->StartsWith(UTF8STRC("application/vnd.ogc.gml")) || this->currResourceInfo->format->Equals(UTF8STRC("text/xml")))
	{
		IO::MemoryStream mstm((UInt8*)sb.ToString(), sb.GetLength(), UTF8STRC("Map.WebMapTileServiceSource.QueryInfo.mstm"));
		IO::ParsedObject *pobj = Parser::FileParser::XMLParser::ParseStream(encFact, &mstm, CSTR("Temp.gml"), 0, 0, 0);
		if (pobj)
		{
			if (pobj->GetParserType() == IO::ParserType::MapLayer)
			{
				Map::IMapDrawLayer *layer = (Map::IMapDrawLayer*)pobj;
				void *nameArr = 0;
				Data::ArrayListInt64 idArr;
				layer->GetAllObjectIds(&idArr, &nameArr);
				if (idArr.GetCount() == 1)
				{
					void *sess = layer->BeginGetObject();
					Math::Geometry::Vector2D *vec = layer->GetNewVectorById(sess, idArr.GetItem(0));
					if (vec)
					{
						UOSInt i = 0;
						UOSInt j = layer->GetColumnCnt();
						while (i < j)
						{
							tmpPtr = layer->GetColumnName(tmpBuff, i);
							nameList->Add(Text::String::NewP(tmpBuff, tmpPtr));
							tmpPtr = layer->GetString(tmpBuff, sizeof(tmpBuff), nameArr, idArr.GetItem(0), i);
							valueList->Add(Text::String::NewP(tmpBuff, tmpPtr));
							i++;
						}
						layer->ReleaseNameArr(nameArr);
						DEL_CLASS(pobj);
						return vec;
					}
				}
				layer->ReleaseNameArr(nameArr);
			}
			DEL_CLASS(pobj);
		}
	}
	else
	{
#if defined(VERBOSE)
		printf("%s\r\n", sb.ToString());
#endif
	}
	return 0;
}

UOSInt Map::WebMapTileServiceSource::GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids)
{
	TileMatrix *tileMatrix = this->GetTileMatrix(level);
	if (tileMatrix == 0)
	{
		return 0;
	}
	TileMatrixDef *tileMatrixDef = this->currDef->tiles.GetItem(level);
	
	rect = rect.OverlapArea(this->currSet->bounds);
	UOSInt ret = 0;
	Int32 minX = (Int32)((rect.tl.x - tileMatrixDef->origin.x) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth)));
	Int32 maxX = (Int32)((rect.br.x - tileMatrixDef->origin.x) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth)));
	Int32 minY = (Int32)((tileMatrixDef->origin.y - rect.br.y) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight)));
	Int32 maxY = (Int32)((tileMatrixDef->origin.y - rect.tl.y) / (tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight)));
	Int32 i = minY;
	Int32 j;
	while (i <= maxY)
	{
		j = minX;
		while (j <= maxX)
		{
			Int64 id = (((Int64)j) << 32) | (UInt32)i;
			ids->Add(id);
			ret++;
			j++;
		}
		i++;
	}
	return ret;
}

Media::ImageList *Map::WebMapTileServiceSource::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly)
{
	Int32 blockX;
	Int32 blockY;
	ImageType it;
	IO::IStreamData *fd;
	IO::ParsedObject *pobj;
	fd = this->LoadTileImageData(level, imgId, bounds, localOnly, &blockX, &blockY, &it);
	if (fd)
	{
		IO::ParserType pt;
		pobj = parsers->ParseFile(fd, &pt);
		DEL_CLASS(fd);
		if (pobj)
		{
			if (pt == IO::ParserType::ImageList)
			{
				return (Media::ImageList*)pobj;
			}
			DEL_CLASS(pobj);
		}
	}
	return 0;
}

UTF8Char *Map::WebMapTileServiceSource::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	UTF8Char tmpBuff[32];
	UTF8Char *tmpPtr;
	UTF8Char *sptrEnd;
	TileMatrix *tileMatrix = this->GetTileMatrix(level);
	if (tileMatrix && this->currResource)
	{
		Int32 x = (Int32)(imgId >> 32);
		Int32 y = (Int32)(imgId & 0xffffffff);
		sptrEnd = this->currResource->templateURL->ConcatTo(sbuff);
		tmpPtr = Text::StrInt32(tmpBuff, x);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		tmpPtr = Text::StrInt32(tmpBuff, y);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{TileMatrixSet}"), this->currSet->id->v, this->currSet->id->leng);
		sptrEnd = Text::StrReplaceC(sbuff, sptrEnd, UTF8STRC("{style}"), UTF8STRC("generic"));
		return sbuff;
	}
	return 0;
}

IO::IStreamData *Map::WebMapTileServiceSource::LoadTileImageData(UOSInt level, Int64 imgId, Math::RectAreaDbl *bounds, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	UOSInt readSize;
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	Net::HTTPClient *cli;
	IO::IStreamData *fd;
	TileMatrix *tileMatrix = this->GetTileMatrix(level);
	if (tileMatrix == 0)
		return 0;
	TileMatrixDef *tileMatrixDef = this->currDef->tiles.GetItem(level);
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = imgX * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth) + tileMatrixDef->origin.x;
	Double y1 = tileMatrixDef->origin.y - imgY * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight);
	Double x2 = (imgX + 1) * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileWidth) + tileMatrixDef->origin.x;
	Double y2 = tileMatrixDef->origin.y - (imgY + 1) * tileMatrixDef->unitPerPixel * UOSInt2Double(tileMatrixDef->tileHeight);

	bounds->tl = Math::Coord2DDbl(x1, y2);
	bounds->br = Math::Coord2DDbl(x2, y1);
	if (!this->currSet->bounds.OverlapOrTouch(*bounds))
		return 0;

#if defined(VERBOSE)
	printf("Loading Tile %d %d %d\r\n", (UInt32)level, imgX, imgY);
#endif

	if (this->cacheDir)
	{
		sptru = this->cacheDir->ConcatTo(filePathU);
		if (sptru[-1] != IO::Path::PATH_SEPERATOR)
			*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = this->currLayer->id->ConcatTo(sptru);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrUOSInt(sptru, level);
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgX);
		IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
		*sptru++ = IO::Path::PATH_SEPERATOR;
		sptru = Text::StrInt32(sptru, imgY);
		*sptru++ = '.';
		sptru = GetExt(this->currResource->imgType).ConcatTo(sptru);
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
		if (fd->GetDataSize() > 0)
		{
			currTime.SetCurrTimeUTC();
			currTime.AddDay(-7);
			((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
			if (dt.CompareTo(&currTime) > 0)
			{
				if (blockX)
					*blockX = imgX;
				if (blockY)
					*blockY = imgY;
				if (it)
					*it = this->currResource->imgType;
				return fd;
			}
			else
			{
				hasTime = true;
			}
		}
		DEL_CLASS(fd);
	}

	if (localOnly)
		return 0;

	UTF8Char tmpBuff[32];
	UTF8Char *tmpPtr;

	urlSb.ClearStr();
	urlSb.Append(this->currResource->templateURL);
	tmpPtr = Text::StrInt32(tmpBuff, imgX);
	urlSb.ReplaceStr(UTF8STRC("{TileCol}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	tmpPtr = Text::StrInt32(tmpBuff, imgY);
	urlSb.ReplaceStr(UTF8STRC("{TileRow}"), tmpBuff, (UOSInt)(tmpPtr - tmpBuff));
	urlSb.ReplaceStr(UTF8STRC("{TileMatrix}"), tileMatrix->id->v, tileMatrix->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{TileMatrixSet}"), this->currSet->id->v, this->currSet->id->leng);
	urlSb.ReplaceStr(UTF8STRC("{style}"), UTF8STRC(""));

#if defined(VERBOSE)
	printf("URL: %s\r\n", urlSb.ToString());
#endif

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("WMTS/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, &dt);
		cli->AddHeaderC(CSTR("If-Modified-Since"), CSTRP(sbuff, sptr));
	}
	if (cli->GetRespStatus() == 304)
	{
		IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		dt.SetCurrTimeUTC();
		fs.SetFileTimes(&dt, 0, 0);
	}
	else
	{
		UInt64 contLeng = cli->GetContentLength();
		UOSInt currPos = 0;
		UInt8 *imgBuff;
		if (contLeng > 0 && contLeng <= 10485760)
		{
			imgBuff = MemAlloc(UInt8, (UOSInt)contLeng);
			while ((readSize = cli->Read(&imgBuff[currPos], (UOSInt)contLeng - currPos)) > 0)
			{
				currPos += readSize;
				if (currPos >= contLeng)
				{
					break;
				}
			}
			if (currPos >= contLeng)
			{
				if (this->cacheDir)
				{
					IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
					fs.Write(imgBuff, (UOSInt)contLeng);
					if (cli->GetLastModified(&dt))
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
			MemFree(imgBuff);
		}

	}
	DEL_CLASS(cli);

	fd = 0;
	if (this->cacheDir)
	{
		NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	}
	if (fd)
	{
		if (fd->GetDataSize() > 0)
		{
			if (blockX)
				*blockX = imgX;
			if (blockY)
				*blockY = imgY;
			if (it)
				*it = IT_PNG;
			return fd;
		}
		DEL_CLASS(fd);
	}
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
	if (this->currLayer == 0)
		return false;
	this->currSet = this->currLayer->tileMatrixes.GetItem(index);
	if (this->currSet == 0)
		return false;
	this->currDef = this->matrixDef.Get(this->currSet->id);
	return true;
}

Bool Map::WebMapTileServiceSource::SetResourceTileType(UOSInt index)
{
	if (this->currLayer == 0)
		return false;
	this->currResource = 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
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
	if (this->currLayer == 0)
		return false;
	this->currResourceInfo = 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
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

Bool Map::WebMapTileServiceSource::SetResourceInfoType(Text::CString name)
{
	if (this->currLayer == 0)
		return false;
	this->currResourceInfo = 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
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
	if (this->currLayer == 0)
		return INVALID_INDEX;
	if (this->currResourceInfo == 0)
		return INVALID_INDEX;
	UOSInt index = 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
		if (resource->resourceType == ResourceType::FeatureInfo)
		{
			if (resource == this->currResourceInfo)
			{
				return index;
			}
			index++;
		}
		i++;
	}
	return INVALID_INDEX;
}

UOSInt Map::WebMapTileServiceSource::GetLayerNames(Data::ArrayList<Text::String*> *layerNames)
{
	UOSInt i = 0;
	UOSInt j = this->layers.GetCount();
	while (i < j)
	{
		layerNames->Add(this->layers.GetItem(i)->title);
		i++;
	}
	return j;
}

UOSInt Map::WebMapTileServiceSource::GetMatrixSetNames(Data::ArrayList<Text::String*> *matrixSetNames)
{
	if (this->currLayer == 0)
	{
		return 0;
	}
	UOSInt i = 0;
	UOSInt j = this->currLayer->tileMatrixes.GetCount();
	while (i < j)
	{
		matrixSetNames->Add(this->currLayer->tileMatrixes.GetItem(i)->id);
		i++;
	}
	return j;
}

UOSInt Map::WebMapTileServiceSource::GetResourceTileTypeNames(Data::ArrayList<Text::String*> *resourceTypeNames)
{
	if (this->currLayer == 0)
		return 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	UOSInt initCnt = resourceTypeNames->GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
		if (resource->resourceType == ResourceType::Tile)
		{
			resourceTypeNames->Add(resource->format);
		}
		i++;
	}
	return resourceTypeNames->GetCount() - initCnt;
}

UOSInt Map::WebMapTileServiceSource::GetResourceInfoTypeNames(Data::ArrayList<Text::String*> *resourceTypeNames)
{
	if (this->currLayer == 0)
		return 0;
	ResourceURL *resource;
	UOSInt i = 0;
	UOSInt j = this->currLayer->resourceURLs.GetCount();
	UOSInt initCnt = resourceTypeNames->GetCount();
	while (i < j)
	{
		resource = this->currLayer->resourceURLs.GetItem(i);
		if (resource->resourceType == ResourceType::FeatureInfo)
		{
			resourceTypeNames->Add(resource->format);
		}
		i++;
	}
	return resourceTypeNames->GetCount() - initCnt;
}

Text::CString Map::WebMapTileServiceSource::GetExt(Map::TileMap::ImageType imgType)
{
	switch (imgType)
	{
	case ImageType::IT_JPG:
		return CSTR("jpg");
	case ImageType::IT_PNG:
		return CSTR("png");
	default:
		return CSTR("");
	}
}