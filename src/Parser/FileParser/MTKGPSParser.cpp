#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/MTKGPSNMEA.h"
#include "Map/GPSTrack.h"
#include "Parser/FileParser/MTKGPSParser.h"

Parser::FileParser::MTKGPSParser::MTKGPSParser()
{
}

Parser::FileParser::MTKGPSParser::~MTKGPSParser()
{
}

Int32 Parser::FileParser::MTKGPSParser::GetName()
{
	return *(Int32*)"MTKG";
}

void Parser::FileParser::MTKGPSParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.bin"), CSTR("MTK GPS Binary file"));
	}
}

IO::ParserType Parser::FileParser::MTKGPSParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::MTKGPSParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (!fd->GetFullFileName()->EndsWith(UTF8STRC(".bin")))
	{
		return 0;
	}
	if (ReadNUInt32(&hdr[508]) != 0xBBBBBBBB)
	{
		return 0;
	}
	UInt64 fileLen = fd->GetDataSize();
	if (fileLen == 0 || fileLen > 8388608 || (fileLen & 0xffff) != 0)
	{
		return 0;
	}

	UOSInt i;
	Map::GPSTrack *trk;
	UInt8 *fileBuff = MemAlloc(UInt8, (UOSInt)fileLen);
	if (fd->GetRealData(0, (UOSInt)fileLen, fileBuff) != fileLen)
	{
		MemFree(fileBuff);
		return 0;
	}
	NEW_CLASS(trk, Map::GPSTrack(fd->GetFullFileName(), true, 0, 0));
	i = 0;
	while (i < fileLen)
	{
		if (!IO::Device::MTKGPSNMEA::ParseBlock(&fileBuff[i], trk))
		{
			break;
		}
		i += 65536;
	}
	MemFree(fileBuff);
	if (trk->GetTrackCnt() > 0)
	{
		return trk;
	}
	DEL_CLASS(trk);
	return 0;
}