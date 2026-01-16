#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/ProtocolBuffersUtil.h"
#include "IO/StreamData.h"
#include "Map/OSM/OSMData.h"
#include "Parser/FileParser/OSMPBFParser.h"

Parser::FileParser::OSMPBFParser::OSMPBFParser()
{
}

Parser::FileParser::OSMPBFParser::~OSMPBFParser()
{
}

Int32 Parser::FileParser::OSMPBFParser::GetName()
{
	return *(Int32*)"OSMP";
}

void Parser::FileParser::OSMPBFParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.osm.pbf"), CSTR("OSM PBF File"));
	}
}

IO::ParserType Parser::FileParser::OSMPBFParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::OSMPBFParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr[4] != 0x0a || hdr[5] != 9 || !Text::StrEqualsC(&hdr[6], 9, UTF8STRC("OSMHeader")))
	{
		return nullptr;
	}
	NN<Map::OSM::OSMData> osmData;
	NEW_CLASSNN(osmData, Map::OSM::OSMData(fd->GetFullFileName()));
	UInt64 ofst = 0;
	UInt64 fileSize = fd->GetDataSize();
	Data::ByteBuffer dataBuff(128);
	UInt8 headerBuff[4];
	Text::CString blobType = nullptr;
	Text::CStringNN nns;
	UIntOS blockOfst;
	UInt64 v;
	Int64 iv;
	Bool error = false;
	UIntOS blobSize;
	NN<IO::MemoryStream> blobStm;
	while (ofst < fileSize)
	{
		if (fd->GetRealData(ofst, 4, Data::ByteArray(headerBuff, 4)) != 4)
		{
			printf("OSMPBFParser: Error reading BlobHeader size\r\n");
			error = true;
			break;
		}
		UInt32 blockSize = ReadMUInt32(&headerBuff[0]);
		ofst += 4;
		if (ofst + blockSize > fileSize)
		{
			printf("OSMPBFParser: BlobHeader size too large: %d\r\n", blockSize);
			error = true;
			break;
		}
		if (dataBuff.GetSize() < blockSize)
		{
			dataBuff.ChangeSizeAndClear(blockSize);
		}
		if (fd->GetRealData(ofst, blockSize, dataBuff.SubArray(0, blockSize)) != blockSize)
		{
			printf("OSMPBFParser: Error reading BlobHeader data, ofst = %llu, size = %u\r\n", ofst, blockSize);
			error = true;
			break;
		}
		ofst += blockSize;
		blockOfst = 0;
		blobSize = 0;
		while (blockOfst < blockSize)
		{
			blockOfst = IO::ProtocolBuffersUtil::ReadVarUInt(dataBuff.Arr(), blockOfst, v);
			UInt8 fieldType = (UInt8)(v & 7);
			UInt32 fieldNum = (UInt32)(v >> 3);
			if (fieldNum == 1 && fieldType == 2)
			{
				blockOfst = IO::ProtocolBuffersUtil::ReadVarUInt(dataBuff.Arr(), blockOfst, v);
				blobType = Text::CString(&dataBuff[blockOfst], (UIntOS)v);
				blockOfst += (UIntOS)v;
			}
			else if (fieldNum == 3 && fieldType == 0)
			{
				blockOfst = IO::ProtocolBuffersUtil::ReadVarInt(dataBuff.Arr(), blockOfst, iv);
				blobSize = (UIntOS)iv;
			}
			else if (fieldNum == 2 && fieldType == 2)
			{
				blockOfst = IO::ProtocolBuffersUtil::ReadVarUInt(dataBuff.Arr(), blockOfst, v);
				blockOfst += (UIntOS)v;
			}
			else
			{
				printf("OSMPBFParser: Unknown field in BlobHeader: num %d type %d\r\n", fieldNum, fieldType);
				error = true;
				break;
			}
		}
		if (error)
			break;
		if (blobSize == 0 || blockOfst + blobSize > fileSize)
		{
			printf("OSMPBFParser: Invalid blob size: %lld, blockOfst %u, fileSize %llu, ofst %llu\r\n", (Int64)blobSize, (UInt32)blockOfst, fileSize, ofst);
			error = true;
			break;
		}
		if (!blobType.SetTo(nns))
		{
			printf("OSMPBFParser: BlobType missing\r\n");
			error = true;
			break;
		}
		if (nns.Equals(UTF8STRC("OSMHeader")))
		{
			if (dataBuff.GetSize() < blobSize)
			{
				dataBuff.ChangeSizeAndClear(blobSize);
			}
			if (fd->GetRealData(ofst, blobSize, dataBuff.SubArray(0, blobSize)) != blobSize)
			{
				printf("OSMPBFParser: Error reading OSMHeader blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				break;
			}
			ofst += blobSize;
			if (!IO::ProtocolBuffersUtil::DecompressBlob(dataBuff.SubArray(0, blobSize)).SetTo(blobStm))
			{
				printf("OSMPBFParser: Error decompressing OSMHeader blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				break;
			}
			if (!IO::ProtocolBuffersUtil::ParseOSMHeader(blobStm, osmData))
			{
				printf("OSMPBFParser: Error parsing OSMHeader blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				blobStm.Delete();
				break;
			}
			blobStm.Delete();
		}
		else if (nns.Equals(UTF8STRC("OSMData")))
		{
			if (dataBuff.GetSize() < blobSize)
			{
				dataBuff.ChangeSizeAndClear(blobSize);
			}
			if (fd->GetRealData(ofst, blobSize, dataBuff.SubArray(0, blobSize)) != blobSize)
			{
				printf("OSMPBFParser: Error reading OSMData blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				break;
			}
			if (!IO::ProtocolBuffersUtil::DecompressBlob(dataBuff.SubArray(0, blobSize)).SetTo(blobStm))
			{
				printf("OSMPBFParser: Error decompressing OSMData blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				break;
			}
			if (!IO::ProtocolBuffersUtil::ParseOSMData(blobStm, osmData))
			{
				printf("OSMPBFParser: Error parsing OSMData blob: ofst %llu, blobSize %u\r\n", ofst, (UInt32)blobSize);
				error = true;
				blobStm.Delete();
				break;
			}
			blobStm.Delete();
			ofst += blobSize;
		}
		else
		{
			printf("OSMPBFParser: Unknown BlobType: %s\r\n", nns.v.Ptr());
			error = true;
			break;
		}
	}
	if (error)
	{
		osmData.Delete();
		return nullptr;
	}
	osmData->BuildIndex();
	return osmData;
}
