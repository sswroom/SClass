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
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(this->clif, 0, this->tmsURL->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
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
			valid = nodeText->Equals(UTF8STRC("TileMap"));
		}

		if (valid)
		{
			Text::StringBuilderUTF8 sb;
			NN<Text::XMLAttrib> attr;
			NN<Text::String> aname;
			NN<Text::String> avalue;
			UOSInt i;
			while (reader.NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("Title")))
				{
					sb.ClearStr();
					if (reader.ReadNodeText(sb))
					{
						SDEL_STRING(this->title);
						this->title = Text::String::New(sb.ToCString()).Ptr();
#if defined(VERBOSE)
						printf("Title = %s\r\n", this->title->v);
#endif
					}
				}
				else if (nodeText->Equals(UTF8STRC("SRS")))
				{
					sb.ClearStr();
					if (reader.ReadNodeText(sb))
					{
						NN<Math::CoordinateSystem> csys;
						if (Math::CoordinateSystemManager::CreateFromName(sb.ToCString()).SetTo(csys))
						{
							this->csys.Delete();
							this->csys = csys;
#if defined(VERBOSE)
							printf("SRID = %d\r\n", this->csys->GetSRID());
#endif
							const Math::CoordinateSystemManager::SpatialRefInfo *srInfo = Math::CoordinateSystemManager::SRGetSpatialRef(this->csys->GetSRID());
							if (srInfo)
							{
								NN<Math::CoordinateSystem> wgs84 = Math::CoordinateSystemManager::CreateWGS84Csys();
								this->csysOrigin = Math::CoordinateSystem::Convert(wgs84, this->csys, Math::Coord2DDbl(srInfo->minXGeo, srInfo->minYGeo));
								wgs84.Delete();
#if defined(VERBOSE)
								printf("SR Origin = (%lf, %lf)\r\n", this->csysOrigin.x, this->csysOrigin.y);
#endif
							}
						}
					}
				}
				else if (nodeText->Equals(UTF8STRC("BoundingBox")))
				{
					i = reader.GetAttribCount();
					while (i-- > 0)
					{
						attr = reader.GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("minx")) && attr->value.SetTo(avalue))
						{
							this->bounds.min.x = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("minx = %lf\r\n", this->bounds.min.x);
#endif
						}
						else if (aname->Equals(UTF8STRC("miny")) && attr->value.SetTo(avalue))
						{
							this->bounds.min.y = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("miny = %lf\r\n", this->bounds.min.y);
#endif
						}
						else if (aname->Equals(UTF8STRC("maxx")) && attr->value.SetTo(avalue))
						{
							this->bounds.max.x = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("maxx = %lf\r\n", this->bounds.max.x);
#endif
						}
						else if (aname->Equals(UTF8STRC("maxy")) && attr->value.SetTo(avalue))
						{
							this->bounds.max.y = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("maxy = %lf\r\n", this->bounds.max.y);
#endif
						}
					}
					reader.SkipElement();
				}
				else if (nodeText->Equals(UTF8STRC("Origin")))
				{
					i = reader.GetAttribCount();
					while (i-- > 0)
					{
						attr = reader.GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("x")) && attr->value.SetTo(avalue))
						{
							this->origin.x = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("originX = %lf\r\n", this->origin.x);
#endif
						}
						else if (aname->Equals(UTF8STRC("y")) && attr->value.SetTo(avalue))
						{
							this->origin.y = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("originY = %lf\r\n", this->origin.y);
#endif
						}
					}
					reader.SkipElement();
				}
				else if (nodeText->Equals(UTF8STRC("Origin")))
				{
					i = reader.GetAttribCount();
					while (i-- > 0)
					{
						attr = reader.GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("x")) && attr->value.SetTo(avalue))
						{
							this->origin.x = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("originX = %lf\r\n", this->origin.x);
#endif
						}
						else if (aname->Equals(UTF8STRC("y")) && attr->value.SetTo(avalue))
						{
							this->origin.y = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
							printf("originY = %lf\r\n", this->origin.y);
#endif
						}
					}
					reader.SkipElement();
				}
				else if (nodeText->Equals(UTF8STRC("TileFormat")))
				{
					i = reader.GetAttribCount();
					while (i-- > 0)
					{
						attr = reader.GetAttribNoCheck(i);
						aname = Text::String::OrEmpty(attr->name);
						if (aname->Equals(UTF8STRC("width")) && attr->value.SetTo(avalue))
						{
							this->tileWidth = avalue->ToUInt32();
#if defined(VERBOSE)
							printf("tileWidth = %d\r\n", (UInt32)this->tileWidth);
#endif
						}
						else if (aname->Equals(UTF8STRC("height")) && attr->value.SetTo(avalue))
						{
							this->tileHeight = avalue->ToUInt32();
#if defined(VERBOSE)
							printf("tileHeight = %d\r\n", (UInt32)this->tileHeight);
#endif
						}
						else if (aname->Equals(UTF8STRC("extension")) && attr->value.SetTo(avalue))
						{
							if (avalue->Equals(UTF8STRC("jpg")))
							{
								SDEL_STRING(this->tileExt);
								this->tileExt = avalue->Clone().Ptr();
								this->imgType = IT_JPG;
							}
							else if (avalue->Equals(UTF8STRC("png")))
							{
								SDEL_STRING(this->tileExt);
								this->tileExt = avalue->Clone().Ptr();
								this->imgType = IT_PNG;
							}
#if defined(VERBOSE)
							printf("found tileExt = %s, use = %s\r\n", avalue->v, STR_PTR(this->tileExt));
#endif
						}
					}
					reader.SkipElement();
				}
				else if (nodeText->Equals(UTF8STRC("TileSets")))
				{
					while (reader.NextElementName().SetTo(nodeText))
					{
						if (nodeText->Equals(UTF8STRC("TileSet")))
						{
							NN<TileLayer> lyr;
							Text::String *href = 0;
							Double unitPerPixel= 0;
							UOSInt order = INVALID_INDEX;

							i = reader.GetAttribCount();
							while (i-- > 0)
							{
								attr = reader.GetAttribNoCheck(i);
								aname = Text::String::OrEmpty(attr->name);
								if (aname->Equals(UTF8STRC("href")) && attr->value.SetTo(avalue))
								{
									SDEL_STRING(href);
									href = avalue->Clone().Ptr();
#if defined(VERBOSE)
									printf("tileHref = %s\r\n", href->v);
#endif
								}
								else if (aname->Equals(UTF8STRC("profile")) && attr->value.SetTo(avalue))
								{
									SDEL_STRING(href);
									href = avalue->Clone().Ptr();
#if defined(VERBOSE)
									printf("tileProfile = %s\r\n", href->v);
#endif
								}
								else if (aname->Equals(UTF8STRC("units-per-pixel")) && attr->value.SetTo(avalue))
								{
									unitPerPixel = avalue->ToDoubleOr(0);
#if defined(VERBOSE)
									printf("tileUnitPerPixel = %lf\r\n", unitPerPixel);
#endif
								}
								else if (aname->Equals(UTF8STRC("order")) && attr->value.SetTo(avalue))
								{
									order = avalue->ToUOSInt();
#if defined(VERBOSE)
									printf("tileOrder = %d\r\n", (UInt32)order);
#endif
								}
							}
							if (href && order != INVALID_INDEX && unitPerPixel != 0)
							{
								lyr = MemAllocNN(TileLayer);
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
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
	}
	cli.Delete();
}

Map::TileMapServiceSource::TileMapServiceSource(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Text::CStringNN tmsURL)
{
	this->clif = clif;
	this->ssl = ssl;
	this->encFact = encFact;
	this->tmsURL = Text::String::New(tmsURL);
	this->origin = Math::Coord2DDbl(0, 0);
	this->csysOrigin = Math::Coord2DDbl(0, 0);
	this->title = 0;
	this->tileExt = 0;
	this->csys = Math::CoordinateSystemManager::CreateWGS84Csys();
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->concurrCnt = 2;
	this->imgType = IT_PNG;
	this->LoadXML();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
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
	this->csys.Delete();
	NN<TileLayer> layer;
	UOSInt i = this->layers.GetCount();
	while (i-- > 0)
	{
		layer = this->layers.GetItemNoCheck(i);
		layer->url->Release();
		MemFreeNN(layer);
	}
}

Text::CStringNN Map::TileMapServiceSource::GetName() const
{
	return this->title?this->title->ToCString():CSTR("Unknown");
}

Bool Map::TileMapServiceSource::IsError() const
{
	return this->layers.GetCount() == 0;
}

Map::TileMap::TileType Map::TileMapServiceSource::GetTileType() const
{
	return Map::TileMap::TT_TMS;
}

UOSInt Map::TileMapServiceSource::GetMinLevel() const
{
	return 0;
}

UOSInt Map::TileMapServiceSource::GetMaxLevel() const
{
	return this->layers.GetCount() - 1;
}

Double Map::TileMapServiceSource::GetLevelScale(UOSInt level) const
{
	NN<TileLayer> layer;
	if (!this->layers.GetItem(level).SetTo(layer))
	{
		return 0;
	}
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
	return layer->unitPerPixel / scaleDiv;
}

UOSInt Map::TileMapServiceSource::GetNearestLevel(Double scale) const
{
	Double minDiff = 1.0E+100;
	UOSInt minLevel = 0;
	UOSInt i = this->layers.GetCount();
	NN<TileLayer> layer;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = Map::TileMapUtil::CalcScaleDiv(this->csys);
	while (i-- > 0)
	{
		layer = this->layers.GetItemNoCheck(i);
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

UOSInt Map::TileMapServiceSource::GetConcurrentCount() const
{
	return this->concurrCnt;
}

Bool Map::TileMapServiceSource::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(this->bounds);
	return true;
}

NN<Math::CoordinateSystem> Map::TileMapServiceSource::GetCoordinateSystem() const
{
	return this->csys;
}

Bool Map::TileMapServiceSource::IsMercatorProj() const
{
	return false;
}

UOSInt Map::TileMapServiceSource::GetTileSize() const
{
	return this->tileWidth;
}

Map::TileMap::ImageType Map::TileMapServiceSource::GetImageType() const
{
	return this->imgType;
}

UOSInt Map::TileMapServiceSource::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, NN<Data::ArrayList<Math::Coord2D<Int32>>> ids)
{
	NN<TileLayer> layer;
	if (!this->layers.GetItem(level).SetTo(layer))
	{
		return 0;
	}
	
	rect = rect.OverlapArea(this->bounds);
	UOSInt ret = 0;
	Int32 minX = (Int32)((rect.min.x - this->csysOrigin.x) / (layer->unitPerPixel * UOSInt2Double(this->tileWidth)));
	Int32 maxX = (Int32)((rect.max.x - this->csysOrigin.x) / (layer->unitPerPixel * UOSInt2Double(this->tileWidth)));
	Int32 minY = (Int32)((rect.min.y - this->csysOrigin.y) / (layer->unitPerPixel * UOSInt2Double(this->tileHeight)));
	Int32 maxY = (Int32)((rect.max.y - this->csysOrigin.y) / (layer->unitPerPixel * UOSInt2Double(this->tileHeight)));
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

Optional<Media::ImageList> Map::TileMapServiceSource::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NN<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
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

UnsafeArrayOpt<UTF8Char> Map::TileMapServiceSource::GetTileImageURL(UnsafeArray<UTF8Char> sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	NN<TileLayer> layer;
	if (this->layers.GetItem(level).SetTo(layer))
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

Bool Map::TileMapServiceSource::GetTileImageURL(NN<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	NN<TileLayer> layer;
	if (this->layers.GetItem(level).SetTo(layer))
	{
		sb->Append(layer->url);
		sb->AppendUTF8Char('/');
		sb->AppendI32(tileId.x);
		sb->AppendUTF8Char('/');
		sb->AppendI32(tileId.y);
		sb->AppendUTF8Char('.');
		sb->Append(this->tileExt);
		return true;
	}
	return false;
}

Optional<IO::StreamData> Map::TileMapServiceSource::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
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
	NN<TileLayer> layer;
	if (!this->layers.GetItem(level).SetTo(layer))
		return 0;
	Double x1 = tileId.x * layer->unitPerPixel * UOSInt2Double(this->tileWidth) + this->csysOrigin.x;
	Double y1 = tileId.y * layer->unitPerPixel * UOSInt2Double(this->tileHeight) + this->csysOrigin.y;
	Double x2 = (tileId.x + 1) * layer->unitPerPixel * UOSInt2Double(this->tileWidth) + this->csysOrigin.x;
	Double y2 = (tileId.y + 1) * layer->unitPerPixel * UOSInt2Double(this->tileHeight) + this->csysOrigin.y;

	Math::RectAreaDbl b = Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2));
	bounds.Set(b);
	if (!this->bounds.OverlapOrTouch(b))
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
	NEW_CLASSNN(fd, IO::StmData::FileData({filePathU, (UOSInt)(sptru - filePathU)}, false));
	if (fd->GetDataSize() > 0)
	{
		currTime.SetCurrTimeUTC();
		currTime.AddDay(-7);
		((IO::StmData::FileData*)fd.Ptr())->GetFileStream()->GetFileTimes(&dt, 0, 0);
		if (dt.CompareTo(currTime) > 0)
		{
			it.Set(this->imgType);
			return fd.Ptr();
		}
		else
		{
			hasTime = true;
		}
	}
	fd.Delete();

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

	cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("TileMapService/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
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
	else
	{
#if defined(VERBOSE)
		printf("Response with status: %d\r\n", (Int32)status);
#endif
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

void Map::TileMapServiceSource::SetConcurrentCount(UOSInt concurrCnt)
{
	this->concurrCnt = concurrCnt;
}
