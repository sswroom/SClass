#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/CDSectorData.h"
#include "IO/ISectorData.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "Parser/ObjParser/ISO9660Parser.h"
#include "Text/Encoding.h"

Parser::ObjParser::ISO9660Parser::ISO9660Parser()
{
}

Parser::ObjParser::ISO9660Parser::~ISO9660Parser()
{
}

Int32 Parser::ObjParser::ISO9660Parser::GetName()
{
	return *(Int32*)"ISO9";
}

void Parser::ObjParser::ISO9660Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.iso"), CSTR("CD/DVD Image File"));
		selector->AddFilter(CSTR("*.img"), CSTR("CD/DVD Image File"));
	}
}

IO::ParserType Parser::ObjParser::ISO9660Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::ObjParser::ISO9660Parser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	IO::ISectorData *data;
	IO::ISectorData *cdData;
	UInt8 sector[2048];
	UOSInt sectorSize;
	if (pobj->GetParserType() != IO::ParserType::SectorData)
		return 0;

	cdData = 0;
	data = (IO::ISectorData*)pobj;
	sectorSize = data->GetBytesPerSector();
	if (sectorSize == 2048)
	{
	}
	else if (sectorSize == 2352)
	{
		NEW_CLASS(cdData, IO::CDSectorData(data, 16, 2048));
		data = cdData;
	}
	else
	{
		return 0;
	}

	UInt32 sectorNum9660 = 16;
	UInt32 sectorNumJoliet = 0;

	if (!data->ReadSector(16, sector))
	{
		SDEL_CLASS(cdData);
		return 0;
	}
	if (ReadMInt32(&sector[0]) != 0x01434430 && ReadMInt32(&sector[4]) != 0x30310100)
	{
		SDEL_CLASS(cdData);
		return 0;
	}
	UInt32 currSector = 17;
	while (data->ReadSector(currSector, sector))
	{
		if (sector[1] != 0x43 || sector[2] != 0x44 || sector[3] != 0x30 || sector[4] != 0x30 || sector[5] != 0x31 || sector[6] != 0x01 || sector[7] != 0x00)
			break;
		if (sector[0] == 0xff)
			break;
		if (sector[0] == 2)
			sectorNumJoliet = currSector;
		currSector++;
	}

	IO::PackageFile *pf;
	if (sectorNumJoliet != 0)
	{
		pf = ParseVol(data, sectorNumJoliet, 1201);
	}
	else
	{
		pf = ParseVol(data, sectorNum9660, 65001);
	}
	SDEL_CLASS(cdData);
	return pf;
}

IO::PackageFile *Parser::ObjParser::ISO9660Parser::ParseVol(IO::ISectorData *sectorData, UInt32 sectorNum, UInt32 codePage)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 sector[2048];
	IO::PackageFile *pkgFile;
	Text::Encoding enc(codePage);

	sectorData->ReadSector(sectorNum, sector);
	NEW_CLASS(pkgFile, IO::PackageFile(sectorData->GetSourceNameObj()));

	
	sptr = enc.UTF8FromBytes(sbuff, &sector[8], 32, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_SYSTEM_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[40], 32, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_VOLUME_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[190], 128, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_VOLUME_SET_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[318], 128, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_PUBLISHER_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[446], 128, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_DATA_PREPARER_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[574], 128, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_APPLICATION_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[702], 37, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_COPYRIGHT_FILE_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[739], 37, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_ABSTRACT_FILE_ID, sbuff);
	}

	sptr = enc.UTF8FromBytes(sbuff, &sector[776], 37, 0);
	sptr = Text::StrTrim(sbuff);
	if (sptr != sbuff)
	{
		pkgFile->SetInfo(IO::PackageFile::IT_BIBLIOGRAHPICAL_FILE_ID, sbuff);
	}

	sptr = sectorData->GetSourceName(sbuff);
	ParseDir(pkgFile, sectorData, ReadUInt32(&sector[158]), ReadUInt32(&sector[166]), sbuff, sptr, codePage);
	return pkgFile;
}

void Parser::ObjParser::ISO9660Parser::ParseDir(IO::PackageFile *pkgFile, IO::ISectorData *sectorData, UInt32 sectorNum, UInt32 recSize, UTF8Char *fileName, UTF8Char *fileNameEnd, UInt32 codePage)
{
	UInt8 *dataBuff = MemAlloc(UInt8, recSize + 2048);
	UInt8 *recBuff = &dataBuff[2048];
	UOSInt sizeLeft = recSize;
	Bool err = false;
	while (sizeLeft > 0)
	{
		if (!sectorData->ReadSector(sectorNum, dataBuff))
		{
			err = true;
			break;
		}
		if (sizeLeft > 2048)
		{
			MemCopyNO(recBuff, dataBuff, 2048);
			recBuff += 2048;
			sizeLeft -= 2048;
		}
		else
		{
			MemCopyNO(recBuff, dataBuff, sizeLeft);
			sizeLeft = 0;
			break;
		}
		sectorNum++;
	}
	if (!err)
	{
		Data::DateTime dt;
		UOSInt fileRecSize;
		UInt32 sectorNum;
		UInt32 fileSize;
		IO::IStreamData *fd;
		UTF8Char *sptr;
		Text::Encoding enc(codePage);

		recBuff = &dataBuff[2048];
		sizeLeft = recSize;
		while (sizeLeft > 32)
		{
			fileRecSize = recBuff[0];
			if (fileRecSize < 34)
				break;
			sectorNum = ReadUInt32(&recBuff[2]);
			fileSize = ReadUInt32(&recBuff[10]);
			dt.SetValue((UInt16)(1900 + recBuff[18]), recBuff[19], recBuff[20], recBuff[21], recBuff[22], recBuff[23], 0, (Int8)recBuff[24]);
			if (recBuff[25] & 2)
			{
				if (recBuff[32] == 1 && recBuff[33] == 0)
				{
				}
				else if (recBuff[32] == 1 && recBuff[33] == 1)
				{
				}
				else
				{
					IO::PackageFile *pf2;
					sptr = fileNameEnd;
					*sptr++ = IO::Path::PATH_SEPERATOR;
					sptr = enc.UTF8FromBytes(sptr, &recBuff[33], recBuff[32], 0);
					NEW_CLASS(pf2, IO::PackageFile(CSTRP(fileName, sptr)));
					pkgFile->AddPack(pf2, CSTRP(&fileNameEnd[1], sptr), dt.ToTicks());
					ParseDir(pf2, sectorData, sectorNum, fileSize, fileName, sptr, codePage);
				}
			}
			else
			{
				UOSInt i;
				sptr = enc.UTF8FromBytes(fileNameEnd, &recBuff[33], recBuff[32], 0);
				i = Text::StrIndexOfChar(fileNameEnd, ';');
				if (i != INVALID_INDEX)
				{
					fileNameEnd[i] = 0;
					sptr = &fileNameEnd[i];
				}
				fd = sectorData->GetStreamData(sectorNum, fileSize);
				pkgFile->AddData(fd, 0, fileSize, CSTRP(fileNameEnd, sptr), dt.ToTicks());
				DEL_CLASS(fd);
			}

			sizeLeft -= fileRecSize;
			recBuff += fileRecSize;
		}
	}
	MemFree(dataBuff);
}
