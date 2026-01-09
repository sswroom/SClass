#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/MapsforgeUtil.h"
#include "Map/OSM/OSMData.h"
#include "Map/OSM/OSMTileMap.h"
#include "Parser/FileParser/MapsforgeParser.h"

Parser::FileParser::MapsforgeParser::MapsforgeParser()
{
}

Parser::FileParser::MapsforgeParser::~MapsforgeParser()
{
}

Int32 Parser::FileParser::MapsforgeParser::GetName()
{
	return *(Int32*)"MFOS";
}

void Parser::FileParser::MapsforgeParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.osm.map"), CSTR("Mapsforge Map File"));
	}
}

IO::ParserType Parser::FileParser::MapsforgeParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::MapsforgeParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!Text::StrStartsWithC(hdr.Arr(), 20, UTF8STRC("mapsforge binary OSM")) || ReadMUInt64(&hdr[28]) != fd->GetDataSize())
	{
		return 0;
	}

	NN<Map::OSM::OSMData> osmData;
	NEW_CLASSNN(osmData, Map::OSM::OSMData(fd->GetFullFileName()->ToCString()));

	UInt32 hdrSize = ReadMUInt32(&hdr[20]);
	UnsafeArray<UInt8> hdrBuff = MemAllocArr(UInt8, hdrSize);
	if (fd->GetRealData(24, hdrSize, Data::ByteArray(hdrBuff, hdrSize)) != hdrSize)
	{
		MemFreeArr(hdrBuff);
		osmData.Delete();
		return 0;
	}

	osmData->SetOSMBase(Data::Timestamp::FromEpochMS(ReadMInt64(&hdrBuff[12]), Data::DateTimeUtil::GetLocalTzQhr()));
	osmData->SetDataBounds(Math::RectAreaDbl(
		Math::Coord2DDbl(ReadMInt32(&hdrBuff[24]) * 0.000001, ReadMInt32(&hdrBuff[20]) * 0.000001),
		Math::Coord2DDbl(ReadMInt32(&hdrBuff[32]) * 0.000001, ReadMInt32(&hdrBuff[28]) * 0.000001)));
	UInt64 v;
	UOSInt ofst = 38;
	UOSInt nextOfst;
	nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
	ofst = nextOfst + (UOSInt)v;
	UInt8 flags = hdrBuff[ofst];
	ofst++;
	if (flags & 0x40)
	{
		ofst += 8;
	}
	if (flags & 0x20)
	{
		ofst++;
	}
	if (flags & 0x10)
	{
		nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
		ofst = nextOfst + (UOSInt)v;
	}
	if (flags & 0x8)
	{
		nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
		osmData->SetNote(Text::CStringNN(hdrBuff + nextOfst, (UOSInt)v));
		ofst = nextOfst + (UOSInt)v;
	}
	if (flags & 0x4)
	{
		nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
		ofst = nextOfst + (UOSInt)v;
	}
	UOSInt i;
	UOSInt nodeTagsCnt = ReadMUInt16(&hdrBuff[ofst]);
	UOSInt index;
	ofst += 2;
	UnsafeArray<TagInfo> nodeTags = MemAllocArr(TagInfo, nodeTagsCnt);
	i = 0;
	while (i < nodeTagsCnt)
	{
		nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
		index = Text::StrIndexOfCharC(hdrBuff + nextOfst, (UOSInt)v, '=');
		if (index == INVALID_INDEX)
		{
			nodeTags[i].k = Text::String::New(&hdrBuff[nextOfst], (UOSInt)v);
			nodeTags[i].v = Text::String::NewEmpty();
		}
		else
		{
			nodeTags[i].k = Text::String::New(&hdrBuff[nextOfst], index);
			nodeTags[i].v = Text::String::New(&hdrBuff[nextOfst + index + 1], (UOSInt)v - index - 1);
		}
		ofst = nextOfst + (UOSInt)v;
		i++;
	}
	UOSInt wayTagsCnt = ReadMUInt16(&hdrBuff[ofst]);
	ofst += 2;
	UnsafeArray<TagInfo> wayTags = MemAllocArr(TagInfo, wayTagsCnt);
	i = 0;
	while (i < wayTagsCnt)
	{
		nextOfst = Map::MapsforgeUtil::ReadVBEU(hdrBuff, ofst, v);
		index = Text::StrIndexOfCharC(hdrBuff + nextOfst, (UOSInt)v, '=');
		if (index == INVALID_INDEX)
		{
			wayTags[i].k = Text::String::New(&hdrBuff[nextOfst], (UOSInt)v);
			wayTags[i].v = Text::String::NewEmpty();
		}
		else
		{
			wayTags[i].k = Text::String::New(&hdrBuff[nextOfst], index);
			wayTags[i].v = Text::String::New(&hdrBuff[nextOfst + index + 1], (UOSInt)v - index - 1);
		}
		ofst = nextOfst + (UOSInt)v;
		i++;
	}
	UInt8 zoomIntCnt = hdrBuff[ofst];
	UOSInt nodeCnt = 0;
	UOSInt wayCnt = 0;
	ofst++;
	i = zoomIntCnt - 1; //0;
	ofst += 19 * i;
	while (i < zoomIntCnt)
	{
		ParseSubFile(fd, osmData, nodeTags, wayTags, hdrBuff[ofst], hdrBuff[ofst + 1], hdrBuff[ofst + 2], ReadMUInt64(&hdrBuff[ofst + 3]), ReadMUInt64(&hdrBuff[ofst + 11]), nodeCnt, wayCnt, nodeTagsCnt, wayTagsCnt);
		ofst += 19;
		i++;
	}

	i = 0;
	while (i < nodeTagsCnt)
	{
		nodeTags[i].k->Release();
		nodeTags[i].v->Release();
		i++;
	}
	MemFreeArr(nodeTags);
	i = 0;
	while (i < wayTagsCnt)
	{
		wayTags[i].k->Release();
		wayTags[i].v->Release();
		i++;
	}
	MemFreeArr(wayTags);
	MemFreeArr(hdrBuff);
	osmData->BuildIndex();
	return osmData;
}

void Parser::FileParser::MapsforgeParser::ParseSubFile(NN<IO::StreamData> fd, NN<Map::OSM::OSMData> osmData, UnsafeArray<TagInfo> nodeTags, UnsafeArray<TagInfo> wayTags, UInt8 baseZoomLevel, UInt8 minZoomLevel, UInt8 maxZoomLevel, UInt64 subfileOfst, UInt64 subfileSize, InOutParam<UOSInt> nodeCnt, InOutParam<UOSInt> wayCnt, UOSInt nodeTagsCnt, UOSInt wayTagsCnt)
{
	Math::RectAreaDbl bounds = osmData->GetDataBounds();
	UnsafeArray<UInt8> subfileBuff = MemAllocArr(UInt8, (UOSInt)subfileSize);
	if (fd->GetRealData(subfileOfst, subfileSize, Data::ByteArray(subfileBuff, (UOSInt)subfileSize)) != subfileSize)
	{
		printf("MapsforgeParser: Error reading subfile: %lld, %lld\r\n", subfileOfst, subfileSize);
		MemFreeArr(subfileBuff);
		return;
	}

	UOSInt currNodeCnt = nodeCnt.Get();
	UOSInt currWayCnt = wayCnt.Get();
	UOSInt ofst = 0;
	UInt64 v;
	Int64 sv;
	Int32 x;
	Int32 y;
	Int32 x1 = Map::OSM::OSMTileMap::Lon2TileX(bounds.min.GetLon(), baseZoomLevel);
	Int32 y1 = Map::OSM::OSMTileMap::Lat2TileY(bounds.max.GetLat(), baseZoomLevel);
	Int32 x2 = Map::OSM::OSMTileMap::Lon2TileX(bounds.max.GetLon(), baseZoomLevel);
	Int32 y2 = Map::OSM::OSMTileMap::Lat2TileY(bounds.min.GetLat(), baseZoomLevel);
	NN<Text::String> strHouseNum = Text::String::New(CSTR("addr:housenumber"));
	NN<Text::String> strEle = Text::String::New(CSTR("ele"));
	Text::PString sarr[2];
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Data::ArrayListNN<Map::OSM::NodeInfo> wayNodeList;
	UOSInt tagIds[16];
	y = y1;
	while (ofst < subfileSize && y <= y2)
	{
		x = x1;
		while (ofst < subfileSize && x <= x2)
		{
			v = ReadMUInt32(&subfileBuff[ofst + 1]);
			v = v | ((UInt64)(subfileBuff[ofst] & 0x7f) << 32);
			if (subfileBuff[ofst] & 0x80)
			{
			}
			else
			{
				Double tileLat = Map::OSM::OSMTileMap::TileY2Lat(y, baseZoomLevel);
				Double tileLon = Map::OSM::OSMTileMap::TileX2Lon(x, baseZoomLevel);
				UOSInt tileStartOfst;
				UOSInt tileOfst = (UOSInt)v;
				UOSInt totalNodeCnt = 0;
				UOSInt totalWayCnt = 0;
				UInt8 z = minZoomLevel;
				while (z <= maxZoomLevel)
				{
					tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
					totalNodeCnt += (UOSInt)v;
					tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
					totalWayCnt += (UOSInt)v;
					z++;
				}
				UInt64 firstWayOfst;
				tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, firstWayOfst);
				tileStartOfst = tileOfst;
				NN<Text::String> s1;
				NN<Text::String> s2;
				UOSInt i = 0;
				Double lat;
				Double lon;
				while (i < totalNodeCnt)
				{
					tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
					lat = tileLat + (Double)sv * 0.000001;
					tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
					lon = tileLon + (Double)sv * 0.000001;
					currNodeCnt++;
					NN<Map::OSM::NodeInfo> node = osmData->NewNode((Int64)currNodeCnt, lat, lon);
					UOSInt ntags = subfileBuff[tileOfst] & 15;
					tileOfst++;
					UOSInt j = 0;
					while (j < ntags)
					{
						tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
						if (v >= nodeTagsCnt)
						{
							printf("MapsforgeParser: Tile x=%d, y=%d, z=%d: Invalid node tag index: %llu at Ofst %llu\r\n", x, y, baseZoomLevel, v, tileOfst + subfileOfst);
						}
						osmData->ElementAddTag(node, nodeTags[(UOSInt)v].k, nodeTags[(UOSInt)v].v);
						j++;
					}
					UInt8 flags = subfileBuff[tileOfst];
					tileOfst++;
					if (flags & 0x80)
					{
						tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
						sb.ClearStr();
						sb.AppendC(&subfileBuff[tileOfst], (UOSInt)v);
						tileOfst += (UOSInt)v;
						sarr[1] = sb;
						UOSInt k;
						UOSInt l;
						while (true)
						{
							k = Text::StrSplitLineP(sarr, 2, sarr[1]);
							l = sarr[0].IndexOf(8);
							if (l == INVALID_INDEX)
							{
								s1 = Text::String::New(CSTR("name"));
								s2 = Text::String::New(sarr[0].ToCString());
								osmData->ElementAddTag(node, s1, s2);
								s1->Release();
								s2->Release();
							}
							else
							{
								sb2.ClearStr();
								sb2.Append(CSTR("name:"));
								sb2.AppendC(sarr[0].v, l);
								s1 = Text::String::New(sb2.ToCString());
								s2 = Text::String::New(sarr[0].Substring(l + 1).ToCString());
								osmData->ElementAddTag(node, s1, s2);
								s1->Release();
								s2->Release();
							}
							if (k != 2)
								break;
						}
					}
					if (flags & 0x40)
					{
						tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
						s2 = Text::String::New(&subfileBuff[tileOfst], (UOSInt)v);
						osmData->ElementAddTag(node, strHouseNum, s2);
						s2->Release();
						tileOfst += (UOSInt)v;
					}
					if (flags & 0x20)
					{
						tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
						sb.ClearStr();
						sb.AppendI64(sv);
						s2 = Text::String::New(sb.ToCString());
						osmData->ElementAddTag(node, strEle, s2);
						s2->Release();
					}
					i++;
				}
				if ((tileOfst - tileStartOfst) != firstWayOfst)
				{
					printf("MapsforgeParser: Tile x=%d, y=%d, z=%d: Node parse error, tileOfst=%llu, firstWayOfst=%llu, fileOfst=%llu\r\n", x, y, baseZoomLevel, tileOfst - tileStartOfst, firstWayOfst, tileOfst + subfileOfst);
				}
				else
				{
					i = 0;
					while (i < totalWayCnt)
					{
						UOSInt wayDataSize;
						UOSInt wayEndOfst;
						tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
						wayDataSize = (UOSInt)v;
						wayEndOfst = tileOfst + wayDataSize;
						tileOfst += 2;
						UOSInt tagCnt = subfileBuff[tileOfst] & 15;
						tileOfst++;
						UOSInt k;
						UOSInt l;
						UOSInt j = 0;
						while (j < tagCnt)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							if (v >= wayTagsCnt)
							{
								printf("MapsforgeParser: Tile x=%d, y=%d, z=%d: Invalid way tag index: %llu at Ofst %llu\r\n", x, y, baseZoomLevel, v, tileOfst + subfileOfst);
								tagIds[j] = INVALID_INDEX;
							}
							else
							{
								tagIds[j] = (UOSInt)v;
							}
							j++;
						}
						tagIds[j] = INVALID_INDEX;
						UOSInt ndataBlocks = 1;
						UOSInt flags = subfileBuff[tileOfst];
						UOSInt nameOfst = INVALID_INDEX;
						UOSInt nameLeng = 0;
						UOSInt houseNumOfst = INVALID_INDEX;
						UOSInt houseNumLeng = 0;
						Bool valid = true;
						tileOfst++;
						if (flags & 0x80)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							nameOfst = tileOfst;
							nameLeng = (UOSInt)v;
							tileOfst += (UOSInt)v;
						}
						if (flags & 0x40)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							houseNumOfst = tileOfst;
							houseNumLeng = (UOSInt)v;
							tileOfst += (UOSInt)v;
						}
						if (flags & 0x20)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							tileOfst += (UOSInt)v;
						}
						if (flags & 0x10)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
							tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
						}
						if (flags & 0x8)
						{
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							ndataBlocks = (UOSInt)v;
						}
						wayNodeList.Clear();
						j = 0;
						while (valid && j < ndataBlocks)
						{
							UOSInt nCoords;
							tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, v);
							nCoords = (UOSInt)v;
							k = 0;
							while (valid && k < nCoords)
							{
								UInt64 nNodes;
								tileOfst = Map::MapsforgeUtil::ReadVBEU(subfileBuff, tileOfst, nNodes);
								if (flags & 0x4)
								{
									Double lastLat = tileLat;
									Double lastLon = tileLon;
									Int64 lastLatOffset = 0;
									Int64 lastLonOffset = 0;
									l = 0;
									while (l < nNodes)
									{
										tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
										if (l > 0)
										{
											lastLatOffset += sv;
											lastLat += (Double)lastLatOffset * 0.000001;
										}
										else
										{
											lastLat += (Double)sv * 0.000001;
										}
										tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
										if (l > 0)
										{
											lastLonOffset += sv;
											lastLon += (Double)lastLonOffset * 0.000001;
										}
										else
										{
											lastLon += (Double)sv * 0.000001;
										}
										if (lastLat < -90.0 || lastLat > 90.0 || lastLon < -180.0 || lastLon > 180.0)
										{
											valid = false;
											break;
										}
										else
										{
											NN<Map::OSM::NodeInfo> node;
											if (!osmData->GetNodeByPos(lastLat, lastLon).SetTo(node))
											{
												currNodeCnt++;
												node = osmData->NewNode((Int64)currNodeCnt, lastLat, lastLon);
											}
											wayNodeList.Add(node);
										}
										
										if (tileOfst > wayEndOfst)
										{
											valid = false;
											break;
										}
										l++;
									}
								}
								else
								{
									Double lastLat = tileLat;
									Double lastLon = tileLon;
									l = 0;
									while (l < nNodes)
									{
										tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
										lastLat += (Double)sv * 0.000001;
										tileOfst = Map::MapsforgeUtil::ReadVBES(subfileBuff, tileOfst, sv);
										lastLon += (Double)sv * 0.000001;
										if (lastLat < -90.0 || lastLat > 90.0 || lastLon < -180.0 || lastLon > 180.0)
										{
											valid = false;
											break;
										}
										else
										{
											NN<Map::OSM::NodeInfo> node;
											if (!osmData->GetNodeByPos(lastLat, lastLon).SetTo(node))
											{
												currNodeCnt++;
												node = osmData->NewNode((Int64)currNodeCnt, lastLat, lastLon);
											}
											wayNodeList.Add(node);
										}
										if (tileOfst > wayEndOfst)
										{
											valid = false;
											break;
										}
										l++;
									}
								}
								k++;
							}
							j++;
						}

						if (valid && tileOfst == wayEndOfst)
						{
							currWayCnt++;
							NN<Map::OSM::WayInfo> way = osmData->NewWay((Int64)currWayCnt);
							j = 0;
							while (tagIds[j] != INVALID_INDEX)
							{
								osmData->ElementAddTag(way, wayTags[tagIds[j]].k, wayTags[tagIds[j]].v);
								j++;
							}
							if (nameOfst != INVALID_INDEX)
							{
								sb.ClearStr();
								sb.AppendC(&subfileBuff[nameOfst], nameLeng);
								sarr[1] = sb;
								while (true)
								{
									j = Text::StrSplitLineP(sarr, 2, sarr[1]);
									k = sarr[0].IndexOf(8);
									if (k == INVALID_INDEX)
									{
										s1 = Text::String::New(CSTR("name"));
										s2 = Text::String::New(sarr[0].ToCString());
										osmData->ElementAddTag(way, s1, s2);
										s1->Release();
										s2->Release();
									}
									else
									{
										sb2.ClearStr();
										sb2.Append(CSTR("name:"));
										sb2.AppendC(sarr[0].v, k);
										s1 = Text::String::New(sb2.ToCString());
										s2 = Text::String::New(sarr[0].Substring(k + 1).ToCString());
										osmData->ElementAddTag(way, s1, s2);
										s1->Release();
										s2->Release();
									}
									if (j != 2)
										break;
								}
							}
							if (houseNumOfst != INVALID_INDEX)
							{
								sb.ClearStr();
								sb.AppendC(&subfileBuff[houseNumOfst], houseNumLeng);
								s2 = Text::String::New(sb.ToCString());
								osmData->ElementAddTag(way, strHouseNum, s2);
								s2->Release();
							}
							j = 0;
							k = wayNodeList.GetCount();
							while (j < k)
							{
								osmData->WayAddNode(way, wayNodeList.GetItemNoCheck(j));
								j++;
							}
						}
						tileOfst = wayEndOfst;
						i++;
					}
				}
			}

			ofst += 5;
			x++;
		}
		y++;
	}
	strHouseNum->Release();
	strEle->Release();
	nodeCnt.Set(currNodeCnt);
	wayCnt.Set(currWayCnt);
	MemFreeArr(subfileBuff);
}
