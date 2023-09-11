#include "Stdafx.h"
#include "Exporter/SPKExporter.h"
#include "IO/Path.h"
#include "Map/MapDrawLayer.h"
#include "Map/OruxDBLayer.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

Exporter::SPKExporter::SPKExporter()
{
}

Exporter::SPKExporter::~SPKExporter()
{
}

Int32 Exporter::SPKExporter::GetName()
{
	return *(Int32*)"SPKE";
}

IO::FileExporter::SupportType Exporter::SPKExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (pobj->GetParserType() == IO::ParserType::MapLayer)
	{
		Map::MapDrawLayer *layer = (Map::MapDrawLayer*)pobj;
		Map::MapDrawLayer::ObjectClass oc = layer->GetObjectClass();
		if (oc == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
		{
			Map::TileMapLayer *tileMapLayer = (Map::TileMapLayer*)layer;
			Map::TileMap *tileMap = tileMapLayer->GetTileMap();
			if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
			{
				return IO::FileExporter::SupportType::NormalStream;
			}
		}
		else if (oc == Map::MapDrawLayer::OC_ORUX_DB_LAYER)
		{
			return IO::FileExporter::SupportType::NormalStream;
		}
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::SPKExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("SPackage File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.spk"));
		return true;
	}
	return false;
}

void Exporter::SPKExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::SPKExporter::ExportFile(NotNullPtr<IO::SeekableStream> stm, Text::CStringNN fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		IO::PackageFile *pkgFile = (IO::PackageFile *)pobj;
		UTF8Char sbuff[512];
		IO::SPackageFile *spkg;
		NEW_CLASS(spkg, IO::SPackageFile(stm, false));
		ExportPackageFile(spkg, pkgFile, sbuff, sbuff);
		DEL_CLASS(spkg);
		return true;
	}
	else if (pobj->GetParserType() == IO::ParserType::MapLayer)
	{
		Map::MapDrawLayer *layer = (Map::MapDrawLayer*)pobj;
		Map::MapDrawLayer::ObjectClass oc = layer->GetObjectClass();
		if (oc == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
		{
			Map::TileMapLayer *tileMapLayer = (Map::TileMapLayer*)layer;
			Map::TileMap *tileMap = tileMapLayer->GetTileMap();
			if (tileMap->GetTileType() == Map::TileMap::TT_OSM)
			{
				Map::OSM::OSMTileMap *osm = (Map::OSM::OSMTileMap*)tileMap;
				IO::SPackageFile *spkg;
				UInt8 *customBuff = MemAlloc(UInt8, 2048);
				UOSInt buffSize = 1;
				UOSInt i = 0;
				UOSInt bSize;
				Text::String *url;
				while (true)
				{
					url = osm->GetOSMURL(i);
					if (url == 0)
						break;

					bSize = url->leng;
					MemCopyNO(&customBuff[buffSize + 1], url->v, bSize);
					customBuff[buffSize] = (UInt8)bSize;
					buffSize += bSize + 1;
					i++;
				}
				customBuff[0] = (UInt8)i;

				NEW_CLASS(spkg, IO::SPackageFile(stm, false, 1, buffSize, Data::ByteArray(customBuff, 2048)));
				DEL_CLASS(spkg);

				MemFree(customBuff);
				return true;
			}
		}
		else if (oc == Map::MapDrawLayer::OC_ORUX_DB_LAYER)
		{
			IO::SPackageFile *spkg;
			Map::OruxDBLayer *orux = (Map::OruxDBLayer*)layer;
			Math::RectAreaDbl bounds;
			Map::NameArray *nameArr;
			Data::ArrayListInt64 objIds;
			Int32 xAdd;
			Int32 yAdd;
			Int32 tileX;
			Int32 tileY;
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			const UInt8 *fileBuff;
			UOSInt fileSize;
			Int64 modTimeTicks;

			UInt32 i;
			UInt32 j;
			UOSInt k;
			NEW_CLASS(spkg, IO::SPackageFile(stm, false));
			IO::MemoryStream mstm;
			i = 0;
			j = 18;
			while (i < j)
			{
				orux->SetCurrLayer(i);
				orux->GetBounds(bounds);
				xAdd = Map::OSM::OSMTileMap::Lon2TileXR(bounds.tl.x, i);
				yAdd = Map::OSM::OSMTileMap::Lat2TileYR(bounds.br.y, i);

				objIds.Clear();
				orux->GetAllObjectIds(objIds, &nameArr);
				k = objIds.GetCount();
				while (k-- > 0)
				{
					mstm.Clear();
					if (orux->GetObjectData(objIds.GetItem(k), &mstm, &tileX, &tileY, &modTimeTicks))
					{
						sptr = Text::StrUInt32(sbuff, i);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, tileX + xAdd);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, tileY + yAdd);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
						fileBuff = mstm.GetBuff(&fileSize);
						spkg->AddFile(fileBuff, fileSize, CSTRP(sbuff, sptr), Data::Timestamp(modTimeTicks, 0));
					}
				}
				orux->ReleaseNameArr(nameArr);

				i++;
			}
			DEL_CLASS(spkg);
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
}

void Exporter::SPKExporter::ExportPackageFile(IO::SPackageFile *spkg, IO::PackageFile *pkgFile, UTF8Char *buff, UTF8Char *buffEnd)
{
	UOSInt i;
	UOSInt j;
	UTF8Char *sptr;
	NotNullPtr<IO::StreamData> fd;
	IO::PackageFile::PackObjectType pot;
	IO::PackageFile *subPkg;
	i = 0;
	j = pkgFile->GetCount();
	while (i < j)
	{
		pot = pkgFile->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			sptr = pkgFile->GetItemName(buffEnd, i);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			
			subPkg = pkgFile->GetItemPackNew(i);
			if (subPkg)
			{
				ExportPackageFile(spkg, subPkg, buff, sptr);
				DEL_CLASS(subPkg);
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			sptr = pkgFile->GetItemName(buffEnd, i);
			if (fd.Set(pkgFile->GetItemStmDataNew(i)))
			{
				spkg->AddFile(fd, {buff, (UOSInt)(sptr - buff)}, pkgFile->GetItemModTime(i));
				fd.Delete();
			}
		}
		i++;
	}
}

