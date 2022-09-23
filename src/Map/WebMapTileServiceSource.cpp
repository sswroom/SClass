#include "Stdafx.h"
#include "Crypto/Hash/CRC32RC.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/WebMapTileServiceSource.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Unit/Distance.h"
#include "Net/HTTPClient.h"
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
	Net::HTTPClient *cli = Net::HTTPClient::CreateConnect(this->sockf, 0, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, false);
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
				if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENT)
				{
					s = reader.GetNodeText();
					if (s->Equals(UTF8STRC("Contents")))
					{
						while (reader.ReadNext())
						{
							if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENT)
							{
								s = reader.GetNodeText();
								if (s->Equals(UTF8STRC("Layer")))
								{
									ReadLayer(&reader);
								}
								else if (s->Equals(UTF8STRC("TileMatrixSet")))
								{
									reader.SkipElement();
								}
								else
								{
									reader.SkipElement();
								}
							}
							else if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
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
				else if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENTEND)
				{
					break;
				}
			}
		}
	}
	DEL_CLASS(cli);
}

void Map::WebMapTileServiceSource::ReadLayer(Text::XMLReader *reader)
{
	////////////////////////////////////////
	reader->SkipElement();
}

Double Map::WebMapTileServiceSource::CalcScaleDiv()
{
	if (this->currMatrix == 0 || this->currMatrix->csys == 0 || this->currMatrix->csys->IsProjected())
	{
		return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 1);
	}
	else
	{
		return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_PIXEL, Math::Unit::Distance::DU_METER, 0.000005);
	}
}

Map::WebMapTileServiceSource::TileSet *Map::WebMapTileServiceSource::GetTileSet(UOSInt level)
{
	if (this->currMatrix == 0)
		return 0;
	return this->currMatrix->tileSets.GetItem(level);
}

void Map::WebMapTileServiceSource::ReleaseLayer(TileLayer *layer)
{
	layer->id->Release();
	UOSInt i = layer->tileMatrixes.GetCount();
	while (i-- > 0)
	{
		this->ReleaseTileMatrix(layer->tileMatrixes.GetItem(i));
	}
	DEL_CLASS(layer);
}

void Map::WebMapTileServiceSource::ReleaseTileMatrix(TileMatrixSet *tileMatrix)
{
	UOSInt i;
	tileMatrix->id->Release();
	tileMatrix->tileExt->Release();
	DEL_CLASS(tileMatrix->csys);
	tileMatrix->url->Release();
	i = tileMatrix->tileSets.GetCount();
	while (i-- > 0)
	{
		MemFree(tileMatrix->tileSets.GetItem(i));
	}
	DEL_CLASS(tileMatrix);
}

Map::WebMapTileServiceSource::WebMapTileServiceSource(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, Text::CString wmtsURL)
{
	this->cacheDir = 0;
	this->sockf = sockf;
	this->encFact = encFact;
	this->wmtsURL = Text::String::New(wmtsURL);
	this->LoadXML();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("wmts"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	Crypto::Hash::CRC32RC crc;
	sptr = Text::StrHexVal32(sptr, crc.CalcDirect(wmtsURL.v, wmtsURL.leng));
	this->cacheDir = Text::String::NewP(sbuff, sptr);
	this->currLayer = this->layers.GetItem(0);
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
	if (this->currMatrix == 0)
		return 0;
	return this->currMatrix->tileSets.GetCount();
}

Double Map::WebMapTileServiceSource::GetLevelScale(UOSInt level)
{
	TileSet *tileSet = this->GetTileSet(level);
	if (tileSet == 0)
	{
		return 0;
	}
	Double scaleDiv = CalcScaleDiv();
	return tileSet->unitPerPixel / scaleDiv;
}

UOSInt Map::WebMapTileServiceSource::GetNearestLevel(Double scale)
{
	if (this->currMatrix == 0)
		return 0;
	Double minDiff = 1.0E+100;
	UOSInt minLevel = 0;
	UOSInt i = this->currMatrix->tileSets.GetCount();
	TileSet *tileSet;
	Double layerScale;
	Double thisDiff;
	Double scaleDiv = CalcScaleDiv();
	while (i-- > 0)
	{
		tileSet = this->currMatrix->tileSets.GetItem(i);
		layerScale = tileSet->unitPerPixel / scaleDiv;
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
	if (this->currMatrix)
	{
		*bounds = this->currMatrix->bounds;
		return true;
	}
	return false;
}

Math::CoordinateSystem *Map::WebMapTileServiceSource::GetCoordinateSystem()
{
	if (this->currMatrix)
	{
		return this->currMatrix->csys;
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
	if (this->currMatrix)
	{
		return this->currMatrix->tileWidth;
	}
	else
	{
		return 0;
	}
}

UOSInt Map::WebMapTileServiceSource::GetImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Int64> *ids)
{
	TileSet *tileSet = this->GetTileSet(level);
	if (tileSet == 0)
	{
		return 0;
	}
	
	rect = rect.OverlapArea(this->currMatrix->bounds);
	UOSInt ret = 0;
	Int32 minX = (Int32)((rect.tl.x - this->currMatrix->csysOrigin.x) / (tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileWidth)));
	Int32 maxX = (Int32)((rect.br.x - this->currMatrix->csysOrigin.x) / (tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileWidth)));
	Int32 minY = (Int32)((rect.tl.y - this->currMatrix->csysOrigin.y) / (tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileHeight)));
	Int32 maxY = (Int32)((rect.br.y - this->currMatrix->csysOrigin.y) / (tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileHeight)));
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
	TileSet *tileSet = this->GetTileSet(level);
	if (tileSet)
	{
		Int32 x = (Int32)(imgId >> 32);
		Int32 y = (Int32)(imgId & 0xffffffff);
		sbuff = this->currMatrix->url->ConcatTo(sbuff);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, x);
		*sbuff++ = '/';
		sbuff = Text::StrInt32(sbuff, y);
		*sbuff++ = '.';
		sbuff = this->currMatrix->tileExt->ConcatTo(sbuff);
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
	TileSet *tileSet = this->GetTileSet(level);
	if (tileSet == 0)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = imgX * tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileWidth) + this->currMatrix->csysOrigin.x;
	Double y1 = imgY * tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileHeight) + this->currMatrix->csysOrigin.y;
	Double x2 = (imgX + 1) * tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileWidth) + this->currMatrix->csysOrigin.x;
	Double y2 = (imgY + 1) * tileSet->unitPerPixel * UOSInt2Double(this->currMatrix->tileHeight) + this->currMatrix->csysOrigin.y;

	bounds->tl = Math::Coord2DDbl(x1, y1);
	bounds->br = Math::Coord2DDbl(x2, y2);
	if (!this->currMatrix->bounds.OverlapOrTouch(*bounds))
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
		sptru = this->currMatrix->tileExt->ConcatTo(sptru);
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
					*it = this->currMatrix->imgType;
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

	////////////////////////////////////////////////
	urlSb.ClearStr();
	urlSb.Append(this->currMatrix->url);
	urlSb.AppendUTF8Char('/');
	urlSb.AppendI32(imgX);
	urlSb.AppendUTF8Char('/');
	urlSb.AppendI32(imgY);
	urlSb.AppendUTF8Char('.');
	urlSb.Append(this->currMatrix->tileExt);

	cli = Net::HTTPClient::CreateClient(this->sockf, 0, CSTR("WMTS/1.0 SSWR/1.0"), true, urlSb.StartsWith(UTF8STRC("https://")));
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
