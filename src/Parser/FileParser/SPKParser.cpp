#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "IO/SPackageFile.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Parser/FileParser/SPKParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::SPKParser::SPKParser()
{
	this->sockf = 0;
	this->parsers = 0;
}

Parser::FileParser::SPKParser::~SPKParser()
{
}

Int32 Parser::FileParser::SPKParser::GetName()
{
	return *(Int32*)"SPKP";
}

void Parser::FileParser::SPKParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::SPKParser::SetSocketFactory(Net::SocketFactory *sockf)
{
	this->sockf = sockf;
}

void Parser::FileParser::SPKParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.spk", (const UTF8Char*)"SPackage File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::SPKParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::SPKParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[24];
	UInt64 dirOfst;
	UInt64 fileSize;
	UInt32 fnameLen;
	Int32 flags;
	UOSInt i;
	UOSInt j;
	UTF8Char fileName[256];
	UTF8Char srcPath[512];
	UTF8Char *srcPtr;
	UTF8Char *srcPtr2;

	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (hdrBuff[0] != 'S' || hdrBuff[1] != 'm' || hdrBuff[2] != 'p' || hdrBuff[3] != 'f')
		return 0;

	flags = ReadInt32(&hdrBuff[4]);
	dirOfst = ReadUInt64(&hdrBuff[8]);
	fileSize = fd->GetDataSize();
	if (dirOfst < 16 || dirOfst > fileSize)
		return 0;
	if (flags & 1 && targetType != IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		Int32 customType;
		UInt32 customSize;
		if (flags & 2)
		{
			i = 24;
		}
		else
		{
			i = 16;
		}
		fd->GetRealData(i, 8, hdrBuff);
		customType = ReadInt32(&hdrBuff[0]);
		customSize = ReadUInt32(&hdrBuff[4]);
		if (customType == 1 && fd->IsFullFile() && this->sockf && this->parsers)
		{
			UInt8 *customBuff = MemAlloc(UInt8, customSize);
			IO::SPackageFile *spkg;
			Map::OSM::OSMTileMap *tileMap;
			fd->GetRealData(i + 8, customSize, customBuff);
			MemCopyNO(srcPath, &customBuff[2], customBuff[1]);
			srcPath[customBuff[1]] = 0;
			j = 2 + (UOSInt)customBuff[1];
			NEW_CLASS(spkg, IO::SPackageFile(fd->GetFullFileName()));
			NEW_CLASS(tileMap, Map::OSM::OSMTileMap(srcPath, spkg, 18, this->sockf));
			i = 1;
			while (i < customBuff[0])
			{
				MemCopyNO(srcPath, &customBuff[j + 1], customBuff[j]);
				srcPath[customBuff[j]] = 0;
				tileMap->AddAlternateURL(srcPath);
				i++;
			}
			MemFree(customBuff);
			Map::TileMapLayer *layer;
			NEW_CLASS(layer, Map::TileMapLayer(tileMap, this->parsers));
			return layer;
		}
	}

	IO::PackageFile *pf;
	IO::PackageFile *pf2;
	IO::PackageFile *pf3;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	srcPtr = Text::StrConcat(srcPath, fd->GetFullName());
	OSInt k;
	OSInt l;
	UTF8Char *sptr;
	UInt8 *dirBuff;
	if (flags & 2)
	{
		UInt64 dirSize;
		fd->GetRealData(16, 8, &hdrBuff[16]);
		dirSize = ReadUInt64(&hdrBuff[16]);
		while (dirOfst != 0 && dirSize >= 16 && dirOfst + dirSize <= fileSize)
		{
			dirBuff = MemAlloc(UInt8, (UOSInt)dirSize);
			fd->GetRealData(dirOfst, (UOSInt)dirSize, dirBuff);
			i = 16;
			while (i < dirSize)
			{
				fnameLen = ReadUInt16(&dirBuff[i + 24]);
				if (i + 26 + fnameLen > dirSize)
					break;

				MemCopyNO(fileName, &dirBuff[i + 26], fnameLen);
				fileName[fnameLen] = 0;
				sptr = fileName;
				srcPtr2 = srcPtr;
				pf2 = pf;
				while (true)
				{
					k = Text::StrIndexOf(sptr, '/');
					l = Text::StrIndexOf(sptr, '\\');
					if (k >= 0)
					{
						sptr[k] = 0;
						*srcPtr2++ = IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcat(srcPtr2, sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(srcPath));
							pf2->AddPack(pf3, sptr, 0);
						}
						pf2 = pf3;
						sptr = &sptr[k + 1];
					}
					else if (l >= 0)
					{
						sptr[l] = 0;
						*srcPtr2++ = IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcat(srcPtr2, sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(srcPath));
							pf2->AddPack(pf3, sptr, 0);
						}
						pf2 = pf3;
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), sptr, ReadInt64(&dirBuff[i + 16]));
						break;
					}
				}
				

				i += 26 + fnameLen;
			}
			dirOfst = ReadUInt64(&dirBuff[0]);
			dirSize = ReadUInt64(&dirBuff[8]);
			MemFree(dirBuff);
			
		}
	}
	else
	{
		if (dirOfst < fileSize)
		{
			j = (UOSInt)(fileSize - dirOfst);
			dirBuff = MemAlloc(UInt8, j);
			fd->GetRealData(dirOfst, j, dirBuff);
			i = 0;
			while (i < j)
			{
				fnameLen = ReadUInt16(&dirBuff[i + 24]);
				if (i + 26 + fnameLen > j)
					break;

				MemCopyNO(fileName, &dirBuff[i + 26], fnameLen);
				fileName[fnameLen] = 0;
				sptr = fileName;
				srcPtr2 = srcPtr;
				pf2 = pf;
				while (true)
				{
					k = Text::StrIndexOf(sptr, '/');
					l = Text::StrIndexOf(sptr, '\\');
					if (k >= 0)
					{
						sptr[k] = 0;
						*srcPtr2++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcat(srcPtr2, sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(srcPath));
							pf2->AddPack(pf3, sptr, 0);
						}
						pf2 = pf3;
						sptr = &sptr[k + 1];
					}
					else if (l >= 0)
					{
						sptr[l] = 0;
						*srcPtr2++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcat(srcPtr2, sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(srcPath));
							pf2->AddPack(pf3, sptr, 0);
						}
						pf2 = pf3;
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), sptr, ReadInt64(&dirBuff[i + 16]));
						break;
					}
				}
				

				i += 26 + fnameLen;
			}

			MemFree(dirBuff);
		}
	}
	return pf;
}
