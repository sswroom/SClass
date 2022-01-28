#include "Stdafx.h"
#include "Text/MyString.h"
#include "Math/Math.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/OSM/OSMLocalTileMap.h"
#include "Map/OSM/OSMTileMap.h"

/*Map::OSM::OSMLocalTileMap::OSMLocalTileMap(const WChar *tileDir)
{
	this->tileDir = Text::StrCopyNew(tileDir);
	this->maxLevel = -1;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->minX = 0;
	this->maxX = 0;
	this->minY = 0;
	this->maxY = 0;

	Int32 minXBlk;
	Int32 maxXBlk;
	Int32 minYBlk;
	Int32 maxYBlk;
	WChar sbuff[512];
	WChar *sptr;
	IO::Path::FindFileSession *sess;
	Int32 currVal;
	UOSInt i;
	IO::Path::PathType pt;

	sptr = Text::StrConcat(sbuff, this->tileDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	Text::StrConcatC(sptr, UTF8STRC("*.*"));
	sess = IO::Path::FindFile(sbuff);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt))
		{
			if (pt == IO::Path::PathType::Directory && sptr[0] != '.')
			{
				if (Text::StrToInt32(sptr, &currVal) && currVal > this->maxLevel)
				{
					this->maxLevel = currVal;
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	if (this->maxLevel >= 0)
	{
		minXBlk = -1;
		maxXBlk = -1;
		sptr = Text::StrInt32(sptr, this->maxLevel);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		Text::StrConcatC(sptr, UTF8STRC("*.*"));
		
		sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			while (IO::Path::FindNextFile(sptr, sess, 0, &pt))
			{
				if (pt == IO::Path::PathType::Directory && sptr[0] != '.')
				{
					if (Text::StrToInt32(sptr, &currVal))
					{
						if (minXBlk == -1 || minXBlk > currVal)
						{
							minXBlk = currVal;
						}
						if (maxXBlk == -1 || maxXBlk < currVal)
						{
							maxXBlk = currVal;
						}
					}
				}
			}
			IO::Path::FindFileClose(sess);
		}
		if (minXBlk != -1)
		{
			minYBlk = -1;
			maxYBlk = -1;
			sptr = Text::StrInt32(sptr, minXBlk);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			Text::StrConcatC(sptr, UTF8STRC("*.png"));

			sess = IO::Path::FindFile(sbuff);
			if (sess)
			{
				while (IO::Path::FindNextFile(sptr, sess, 0, &pt))
				{
					if (pt == IO::Path::PathType::File)
					{
						i = Text::StrIndexOfChar(sptr, '.');
						if (i != INVALID_INDEX)
						{
							sptr[i] = 0;
							if (Text::StrToInt32(sptr, &currVal))
							{
								if (minYBlk == -1 || minYBlk > currVal)
								{
									minYBlk = currVal;
								}
								if (maxYBlk == -1 || maxYBlk < currVal)
								{
									maxYBlk = currVal;
								}
							}
						}
					}
				}
				IO::Path::FindFileClose(sess);

				if (minYBlk != -1)
				{
					this->minX = Map::OSM::OSMTileMap::TileX2Lon(minXBlk, this->maxLevel);
					this->maxX = Map::OSM::OSMTileMap::TileX2Lon(maxXBlk + 1, this->maxLevel);
					this->minY = Map::OSM::OSMTileMap::TileY2Lat(maxYBlk + 1, this->maxLevel);
					this->maxY = Map::OSM::OSMTileMap::TileY2Lat(minYBlk, this->maxLevel);
				}
			}
		}
	}
}

Map::OSM::OSMLocalTileMap::~OSMLocalTileMap()
{
	SDEL_TEXT(this->tileDir);
}*/

Map::OSM::OSMLocalTileMap::OSMLocalTileMap(IO::PackageFile *pkgFile)
{
	this->pkgFile = pkgFile;
	this->maxLevel = 0;
	this->tileWidth = 256;
	this->tileHeight = 256;
	this->minX = 0;
	this->maxX = 0;
	this->minY = 0;
	this->maxY = 0;

	UInt32 minXBlk;
	UInt32 maxXBlk;
	UInt32 minYBlk;
	UInt32 maxYBlk;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt32 currVal;
	UOSInt i;
	UOSInt j;

	if (pkgFile->GetCount() == 1 && pkgFile->GetItemType(0) == IO::PackageFile::POT_PACKAGEFILE)
	{
		pkgFile = pkgFile->GetItemPack(0);
		if (pkgFile)
		{
			DEL_CLASS(this->pkgFile);
			this->pkgFile = pkgFile;
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
		if (pkgFile->GetItemType(i) == IO::PackageFile::POT_PACKAGEFILE)
		{
			pkgFile->GetItemName(sbuff, i);
			if (sbuff[0] != '.')
			{
				if (Text::StrToUInt32(sbuff, &currVal) && currVal > this->maxLevel)
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
		
		IO::PackageFile *xPkg;
		sptr = Text::StrUOSInt(sbuff, this->maxLevel);
		xPkg = pkgFile->GetItemPack((UOSInt)pkgFile->GetItemIndex(sbuff, (UOSInt)(sptr - sbuff)));
		if (xPkg)
		{
			i = 0;
			j = xPkg->GetCount();
			while (i < j)
			{
				if (xPkg->GetItemType(i) == IO::PackageFile::POT_PACKAGEFILE)
				{
					xPkg->GetItemName(sbuff, i);
					if (sbuff[0] != '.')
					{
						if (Text::StrToUInt32(sbuff, &currVal))
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

				IO::PackageFile *yPkg;
				sptr = Text::StrUInt32(sbuff, minXBlk);
				yPkg = xPkg->GetItemPack((UOSInt)xPkg->GetItemIndex(sbuff, (UOSInt)(sptr - sbuff)));
				if (yPkg)
				{
					i = yPkg->GetCount();
					while (i-- > 0)
					{
						if (yPkg->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
						{
							yPkg->GetItemName(sbuff, i);
							j = Text::StrIndexOfChar(sbuff, '.');
							if (j != INVALID_INDEX)
							{
								sbuff[j] = 0;
								if (Text::StrToUInt32(sbuff, &currVal))
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
					DEL_CLASS(yPkg);

					if (minYBlk != (UInt32)-1)
					{
						this->minX = Map::OSM::OSMTileMap::TileX2Lon((Int32)minXBlk, this->maxLevel);
						this->maxX = Map::OSM::OSMTileMap::TileX2Lon((Int32)maxXBlk + 1, this->maxLevel);
						this->minY = Map::OSM::OSMTileMap::TileY2Lat((Int32)maxYBlk + 1, this->maxLevel);
						this->maxY = Map::OSM::OSMTileMap::TileY2Lat((Int32)minYBlk, this->maxLevel);
					}
				}
			}
			DEL_CLASS(xPkg);
		}
	}
}

Map::OSM::OSMLocalTileMap::~OSMLocalTileMap()
{
	DEL_CLASS(this->pkgFile);
}

Text::CString Map::OSM::OSMLocalTileMap::GetName()
{
	return CSTR("OSMLocalTileMap");
}

Bool Map::OSM::OSMLocalTileMap::IsError()
{
	return this->maxLevel < 0;
}

Map::TileMap::TileType Map::OSM::OSMLocalTileMap::GetTileType()
{
	return Map::TileMap::TT_OSMLOCAL;
}

UOSInt Map::OSM::OSMLocalTileMap::GetLevelCount()
{
	return this->maxLevel + 1;
}


Double Map::OSM::OSMLocalTileMap::GetLevelScale(UOSInt index)
{
	return 204094080000.0 / UOSInt2Double(this->tileWidth) / (1 << index);
}

UOSInt Map::OSM::OSMLocalTileMap::GetNearestLevel(Double scale)
{
	Int32 level = Double2Int32(Math_Log10(204094080000.0 / scale / UOSInt2Double(this->tileWidth)) / Math_Log10(2));
	if (level < 0)
		level = 0;
	else if (level >= (Int32)GetLevelCount())
		level = (Int32)GetLevelCount() - 1;
	return (UOSInt)level;
}

UOSInt Map::OSM::OSMLocalTileMap::GetConcurrentCount()
{
	return 1;
}

Bool Map::OSM::OSMLocalTileMap::GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY)
{
	*minX = this->minX;
	*minY = this->minY;
	*maxX = this->maxX;
	*maxY = this->maxY;
	return this->minX != 0 || this->minY != 0 || this->maxX != 0 || this->maxY != 0;
}

Map::TileMap::ProjectionType Map::OSM::OSMLocalTileMap::GetProjectionType()
{
	return Map::TileMap::PT_MERCATOR;
}

UOSInt Map::OSM::OSMLocalTileMap::GetTileSize()
{
	return this->tileWidth;
}

UOSInt Map::OSM::OSMLocalTileMap::GetImageIDs(UOSInt level, Double x1, Double y1, Double x2, Double y2, Data::ArrayList<Int64> *ids)
{
	Int32 i;
	Int32 j;
	if (y1 < this->minY)
		y1 = this->minY;
	else if (y1 > this->maxY)
		y1 = this->maxY;
	if (y2 < this->minY)
		y2 = this->minY;
	else if (y2 > this->maxY)
		y2 = this->maxY;
	
	if (x1 < this->minX)
		x1 = this->minX;
	else if (x1 > this->maxX)
		x1 = this->maxX;
	if (x2 < this->minX)
		x2 = this->minX;
	else if (x2 > this->maxX)
		x2 = this->maxX;
	if (x1 == x2)
		return 0;
	if (y1 == y2)
		return 0;
	Int32 pixX1 = Map::OSM::OSMTileMap::Lon2TileX(x1, level);
	Int32 pixX2 = Map::OSM::OSMTileMap::Lon2TileX(x2, level);
	Int32 pixY1 = Map::OSM::OSMTileMap::Lat2TileY(y1, level);
	Int32 pixY2 = Map::OSM::OSMTileMap::Lat2TileY(y2, level);
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
			ids->Add((((Int64)(UInt32)j) << 32) | (UInt32)i);
			j++;
		}
		i++;
	}
	return (UOSInt)((pixX2 - pixX1 + 1) * (pixY2 - pixY1 + 1));
}

Media::ImageList *Map::OSM::OSMLocalTileMap::LoadTileImage(UOSInt level, Int64 imgId, Parser::ParserList *parsers, Double *boundsXY, Bool localOnly)
{
	Int32 blockX;
	Int32 blockY;
	ImageType it;
	IO::IStreamData *fd;
	IO::ParsedObject *pobj;
	fd = this->LoadTileImageData(level, imgId, boundsXY, localOnly, &blockX, &blockY, &it);
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

UTF8Char *Map::OSM::OSMLocalTileMap::GetImageURL(UTF8Char *sbuff, UOSInt level, Int64 imgId)
{
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
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
	sptr = Text::StrInt32(sptr, imgX);
	sptr = Text::StrConcatC(sptr, UTF8STRC("/"));
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	return sptr;
}

/*IO::IStreamData *Map::OSM::OSMLocalTileMap::LoadTileImageData(OSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	WChar filePath[512];
	WChar *sptr;
	IO::StmData::FileData *fd;
	if (level < 0 || level > this->maxLevel)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = Map::OSM::OSMTileMap::TileX2Lon(imgX, level);
	Double y1 = Map::OSM::OSMTileMap::TileY2Lat(imgY, level);
	Double x2 = Map::OSM::OSMTileMap::TileX2Lon(imgX + 1, level);
	Double y2 = Map::OSM::OSMTileMap::TileY2Lat(imgY + 1, level);

	boundsXY[0] = x1;
	boundsXY[1] = y1;
	boundsXY[2] = x2;
	boundsXY[3] = y2;
	if (x1 > 180 || y1 < -90)
		return 0;

	sptr = Text::StrConcat(filePath, this->tileDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, (Int32)level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, imgX);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
	NEW_CLASS(fd, IO::StmData::FileData(filePath, false));
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
	return 0;
}
*/

IO::IStreamData *Map::OSM::OSMLocalTileMap::LoadTileImageData(UOSInt level, Int64 imgId, Double *boundsXY, Bool localOnly, Int32 *blockX, Int32 *blockY, ImageType *it)
{
	UTF8Char u8buff[512];
	UTF8Char *sptr;
	IO::IStreamData *fd;
	if (level < 0 || level > this->maxLevel)
		return 0;
	Int32 imgX = (Int32)(imgId >> 32);
	Int32 imgY = (Int32)(imgId & 0xffffffffLL);
	Double x1 = Map::OSM::OSMTileMap::TileX2Lon(imgX, level);
	Double y1 = Map::OSM::OSMTileMap::TileY2Lat(imgY, level);
	Double x2 = Map::OSM::OSMTileMap::TileX2Lon(imgX + 1, level);
	Double y2 = Map::OSM::OSMTileMap::TileY2Lat(imgY + 1, level);

	boundsXY[0] = x1;
	boundsXY[1] = y1;
	boundsXY[2] = x2;
	boundsXY[3] = y2;
	if (x1 > 180 || y1 < -90)
		return 0;

	IO::PackageFile *xPkg;
	IO::PackageFile *yPkg;
	fd = 0;
	sptr = Text::StrUOSInt(u8buff, level);
	xPkg = this->pkgFile->GetItemPack((UOSInt)this->pkgFile->GetItemIndex(u8buff, (UOSInt)(sptr - u8buff)));
	if (xPkg)
	{
		sptr = Text::StrInt32(u8buff, imgX);
		yPkg = xPkg->GetItemPack((UOSInt)xPkg->GetItemIndex(u8buff, (UOSInt)(sptr - u8buff)));
		if (yPkg)
		{
			sptr = Text::StrInt32(u8buff, imgY);
			sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
			fd = yPkg->GetItemStmData((UOSInt)yPkg->GetItemIndex(u8buff, (UOSInt)(sptr - u8buff)));
			if (fd)
			{
				if (blockX)
					*blockX = imgX;
				if (blockY)
					*blockY = imgY;
				if (it)
					*it = IT_PNG;
			}
			DEL_CLASS(yPkg);
		}
		DEL_CLASS(xPkg);
	}
	return fd;
/*	sptr = Text::StrConcat(filePath, this->tileDir);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, (Int32)level);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, imgX);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt32(sptr, imgY);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));*/
}

Bool Map::OSM::OSMLocalTileMap::GetTileBounds(UOSInt level, Int32 *minX, Int32 *minY, Int32 *maxX, Int32 *maxY)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 x;
	Int32 y;
	Bool found = false;
	UTF8Char u8buff[512];
	i = 0;
	j = pkgFile->GetCount();
	while (i < j)
	{
		if (pkgFile->GetItemType(i) == IO::PackageFile::POT_PACKAGEFILE)
		{
			pkgFile->GetItemName(u8buff, i);
			if (u8buff[0] != '.')
			{
				if (Text::StrToInt32(u8buff, &x) && x == (OSInt)level)
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
	IO::PackageFile *levPkg = pkgFile->GetItemPack(i);
	if (levPkg)
	{
		i = levPkg->GetCount();
		while (i-- > 0)
		{
			if (levPkg->GetItemType(i) == IO::PackageFile::POT_PACKAGEFILE)
			{
				levPkg->GetItemName(u8buff, i);
				if (Text::StrToInt32(u8buff, &x))
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
					IO::PackageFile *xPkg = levPkg->GetItemPack(i);
					if (xPkg)
					{
						j = xPkg->GetCount();
						while (j-- > 0)
						{
							xPkg->GetItemName(u8buff, j);
							if (xPkg->GetItemType(j) == IO::PackageFile::POT_STREAMDATA)
							{
								k = Text::StrIndexOf(u8buff, (const UTF8Char*)".png");
								if (k != INVALID_INDEX)
								{
									u8buff[k] = 0;
									if (Text::StrToInt32(u8buff, &y))
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
						DEL_CLASS(xPkg);
					}
				}
			}
		}
		
		DEL_CLASS(levPkg);
		if (found)
		{
			*minX = thisMinX;
			*maxX = thisMaxX;
			*minY = thisMinY;
			*maxY = thisMaxY;
		}
	}
	return found;
}
