#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Parser/FileParser/MKVParser.h"

#define BUFFSIZE 1024

Parser::FileParser::MKVParser::MKVParser()
{
}

Parser::FileParser::MKVParser::~MKVParser()
{
}

Int32 Parser::FileParser::MKVParser::GetName()
{
	return *(Int32*)"MKVP";
}

void Parser::FileParser::MKVParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.mkv"), CSTR("MKV File"));
	}
}

IO::ParserType Parser::FileParser::MKVParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

Optional<IO::ParsedObject> Parser::FileParser::MKVParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 buff[BUFFSIZE];
	MKVStatus status;
	UInt64 dataSize;
	UInt32 segmId;
	if (ReadMInt32(&hdr[0]) != 0x1a45dfa3)
		return nullptr;
	status.buffSize = 256;
	status.nextReadOfst = 256;
	status.fd = fd;
	status.buff = BYTEARR(buff);
	status.currOfst = 4;
	MemCopyNO(buff, &hdr[0], 256);
	if (ReadDataSize(status, dataSize) == 0)
		return nullptr;
	if (ReadHeader(status, dataSize) == 0)
		return nullptr;
	segmId = 0;
	ReadID(status, segmId);
	if (segmId != 0x18538067)
		return nullptr;
	if (ReadDataSize(status, dataSize) == 0)
		return nullptr;
	Optional<IO::ParsedObject> pobj = ReadSegment(status, dataSize);
	return pobj;
}

UIntOS Parser::FileParser::MKVParser::ReadDataSize(NN<MKVStatus> status, OutParam<UInt64> dataSize)
{
	if (status->currOfst >= status->buffSize)
	{
		if (ReadBuffer(status) == 0)
			return 0;
	}
	UInt8 b = status->buff[status->currOfst];
	if (b & 0x80)
	{
		dataSize.Set(b & 0x7f);
		status->currOfst++;
		return 1;
	}
	else if (b & 0x40)
	{
		if (status->buffSize - status->currOfst < 2)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 2)
				return 0;
		}
		dataSize.Set(ReadMUInt16(&status->buff[status->currOfst]) & 0x3fff);
		status->currOfst += 2;
		return 2;
	}
	else if (b & 0x20)
	{
		if (status->buffSize - status->currOfst < 3)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 3)
				return 0;
		}
		dataSize.Set(ReadMUInt24(&status->buff[status->currOfst]) & 0x1fffff);
		status->currOfst += 3;
		return 3;
	}
	else if (b & 0x10)
	{
		if (status->buffSize - status->currOfst < 4)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 4)
				return 0;
		}
		dataSize.Set(ReadMUInt32(&status->buff[status->currOfst]) & 0xfffffff);
		status->currOfst += 4;
		return 4;
	}
	else if (b & 8)
	{
		if (status->buffSize - status->currOfst < 5)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 5)
				return 0;
		}
		dataSize.Set((((UInt64)status->buff[status->currOfst] & 7) << 32) | ReadMUInt32(&status->buff[status->currOfst + 1]));
		status->currOfst += 5;
		return 5;
	}
	else if (b & 4)
	{
		if (status->buffSize - status->currOfst < 6)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 6)
				return 0;
		}
		dataSize.Set((((UInt64)ReadMUInt16(&status->buff[status->currOfst]) & 0x3ff) << 32) | ReadMUInt32(&status->buff[status->currOfst + 2]));
		status->currOfst += 6;
		return 6;
	}
	else if (b & 2)
	{
		if (status->buffSize - status->currOfst < 7)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 7)
				return 0;
		}
		dataSize.Set((((UInt64)ReadMUInt24(&status->buff[status->currOfst]) & 0x1ffff) << 32) | ReadMUInt32(&status->buff[status->currOfst + 3]));
		status->currOfst += 7;
		return 7;
	}
	else if (b & 1)
	{
		if (status->buffSize - status->currOfst < 8)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 8)
				return 0;
		}
		dataSize.Set((((UInt64)ReadMUInt24(&status->buff[status->currOfst + 1])) << 32) | ReadMUInt32(&status->buff[status->currOfst + 4]));
		status->currOfst += 8;
		return 8;
	}
	else
	{
		return 0;
	}
}

UIntOS Parser::FileParser::MKVParser::ReadID(NN<MKVStatus> status, OutParam<UInt32> eleId)
{
	if (status->currOfst >= status->buffSize)
	{
		if (ReadBuffer(status) == 0)
			return 0;
	}
	UInt8 b = status->buff[status->currOfst];
	if (b & 0x80)
	{
		eleId.Set(b);
		status->currOfst++;
		return 1;
	}
	else if (b & 0x40)
	{
		if (status->buffSize - status->currOfst < 2)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 2)
				return 0;
		}
		eleId.Set(ReadMUInt16(&status->buff[status->currOfst]));
		status->currOfst += 2;
		return 2;
	}
	else if (b & 0x20)
	{
		if (status->buffSize - status->currOfst < 3)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 3)
				return 0;
		}
		eleId.Set(ReadMUInt24(&status->buff[status->currOfst]));
		status->currOfst += 3;
		return 3;
	}
	else if (b & 0x10)
	{
		if (status->buffSize - status->currOfst < 4)
		{
			if (!ReadBuffer(status))
				return 0;
			if (status->buffSize - status->currOfst < 4)
				return 0;
		}
		eleId.Set(ReadMUInt32(&status->buff[status->currOfst]));
		status->currOfst += 4;
		return 4;
	}
	else
	{
		return 0;
	}
}

UIntOS Parser::FileParser::MKVParser::ReadData(NN<MKVStatus> status, UInt64 dataSize, Data::ByteArray buff)
{
	UIntOS readSize;
	UIntOS thisReadSize;
	if (status->currOfst + dataSize > status->buffSize)
	{
		readSize = 0;
		if (status->currOfst < status->buffSize)
		{
			readSize = status->buffSize - status->currOfst;
			buff.CopyFrom(status->buff.SubArray(status->currOfst, readSize));
			dataSize -= readSize;
			status->currOfst += readSize;
			buff += readSize;
		}
		if (dataSize > BUFFSIZE)
		{
			thisReadSize = status->fd->GetRealData(status->nextReadOfst, (UIntOS)dataSize, buff);
			status->nextReadOfst += thisReadSize;
			readSize += thisReadSize;
		}
		else
		{
			thisReadSize = ReadBuffer(status);
			if (thisReadSize < dataSize)
			{
				buff.CopyFrom(status->buff.SubArray(status->currOfst, thisReadSize));
				status->currOfst += thisReadSize;
				readSize += thisReadSize;
			}
			else
			{
				buff.CopyFrom(status->buff.SubArray(status->currOfst, (UIntOS)dataSize));
				status->currOfst += (UIntOS)dataSize;
				readSize += (UIntOS)dataSize;
			}
		}
		return readSize;
	}
	else
	{
		buff.CopyFrom(status->buff.SubArray(status->currOfst, (UIntOS)dataSize));
		status->currOfst += (UIntOS)dataSize;
		return (UIntOS)dataSize;
	}
}

UIntOS Parser::FileParser::MKVParser::ReadBuffer(NN<MKVStatus> status)
{
	UIntOS readSize;
	if (status->currOfst < status->buffSize)
	{
		status->buff.CopyInner(0, status->currOfst, status->buffSize - status->currOfst);
		status->buffSize -= status->currOfst;
		status->currOfst = 0;
	}
	else
	{
		status->buffSize = 0;
		status->currOfst = 0;
	}
	readSize = status->fd->GetRealData(status->nextReadOfst, 1024 - status->buffSize, status->buff.SubArray(status->buffSize));
	if (readSize == 0)
		return 0;
	status->nextReadOfst += readSize;
	status->buffSize += readSize;
	return readSize;
}

Bool Parser::FileParser::MKVParser::SkipBuffer(NN<MKVStatus> status, UIntOS skipSize)
{
	if (status->currOfst + skipSize > status->buffSize)
	{
		status->nextReadOfst += skipSize - (status->buffSize - status->currOfst);
		status->buffSize = 0;
		status->currOfst = 0;
		return true;
	}
	else
	{
		status->currOfst += skipSize;
		return true;
	}
}

Bool Parser::FileParser::MKVParser::ReadHeader(NN<MKVStatus> status, UInt64 dataSize)
{
	UIntOS readSize;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[16];
	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
			return false;
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
			return false;
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0x4286: //EBMLVersion
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x42f7: //EBMLReadVersion
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x42f2: //EBMLMaxIDLength
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x42f3: //EBMLMaxSizeLength
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x4282: //DocType
			if (elementSize > 16)
			{
				SkipBuffer(status, (UIntOS)elementSize);
			}
			else
			{
				ReadData(status, (UIntOS)elementSize, BYTEARR(buff));

			}
			break;
		case 0x4287: //DocTypeVersion
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x4285: //DocTypeReadVersion
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
	}
	return dataSize == 0;
}

Optional<IO::ParsedObject> Parser::FileParser::MKVParser::ReadSegment(NN<MKVStatus> status, UInt64 dataSize)
{
	IO::ParsedObject *pobj = 0;
	UIntOS readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return nullptr;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return nullptr;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x114d9b74: //SeekHead
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x1254c367: //Tags
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x1549a966: //Info
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x1654ae6b: //Track
			valid = ReadTrack(status, elementSize);
			break;
		case 0x1c53bb6b: //Cues
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x1f43b675: //Cluster
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return nullptr;
		}
	}
	return pobj;
}

Bool Parser::FileParser::MKVParser::ReadTrack(NN<MKVStatus> status, UInt64 dataSize)
{
	UIntOS readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0xae: //TrackEntry
			valid = ReadTrackEntry(status, (UIntOS)elementSize);
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
			return false;
	}
	return dataSize == 0;
}

Bool Parser::FileParser::MKVParser::ReadTrackEntry(NN<MKVStatus> status, UInt64 dataSize)
{
	UIntOS readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[64];

//	Int32 trackNumber = 0;
//	Int32 trackUID = 0;
//	Int32 trackType = 0;
//	Double frameTime = 0;
	Media::FrameInfo frameInfo;
	Media::AudioFormat audioFmt;
	audioFmt.Clear();

	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x83: //TrackType
			if (ReadData(status, elementSize, BYTEARR(buff)) != elementSize)
				valid = false;
//			trackType = buff[0];
			break;
		case 0x86: //CodecID
			if (ReadData(status, elementSize, BYTEARR(buff)) != elementSize)
			{
				valid = false;
			}
			else
			{
				buff[elementSize] = 0;
			}
			break;
		case 0x9c: //FlagLacing
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0xd7: //TrackNumber
			ReadData(status, elementSize, BYTEARR(buff));
			if (elementSize == 1)
			{
//				trackNumber = buff[0];
			}
			else if (elementSize == 2)
			{
//				trackNumber = ReadMUInt16(buff);
			}
			else if (elementSize == 3)
			{
//				trackNumber = ReadMUInt24(buff);
			}
			else if (elementSize >= 4)
			{
//				trackNumber = ReadMUInt32(buff);
			}
			break;
		case 0xe0: //Video
			valid = ReadVideo(status, elementSize, frameInfo);
			break;
		case 0xe1: //Audio
			valid = ReadAudio(status, elementSize, audioFmt);
			break;
		case 0x63a2: //CodecPrivate
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x73c5: //TrackUID
			ReadData(status, elementSize, BYTEARR(buff));
			if (elementSize == 1)
			{
//				trackUID = buff[0];
			}
			else if (elementSize == 2)
			{
//				trackUID = ReadMUInt16(buff);
			}
			else if (elementSize == 3)
			{
//				trackUID = ReadMUInt24(buff);
			}
			else if (elementSize >= 4)
			{
//				trackUID = ReadMUInt32(buff);
			}
			break;
		case 0x22b59c: //Language
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x23e383: //DefaultDuration
			if (elementSize == 4)
			{
				valid = ReadData(status, elementSize, BYTEARR(buff)) == elementSize;
//				frameTime = ReadMUInt32(buff) * 0.000000001;
			}
			else
			{
				SkipBuffer(status, (UIntOS)elementSize);
			}
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
		{
			return false;
		}
	}
	return dataSize == 0;
}

Bool Parser::FileParser::MKVParser::ReadVideo(NN<MKVStatus> status, UInt64 dataSize, NN<Media::FrameInfo> frameInfo)
{
	UIntOS readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[64];

	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0xb0: //PixelWidth
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 2)
			{
				frameInfo->storeSize.x = ReadMUInt16(buff);
				frameInfo->dispSize.x = frameInfo->storeSize.x;
			}
			else
			{
				valid = false;
			}
			break;
		case 0xba: //PixelHeight
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 2)
			{
				frameInfo->storeSize.y = ReadMUInt16(buff);
				frameInfo->dispSize.y = frameInfo->storeSize.y;
			}
			else
			{
				valid = false;
			}
			break;
		case 0x54b0: //DisplayWidth
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 2)
			{
				frameInfo->dispSize.x = ReadMUInt16(buff);
			}
			else
			{
				valid = false;
			}
			break;
		case 0x54ba: //DisplayHeight
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 2)
			{
				frameInfo->dispSize.y = ReadMUInt16(buff);
			}
			else
			{
				valid = false;
			}
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
		{
			return false;
		}
	}
	return dataSize == 0;
}

Bool Parser::FileParser::MKVParser::ReadAudio(NN<MKVStatus> status, UInt64 dataSize, NN<Media::AudioFormat> audioFmt)
{
	UIntOS readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[64];

	while (dataSize > 0)
	{
		readSize = ReadID(status, hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		case 0x9f: //Channels
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 1)
			{
				audioFmt->nChannels = buff[0];
			}
			else
			{
				valid = false;
			}
			break;
		case 0xb5: //SamplingFrequency
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 8)
			{
				audioFmt->frequency = (UInt32)Double2Int32(ReadMDouble(buff));
			}
			else
			{
				valid = false;
			}
			break;
		case 0x6264:
			if (elementSize != ReadData(status, elementSize, BYTEARR(buff)))
				valid = false;
			if (elementSize == 1)
			{
				audioFmt->bitpersample = buff[0];
			}
			else
			{
				valid = false;
			}
			break;
		default:
			SkipBuffer(status, (UIntOS)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
		{
			return false;
		}
	}
	return dataSize == 0;
}
