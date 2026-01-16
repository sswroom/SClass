#include "Stdafx.h"
#include "Data/ArrayListInt32.h"
#include "Data/Compress/InflateStream.h"
#include "IO/ProtocolBuffersUtil.h"

Int64 IO::ProtocolBuffersUtil::ToSInt64(UInt64 val)
{
	return (Int64)((val >> 1) ^ (~(val & 1) + 1));
}

UIntOS IO::ProtocolBuffersUtil::ReadVarUInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<UInt64> val)
{
	UInt64 outVal;
	UInt64 v;
	UIntOS shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		if ((v & 0x80) == 0)
		{
			break;
		}
		shift += 7;
	}
	val.Set(outVal);
	return buffOfst;
}

UIntOS IO::ProtocolBuffersUtil::ReadVarInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<Int64> val)
{
	UInt64 outVal;
	UInt64 v;
	UIntOS shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		shift += 7;
		if ((v & 0x80) == 0)
		{
			break;
		}
	}
	if (outVal & (1 << (shift - 1)))
	{
		outVal |= (~0ULL) << shift;
	}
	val.Set((Int64)outVal);
	return buffOfst;
}

UIntOS IO::ProtocolBuffersUtil::ReadVarSInt(UnsafeArray<const UInt8> buff, UIntOS buffOfst, OutParam<Int64> val)
{
	UInt64 outVal;
	UInt64 v;
	UIntOS shift = 0;
	outVal = 0;
	while (true)
	{
		v = buff[buffOfst];
		outVal |= (v & 0x7f) << shift;
		buffOfst++;
		if ((v & 0x80) == 0)
		{
			break;
		}
		shift += 7;
	}
	val.Set(ToSInt64(outVal));
	return buffOfst;
}

Text::CStringNN IO::ProtocolBuffersUtil::WireTypeGetName(UInt8 wireType)
{
	switch (wireType)
	{
	case 0:
		return CSTR("Varint");
	case 1:
		return CSTR("64-bit");
	case 2:
		return CSTR("Length-delimited");
	case 3:
		return CSTR("Start group");
	case 4:
		return CSTR("End group");
	case 5:
		return CSTR("32-bit");
	default:
		return CSTR("Unknown");
	}
}

Optional<IO::MemoryStream> IO::ProtocolBuffersUtil::DecompressBlob(Data::ByteArrayR blobData)
{
	Int64 rawSize = -1;
	UIntOS ofst = 0;
	UInt64 v;
	while (ofst < blobData.GetSize())
	{
		ofst = ReadVarUInt(blobData.Arr(), ofst, v);
		UInt8 fieldType = (UInt8)(v & 7);
		UInt32 fieldNum = (UInt32)(v >> 3);
		if (fieldNum == 2 && fieldType == 0)
		{
			ofst = ReadVarInt(blobData.Arr(), ofst, rawSize);
		}
		else if (fieldNum == 1 && fieldType == 2) // raw
		{
			ofst = ReadVarUInt(blobData.Arr(), ofst, v);
			if (ofst + (UIntOS)v > blobData.GetSize())
			{
				printf("ProtocolBuffersUtil: Blob size too large: %llu\r\n", v);
				return nullptr;
			}
			NN<IO::MemoryStream> memStm;
			NEW_CLASSNN(memStm, IO::MemoryStream((UIntOS)v));
			memStm->Write(blobData.SubArray(ofst, (UIntOS)v));
			return memStm;
		}
		else if (fieldNum == 3 && fieldType == 2) // zlib
		{
			
			ofst = ReadVarUInt(blobData.Arr(), ofst, v);
			if (ofst + (UIntOS)v > blobData.GetSize())
			{
				printf("ProtocolBuffersUtil: Blob size too large: %llu\r\n", v);
				return nullptr;
			}
			NN<IO::MemoryStream> memStm;
			NEW_CLASSNN(memStm, IO::MemoryStream((UIntOS)v));
			{
				Data::Compress::InflateStream inflateStream(memStm, true);
				inflateStream.Write(blobData.SubArray(ofst, (UIntOS)v));
			}
			if (rawSize < 0 || memStm->GetLength() == (UInt64)rawSize)
			{
				return memStm;
			}
			else
			{
				printf("ProtocolBuffersUtil: Decompressed size mismatch: %llu vs %lld\r\n", memStm->GetLength(), rawSize);
				memStm.Delete();
				return nullptr;
			}
		}
		else
		{
			printf("ProtocolBuffersUtil: Unknown field: %u %u\r\n", fieldNum, fieldType);
			return nullptr;
		}
	}
	printf("ProtocolBuffersUtil: No valid blob found\r\n");
	return nullptr;
}

Bool IO::ProtocolBuffersUtil::ParseOSMHeader(NN<IO::MemoryStream> blobStm, NN<Map::OSM::OSMData> osmData)
{
	UInt64 buffSize;
	UnsafeArray<UInt8> buff = blobStm->GetBuff(buffSize);
	UIntOS ofst = 0;
	UInt64 v;
	Int64 iv;
	while (ofst < buffSize)
	{
		ofst = ReadVarUInt(buff, ofst, v);
		UInt8 fieldType = (UInt8)(v & 7);
		UInt32 fieldNum = (UInt32)(v >> 3);
		if (fieldNum == 1 && fieldType == 2) //bbox
		{
			UIntOS endOfst;
			ofst = ReadVarUInt(buff, ofst, v);
			endOfst = ofst + (UIntOS)v;
			Math::RectAreaDbl bbox;
			while (ofst < endOfst)
			{
				ofst = ReadVarUInt(buff, ofst, v);
				UInt8 fType = (UInt8)(v & 7);
				UInt32 fNum = (UInt32)(v >> 3);
				if (fNum == 1 && fType == 0) //left
				{
					ofst = ReadVarInt(buff, ofst, iv);
					bbox.min.x = (Double)iv * 0.000000001;
				}
				else if (fNum == 2 && fType == 0) //right
				{
					ofst = ReadVarInt(buff, ofst, iv);
					bbox.max.x = (Double)iv * 0.000000001;
				}
				else if (fNum == 3 && fType == 0) //top
				{
					ofst = ReadVarInt(buff, ofst, iv);
					bbox.max.y = (Double)iv * 0.000000001;
				}
				else if (fNum == 4 && fType == 0) //bottom
				{
					ofst = ReadVarInt(buff, ofst, iv);
					bbox.min.y = (Double)iv * 0.000000001;
				}
				else
				{
					printf("ProtocolBuffersUtil.OSMHeader: Unknown field in bbox: num %d type %d\r\n", fNum, fType);
					return false;
				}
			}
			osmData->SetDataBounds(bbox);
			ofst = endOfst;
		}
		else if (fieldNum == 32 && fieldType == 0) //osmosis_replication_timestamp
		{
			ofst = ReadVarInt(buff, ofst, iv);
			osmData->SetOSMBase(Data::Timestamp::FromEpochSec((Int64)iv, Data::DateTimeUtil::GetLocalTzQhr()));
		}
		else if (fieldType == 0)
		{
			ofst = ReadVarInt(buff, ofst, iv);
		}
		else if (fieldType == 1)
		{
			ofst += 8;
		}
		else if (fieldType == 5)
		{
			ofst += 4;
		}
		else if (fieldType == 2)
		{
			ofst = ReadVarUInt(buff, ofst, v);
			ofst += (UIntOS)v;
		}
		else
		{
			printf("ProtocolBuffersUtil.OSMHeader: Unknown field: num %d type %d\r\n", fieldNum, fieldType);
			return false;
		}
	}
	return true;
}

Bool IO::ProtocolBuffersUtil::ParseOSMData(NN<IO::MemoryStream> blobStm, NN<Map::OSM::OSMData> osmData)
{
	Int32 granularity = 100;
	Int64 latOffset = 0;
	Int64 lonOffset = 0;
//	Int64 dateGranularity = 1000;
	UInt64 buffSize;
	UnsafeArray<UInt8> buff = blobStm->GetBuff(buffSize);
	UIntOS ofst = 0;
	UInt64 v;
	Int64 iv;
	Data::ArrayListStringNN stringTable;
	while (ofst < buffSize)
	{
		ofst = ReadVarUInt(buff, ofst, v);
		UInt8 fieldType = (UInt8)(v & 7);
		UInt32 fieldNum = (UInt32)(v >> 3);
		if (fieldNum == 1 && fieldType == 2) //stringtable
		{
			UIntOS endOfst;
			ofst = ReadVarUInt(buff, ofst, v);
			endOfst = ofst + (UIntOS)v;
			while (ofst < endOfst)
			{
				ofst = ReadVarUInt(buff, ofst, v);
				if (v != 0xA)
				{
					printf("ProtocolBuffersUtil.OSMData: Unknown stringtable entry\r\n");
					stringTable.FreeAll();
					return false;
				}
				ofst = ReadVarUInt(buff, ofst, v);
				if (ofst + (UIntOS)v > endOfst)
				{
					printf("ProtocolBuffersUtil.OSMData: Stringtable entry size too large: %llu\r\n", v);
					stringTable.FreeAll();
					return false;
				}
				stringTable.Add(Text::String::New(&buff[ofst], (UIntOS)v));
				ofst += (UIntOS)v;
			}
			ofst = endOfst;
		}
		else if (fieldNum == 2 && fieldType == 2) //primitivegroup
		{
			UIntOS endOfst;
			ofst = ReadVarUInt(buff, ofst, v);
			endOfst = ofst + (UIntOS)v;
			while (ofst < endOfst)
			{
				ofst = ReadVarUInt(buff, ofst, v);
				UInt8 entryType = (UInt8)(v & 7);
				UInt32 entryNum = (UInt32)(v >> 3);
				if (entryType != 2)
				{
					printf("ProtocolBuffersUtil.OSMData: Unknown PrimitiveGroup entry type: %d\r\n", entryType);
					stringTable.FreeAll();
					return false;
				}
				ofst = ReadVarUInt(buff, ofst, v);
				UInt64 entryEndOfst = ofst + (UIntOS)v;
				if (entryNum == 1) //nodes
				{
					Int64 id = 0;
					Data::ArrayListUInt32 keys;
					Data::ArrayListUInt32 vals;
					Int64 lat = 0;
					Int64 lon = 0;
					while (ofst < entryEndOfst)
					{
						ofst = ReadVarUInt(buff, ofst, v);
						UInt8 nType = (UInt8)(v & 7);
						UInt32 nNum = (UInt32)(v >> 3);
						if (nNum == 1 && nType == 0) //id
						{
							ofst = ReadVarSInt(buff, ofst, iv);
							id = iv;
						}
						else if (nNum == 2 && nType == 2) //keys
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS keysEndOfst = ofst + (UIntOS)v;
							keys.Clear();
							while (ofst < keysEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								keys.Add((UInt32)v);
							}
							ofst = keysEndOfst;
						}
						else if (nNum == 3 && nType == 2) //vals
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS valsEndOfst = ofst + (UIntOS)v;
							vals.Clear();
							while (ofst < valsEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								vals.Add((UInt32)v);
							}
							ofst = valsEndOfst;
						}
						else if (nNum == 4 && nType == 2) //info
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v;
						}
						else if (nNum == 8 && nType == 0) //lat
						{
							ofst = ReadVarSInt(buff, ofst, iv);
							lat += iv;
						}
						else if (nNum == 9 && nType == 0) //lon
						{
							ofst = ReadVarSInt(buff, ofst, iv);
							lon += iv;
						}
						else
						{
							printf("ProtocolBuffersUtil.OSMData: Unknown node field: num %d type %d\r\n", nNum, nType);
							stringTable.FreeAll();
							return false;
						}
					}
					if (keys.GetCount() != vals.GetCount())
					{
						printf("ProtocolBuffersUtil.OSMData: Node tag keys/vals count mismatch\r\n");
						stringTable.FreeAll();
						return false;
					}
					NN<Map::OSM::NodeInfo> node = osmData->NewNode(id, (Double)(latOffset + lat * granularity) * 0.000000001, (Double)(lonOffset + lon * granularity) * 0.000000001);
					UIntOS i = 0;
					UIntOS j = keys.GetCount();
					while (i < j)
					{
						NN<Text::String> k;
						NN<Text::String> v;
						if (stringTable.GetItem(keys.GetItem(i)).SetTo(k) && stringTable.GetItem(vals.GetItem(i)).SetTo(v))
						{
							osmData->ElementAddTag(node, k, v);
						}
						i++;
					}
				}
				else if (entryNum == 2) //dense nodes
				{
					Data::ArrayListInt64 ids;
					Data::ArrayListInt64 lats;
					Data::ArrayListInt64 lons;
					Data::ArrayListUInt32 keysVals;
					Int64 lastId = 0;
					Int64 lastLat = 0;
					Int64 lastLon = 0;
					while (ofst < entryEndOfst)
					{
						ofst = ReadVarUInt(buff, ofst, v);
						UInt8 nType = (UInt8)(v & 7);
						UInt32 nNum = (UInt32)(v >> 3);
						if (nNum == 1 && nType == 2) //id
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS idEndOfst = ofst + (UIntOS)v;
							ids.Clear();
							while (ofst < idEndOfst)
							{
								ofst = ReadVarSInt(buff, ofst, iv);
								lastId += iv;
								ids.Add(lastId);
							}
							ofst = idEndOfst;
						}
						else if (nNum == 8 && nType == 2) //lat
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS latEndOfst = ofst + (UIntOS)v;
							lats.Clear();
							while (ofst < latEndOfst)
							{
								ofst = ReadVarSInt(buff, ofst, iv);
								lastLat += iv;
								lats.Add(lastLat);
							}
							ofst = latEndOfst;
						}
						else if (nNum == 9 && nType == 2) //lon
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS lonEndOfst = ofst + (UIntOS)v;
							lons.Clear();
							while (ofst < lonEndOfst)
							{
								ofst = ReadVarSInt(buff, ofst, iv);
								lastLon += iv;
								lons.Add(lastLon);
							}
							ofst = lonEndOfst;
						}
						else if (nNum == 10 && nType == 2) //keys_vals
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS kvEndOfst = ofst + (UIntOS)v;
							keysVals.Clear();
							while (ofst < kvEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								keysVals.Add((UInt32)v);
							}
							ofst = kvEndOfst;
						}
						else if (nNum == 5 && nType == 2) //info
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v;
						}
						else
						{
							printf("ProtocolBuffersUtil.OSMData: Unknown dense node field: num %d type %d\r\n", nNum, nType);
							stringTable.FreeAll();
							return false;
						}
					}
					if (ids.GetCount() != lats.GetCount() || ids.GetCount() != lons.GetCount())
					{
						printf("ProtocolBuffersUtil.OSMData: Dense Node count mismatch\r\n");
						stringTable.FreeAll();
						return false;
					}
					UIntOS i = 0;
					UIntOS j = ids.GetCount();
					UIntOS kvOfst = 0;
					while (i < j)
					{
						NN<Map::OSM::NodeInfo> node = osmData->NewNode(ids.GetItem(i), (Double)(latOffset + lats.GetItem(i) * granularity) * 0.000000001, (Double)(lonOffset + lons.GetItem(i) * granularity) * 0.000000001);
						while (kvOfst < keysVals.GetCount())
						{
							UInt32 keyIndex = keysVals.GetItem(kvOfst++);
							if (keyIndex == 0)
							{
								break;
							}
							NN<Text::String> k;
							NN<Text::String> v;
							if (stringTable.GetItem(keyIndex).SetTo(k) && stringTable.GetItem(keysVals.GetItem(kvOfst++)).SetTo(v))
							{
								osmData->ElementAddTag(node, k, v);
							}
						}
						i++;
					}
				}
				else if (entryNum == 3) //ways
				{
					Int64 id = 0;
					Data::ArrayListUInt32 keys;
					Data::ArrayListUInt32 vals;
					Data::ArrayListInt64 refs;
					while (ofst < entryEndOfst)
					{
						ofst = ReadVarUInt(buff, ofst, v);
						UInt8 nType = (UInt8)(v & 7);
						UInt32 nNum = (UInt32)(v >> 3);
						if (nNum == 1 && nType == 0) //id
						{
							ofst = ReadVarUInt(buff, ofst, v);
							id = (Int64)v;
						}
						else if (nNum == 2 && nType == 2) //keys
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS keysEndOfst = ofst + (UIntOS)v;
							keys.Clear();
							while (ofst < keysEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								keys.Add((UInt32)v);
							}
							ofst = keysEndOfst;
						}
						else if (nNum == 3 && nType == 2) //vals
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS valsEndOfst = ofst + (UIntOS)v;
							vals.Clear();
							while (ofst < valsEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								vals.Add((UInt32)v);
							}
							ofst = valsEndOfst;
						}
						else if (nNum == 8 && nType == 2) //refs
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS refsEndOfst = ofst + (UIntOS)v;
							refs.Clear();
							Int64 lastRef = 0;
							while (ofst < refsEndOfst)
							{
								ofst = ReadVarSInt(buff, ofst, iv);
								lastRef += iv;
								refs.Add(lastRef);
							}
							ofst = refsEndOfst;
						}
						else if (nNum == 4 && nType == 2) //info
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v;
						}
						else if (nNum == 9 && nType == 2) //lat
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v; //--- IGNORE ---
						}
						else if (nNum == 10 && nType == 2) //lon
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v; //--- IGNORE ---
						}
						else
						{
							printf("ProtocolBuffersUtil.OSMData: Unknown way field: num %d type %d\r\n", nNum, nType);
							stringTable.FreeAll();
							return false;
						}
					}
					if (id <= 0 || refs.GetCount() < 2 || keys.GetCount() != vals.GetCount())
					{
						printf("ProtocolBuffersUtil.OSMData: Invalid way data: id %lld, refs %u, keys %u, vals %u, ofst %llu\r\n", id, (UInt32)refs.GetCount(), (UInt32)keys.GetCount(), (UInt32)vals.GetCount(), ofst);
						stringTable.FreeAll();
						return false;
					}
					NN<Map::OSM::WayInfo> way = osmData->NewWay(id);
					UIntOS i = 0;
					UIntOS j = keys.GetCount();
					while (i < j)
					{
						NN<Text::String> k;
						NN<Text::String> v;
						if (stringTable.GetItem(keys.GetItem(i)).SetTo(k) && stringTable.GetItem(vals.GetItem(i)).SetTo(v))
						{
							osmData->ElementAddTag(way, k, v);
						}
						i++;
					}
					NN<Map::OSM::NodeInfo> node;
					i = 0;
					j = refs.GetCount();
					while (i < j)
					{
						if (!osmData->GetNodeById(refs.GetItem(i)).SetTo(node))
						{
							printf("ProtocolBuffersUtil.OSMData: Way references non-existing node: %lld\r\n", refs.GetItem(i));
							stringTable.FreeAll();
							return false;
						}
						osmData->WayAddNode(way, node);
						i++;
					}
				}
				else if (entryNum == 4) //relations
				{
					Int64 id = 0;
					Data::ArrayListUInt32 keys;
					Data::ArrayListUInt32 vals;
					Data::ArrayListInt64 memberIds;
					Data::ArrayListInt32 rolesSids;
					Data::ArrayListInt32 memberTypes;
					while (ofst < entryEndOfst)
					{
						ofst = ReadVarUInt(buff, ofst, v);
						UInt8 nType = (UInt8)(v & 7);
						UInt32 nNum = (UInt32)(v >> 3);
						if (nNum == 1 && nType == 0) //id
						{
							ofst = ReadVarUInt(buff, ofst, v);
							id = (Int64)v;
						}
						else if (nNum == 2 && nType == 2) //keys
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS keysEndOfst = ofst + (UIntOS)v;
							keys.Clear();
							while (ofst < keysEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								keys.Add((UInt32)v);
							}
							ofst = keysEndOfst;
						}
						else if (nNum == 3 && nType == 2) //vals
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS valsEndOfst = ofst + (UIntOS)v;
							vals.Clear();
							while (ofst < valsEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								vals.Add((UInt32)v);
							}
							ofst = valsEndOfst;
						}
						else if (nNum == 9 && nType == 2) //memids
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS memIdEndOfst = ofst + (UIntOS)v;
							memberIds.Clear();
							Int64 lastMemId = 0;
							while (ofst < memIdEndOfst)
							{
								ofst = ReadVarSInt(buff, ofst, iv);
								lastMemId += iv;
								memberIds.Add(lastMemId);
							}
							ofst = memIdEndOfst;
						}
						else if (nNum == 8 && nType == 2) //roles_sids
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS rolesEndOfst = ofst + (UIntOS)v;
							rolesSids.Clear();
							while (ofst < rolesEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								rolesSids.Add((Int32)v);
							}
							ofst = rolesEndOfst;
						}
						else if (nNum == 10 && nType == 2) //types
						{
							ofst = ReadVarUInt(buff, ofst, v);
							UIntOS typesEndOfst = ofst + (UIntOS)v;
							memberTypes.Clear();
							while (ofst < typesEndOfst)
							{
								ofst = ReadVarUInt(buff, ofst, v);
								memberTypes.Add((Int32)v);
							}
							ofst = typesEndOfst;
						}
						else if (nNum == 4 && nType == 2) //info
						{
							ofst = ReadVarUInt(buff, ofst, v);
							ofst += (UIntOS)v;
						}
						else
						{
							printf("ProtocolBuffersUtil.OSMData: Unknown relation field: num %d type %d\r\n", nNum, nType);
							stringTable.FreeAll();
							return false;
						}
					}
					if (id <= 0 || memberIds.GetCount() != rolesSids.GetCount() || memberIds.GetCount() != memberTypes.GetCount() || keys.GetCount() != vals.GetCount())
					{
						printf("ProtocolBuffersUtil.OSMData: Invalid relation data\r\n");
						stringTable.FreeAll();
						return false;
					}
					NN<Map::OSM::RelationInfo> rel = osmData->NewRelation(id);
					UIntOS i = 0;
					UIntOS j = keys.GetCount();
					while (i < j)
					{
						NN<Text::String> k;
						NN<Text::String> v;
						if (stringTable.GetItem(keys.GetItem(i)).SetTo(k) && stringTable.GetItem(vals.GetItem(i)).SetTo(v))
						{
							osmData->ElementAddTag(rel, k, v);
						}
						i++;
					}
					i = 0;
					j = memberIds.GetCount();
					while (i < j)
					{
						Map::OSM::ElementType memberType;
						switch (memberTypes.GetItem(i))
						{
						case 0:
							memberType = Map::OSM::ElementType::Node;
							break;
						case 1:	
							memberType = Map::OSM::ElementType::Way;
							break;
						case 2:
							memberType = Map::OSM::ElementType::Relation;
							break;
						default:
							printf("ProtocolBuffersUtil.OSMData: Unknown relation member type: %d\r\n", memberTypes.GetItem(i));
							stringTable.FreeAll();
							return false;
						}
/*						if (osmData->GetElementById(memberIds.GetItem(i), memberType).IsNull())
						{
							printf("ProtocolBuffersUtil.OSMData: Relation references non-existing member: %d %lld\r\n", memberTypes.GetItem(i), memberIds.GetItem(i));
							stringTable.FreeAll();
							return false;
						}*/
						osmData->RelationAddMember(rel, memberType, memberIds.GetItem(i), stringTable.GetItem((UIntOS)rolesSids.GetItem(i)));
						i++;
					}
				}
				else
				{
					printf("ProtocolBuffersUtil.OSMData: Unknown PrimitiveGroup entry num: %d\r\n", entryNum);
					stringTable.FreeAll();
					return false;
				}
				ofst = entryEndOfst;
			}
			ofst = endOfst;
		}
		else if (fieldNum == 17 && fieldType == 0) //granularity
		{
			ofst = ReadVarInt(buff, ofst, iv);
			granularity = (Int32)iv;
		}
		else if (fieldNum == 19 && fieldType == 0) //lat_offset
		{
			ofst = ReadVarInt(buff, ofst, iv);
			latOffset = iv;
		}
		else if (fieldNum == 20 && fieldType == 0) //lon_offset
		{
			ofst = ReadVarInt(buff, ofst, iv);
			lonOffset = iv;
		}
		else if (fieldNum == 18 && fieldType == 0) //date_granularity
		{
			ofst = ReadVarInt(buff, ofst, iv);
			//dateGranularity = iv;
		}
		else
		{
			stringTable.FreeAll();
			printf("ProtocolBuffersUtil.OSMData: Unknown field: num %d type %d\r\n", fieldNum, fieldType);
			return false;
		}
	}
	stringTable.FreeAll();
	return true;
}
