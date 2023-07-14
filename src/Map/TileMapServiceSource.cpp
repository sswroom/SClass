#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/TileMapServiceSource.h"
#include "Map/TileMapUtil.h"
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
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->sockf, 0, this->tmsURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
	if (cli->IsError())
	{
		cli.Delete();
		return;
	}
	{
		Bool valid = false;
		Text::XMLReader reader(this->encFact, cli, Text::XMLReader::PM_XML);
		while (reader.ReadNext())
		{
			if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
			{
				if (reader.GetNodeText()->Equals(UTF8STRC("TileMap")))
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
			Text::XMLAttrib *attr;
			Text::XMLNode::NodeType nt;
			UOSInt i;
			while (reader.ReadNext())
			{
				if (reader.GetNodeType() == Text::XMLNode::NodeType::Element)
				{
					s = reader.GetNodeText();
					if (s->Equals(UTF8STRC("Title")))
					{
						sb.ClearStr();
						if (reader.ReadNodeText(&sb))
						{
							SDEL_STRING(this->title);
							this->title = Text::String::New(sb.ToCString()).Ptr();
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
							if (this->csys)
							{
#if defined(VERBOSE)
								printf("SRID = %d\r\n", this->csys->GetSRID());
#endif
								const Math::CoordinateSystemManager::SpatialRefInfo *srInfo = Math::CoordinateSystemManager::SRGetSpatialRef(this->csys->GetSRID());
								if (srInfo)
								{
									Double x;
									Double y;
									Double z;
									Math::CoordinateSystem *wgs84 = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84);
									Math::CoordinateSystem::ConvertXYZ(wgs84, this->csys, srInfo->minXGeo, srInfo->minYGeo, 0, &x, &y, &z);
									this->csysOrigin = Math::Coord2DDbl(x, y);
									DEL_CLASS(wgs84);
#if defined(VERBOSE)
									printf("SR Origin = (%lf, %lf)\r\n", this->csysOrigin.x, this->csysOrigin.y);
#endif
								}
							}
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
								if (attr->value->Equals(UTF8STRC("jpg")))
								{
									SDEL_STRING(this->tileExt);
									this->tileExt = attr->value->Clone().Ptr();
									this->imgType = IT_JPG;
								}
								else if (attr->value->Equals(UTF8STRC("png")))
								{
									SDEL_STRING(this->tileExt);
									this->tileExt = attr->value->Clone().Ptr();
									this->imgType = IT_PNG;
								}
#if defined(VERBOSE)
								printf("found tileExt = %s, use = %s\r\n", attr->value->v, STR_PTR(this->tileExt));
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
							if (nt == Text::XMLNode::NodeType::Element)
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
											href = attr->value->Clone().Ptr();
#if defined(VERBOSE)
											printf("tileHref = %s\r\n", href->v);
#endif
										}
										else if (attr->name->Equals(UTF8STRC("profile")))
										{
											SDEL_STRING(href);
											href = attr->value->Clone().Ptr();
#if defined(VERBOSE)
											printf("tileProfile = %s\r\n", href->v);
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
#if defined(VERBOSE)
										printf("Added Layer Cnt=%d, order=%d, upp=%lf, url=%s\r\n", (UInt32)this->layers.GetCount(), (UInt32)order, unitPerPixel, href->v);
#endif
									}
									SDEL_STRING(href);
								}
								reader.SkipElement();
							}
							else if (nt == Text::XMLNode::NodeType::ElementEnd)
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
	cli.Delete();
}

Map::TileMapServiceSource::TileMapServiceSource(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::CString tmsURL)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->encFact = encFact;
	this->tmsURL = Text::String::New(tmsURL);
	this->origin = Math::Coord2DDbl(0, 0);
	this->csysOrigin = Math::Coord2DDbl(0, 0);
	this->title = 0;
	this->tileExt = 0;
	this->csys = 0;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->concurrCnt = 2;
	this->imgType = IT_PNG;
	this->LoadXML();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("tms"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Crypto::Hash::CRC32RC crc;
	sptr = Text::StrHexVal32(sptr, crc.CalcDirect(tmsURL.v, tmsURL.leng));
	this->cacheDir = Text::String::NewP(sbuff, sptr);
}

Map::TileMapServiceSource::~TileMapServiceSource()
{
	this->tmsURL->Release();
	SDEL_STRING(this->title);
	SDEL_STRING(this->tileExt);
	this->cacheDir->Release();
	SDEL_CLASS(this->csys);
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
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
	return layer->unitPerPixel / scaleDiv;
}

UOSInt Map::TileMapServiceSource::GetNearestLevel(Double scale)
{
	Double minDiff = 1.0E+100;
	UOSInt minLevel = 0;
	UOSInt i = this->layers.GetCount();
	TileLayer *layer;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
	while (i-- > 0)
	{
		layer = this->layers.GetItem(i);
		layerScale = layer->unitPerPixel / scaleDiv;
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

UOSInt Map::TileMapServiceSource::GetConcurrentCount()
{
	return this->concurrCnt;
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

UOSInt Map::TileMapServiceSource::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer == 0)
	{
		return 0;
	}
	
	rect = rect.OverlapArea(this->bounds);
	UOSInt ret = 0;
	Int32 minX = (Int32)((rect.tl.x - this->csysOrigin.x) / (layer->unitPerPixel * UOSInt2Double(this->tileWidth)));
	Int32 maxX = (Int32)((rect.br.x - this->csysOrigin.x) / (layer->unitPerPixel * UOSInt2Double(this->tileWidth)));
	Int32 minY = (Int32)((rect.tl.y - this->csysOrigin.y) / (layer->unitPerPixel * UOSInt2Double(this->tileHeight)));
	Int32 maxY = (Int32)((rect.br.y - this->csysOrigin.y) / (layer->unitPerPixel * UOSInt2Double(this->tileHeight)));
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

Media::ImageList *Map::TileMapServiceSource::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, Parser::ParserList *parsers, Math::RectAreaDbl *bounds, Bool localOnly)
{
	ImageType it;
	IO::StreamData *fd;
	IO::ParsedObject *pobj;
	fd = this->LoadTileImageData(level, tileId, bounds, localOnly, &it);
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

UTF8Char *Map::TileMapServiceSource::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	TileLayer *layer = this->layers.GetItem(level);
	if (layer)
	{
		sbuff = layer->url->ConcatTo(sbuff);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, tileId.x);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, tileId.y);
		*sbuff++ = '.';
		sbuff = this->tileExt->ConcatTo(sbuff);
		return sbuff;
	}
	return 0;
}

IO::StreamData *Map::TileMapServiceSource::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, Math::RectAreaDbl *bounds, Bool localOnly, ImageType *it)
{
	UTF8Char filePathU[512];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 urlSb;
	UTF8Char *sptru = filePathU;
	Bool hasTime = false;
	Data::DateTime dt;
	Data::DateTime currTime;
	NotNullPtr<Net::HTTPClient> cli;
	IO::StreamData *fd;
	TileLayer *layer = this->layers.GetItem(level);
	if (layer == 0)
		return 0;
	Double x1 = tileId.x * layer->unitPerPixel * UOSInt2Double(this->tileWidth) + this->csysOrigin.x;
	Double y1 = tileId.y * layer->unitPerPixel * UOSInt2Double(this->tileHeight) + this->csysOrigin.y;
	Double x2 = (tileId.x + 1) * layer->unitPerPixel * UOSInt2Double(this->tileWidth) + this->csysOrigin.x;
	Double y2 = (tileId.y + 1) * layer->unitPerPixel * UOSInt2Double(this->tileHeight) + this->csysOrigin.y;

	bounds->tl = Math::Coord2DDbl(x1, y1);
	bounds->br = Math::Coord2DDbl(x2, y2);
	if (!this->bounds.OverlapOrTouch(*bounds))
		return 0;

#if defined(VERBOSE)
//	printf("Loading Tile %d %d %d\r\n", (UInt32)level, imgX, imgY);
#endif

	sptru = this->cacheDir->ConcatTo(filePathU);
	if (sptru[-1] != IO::Path::PATH_SEPERATOR)
		*sptru++ = IO::Path::PATH_SEPERATOR;
	sptru = Text::StrUOSInt(sptru, level);
	*sptru++ = IO::Path::PATH_SEPERATOR;
	sptru = Text::StrInt32(sptru, tileId.x);
	IO::Path::CreateDirectory(CSTRP(filePathU, sptru));
	*sptru++ = IO::Path::PATH_SEPERATOR;
	sptru = Text::StrInt32(sptru, tileId.y);
	*sptru++ = '.';
	sptru = this->tileExt->ConcatTo(sptru);
	NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	if (fd->GetDataSize() > 0)
	{
		currTime.SetCurrTimeUTC();
		currTime.AddDay(-7);
		((IO::StmData::FileData*)fd)->GetFileStream()->GetFileTimes(&dt, 0, 0);
		if (dt.CompareTo(&currTime) > 0)
		{
			if (it)
				*it = this->imgType;
			return fd;
		}
		else
		{
			hasTime = true;
		}
	}
	DEL_CLASS(fd);

	if (localOnly)
		return 0;

	urlSb.ClearStr();
	urlSb.Append(layer->url);
	urlSb.AppendUTF8Char('/');
	urlSb.AppendI32(tileId.x);
	urlSb.AppendUTF8Char('/');
	urlSb.AppendI32(tileId.y);
	urlSb.AppendUTF8Char('.');
	urlSb.Append(this->tileExt);

#if defined(VERBOSE)
	printf("Tile URL: %s\r\n", urlSb.ToString());
#endif

	cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("TileMapService/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
	cli->Connect(urlSb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
	if (hasTime)
	{
		sptr = Net::WebUtil::Date2Str(sbuff, &dt);
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
		if (cli->ReadAllContent(&mstm, 16384, 10485760))
		{
			IO::FileStream fs({filePathU, (UOSInt)(sptru - filePathU)}, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
			fs.Write(mstm.GetBuff(), (UOSInt)mstm.GetLength());
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
	else
	{
#if defined(VERBOSE)
		printf("Response with status: %d\r\n", (Int32)status);
#endif
	}
	cli.Delete();

	NEW_CLASS(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	if (fd)
	{
		if (fd->GetDataSize() > 0)
		{
			if (it)
				*it = IT_PNG;
			return fd;
		}
		DEL_CLASS(fd);
	}
	return 0;
}

void Map::TileMapServiceSource::SetConcurrentCount(UOSInt concurrCnt)
{
	this->concurrCnt = concurrCnt;
}
