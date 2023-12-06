#include "Stdafx.h"
#include "Text/MyString.h"
#include "Math/Math.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Map/OSM/OSMTileMap.h"
#include "Math/CoordinateSystemManager.h"

Map::OSM::OSMLocalTileMap::OSMLocalTileMap(NotNullPtr<IO::PackageFile> pkgFile)
{
	this->pkgFile = pkgFile;
	this->name = 0;
	this->rootPkg = 0;
	this->minLevel = 0;
	this->maxLevel = 0;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->fmt = Text::String::New(CSTR("png"));
	this->min = Math::Coord2DDbl(0, 0);
	this->max = Math::Coord2DDbl(0, 0);
	this->csys = Math::CoordinateSystemManager::CreateDefaultCsys();

	UInt32 minXBlk;
	UInt32 maxXBlk;
	UInt32 minYBlk;
	UInt32 maxYBlk;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt32 currVal;
	UOSInt i;
	UOSInt j;

	if (pkgFile->GetCount() == 1 && pkgFile->GetItemType(0) == IO::PackageFile::PackObjectType::PackageFileType)
	{
		Bool needDelete;
		if (pkgFile->GetItemPack(0, needDelete).SetTo(pkgFile))
		{
			if (needDelete)
			{
				this->pkgFile.Delete();
				this->pkgFile = pkgFile;
			}
			else
			{
				this->rootPkg = this->pkgFile;
				this->pkgFile = pkgFile;
			}
		}
		else
		{
			pkgFile = this->pkgFile;
		}
	}

	i = 0;
	j = pkgFile->GetCount();
	while (i < j)
	{
		if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFileType)
		{
			pkgFile->GetItemName(sbuff, i);
			if (sbuff[0] != '.')
			{
				if (Text::StrToUInt32(sbuff, currVal) && currVal > this->maxLevel)
				{
					this->maxLevel = currVal;
				}
			}
		}
		i++;
	}
	if (this->maxLevel >= 0)
	{
		minXBlk = (UInt32)-1;
		maxXBlk = (UInt32)-1;
		
		Bool xNeedDelete;
		NotNullPtr<IO::PackageFile> xPkg;
		sptr = Text::StrUOSInt(sbuff, this->maxLevel);
		if (pkgFile->GetItemPack((UOSInt)pkgFile->GetItemIndex(CSTRP(sbuff, sptr)), xNeedDelete).SetTo(xPkg))
		{
			i = 0;
			j = xPkg->GetCount();
			while (i < j)
			{
				if (xPkg->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFileType)
				{
					xPkg->GetItemName(sbuff, i);
					if (sbuff[0] != '.')
					{
						if (Text::StrToUInt32(sbuff, currVal))
						{
							if (minXBlk == (UInt32)-1 || minXBlk > currVal)
							{
								minXBlk = currVal;
							}
							if (maxXBlk == (UInt32)-1 || maxXBlk < currVal)
							{
								maxXBlk = currVal;
							}
						}
					}
				}
				i++;
			}
			if (minXBlk != (UInt32)-1)
			{
				minYBlk = (UInt32)-1;
				maxYBlk = (UInt32)-1;

				Bool yNeedDelete;
				NotNullPtr<IO::PackageFile> yPkg;
				sptr = Text::StrUInt32(sbuff, minXBlk);
				if (xPkg->GetItemPack((UOSInt)xPkg->GetItemIndex(CSTRP(sbuff, sptr)), yNeedDelete).SetTo(yPkg))
				{
					i = yPkg->GetCount();
					while (i-- > 0)
					{
						if (yPkg->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
						{
							yPkg->GetItemName(sbuff, i);
							j = Text::StrIndexOfChar(sbuff, '.');
							if (j != INVALID_INDEX)
							{
								sbuff[j] = 0;
								if (Text::StrToUInt32(sbuff, currVal))
								{
									if (minYBlk == (UInt32)-1 || minYBlk > currVal)
									{
										minYBlk = currVal;
									}
									if (maxYBlk == (UInt32)-1 || maxYBlk < currVal)
									{
										maxYBlk = currVal;
									}
								}
							}
						}
					}
					if (yNeedDelete)
					{
						yPkg.Delete();
					}

					if (minYBlk != (UInt32)-1)
					{
						this->min.x = Map::OSM::OSMTileMap::TileX2Lon((Int32)minXBlk, this->maxLevel);
						this->max.x = Map::OSM::OSMTileMap::TileX2Lon((Int32)maxXBlk + 1, this->maxLevel);
						this->min.y = Map::OSM::OSMTileMap::TileY2Lat((Int32)maxYBlk + 1, this->maxLevel);
						this->max.y = Map::OSM::OSMTileMap::TileY2Lat((Int32)minYBlk, this->maxLevel);
					}
				}
			}
			if (xNeedDelete)
			{
				xPkg.Delete();
			}
		}
	}
}

Map::OSM::OSMLocalTileMap::OSMLocalTileMap(NotNullPtr<IO::PackageFile> pkgFile, NotNullPtr<Text::String> name, NotNullPtr<Text::String> format, UOSInt minZoom, UOSInt maxZoom, Math::Coord2DDbl minCoord, Math::Coord2DDbl maxCoord)
{
	this->pkgFile = pkgFile;
	this->name = name->Clone().Ptr();
	this->rootPkg = 0;
	this->minLevel = minZoom;
	this->maxLevel = maxZoom;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->fmt = format->Clone();
	this->min = minCoord;
	this->max = maxCoord;
	this->csys = Math::CoordinateSystemManager::CreateDefaultCsys();
}

Map::OSM::OSMLocalTileMap::~OSMLocalTileMap()
{
	NotNullPtr<IO::PackageFile> pkg;
	if (this->rootPkg.SetTo(pkg))
	{
		pkg.Delete();
	}
	else
	{
		this->pkgFile.Delete();
	}
	this->csys.Delete();
	this->fmt->Release();
	SDEL_STRING(this->name);
}

Text::CStringNN Map::OSM::OSMLocalTileMap::GetName() const
{
	if (this->name)
		return this->name->ToCString();
	return CSTR("OSMLocalTileMap");
}

Bool Map::OSM::OSMLocalTileMap::IsError() const
{
	return this->maxLevel < this->minLevel;
}

Map::TileMap::TileType Map::OSM::OSMLocalTileMap::GetTileType() const
{
	return Map::TileMap::TT_OSMLOCAL;
}

UOSInt Map::OSM::OSMLocalTileMap::GetMinLevel() const
{
	return this->minLevel;
}

UOSInt Map::OSM::OSMLocalTileMap::GetMaxLevel() const
{
	return this->maxLevel;
}


Double Map::OSM::OSMLocalTileMap::GetLevelScale(UOSInt index) const
{
	return 204094080000.0 / UOSInt2Double(this->tileWidth) / (1 << index);
}

UOSInt Map::OSM::OSMLocalTileMap::GetNearestLevel(Double scale) const
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / UOSInt2Double(this->tileWidth)) / Math_Log10(2));
	if (level < (Int32)this->minLevel)
		level = 0;
	else if (level > (Int32)this->maxLevel)
		level = (Int32)this->maxLevel;
	return (UOSInt)level;
}

UOSInt Map::OSM::OSMLocalTileMap::GetConcurrentCount() const
{
	return 1;
}

Bool Map::OSM::OSMLocalTileMap::GetBounds(OutParam<Math::RectAreaDbl> bounds) const
{
	bounds.Set(Math::RectAreaDbl(this->min, this->max));
	return this->min.x != 0 || this->min.y != 0 || this->max.x != 0 || this->max.y != 0;
}

NotNullPtr<Math::CoordinateSystem> Map::OSM::OSMLocalTileMap::GetCoordinateSystem() const
{
	return this->csys;
}

Bool Map::OSM::OSMLocalTileMap::IsMercatorProj() const
{
	return true;
}

UOSInt Map::OSM::OSMLocalTileMap::GetTileSize() const
{
	return this->tileWidth;
}

Map::TileMap::ImageType Map::OSM::OSMLocalTileMap::GetImageType() const
{
	if (this->fmt->Equals(UTF8STRC("webp")))
	{
		return IT_WEBP;
	}
	else if (this->fmt->Equals(UTF8STRC("jpg")))
	{
		return IT_JPG;
	}
	else
	{
		return IT_PNG;
	}
}

UOSInt Map::OSM::OSMLocalTileMap::GetTileImageIDs(UOSInt level, Math::RectAreaDbl rect, Data::ArrayList<Math::Coord2D<Int32>> *ids)
{
	Int32 i;
	Int32 j;
	rect.tl = rect.tl.Max(this->min);
	rect.br = rect.br.Min(this->max);
	if (rect.tl.x >= rect.br.x)
		return 0;
	if (rect.tl.y >= rect.br.y)
		return 0;
	if (level < this->minLevel || level > this->maxLevel)
		return 0;
	Int32 pixX1 = Map::OSM::OSMTileMap::Lon2TileX(rect.tl.x, level);
	Int32 pixX2 = Map::OSM::OSMTileMap::Lon2TileX(rect.br.x, level);
	Int32 pixY1 = Map::OSM::OSMTileMap::Lat2TileY(rect.tl.y, level);
	Int32 pixY2 = Map::OSM::OSMTileMap::Lat2TileY(rect.br.y, level);
	if (pixX1 > pixX2)
	{
		i = pixX1;
		pixX1 = pixX2;
		pixX2 = i;
	}
	if (pixY1 > pixY2)
	{
		i = pixY1;
		pixY1 = pixY2;
		pixY2 = i;
	}
	if (pixX1 < 0)
		pixX1 = 0;
	if (pixY1 < 0)
		pixY1 = 0;
	if (pixX2 < 0)
		pixX2 = 0;
	if (pixY2 < 0)
		pixY2 = 0;
	if (pixX2 >= (1 << level))
	{
		pixX2 = (1 << level) - 1;
	}
	i = pixY1;
	while (i <= pixY2)
	{
		j = pixX1;
		while (j <= pixX2)
		{
			ids->Add(Math::Coord2D<Int32>(j, i));
			j++;
		}
		i++;
	}
	return (UOSInt)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Media::ImageList *Map::OSM::OSMLocalTileMap::LoadTileImage(UOSInt level, Math::Coord2D<Int32> tileId, NotNullPtr<Parser::ParserList> parsers, OutParam<Math::RectAreaDbl> bounds, Bool localOnly)
{
	ImageType it;
	NotNullPtr<IO::StreamData> fd;
	IO::ParsedObject *pobj;
	if (this->LoadTileImageData(level, tileId, bounds, localOnly, it).SetTo(fd))
	{
		IO::ParserType pt;
		pobj = parsers->ParseFile(fd, &pt);
		fd.Delete();
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

UTF8Char *Map::OSM::OSMLocalTileMap::GetTileImageURL(UTF8Char *sbuff, UOSInt level, Math::Coord2D<Int32> tileId)
{
	UTF8Char *sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("file:///"));
	sptr = this->pkgFile->GetSourceName(sptr);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		Text::StrReplace(sbuff, '\\', '/');
	}
	sptr = Text::StrUOSInt(sptr, level);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, tileId.x);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, tileId.y);
	*sptr++ = '.';
	sptr = this->fmt->ConcatTo(sptr);
	return sptr;
}

Bool Map::OSM::OSMLocalTileMap::GetTileImageURL(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt level, Math::Coord2D<Int32> tileId)
{
	sb->AppendC(UTF8STRC("file:///"));
	sb->Append(this->pkgFile->GetSourceNameObj());
	if (!sb->EndsWith(IO::Path::PATH_SEPERATOR))
		sb->AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	if (IO::Path::PATH_SEPERATOR == '\\')
	{
		sb->Replace('\\', '/');
	}
	sb->AppendUOSInt(level);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileId.x);
	sb->AppendUTF8Char('/');
	sb->AppendI32(tileId.y);
	sb->AppendUTF8Char('.');
	sb->Append(this->fmt);
	return true;
}

Optional<IO::StreamData> Map::OSM::OSMLocalTileMap::LoadTileImageData(UOSInt level, Math::Coord2D<Int32> tileId, OutParam<Math::RectAreaDbl> bounds, Bool localOnly, OptOut<ImageType> it)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Optional<IO::StreamData> fd;
	if (level < this->minLevel || level > this->maxLevel)
		return 0;
	Double x1 = Map::OSM::OSMTileMap::TileX2Lon(tileId.x, level);
	Double y1 = Map::OSM::OSMTileMap::TileY2Lat(tileId.y, level);
	Double x2 = Map::OSM::OSMTileMap::TileX2Lon(tileId.x + 1, level);
	Double y2 = Map::OSM::OSMTileMap::TileY2Lat(tileId.y + 1, level);

	bounds.Set(Math::RectAreaDbl(Math::Coord2DDbl(x1, y1), Math::Coord2DDbl(x2, y2)));
	if (x1 > 180 || y1 < -90)
		return 0;

	Bool xNeedDelete;
	NotNullPtr<IO::PackageFile> xPkg;
	Bool yNeedDelete;
	NotNullPtr<IO::PackageFile> yPkg;
	fd = 0;
	sptr = Text::StrUOSInt(sbuff, level);
	if (this->pkgFile->GetItemPack((UOSInt)this->pkgFile->GetItemIndex(CSTRP(sbuff, sptr)), xNeedDelete).SetTo(xPkg))
	{
		sptr = Text::StrInt32(sbuff, tileId.x);
		if (xPkg->GetItemPack((UOSInt)xPkg->GetItemIndex(CSTRP(sbuff, sptr)), yNeedDelete).SetTo(yPkg))
		{
			sptr = Text::StrInt32(sbuff, tileId.y);
			*sptr++ = '.';
			sptr = this->fmt->ConcatTo(sptr);
			fd = yPkg->GetItemStmDataNew((UOSInt)yPkg->GetItemIndex(CSTRP(sbuff, sptr)));
			if (!fd.IsNull())
			{
				if (it.IsNotNull())
				{
					it.SetNoCheck(this->GetImageType());
				}
			}
			if (yNeedDelete)
			{
				yPkg.Delete();
			}
		}
		if (xNeedDelete)
		{
			xPkg.Delete();
		}
	}
	return fd;
}

Bool Map::OSM::OSMLocalTileMap::GetTileBounds(UOSInt level, OutParam<Int32> minX, OutParam<Int32> minY, OutParam<Int32> maxX, OutParam<Int32> maxY)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 x;
	Int32 y;
	Bool found = false;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char sbuff2[32];
	UTF8Char *sptr2;
	sbuff2[0] = '.';
	sptr2 = this->fmt->ConcatTo(&sbuff2[1]);
	i = 0;
	j = pkgFile->GetCount();
	while (i < j)
	{
		if (pkgFile->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFileType)
		{
			pkgFile->GetItemName(sbuff, i);
			if (sbuff[0] != '.')
			{
				if (Text::StrToInt32(sbuff, x) && x == (OSInt)level)
				{
					found = true;
					break;
				}
			}
		}
		i++;
	}
	if (!found)
	{
		return false;
	}

	found = false;
	Bool foundX = false;
	Int32 thisMinX = 0;
	Int32 thisMinY = 0;
	Int32 thisMaxX = 0;
	Int32 thisMaxY = 0;
	Bool levNeedDelete;
	NotNullPtr<IO::PackageFile> levPkg;
	if (pkgFile->GetItemPack(i, levNeedDelete).SetTo(levPkg))
	{
		i = levPkg->GetCount();
		while (i-- > 0)
		{
			if (levPkg->GetItemType(i) == IO::PackageFile::PackObjectType::PackageFileType)
			{
				levPkg->GetItemName(sbuff, i);
				if (Text::StrToInt32(sbuff, x))
				{
					if (foundX)
					{
						if (x > thisMaxX)
							thisMaxX = x;
						if (x < thisMinX)
							thisMinX = x;
					}
					else
					{
						foundX = true;
						thisMinX = x;
						thisMaxX = x;
					}
					Bool xNeedDelete;
					NotNullPtr<IO::PackageFile> xPkg;
					if (levPkg->GetItemPack(i, xNeedDelete).SetTo(xPkg))
					{
						j = xPkg->GetCount();
						while (j-- > 0)
						{
							sptr = xPkg->GetItemName(sbuff, j);
							if (xPkg->GetItemType(j) == IO::PackageFile::PackObjectType::StreamData)
							{
								k = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), sbuff2, (UOSInt)(sptr2 - sbuff2));
								if (k != INVALID_INDEX)
								{
									sbuff[k] = 0;
									if (Text::StrToInt32(sbuff, y))
									{
										if (found)
										{
											if (y > thisMaxY)
												thisMaxY = y;
											if (y < thisMinY)
												thisMinY = y;
										}
										else
										{
											found = true;
											thisMinY = y;
											thisMaxY = y;
										}
									}
								}
							}
						}
						if (xNeedDelete)
						{
							xPkg.Delete();
						}
					}
				}
			}
		}
		if (levNeedDelete)
		{
			levPkg.Delete();
		}
		if (found)
		{
			minX.Set(thisMinX);
			maxX.Set(thisMaxX);
			minY.Set(thisMinY);
			maxY.Set(thisMaxY);
		}
	}
	return found;
}
