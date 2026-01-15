#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
#include "IO/Path.h"
#include "IO/SPackageFile.h"
#include "IO/VirtualPackageFileFast.h"
#include "Map/TileMapLayer.h"
#include "Map/OSM/OSMTileMap.h"
#include "Parser/FileParser/SPKParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::SPKParser::SPKParser()
{
	this->clif = nullptr;
	this->parsers = nullptr;
}

Parser::FileParser::SPKParser::~SPKParser()
{
}

Int32 Parser::FileParser::SPKParser::GetName()
{
	return *(Int32*)"SPKP";
}

void Parser::FileParser::SPKParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::SPKParser::SetTCPClientFactory(NN<Net::TCPClientFactory> clif)
{
	this->clif = clif;
}

void Parser::FileParser::SPKParser::SetSSLEngine(Optional<Net::SSLEngine> ssl)
{
	this->ssl = ssl;
}

void Parser::FileParser::SPKParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::SPKParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 dirOfst;
	UInt64 fileSize;
	UInt32 fnameLen;
	Int32 flags;
	UIntOS i;
	UIntOS j;
	UTF8Char fileName[256];
	UTF8Char srcPath[512];
	UnsafeArray<UTF8Char> srcPtr;
	UnsafeArray<UTF8Char> srcPtr2;

	if (hdr[0] != 'S' || hdr[1] != 'm' || hdr[2] != 'p' || hdr[3] != 'f')
		return nullptr;

	flags = ReadInt32(&hdr[4]);
	dirOfst = ReadUInt64(&hdr[8]);
	fileSize = fd->GetDataSize();
	if (dirOfst < 16 || dirOfst > fileSize)
		return nullptr;
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
		NN<Net::TCPClientFactory> clif;
		NN<Parser::ParserList> parsers;
		if (customType == 1 && fd->IsFullFile() && this->clif.SetTo(clif) && this->parsers.SetTo(parsers))
		{
			Data::ByteBuffer customBuff(customSize);
			IO::SPackageFile *spkg;
			NN<Map::OSM::OSMTileMap> tileMap;
			fd->GetRealData(i + 8, customSize, customBuff);
			MemCopyNO(srcPath, &customBuff[2], customBuff[1]);
			srcPath[customBuff[1]] = 0;
			j = 2 + (UIntOS)customBuff[1];
			NEW_CLASS(spkg, IO::SPackageFile(fd->GetFullFileName()->ToCString()));
			NEW_CLASSNN(tileMap, Map::OSM::OSMTileMap({srcPath, customBuff[1]}, spkg, 0, 18, clif, this->ssl));
			i = 1;
			while (i < customBuff[0])
			{
				MemCopyNO(srcPath, &customBuff[j + 1], customBuff[j]);
				srcPath[customBuff[j]] = 0;
				tileMap->AddAlternateURL({srcPath, customBuff[j]});
				i++;
			}
			Map::TileMapLayer *layer;
			NEW_CLASS(layer, Map::TileMapLayer(tileMap, parsers));
			return layer;
		}
	}

	IO::VirtualPackageFile *pf;
	IO::VirtualPackageFile *pf2;
	NN<IO::PackageFile> pf3;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	srcPtr = fd->GetFullName()->ConcatTo(srcPath);
	UIntOS k;
	UIntOS l;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	if (flags & 2)
	{
		UInt64 dirSize;
		dirSize = ReadUInt64(&hdr[16]);
		while (dirOfst != 0 && dirSize >= 16 && dirOfst + dirSize <= fileSize)
		{
			Data::ByteBuffer dirBuff((UIntOS)dirSize);
			fd->GetRealData(dirOfst, (UIntOS)dirSize, dirBuff);
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
						if (!pf2->GetPackFile({sptr, k}).SetTo(pf3))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(srcPath, srcPtr2)));
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
						if (!pf2->GetPackFile({sptr, l}).SetTo(pf3))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, l}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), IO::PackFileItem::HeaderType::No, CSTRP(sptr, sptrEnd), Data::Timestamp(ReadInt64(&dirBuff[i + 16]), 0), 0, 0, 0);
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
			j = (UIntOS)(fileSize - dirOfst);
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
						if (!pf2->GetPackFile({sptr, k}).SetTo(pf3))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(srcPath, srcPtr2)));
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
						if (!pf2->GetPackFile({sptr, l}).SetTo(pf3))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(srcPath, srcPtr2)));
							pf2->AddPack(pf3, {sptr, l}, 0, 0, 0, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[l + 1];
					}
					else
					{
						pf2->AddData(fd, ReadUInt64(&dirBuff[i]), ReadUInt64(&dirBuff[i + 8]), IO::PackFileItem::HeaderType::No, CSTRP(sptr, sptrEnd), Data::Timestamp(ReadInt64(&dirBuff[i + 16]), 0), 0, 0, 0);
						break;
					}
				}
				

				i += 26 + fnameLen;
			}
		}
	}
	return pf;
}
