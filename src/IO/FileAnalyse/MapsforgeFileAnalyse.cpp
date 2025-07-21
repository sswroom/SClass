#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/FileAnalyse/MapsforgeFileAnalyse.h"
#include "Map/OSM/OSMTileMap.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/XLSUtil.h"

// https://github.com/mapsforge/mapsforge/blob/master/docs/Specification-Binary-Map-File.md
void __stdcall IO::FileAnalyse::MapsforgeFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::MapsforgeFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::MapsforgeFileAnalyse>();
	UInt8 readBuff[256];
	UOSInt readSize;
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	NN<IO::StreamData> fd;
	UOSInt ofst;
	UOSInt nextOfst;
	UInt64 v;
	UOSInt cnt;
	UOSInt i;
	if (me->fd.SetTo(fd))
	{
		readSize = fd->GetRealData(0, 256, BYTEARR(readBuff));
		if (readSize != 256 || !Text::StrStartsWithC(readBuff, 256, UTF8STRC("mapsforge binary OSM")))
		{
			return;
		}
		UInt32 headerSize = ReadMUInt32(&readBuff[20]);
		UInt64 fileSize = ReadMUInt64(&readBuff[28]);
		if (fileSize == fd->GetDataSize() && headerSize >= 73 && headerSize < fileSize)
		{
			pack = MemAllocNN(PackInfo);
			pack->fileOfst = 0;
			pack->packSize = headerSize + 24;
			pack->packType = PackType::FileHeader;
			pack->baseZoomLevel = 0;
			pack->minZoomLevel = 0;
			pack->maxZoomLevel = 0;
			pack->tileX = 0;
			pack->tileY = 0;
			me->packs.Add(pack);

			UnsafeArray<UInt8> header = MemAllocArr(UInt8, headerSize + 24);
			if (fd->GetRealData(0, headerSize + 24, Data::ByteArray(header, headerSize + 24)) != headerSize + 24)
			{
				MemFreeArr(header);
				return;
			}
			me->minLat = ReadMInt32(&header[44]) * 0.000001;
			me->minLon = ReadMInt32(&header[48]) * 0.000001;
			me->maxLat = ReadMInt32(&header[52]) * 0.000001;
			me->maxLon = ReadMInt32(&header[56]) * 0.000001;
			me->tileSize = ReadMUInt16(&header[60]);

			ofst = 62;
			nextOfst = ReadVBEU(header, ofst, v);
			ofst = nextOfst + (UOSInt)v;
			me->flags = header[ofst];
			ofst++;
			if (me->flags & 0x40)
			{
				ofst += 8;
			}
			if (me->flags & 0x20)
			{
				ofst++;
			}
			if (me->flags & 0x10)
			{
				nextOfst = ReadVBEU(header, ofst, v);
				ofst = nextOfst + (UOSInt)v;
			}
			if (me->flags & 0x8)
			{
				nextOfst = ReadVBEU(header, ofst, v);
				ofst = nextOfst + (UOSInt)v;
			}
			if (me->flags & 0x4)
			{
				nextOfst = ReadVBEU(header, ofst, v);
				ofst = nextOfst + (UOSInt)v;
			}
			cnt = ReadMUInt16(&header[ofst]);
			ofst += 2;
			i = 0;
			while (i < cnt)
			{
				nextOfst = ReadVBEU(header, ofst, v);
				me->poiTags.Add(Text::String::New(&header[nextOfst], (UOSInt)v));
				ofst = nextOfst + (UOSInt)v;
				i++;
			}

			cnt = ReadMUInt16(&header[ofst]);
			ofst += 2;
			i = 0;
			while (i < cnt)
			{
				nextOfst = ReadVBEU(header, ofst, v);
				me->wayTags.Add(Text::String::New(&header[nextOfst], (UOSInt)v));
				ofst = nextOfst + (UOSInt)v;
				i++;
			}

			cnt = header[ofst];
			ofst++;
			i = 0;
			while (i < cnt)
			{
				UInt8 baseZoomLevel = header[ofst];
				UInt8 minZoomLevel = header[ofst + 1];
				UInt8 maxZoomLevel = header[ofst + 2];
				UInt64 fileOfst = ReadMUInt64(&header[ofst + 3]);
//				UInt64 subfileSize = ReadMUInt64(&header[ofst + 11]);
				Int32 x1 = Map::OSM::OSMTileMap::Lon2TileX(me->minLon, baseZoomLevel);
				Int32 y1 = Map::OSM::OSMTileMap::Lat2TileY(me->maxLat, baseZoomLevel);
				Int32 x2 = Map::OSM::OSMTileMap::Lon2TileX(me->maxLon, baseZoomLevel);
				Int32 y2 = Map::OSM::OSMTileMap::Lat2TileY(me->minLat, baseZoomLevel);
				pack = MemAllocNN(PackInfo);
				pack->fileOfst = fileOfst;
				if (me->flags & 0x80)
				{
					pack->packSize = 16 + (UInt32)(5 * (x2 - x1 + 1) * (y2 - y1 + 1));
				}
				else
				{
					pack->packSize = (UInt32)(5 * (x2 - x1 + 1) * (y2 - y1 + 1));
				}
				pack->packType = PackType::TileIndex;
				pack->baseZoomLevel = baseZoomLevel;
				pack->minZoomLevel = minZoomLevel;
				pack->maxZoomLevel = maxZoomLevel;
				pack->tileX = 0;
				pack->tileY = 0;
				me->packs.Add(pack);

				UnsafeArray<UInt8> tileIndex = MemAllocArr(UInt8, pack->packSize);
				UOSInt tileIndexSize;
				if (me->flags & 0x80)
				{
					tileIndexSize = fd->GetRealData(pack->fileOfst + 16, pack->packSize - 16, Data::ByteArray(tileIndex, pack->packSize - 16));
				}
				else
				{
					tileIndexSize = fd->GetRealData(pack->fileOfst, pack->packSize, Data::ByteArray(tileIndex, pack->packSize));
				}
				if (tileIndexSize == (UOSInt)(5 * (x2 - x1 + 1) * (y2 - y1 + 1)))
				{
					UOSInt tileIndexOfst = 5;
					UInt64 lastOfst = ReadMUInt32(&tileIndex[1]) | ((UInt64)(tileIndex[0] & 0x7f) << 32);
					UInt64 thisOfst;
					Int32 x;
					Int32 y = y1;
					while (y <= y2)
					{
						x = x1;
						while (x <= x2)
						{
							if (tileIndexOfst < tileIndexSize)
							{
								thisOfst = ReadMUInt32(&tileIndex[tileIndexOfst + 1]) | ((UInt64)(tileIndex[tileIndexOfst] & 0x7f) << 32);
							}
							else
							{
								thisOfst = pack->packSize;
							}
							if (thisOfst != lastOfst)
							{
								pack = MemAllocNN(PackInfo);
								pack->fileOfst = fileOfst + lastOfst;
								pack->packSize = (UOSInt)(thisOfst - lastOfst);
								pack->packType = PackType::TileData;
								pack->baseZoomLevel = baseZoomLevel;
								pack->minZoomLevel = minZoomLevel;
								pack->maxZoomLevel = maxZoomLevel;
								pack->tileX = x;
								pack->tileY = y;
								me->packs.Add(pack);
							}

							lastOfst = thisOfst;
							tileIndexOfst += 5;
							x++;
						}
						y++;
					}
				}
				MemFreeArr(tileIndex);

				ofst += 19;
				i++;
			}

			MemFreeArr(header);
		}
	}
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::ReadVBEU(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<UInt64> v)
{
	UInt64 tmpV = 0;
	UOSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = buff[ofst];
		ofst++;
		tmpV = tmpV | ((UInt64)(b & 0x7f) << sh);
		if ((b & 0x80) == 0)
			break;
		sh += 7;
	}
	v.Set(tmpV);
	return ofst;
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::ReadVBES(UnsafeArray<UInt8> buff, UOSInt ofst, OutParam<Int64> v)
{
	Int64 tmpV = 0;
	UOSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = buff[ofst];
		ofst++;
		if ((b & 0x80) == 0)
		{
			tmpV = tmpV | ((Int64)(b & 0x3f) << sh);
			if (b & 0x40)
				tmpV = -tmpV;
			break;
		}
		else
		{
			tmpV = tmpV | ((Int64)(b & 0x7f) << sh);
		}
		sh += 7;
	}
	v.Set(tmpV);
	return ofst;
}

IO::FileAnalyse::MapsforgeFileAnalyse::MapsforgeFileAnalyse(NN<IO::StreamData> fd) : thread(ParseThread, this, CSTR("MapsforgeFileAnalyse"))
{
	UInt8 buff[256];
	this->fd = 0;
	this->pauseParsing = false;
	fd->GetRealData(0, 256, BYTEARR(buff));
	if (!Text::StrStartsWithC(buff, 256, UTF8STRC("mapsforge binary OSM")))
	{
		return;
	}
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->thread.Start();
}

IO::FileAnalyse::MapsforgeFileAnalyse::~MapsforgeFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->packs.MemFreeAll();
	this->poiTags.FreeAll();
	this->wayTags.FreeAll();
}

Text::CStringNN IO::FileAnalyse::MapsforgeFileAnalyse::GetFormatName()
{
	return CSTR("MFO");
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameCount()
{
	return this->packs.GetCount();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	if (!this->packs.GetItem(index).SetTo(pack))
		return false;
	sb->AppendU64(pack->fileOfst);
	sb->AppendC(UTF8STRC(": Num="));
	sb->AppendUOSInt(index);
	sb->AppendC(UTF8STRC(", Type="));
	switch (pack->packType)
	{
	case PackType::FileHeader:
		sb->AppendC(UTF8STRC("File Header"));
		break;
	case PackType::TileIndex:
		sb->AppendC(UTF8STRC("Tile Index"));
		break;
	case PackType::TileData:
		sb->AppendC(UTF8STRC("Tile Data"));
		break;
	}
	return true;
}

UOSInt IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	OSInt i = 0;
	OSInt j = (OSInt)this->packs.GetCount() - 1;
	OSInt k;
	NN<PackInfo> pack;
	while (i <= j)
	{
		k = (i + j) >> 1;
		pack = this->packs.GetItemNoCheck((UOSInt)k);
		if (ofst < pack->fileOfst)
		{
			j = k - 1;
		}
		else if (ofst >= pack->fileOfst + pack->packSize)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::MapsforgeFileAnalyse::GetFrameDetail(UOSInt index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;
	NN<IO::FileAnalyse::MapsforgeFileAnalyse::PackInfo> pack;
	UnsafeArray<UInt8> packBuff;
	NN<IO::StreamData> fd;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt ofst;
	UOSInt nextOfst;
	UInt64 v;
	Int64 iv;
	UInt8 flags;
	UOSInt cnt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	if (!this->packs.GetItem(index).SetTo(pack) || !this->fd.SetTo(fd))
		return 0;

	packBuff = MemAllocArr(UInt8, pack->packSize);
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(pack->fileOfst, pack->packSize));
	fd->GetRealData(pack->fileOfst, pack->packSize, Data::ByteArray(packBuff, pack->packSize));
	switch (pack->packType)
	{
	case PackType::FileHeader:
		frame->AddField(0, 20, CSTR("Magic byte"), Text::CStringNN(packBuff, 20));
		frame->AddUInt(20, 4, CSTR("Header size"), ReadMUInt32(&packBuff[20]));
		frame->AddUInt(24, 4, CSTR("File version"), ReadMUInt32(&packBuff[24]));
		frame->AddUInt64(28, CSTR("File size"), ReadMUInt64(&packBuff[28]));
		sptr = Data::Timestamp(ReadMInt64(&packBuff[36]), Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff);
		frame->AddUInt64Name(36, 8, CSTR("Date of creation"), ReadMUInt64(&packBuff[36]), CSTRP(sbuff, sptr));
		frame->AddFloat(44, 4, CSTR("Min Lat"), ReadMInt32(&packBuff[44]) * 0.000001);
		frame->AddFloat(48, 4, CSTR("Min Lon"), ReadMInt32(&packBuff[48]) * 0.000001);
		frame->AddFloat(52, 4, CSTR("Max Lat"), ReadMInt32(&packBuff[52]) * 0.000001);
		frame->AddFloat(56, 4, CSTR("Max Lon"), ReadMInt32(&packBuff[56]) * 0.000001);
		frame->AddUInt(60, 2, CSTR("Tile size"), ReadMUInt16(&packBuff[60]));
		ofst = 62;
		nextOfst = ReadVBEU(packBuff, ofst, v);
		frame->AddField(ofst, nextOfst + v - ofst, CSTR("Projection"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
		ofst = nextOfst + (UOSInt)v;
		flags = packBuff[ofst];
		frame->AddBit(ofst, CSTR("existence of debug information"), flags, 7);
		frame->AddBit(ofst, CSTR("existence of the map start position field"), flags, 6);
		frame->AddBit(ofst, CSTR("existence of the start zoom level field"), flags, 5);
		frame->AddBit(ofst, CSTR("existence of the language(s) preference field"), flags, 4);
		frame->AddBit(ofst, CSTR("existence of the comment field"), flags, 3);
		frame->AddBit(ofst, CSTR("existence of the created by field"), flags, 2);
		frame->AddBit(ofst, CSTR("reserved for future use"), flags, 1);
		frame->AddBit(ofst, CSTR("reserved for future use"), flags, 0);
		ofst++;
		if (flags & 0x40)
		{
			frame->AddFloat(ofst, 4, CSTR("Map Start Lat"), ReadMInt32(&packBuff[ofst]) * 0.000001);
			frame->AddFloat(ofst + 4, 4, CSTR("Map Start Lon"), ReadMInt32(&packBuff[ofst + 4]) * 0.000001);
			ofst += 8;
		}
		if (flags & 0x20)
		{
			frame->AddUInt(ofst, 1, CSTR("Start zoom level"), packBuff[ofst]);
			ofst++;
		}
		if (flags & 0x10)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Language(s) preference"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		if (flags & 0x8)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Comment"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		if (flags & 0x4)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Created by"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
		}
		cnt = ReadMUInt16(&packBuff[ofst]);
		frame->AddUInt(ofst, 2, CSTR("POI tags count"), cnt);
		ofst += 2;
		i = 0;
		while (i < cnt)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			sptr = Text::StrUOSInt(sbuff, i);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("POI tag ID"), CSTRP(sbuff, sptr));
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("POI tag"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
			i++;
		}

		cnt = ReadMUInt16(&packBuff[ofst]);
		frame->AddUInt(ofst, 2, CSTR("Way tags count"), cnt);
		ofst += 2;
		i = 0;
		while (i < cnt)
		{
			nextOfst = ReadVBEU(packBuff, ofst, v);
			sptr = Text::StrUOSInt(sbuff, i);
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Way tag ID"), CSTRP(sbuff, sptr));
			frame->AddField(ofst, nextOfst + v - ofst, CSTR("Way tag"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
			ofst = nextOfst + (UOSInt)v;
			i++;
		}

		cnt = packBuff[ofst];
		frame->AddUInt(ofst, 1, CSTR("Amount of zoom intervals"), cnt);
		ofst++;
		i = 0;
		while (i < cnt)
		{
			frame->AddUInt(ofst, 1, CSTR("Base zoom level"), packBuff[ofst]);
			frame->AddUInt(ofst + 1, 1, CSTR("Minimal zoom level"), packBuff[ofst + 1]);
			frame->AddUInt(ofst + 2, 1, CSTR("Maximal zoom level"), packBuff[ofst + 2]);
			frame->AddUInt64(ofst + 3, CSTR("Absolute start position of subfile"), ReadMUInt64(&packBuff[ofst + 3]));
			frame->AddUInt64(ofst + 11, CSTR("Size of subfile"), ReadMUInt64(&packBuff[ofst + 11]));
			ofst += 19;
			i++;
		}

		break;
	case PackType::TileIndex:
		{
			ofst = 0;
			Text::StringBuilderUTF8 sb;
			UInt64 v;
			Int32 x;
			Int32 y;
			Int32 x1 = Map::OSM::OSMTileMap::Lon2TileX(this->minLon, pack->baseZoomLevel);
			Int32 y1 = Map::OSM::OSMTileMap::Lat2TileY(this->maxLat, pack->baseZoomLevel);
			Int32 x2 = Map::OSM::OSMTileMap::Lon2TileX(this->maxLon, pack->baseZoomLevel);
			Int32 y2 = Map::OSM::OSMTileMap::Lat2TileY(this->minLat, pack->baseZoomLevel);
			y = y1;
			while (ofst < pack->packSize && y <= y2)
			{
				x = x1;
				while (ofst < pack->packSize && y <= x2)
				{
					v = ReadMUInt32(&packBuff[ofst + 1]);
					v = v | ((UInt64)(packBuff[ofst] & 0x7f) << 32);
					sb.ClearStr();
					sb.Append(CSTR("Tile x="))->AppendI32(x)->Append(CSTR(", y="))->AppendI32(y)->Append(CSTR(", z="))->AppendU16(pack->baseZoomLevel);
					frame->AddField(ofst, 5, sb.ToCString(), 0);
					frame->AddUInt(ofst, 5, CSTR("Offset"), v);
					frame->AddBit(ofst, CSTR("All water"), packBuff[ofst], 7);
					ofst += 5;
					x++;
				}
				y++;
			}
		}
		break;
	case PackType::TileData:
		{
			Text::StringBuilderUTF8 sb;
			ofst = 0;
			if (this->flags & 0x80)
			{
				frame->AddStrC(0, 32, CSTR("Tile Signature"), packBuff);
				ofst += 32;
			}
			UOSInt nPOI[16];
			UOSInt nWay[16];
			UInt8 z = pack->minZoomLevel;
			while (z <= pack->maxZoomLevel)
			{
				sb.ClearStr();
				sb.Append(CSTR("Level "));
				sb.AppendU16(z);
				sb.Append(CSTR(" Number of POIs"));
				nextOfst = ReadVBEU(packBuff, ofst, v);
				frame->AddUInt(ofst, (UOSInt)(nextOfst - ofst), sb.ToCString(), v);
				ofst = nextOfst;
				nPOI[z] = (UOSInt)v;
				sb.ClearStr();
				sb.Append(CSTR("Level "));
				sb.AppendU16(z);
				sb.Append(CSTR(" Number of ways"));
				nextOfst = ReadVBEU(packBuff, ofst, v);
				frame->AddUInt(ofst, (UOSInt)(nextOfst - ofst), sb.ToCString(), v);
				ofst = nextOfst;
				nWay[z] = (UOSInt)v;
				z++;
			}
			nextOfst = ReadVBEU(packBuff, ofst, v);
			frame->AddUInt(ofst, (UOSInt)(nextOfst - ofst), CSTR("First way offset"), v);
			ofst = nextOfst;

			z = pack->minZoomLevel;
			while (z <= pack->maxZoomLevel)
			{
				UOSInt j = 0;
				while (j < nPOI[z])
				{
					if (this->flags & 0x80)
					{
						frame->AddStrC(ofst, 32, CSTR("POI Signature"), packBuff);
						ofst += 32;
					}
					nextOfst = ReadVBES(packBuff, ofst, iv);
					frame->AddInt(ofst, (UOSInt)(nextOfst - ofst), CSTR("Lat Diff"), iv);
					frame->AddFloat(ofst, (UOSInt)(nextOfst - ofst), CSTR("Latitude"), Map::OSM::OSMTileMap::TileY2Lat(pack->tileY, pack->baseZoomLevel) + (Double)iv * 0.000001);
					ofst = nextOfst;
					nextOfst = ReadVBES(packBuff, ofst, iv);
					frame->AddInt(ofst, (UOSInt)(nextOfst - ofst), CSTR("Lon Diff"), iv);
					frame->AddFloat(ofst, (UOSInt)(nextOfst - ofst), CSTR("Longitude"), Map::OSM::OSMTileMap::TileX2Lon(pack->tileX, pack->baseZoomLevel) + (Double)iv * 0.000001);
					ofst = nextOfst;
					frame->AddUInt(ofst, 1, CSTR("Layer"), packBuff[ofst] >> 4);
					frame->AddUInt(ofst, 1, CSTR("Amount of tags for the POI"), packBuff[ofst] & 15);
					cnt = packBuff[ofst] & 15;
					ofst++;
					i = 0;
					while (i < cnt)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddUIntName(ofst, (UOSInt)(nextOfst - ofst), CSTR("POI Tag"), v, OPTSTR_CSTR(this->poiTags.GetItem((UOSInt)v)));
						ofst = nextOfst;
						i++;
					}
					UInt8 flags = packBuff[ofst];
					frame->AddBit(ofst, CSTR("Existence of a POI name"), flags, 7);
					frame->AddBit(ofst, CSTR("Existence of a house number"), flags, 6);
					frame->AddBit(ofst, CSTR("Existence of an elevation"), flags, 5);
					ofst++;
					if (flags & 0x80)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddField(ofst, nextOfst + v - ofst, CSTR("Name"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
						ofst = nextOfst + (UOSInt)v;
					}
					if (flags & 0x40)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddField(ofst, nextOfst + v - ofst, CSTR("House number"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
						ofst = nextOfst + (UOSInt)v;
					}
					if (flags & 0x20)
					{
						nextOfst = ReadVBES(packBuff, ofst, iv);
						frame->AddInt(ofst, nextOfst - ofst, CSTR("Elevation"), iv);
						ofst = nextOfst;
					}
					j++;
				}
				z++;
			}

			z = pack->minZoomLevel;
			while (z <= pack->maxZoomLevel)
			{
				UInt64 startOfst;
				UOSInt i2 = 0;
				while (i2 < nWay[z])
				{
					startOfst = ofst;
					if (this->flags & 0x80)
					{
						frame->AddStrC(ofst, 32, CSTR("Way Signature"), &packBuff[ofst]);
						ofst += 32;
					}
					nextOfst = ReadVBEU(packBuff, ofst, v);
					frame->AddUInt(ofst, nextOfst - ofst, CSTR("Way data size"), v);
					ofst = nextOfst;
					UInt64 wayEndOfst = ofst + v;
					frame->AddUInt(ofst, 2, CSTR("Sub tile bitmap"), ReadMUInt16(&packBuff[ofst]));
					frame->AddUInt(ofst + 2, 1, CSTR("Layer"), packBuff[ofst + 2] >> 4);
					cnt = packBuff[ofst + 2] & 15;
					frame->AddUInt(ofst + 2, 1, CSTR("Amount of tags for the way"), cnt);
					ofst += 3;
					i = 0;
					while (i < cnt)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddUIntName(ofst, (UOSInt)(nextOfst - ofst), CSTR("Way Tag"), v, OPTSTR_CSTR(this->wayTags.GetItem((UOSInt)v)));
						ofst = nextOfst;
						i++;
					}
					UInt8 wayFlags = packBuff[ofst];
					frame->AddBit(ofst, CSTR("Existence of a way name"), wayFlags, 7);
					frame->AddBit(ofst, CSTR("Existence of a house number"), wayFlags, 6);
					frame->AddBit(ofst, CSTR("Existence of a reference"), wayFlags, 5);
					frame->AddBit(ofst, CSTR("Existence of a label position"), wayFlags, 4);
					frame->AddBit(ofst, CSTR("Existence of number of way data blocks field"), wayFlags, 3);
					frame->AddBit(ofst, CSTR("Double delta encoding"), wayFlags, 2);
					ofst++;
					if (wayFlags & 0x80)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddField(ofst, nextOfst + v - ofst, CSTR("Name"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
						ofst = nextOfst + (UOSInt)v;
					}
					if (wayFlags & 0x40)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddField(ofst, nextOfst + v - ofst, CSTR("House number"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
						ofst = nextOfst + (UOSInt)v;
					}
					if (wayFlags & 0x20)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddField(ofst, nextOfst + v - ofst, CSTR("Reference"), Text::CStringNN(&packBuff[nextOfst], (UOSInt)v));
						ofst = nextOfst + (UOSInt)v;
					}
					if (wayFlags & 0x10)
					{
						nextOfst = ReadVBES(packBuff, ofst, iv);
						frame->AddInt(ofst, nextOfst - ofst, CSTR("Label position lat-diff"), iv);
						frame->AddFloat(ofst, nextOfst - ofst, CSTR("Label position latitude"), Map::OSM::OSMTileMap::TileY2Lat(pack->tileY, pack->baseZoomLevel) + (Double)iv * 0.000001);
						ofst = nextOfst;
						nextOfst = ReadVBES(packBuff, ofst, iv);
						frame->AddInt(ofst, nextOfst - ofst, CSTR("Label position lon-diff"), iv);
						frame->AddFloat(ofst, nextOfst - ofst, CSTR("Label position longitude"), Map::OSM::OSMTileMap::TileX2Lon(pack->tileX, pack->baseZoomLevel) + (Double)iv * 0.000001);
						ofst = nextOfst;
					}
					UOSInt nDataBlk;
					if (wayFlags & 0x8)
					{
						nextOfst = ReadVBEU(packBuff, ofst, v);
						frame->AddUInt(ofst, nextOfst - ofst, CSTR("Number of way data blocks"), v);
						nDataBlk = (UOSInt)v;
						ofst = nextOfst;
					}
					else
					{
						nDataBlk = 1;
					}
					i = 0;
					while (i < nDataBlk)
					{
						UInt64 nWayBlocks;
						nextOfst = ReadVBEU(packBuff, ofst, nWayBlocks);
						frame->AddUInt(ofst, nextOfst - ofst, CSTR("Number of way coordinate blocks"), nWayBlocks);
						ofst = nextOfst;
						j = 0;
						while (j < nWayBlocks)
						{
							UInt64 nNodes;
							nextOfst = ReadVBEU(packBuff, ofst, nNodes);
							frame->AddUInt(ofst, nextOfst - ofst, CSTR("Amount of way nodes of this way"), nNodes);
							ofst = nextOfst;
							if (wayFlags & 4)
							{
								Double lastLat = Map::OSM::OSMTileMap::TileY2Lat(pack->tileY, pack->baseZoomLevel);
								Double lastLon = Map::OSM::OSMTileMap::TileX2Lon(pack->tileX, pack->baseZoomLevel);
								Int64 lastLatOffset = 0;
								Int64 lastLonOffset = 0;
								k = 0;
								while (k < nNodes)
								{
									nextOfst = ReadVBES(packBuff, ofst, iv);
									frame->AddInt(ofst, nextOfst - ofst, CSTR("Way node lat-diff"), iv);
									if (k > 0)
									{
										lastLatOffset += iv;
										lastLat += (Double)lastLatOffset * 0.000001;
									}
									else
									{
										lastLat += (Double)iv * 0.000001;
									}
									frame->AddFloat(ofst, nextOfst - ofst, CSTR("Way node latitude"), lastLat);
									ofst = nextOfst;
									nextOfst = ReadVBES(packBuff, ofst, iv);
									frame->AddInt(ofst, nextOfst - ofst, CSTR("Way node lon-diff"), iv);
									if (k > 0)
									{
										lastLonOffset += iv;
										lastLon += (Double)iv * 0.000001;
									}
									else
									{
										lastLon += (Double)iv * 0.000001;
									}
									frame->AddFloat(ofst, nextOfst - ofst, CSTR("Way node longitude"), lastLon);
									ofst = nextOfst;
									
									if (ofst > wayEndOfst)
									{
										j = nWayBlocks;
										i = nDataBlk;
										break;
									}
									k++;
								}
							}
							else
							{
								Double lastLat = Map::OSM::OSMTileMap::TileY2Lat(pack->tileY, pack->baseZoomLevel);
								Double lastLon = Map::OSM::OSMTileMap::TileX2Lon(pack->tileX, pack->baseZoomLevel);
								k = 0;
								while (k < nNodes)
								{
									nextOfst = ReadVBES(packBuff, ofst, iv);
									frame->AddInt(ofst, nextOfst - ofst, CSTR("Way node lat-diff"), iv);
									lastLat += (Double)iv * 0.000001;
									frame->AddFloat(ofst, nextOfst - ofst, CSTR("Way node latitude"), lastLat);
									ofst = nextOfst;
									nextOfst = ReadVBES(packBuff, ofst, iv);
									frame->AddInt(ofst, nextOfst - ofst, CSTR("Way node lon-diff"), iv);
									lastLon += (Double)iv * 0.000001;
									frame->AddFloat(ofst, nextOfst - ofst, CSTR("Way node longitude"), lastLon);
									ofst = nextOfst;

									if (ofst > wayEndOfst)
									{
										j = nWayBlocks;
										i = nDataBlk;
										break;
									}
									k++;
								}
							}
							j++;
						}
						i++;
					}
					if (ofst != wayEndOfst)
					{
						printf("Way Offset mismatch: i2 = %d, z = %d, ofst = 0x%llx, endOfst = 0x%llx\r\n", (UInt32)i2, z, startOfst + pack->fileOfst, wayEndOfst + pack->fileOfst);
						ofst = wayEndOfst;
						//break;
					}
					else if (ofst >= pack->packSize)
					{
						printf("End of pack: i2 = %d, z = %d\r\n", (UInt32)i2, z);
						z = pack->maxZoomLevel + 1;
						break;
					}
					i2++;
				}
				z++;
			}
		}
		break;
	}
	MemFreeArr(packBuff);
	return frame;
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::MapsforgeFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
