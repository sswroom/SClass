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

IO::FileExporter::SupportType Exporter::SPKExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	else if (pobj->GetParserType() == IO::ParserType::MapLayer)
	{
		NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
		Map::MapDrawLayer::ObjectClass oc = layer->GetObjectClass();
		if (oc == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
		{
			NN<Map::TileMapLayer> tileMapLayer = NN<Map::TileMapLayer>::ConvertFrom(layer);
			NN<Map::TileMap> tileMap = tileMapLayer->GetTileMap();
			if (tileMap->GetTileType() == Map::TileMap::TileType::OSM)
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

Bool Exporter::SPKExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
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

Bool Exporter::SPKExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		NN<IO::PackageFile> pkgFile = NN<IO::PackageFile>::ConvertFrom(pobj);
		UTF8Char sbuff[512];
		IO::SPackageFile spkg(stm, false);
		ExportPackageFile(spkg, pkgFile, sbuff, sbuff);
		return true;
	}
	else if (pobj->GetParserType() == IO::ParserType::MapLayer)
	{
		NN<Map::MapDrawLayer> layer = NN<Map::MapDrawLayer>::ConvertFrom(pobj);
		Map::MapDrawLayer::ObjectClass oc = layer->GetObjectClass();
		if (oc == Map::MapDrawLayer::OC_TILE_MAP_LAYER)
		{
			NN<Map::TileMapLayer> tileMapLayer = NN<Map::TileMapLayer>::ConvertFrom(layer);
			NN<Map::TileMap> tileMap = tileMapLayer->GetTileMap();
			if (tileMap->GetTileType() == Map::TileMap::TileType::OSM)
			{
				NN<Map::OSM::OSMTileMap> osm = NN<Map::OSM::OSMTileMap>::ConvertFrom(tileMap);
				IO::SPackageFile *spkg;
				UInt8 *customBuff = MemAlloc(UInt8, 2048);
				UIntOS buffSize = 1;
				UIntOS i = 0;
				UIntOS bSize;
				NN<Text::String> url;
				while (true)
				{
					if (!osm->GetOSMURL(i).SetTo(url))
						break;

					bSize = url->leng;
					MemCopyNO(&customBuff[buffSize + 1], url->v.Ptr(), bSize);
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
			NN<Map::OruxDBLayer> orux = NN<Map::OruxDBLayer>::ConvertFrom(layer);
			Math::RectAreaDbl bounds;
			Optional<Map::NameArray> nameArr;
			Data::ArrayListInt64 objIds;
			Int32 xAdd;
			Int32 yAdd;
			Int32 tileX;
			Int32 tileY;
			UTF8Char sbuff[256];
			UnsafeArray<UTF8Char> sptr;
			UnsafeArray<const UInt8> fileBuff;
			UIntOS fileSize;
			Int64 modTimeTicks;

			UInt32 i;
			UInt32 j;
			UIntOS k;
			NEW_CLASS(spkg, IO::SPackageFile(stm, false));
			IO::MemoryStream mstm;
			i = 0;
			j = 18;
			while (i < j)
			{
				orux->SetCurrLayer(i);
				orux->GetBounds(bounds);
				xAdd = Map::OSM::OSMTileMap::Lon2TileXR(bounds.min.x, i);
				yAdd = Map::OSM::OSMTileMap::Lat2TileYR(bounds.max.y, i);

				objIds.Clear();
				orux->GetAllObjectIds(objIds, nameArr);
				k = objIds.GetCount();
				while (k-- > 0)
				{
					mstm.Clear();
					if (orux->GetObjectData(objIds.GetItem(k), mstm, tileX, tileY, modTimeTicks))
					{
						sptr = Text::StrUInt32(sbuff, i);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, tileX + xAdd);
						*sptr++ = IO::Path::PATH_SEPERATOR;
						sptr = Text::StrInt32(sptr, tileY + yAdd);
						sptr = Text::StrConcatC(sptr, UTF8STRC(".png"));
						fileBuff = mstm.GetBuff(fileSize);
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

void Exporter::SPKExporter::ExportPackageFile(NN<IO::SPackageFile> spkg, NN<IO::PackageFile> pkgFile, UnsafeArray<UTF8Char> buff, UnsafeArray<UTF8Char> buffEnd)
{
	UIntOS i;
	UIntOS j;
	UnsafeArray<UTF8Char> sptr;
	NN<IO::StreamData> fd;
	IO::PackageFile::PackObjectType pot;
	NN<IO::PackageFile> subPkg;
	i = 0;
	j = pkgFile->GetCount();
	while (i < j)
	{
		pot = pkgFile->GetItemType(i);
		if (pot == IO::PackageFile::PackObjectType::PackageFileType)
		{
			sptr = pkgFile->GetItemName(buffEnd, i).Or(buffEnd);
			*sptr++ = IO::Path::PATH_SEPERATOR;
			
			Bool subNeedDelete;
			if (pkgFile->GetItemPack(i, subNeedDelete).SetTo(subPkg))
			{
				ExportPackageFile(spkg, subPkg, buff, sptr);
				if (subNeedDelete)
					subPkg.Delete();
			}
		}
		else if (pot == IO::PackageFile::PackObjectType::StreamData)
		{
			if (pkgFile->GetItemName(buffEnd, i).SetTo(sptr) && pkgFile->GetItemStmDataNew(i).SetTo(fd))
			{
				spkg->AddFile(fd, {buff, (UIntOS)(sptr - buff)}, pkgFile->GetItemModTime(i));
				fd.Delete();
			}
		}
		i++;
	}
}

