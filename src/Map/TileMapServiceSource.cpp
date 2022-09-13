#include "Stdafx.h"
#include "Map/TileMapServiceSource.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Unit/Distance.h"
#include "Net/HTTPClient.h"
#include "Text/XMLReader.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void Map::TileMapServiceSource::LoadXML()
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, 0, this->tmsURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
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
			if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENT)
			{
				if (reader.GetNodeText()->Equals(UTF8STRC("TileMap")))
				{
					valid = true;
					break;
				}
			}
		}

		if (valid)
		{
			Text::StringBuilderUTF8 sb;
			Text::String *s;
			Text::XMLAttrib *attr;
			Text::XMLNode::NodeType nt;
			UOSInt i;
			while (reader.ReadNext())
			{
				if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENT)
				{
					s = reader.GetNodeText();
					if (s->Equals(UTF8STRC("Title")))
					{
						sb.ClearStr();
						if (reader.ReadNodeText(&sb))
						{
							SDEL_STRING(this->title);
							this->title = Text::String::New(sb.ToCString());
#if defined(VERBOSE)
							printf("Title = %s\r\n", this->title->v);
#endif
						}
					}
					else if (s->Equals(UTF8STRC("SRS")))
					{
						sb.ClearStr();
						if (reader.ReadNodeText(&sb))
						{
							SDEL_CLASS(this->csys);
							this->csys = Math::CoordinateSystemManager::CreateFromName(sb.ToCString());
#if defined(VERBOSE)
							if (this->csys)
							{
								printf("SRID = %d\r\n", this->csys->GetSRID());
							}
#endif
						}
					}
					else if (s->Equals(UTF8STRC("BoundingBox")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttrib(i);
							if (attr->name->Equals(UTF8STRC("minx")))
							{
								this->bounds.tl.x = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("minx = %lf\r\n", this->bounds.tl.x);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("miny")))
							{
								this->bounds.tl.y = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("miny = %lf\r\n", this->bounds.tl.y);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("maxx")))
							{
								this->bounds.br.x = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("maxx = %lf\r\n", this->bounds.br.x);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("maxy")))
							{
								this->bounds.br.y = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("maxy = %lf\r\n", this->bounds.br.y);
#endif
							}
						}
						reader.SkipElement();
					}
					else if (s->Equals(UTF8STRC("Origin")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttrib(i);
							if (attr->name->Equals(UTF8STRC("x")))
							{
								this->origin.x = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("originX = %lf\r\n", this->origin.x);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("y")))
							{
								this->origin.y = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("originY = %lf\r\n", this->origin.y);
#endif
							}
						}
						reader.SkipElement();
					}
					else if (s->Equals(UTF8STRC("Origin")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttrib(i);
							if (attr->name->Equals(UTF8STRC("x")))
							{
								this->origin.x = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("originX = %lf\r\n", this->origin.x);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("y")))
							{
								this->origin.y = attr->value->ToDouble();
#if defined(VERBOSE)
								printf("originY = %lf\r\n", this->origin.y);
#endif
							}
						}
						reader.SkipElement();
					}
					else if (s->Equals(UTF8STRC("TileFormat")))
					{
						i = reader.GetAttribCount();
						while (i-- > 0)
						{
							attr = reader.GetAttrib(i);
							if (attr->name->Equals(UTF8STRC("width")))
							{
								this->tileWidth = attr->value->ToUInt32();
#if defined(VERBOSE)
								printf("tileWidth = %d\r\n", (UInt32)this->tileWidth);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("height")))
							{
								this->tileHeight = attr->value->ToUInt32();
#if defined(VERBOSE)
								printf("tileHeight = %d\r\n", (UInt32)this->tileHeight);
#endif
							}
							else if (attr->name->Equals(UTF8STRC("extension")))
							{
								SDEL_STRING(this->tileExt);
								this->tileExt = attr->value->Clone();
#if defined(VERBOSE)
								printf("tileExt = %s\r\n", this->tileExt->v);
#endif
							}
						}
						reader.SkipElement();
					}
					else if (s->Equals(UTF8STRC("TileSets")))
					{
						while (reader.ReadNext())
						{
							nt = reader.GetNodeType();
							if (nt == Text::XMLNode::NT_ELEMENT)
							{
								if (reader.GetNodeText()->Equals(UTF8STRC("TileSet")))
								{
									TileLayer *lyr;
									Text::String *href = 0;
									Double unitPerPixel= 0;
									UOSInt order = INVALID_INDEX;

									i = reader.GetAttribCount();
									while (i-- > 0)
									{
										attr = reader.GetAttrib(i);
										if (attr->name->Equals(UTF8STRC("href")))
										{
											SDEL_STRING(href);
											href = attr->value->Clone();
#if defined(VERBOSE)
											printf("tileHref = %s\r\n", href->v);
#endif
										}
										else if (attr->name->Equals(UTF8STRC("units-per-pixel")))
										{
											unitPerPixel = attr->value->ToDouble();
#if defined(VERBOSE)
											printf("tileUnitPerPixel = %lf\r\n", unitPerPixel);
#endif
										}
										else if (attr->name->Equals(UTF8STRC("order")))
										{
											order = attr->value->ToUOSInt();
#if defined(VERBOSE)
											printf("tileOrder = %d\r\n", (UInt32)order);
#endif
										}
									}
									if (href && order != INVALID_INDEX && unitPerPixel != 0)
									{
										lyr = MemAlloc(TileLayer, 1);
										lyr->url = href->Clone();
										lyr->unitPerPixel = unitPerPixel;
										lyr->order = order;
										this->layers.Add(lyr);
									}
									SDEL_STRING(href);
								}
								reader.SkipElement();
							}
							else if (nt == Text::XMLNode::NT_ELEMENTEND)
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
			}
		}
	}
	DEL_CLASS(cli);
}

Map::TileMapServiceSource::TileMapServiceSource(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, Text::CString tmsURL)
{
	this->sockf = sockf;
	this->encFact = encFact;
	this->tmsURL = Text::String::New(tmsURL);
	this->title = 0;
	this->tileExt = 0;
	this->csys = 0;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->LoadXML();
}

Map::TileMapServiceSource::~TileMapServiceSource()
{
	this->tmsURL->Release();
	SDEL_STRING(this->title);
	SDEL_STRING(this->tileExt);
	TileLayer *layer;
	UOSInt i = this->layers.GetCount();
	while (i-- > 0)
	{
		layer = this->layers.GetItem(i);
		layer->url->Release();
		MemFree(layer);
	}
}

Text::CString Map::TileMapServiceSource::GetName()
{
	return STR_CSTR(this->title);
}

Bool Map::TileMapServiceSource::IsError()
{
	return this->layers.GetCount() == 0;
}

Map::TileMap::TileType Map::TileMapServiceSource::GetTileType()
{
	return Map::TileMap::TT_TMS;
}

UOSInt Map::TileMapServiceSource::GetLevelCount()
{
	return this->layers.GetCount();
}

Double Map::TileMapServiceSource::GetLevelScale(UOSInt level)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer == 0)
	{
		return 0;
	}
	return layer->unitPerPixel / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 1);
}

UOSInt Map::TileMapServiceSource::GetNearestLevel(Double scale)
{
	//////////////////////////
	return 0;
}

UOSInt Map::TileMapServiceSource::GetConcurrentCount()
{
	return 2;
}

Bool Map::TileMapServiceSource::GetBounds(Math::RectAreaDbl *bounds)
{
	*bounds = this->bounds;
	return true;
}

Math::CoordinateSystem *Map::TileMapServiceSource::GetCoordinateSystem()
{
	return this->csys;
}

Bool Map::TileMapServiceSource::IsMercatorProj()
{
	return false;
}

UOSInt Map::TileMapServiceSource::GetTileSize()
{
	return this->tileWidth;
}

UOSInt Map::TileMapServiceSource::GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer == 0)
	{
		return 0;
	}
	
	rect = rect.OverlapArea(this->bounds);
	UOSInt ret = 0;
	Int32 minX = (rect.tl.x - this->origin.x) / (layer->unitPerPixel * this->tileWidth);
	Int32 maxX = (rect.br.x - this->origin.x) / (layer->unitPerPixel * this->tileWidth);
	Int32 minY = (rect.tl.y - this->origin.y) / (layer->unitPerPixel * this->tileHeight);
	Int32 maxY = (rect.tl.y - this->origin.y) / (layer->unitPerPixel * this->tileHeight);
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

Media::ImageList *Map::TileMapServiceSource::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly)
{
	///////////////////////////////////////
	return 0;
}

UTF8Char *Map::TileMapServiceSource::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer)
	{
		Int32 x = (Int32)(imgId >> 32);
		Int32 y = (Int32)(imgId & 0xffffffff);
		sbuff = layer->url->ConcatTo(sbuff);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, x);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, y);
		*sbuff++ = '.';
		sbuff = this->tileExt->ConcatTo(sbuff);
		return sbuff;
	}
	return 0;
}

IO::IStreamData *Map::TileMapServiceSource::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	///////////////////////////////////////
	return 0;
}
