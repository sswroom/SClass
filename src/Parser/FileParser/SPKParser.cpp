#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "IO/SPackageFile.h"
#include "IO/VirtualPackageFile.h"
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

void Parser::FileParser::SPKParser::SetSocketFactory(NotNullPtr<Net::SocketFactory> sockf)
{
	this->sockf = sockf.Ptr();
}

void Parser::FileParser::SPKParser::SetSSLEngine(Net::SSLEngine *ssl)
{
	this->ssl = ssl;
}

void Parser::FileParser::SPKParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.spk"), CSTR("SPackage File"));
	}
}

IO::ParserType Parser::FileParser::SPKParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::SPKParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
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

	if (hdr[0] != 'S' || hdr[1] != 'm' || hdr[2] != 'p' || hdr[3] != 'f')
		return 0;

	flags = ReadInt32(&hdr[4]);
	dirOfst = ReadUInt64(&hdr[8]);
	fileSize = fd->GetDataSize();
	if (dirOfst < 16 || dirOfst > fileSize)
		return 0;
	if (flags & 1 && targetType != IO::ParserType::PackageFile)
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
		customType = ReadInt32(&hdr[i + 0]);
		customSize = ReadUInt32(&hdr[i + 4]);
		NotNullPtr<Net::SocketFactory> sockf;
		if (customType == 1 && fd->IsFullFile() && sockf.Set(this->sockf) && this->parsers)
		{
			Data::ByteBuffer customBuff(customSize);
			IO::SPackageFile *spkg;
			NotNullPtr<Map::OSM::OSMTileMap> tileMap;
			fd->GetRealData(i + 8, customSize, customBuff);
			MemCopyNO(srcPath, &customBuff[2], customBuff[1]);
			srcPath[customBuff[1]] = 0;
			j = 2 + (UOSInt)customBuff[1];
			NEW_CLASS(spkg, IO::SPackageFile(fd->GetFullFileName()->ToCString()));
			NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap({srcPath, customBuff[1]}, spkg, 0, 18, sockf, this->ssl));
			i = 1;
			while (i < customBuff[0])
			{
				MemCopyNO(srcPath, &customBuff[j + 1], customBuff[j]);
				srcPath[customBuff[j]] = 0;
				tileMap->AddAlternateURL({srcPath, customBuff[j]});
				i++;
			}
			Map::TileMapLayer *layer;
			NEW_CLASS(layer, Map::TileMapLayer(tileMap, this->parsers));
			return layer;
		}
	}

	IO::VirtualPackageFile *pf;
	IO::VirtualPackageFile *pf2;
	NotNullPtr<IO::PackageFile> pf3;
	NEW_CLASS(pf, IO::VirtualPackageFile(fd->GetFullName()));
	srcPtr = fd->GetFullName()->ConcatTo(srcPath);
	UOSInt k;
	UOSInt l;
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	if (flags & 2)
	{
		UInt64 dirSize;
		dirSize = ReadUInt64(&hdr[16]);
		while (dirOfst != 0 && dirSize >= 16 && dirOfst + dirSize <= fileSize)
		{
			Data::ByteBuffer dirBuff((UOSInt)dirSize);
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
				sptrEnd = sptr + fnameLen;
				srcPtr2 = srcPtr;
				pf2 = pf;
				while (true)
				{
					if ((k = Text::StrIndexOfChar(sptr, '/')) != INVALID_INDEX)
					{
						sptr[k] = 0;
						*srcPtr2++ = IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcatC(srcPtr2, sptr, k);
						if (!pf3.Set(pf2->GetPackFile({sptr, k})))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFile(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, k}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[k + 1];
					}
					else if ((l = Text::StrIndexOfChar(sptr, '\\')) != INVALID_INDEX)
					{
						sptr[l] = 0;
						*srcPtr2++ = IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcatC(srcPtr2, sptr, l);
						if (!pf3.Set(pf2->GetPackFile({sptr, l})))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFile(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, l}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), CSTRP(sptr, sptrEnd), Data::Timestamp(ReadInt64(&dirBuff[i + 16]), 0), 0, 0, 0);
						break;
					}
				}
				

				i += 26 + fnameLen;
			}
			dirOfst = ReadUInt64(&dirBuff[0]);
			dirSize = ReadUInt64(&dirBuff[8]);
		}
	}
	else
	{
		if (dirOfst < fileSize)
		{
			j = (UOSInt)(fileSize - dirOfst);
			Data::ByteBuffer dirBuff(j);
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
				sptrEnd = sptr + fnameLen;
				srcPtr2 = srcPtr;
				pf2 = pf;
				while (true)
				{
					if ((k = Text::StrIndexOfChar(sptr, '/')) != INVALID_INDEX)
					{
						sptr[k] = 0;
						*srcPtr2++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcatC(srcPtr2, sptr, k);
						if (!pf3.Set(pf2->GetPackFile({sptr, k})))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFile(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, k}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[k + 1];
					}
					else if ((l = Text::StrIndexOfChar(sptr, '\\')) != INVALID_INDEX)
					{
						sptr[l] = 0;
						*srcPtr2++ = (UTF8Char)IO::Path::PATH_SEPERATOR;
						srcPtr2 = Text::StrConcatC(srcPtr2, sptr, l);
						if (!pf3.Set(pf2->GetPackFile({sptr, l})))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFile(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, l}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), CSTRP(sptr, sptrEnd), Data::Timestamp(ReadInt64(&dirBuff[i + 16]), 0), 0, 0, 0);
						break;
					}
				}
				

				i += 26 + fnameLen;
			}
		}
	}
	return pf;
}
