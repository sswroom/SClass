#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
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

IO::ParsedObject *Parser::FileParser::MKVParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt8 buff[BUFFSIZE];
	MKVStatus status;
	UInt64 dataSize;
	UInt32 segmId;
	if (ReadMInt32(hdr) != 0x1a45dfa3)
		return 0;
	status.buffSize = 256;
	status.nextReadOfst = 256;
	status.fd = fd;
	status.buff = BYTEARR(buff);
	status.currOfst = 4;
	MemCopyNO(buff, hdr, 256);
	if (ReadDataSize(&status, &dataSize) == 0)
		return 0;
	if (ReadHeader(&status, dataSize) == 0)
		return 0;
	segmId = 0;
	ReadID(&status, &segmId);
	if (segmId != 0x18538067)
		return 0;
	if (ReadDataSize(&status, &dataSize) == 0)
		return 0;
	IO::ParsedObject *pobj = ReadSegment(&status, dataSize);
	return pobj;
}

UOSInt Parser::FileParser::MKVParser::ReadDataSize(MKVStatus *status, UInt64 *dataSize)
{
	if (status->currOfst >= status->buffSize)
	{
		if (ReadBuffer(status) == 0)
			return 0;
	}
	UInt8 b = status->buff[status->currOfst];
	if (b & 0x80)
	{
		*dataSize = b & 0x7f;
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
		*dataSize = ReadMUInt16(&status->buff[status->currOfst]) & 0x3fff;
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
		*dataSize = ReadMUInt24(&status->buff[status->currOfst]) & 0x1fffff;
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
		*dataSize = ReadMUInt32(&status->buff[status->currOfst]) & 0xfffffff;
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
		*dataSize = (((UInt64)status->buff[status->currOfst] & 7) << 32) | ReadMUInt32(&status->buff[status->currOfst + 1]);
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
		*dataSize = (((UInt64)ReadMUInt16(&status->buff[status->currOfst]) & 0x3ff) << 32) | ReadMUInt32(&status->buff[status->currOfst + 2]);
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
		*dataSize = (((UInt64)ReadMUInt24(&status->buff[status->currOfst]) & 0x1ffff) << 32) | ReadMUInt32(&status->buff[status->currOfst + 3]);
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
		*dataSize = (((UInt64)ReadMUInt24(&status->buff[status->currOfst + 1])) << 32) | ReadMUInt32(&status->buff[status->currOfst + 4]);
		status->currOfst += 8;
		return 8;
	}
	else
	{
		return 0;
	}
}

UOSInt Parser::FileParser::MKVParser::ReadID(MKVStatus *status, UInt32 *eleId)
{
	if (status->currOfst >= status->buffSize)
	{
		if (ReadBuffer(status) == 0)
			return 0;
	}
	UInt8 b = status->buff[status->currOfst];
	if (b & 0x80)
	{
		*eleId = b;
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
		*eleId = ReadMUInt16(&status->buff[status->currOfst]);
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
		*eleId = ReadMUInt24(&status->buff[status->currOfst]);
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
		*eleId = ReadMUInt32(&status->buff[status->currOfst]);
		status->currOfst += 4;
		return 4;
	}
	else
	{
		return 0;
	}
}

UOSInt Parser::FileParser::MKVParser::ReadData(MKVStatus *status, UInt64 dataSize, Data::ByteArray buff)
{
	UOSInt readSize;
	UOSInt thisReadSize;
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
			thisReadSize = status->fd->GetRealData(status->nextReadOfst, (UOSInt)dataSize, buff);
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
				buff.CopyFrom(status->buff.SubArray(status->currOfst, (UOSInt)dataSize));
				status->currOfst += (UOSInt)dataSize;
				readSize += (UOSInt)dataSize;
			}
		}
		return readSize;
	}
	else
	{
		buff.CopyFrom(status->buff.SubArray(status->currOfst, (UOSInt)dataSize));
		status->currOfst += (UOSInt)dataSize;
		return (UOSInt)dataSize;
	}
}

UOSInt Parser::FileParser::MKVParser::ReadBuffer(MKVStatus *status)
{
	UOSInt readSize;
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

Bool Parser::FileParser::MKVParser::SkipBuffer(MKVStatus *status, UOSInt skipSize)
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

Bool Parser::FileParser::MKVParser::ReadHeader(MKVStatus *status, UInt64 dataSize)
{
	UOSInt readSize;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[16];
	while (dataSize > 0)
	{
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
			return false;
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
			return false;
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0x4286: //EBMLVersion
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x42f7: //EBMLReadVersion
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x42f2: //EBMLMaxIDLength
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x42f3: //EBMLMaxSizeLength
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x4282: //DocType
			if (elementSize > 16)
			{
				SkipBuffer(status, (UOSInt)elementSize);
			}
			else
			{
				ReadData(status, (UOSInt)elementSize, BYTEARR(buff));

			}
			break;
		case 0x4287: //DocTypeVersion
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x4285: //DocTypeReadVersion
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		default:
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		}
		dataSize -= elementSize;
	}
	return dataSize == 0;
}

IO::ParsedObject *Parser::FileParser::MKVParser::ReadSegment(MKVStatus *status, UInt64 dataSize)
{
	IO::ParsedObject *pobj = 0;
	UOSInt readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	while (dataSize > 0)
	{
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return 0;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return 0;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x114d9b74: //SeekHead
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x1254c367: //Tags
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x1549a966: //Info
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x1654ae6b: //Track
			valid = ReadTrack(status, elementSize);
			break;
		case 0x1c53bb6b: //Cues
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x1f43b675: //Cluster
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		default:
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
		{
			if (pobj)
				DEL_CLASS(pobj);
			return 0;
		}
	}
	return pobj;
}

Bool Parser::FileParser::MKVParser::ReadTrack(MKVStatus *status, UInt64 dataSize)
{
	UOSInt readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	while (dataSize > 0)
	{
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0xae: //TrackEntry
			valid = ReadTrackEntry(status, (UOSInt)elementSize);
			break;
		default:
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		}
		dataSize -= elementSize;
		if (!valid)
			return false;
	}
	return dataSize == 0;
}

Bool Parser::FileParser::MKVParser::ReadTrackEntry(MKVStatus *status, UInt64 dataSize)
{
	UOSInt readSize;
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
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UOSInt)elementSize);
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
			SkipBuffer(status, (UOSInt)elementSize);
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
			valid = ReadVideo(status, elementSize, &frameInfo);
			break;
		case 0xe1: //Audio
			valid = ReadAudio(status, elementSize, &audioFmt);
			break;
		case 0x63a2: //CodecPrivate
			SkipBuffer(status, (UOSInt)elementSize);
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
			SkipBuffer(status, (UOSInt)elementSize);
			break;
		case 0x23e383: //DefaultDuration
			if (elementSize == 4)
			{
				valid = ReadData(status, elementSize, BYTEARR(buff)) == elementSize;
//				frameTime = ReadMUInt32(buff) * 0.000000001;
			}
			else
			{
				SkipBuffer(status, (UOSInt)elementSize);
			}
			break;
		default:
			SkipBuffer(status, (UOSInt)elementSize);
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

Bool Parser::FileParser::MKVParser::ReadVideo(MKVStatus *status, UInt64 dataSize, Media::FrameInfo *frameInfo)
{
	UOSInt readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[64];

	while (dataSize > 0)
	{
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UOSInt)elementSize);
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
			SkipBuffer(status, (UOSInt)elementSize);
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

Bool Parser::FileParser::MKVParser::ReadAudio(MKVStatus *status, UInt64 dataSize, Media::AudioFormat *audioFmt)
{
	UOSInt readSize;
	Bool valid = true;
	UInt64 elementSize;
	UInt32 hdrId;
	UInt8 buff[64];

	while (dataSize > 0)
	{
		readSize = ReadID(status, &hdrId);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		readSize = ReadDataSize(status, &elementSize);
		if (readSize == 0)
		{
			return false;
		}
		dataSize -= readSize;
		switch (hdrId)
		{
		case 0xec: //Void
			SkipBuffer(status, (UOSInt)elementSize);
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
			SkipBuffer(status, (UOSInt)elementSize);
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
