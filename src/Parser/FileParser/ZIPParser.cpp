#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/StringUTF8Map.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "Parser/FileParser/XMLParser.h"
#include "Parser/FileParser/ZIPParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::ZIPParser::ZIPParser()
{
	this->codePage = 65001;
	this->parsers = 0;
	this->encFact = 0;
}

Parser::FileParser::ZIPParser::~ZIPParser()
{
}

Int32 Parser::FileParser::ZIPParser::GetName()
{
	return *(Int32*)"ZIPP";
}

void Parser::FileParser::ZIPParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::ZIPParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::ZIPParser::SetEncFactory(Text::EncodingFactory *encFact)
{
	this->encFact = encFact;
}

void Parser::FileParser::ZIPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter((const UTF8Char*)"*.zip", (const UTF8Char*)"ZIP File");
	}
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.kmz", (const UTF8Char*)"KMZ File");
	}
}

IO::ParserType Parser::FileParser::ZIPParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

typedef struct
{
	Int32 crc;
	UInt32 fnameSize;
	UInt64 compSize;
	UInt64 decSize;
	UInt32 extraSize;
	UInt32 commentSize;
} ZIPInfoEntry;

IO::ParsedObject *Parser::FileParser::ZIPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UInt8 buff[512];
	UOSInt i;
	UInt64 currOfst;
	UInt64 fileSize = fd->GetDataSize();

	fd->GetRealData(0, 512, buff);
	if (ReadInt32(buff) != 0x04034b50)
	{
		return 0;
	}
	IO::PackageFile *pf;
	IO::PackageFile *pf2;
	IO::PackageFile *pf3;
	Text::Encoding enc(this->codePage);
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	Data::StringUTF8Map<UInt64> ofsts;
	Data::StringUTF8Map<ZIPInfoEntry*> zipInfos;
	ZIPInfoEntry *zipInfo;
	ZIPInfoEntry *zipInfo2;
	dt.ToLocalTime();
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	fd->GetRealData(fileSize - 22, 22, buff);
	if (ReadInt32(buff) == 0x06054b50)
	{
		currOfst = ReadUInt32(&buff[16]);
		if (currOfst > 0 && currOfst < fileSize - 6)
		{
			while (true)
			{
				if (currOfst >= fileSize)
				{
					break;
				}
				fd->GetRealData(currOfst, 512, buff);
				if (ReadInt32(buff) == 0x08074b50)
				{
					currOfst += 16;
				}
				else if (ReadInt32(buff) == 0x02014b50)
				{
					UInt16 flags = ReadUInt16(&buff[8]);
					zipInfo = MemAlloc(ZIPInfoEntry, 1);
					zipInfo->crc = ReadInt32(&buff[16]);
					zipInfo->compSize = ReadUInt32(&buff[20]);
					zipInfo->decSize = ReadUInt32(&buff[24]);
					zipInfo->fnameSize = ReadUInt16(&buff[28]);
					zipInfo->extraSize = ReadUInt16(&buff[30]);
					zipInfo->commentSize = ReadUInt16(&buff[32]);
					if (flags & 0x800)
					{
						Text::StrConcatC(sbuff, &buff[46], zipInfo->fnameSize);
					}
					else
					{
						enc.UTF8FromBytes(sbuff, &buff[46], zipInfo->fnameSize, 0);
					}
					zipInfo2 = zipInfos.Put(sbuff, zipInfo);
					if (zipInfo2)
					{
						MemFree(zipInfo2);
					}
					currOfst += 46 + zipInfo->fnameSize + zipInfo->extraSize + zipInfo->commentSize;
				}
				else if (ReadInt32(buff) == 0x06054b50)
				{
					break;
				}
				else
				{
					break;
				}
			}
		}
	}

	currOfst = 0;
	while (true)
	{
		if (currOfst >= fileSize)
		{
			DEL_CLASS(pf);
			return 0;
		}
		fd->GetRealData(currOfst, 512, buff);
		if (ReadInt32(buff) == 0x04034b50)
		{
			UInt32 fnameSize = ReadUInt16(&buff[26]);
			UInt32 extraSize = ReadUInt16(&buff[28]);
			UInt32 compMeth = ReadUInt16(&buff[8]);
			UInt64 dataSize = ReadUInt32(&buff[18]);
//			UInt64 decompSize = ReadUInt32(&buff[22]);
			UInt16 modTime = ReadUInt16(&buff[10]);
			UInt16 modDate = ReadUInt16(&buff[12]);
			dt.SetMSDOSTime(modDate, modTime);
			OSInt extraStart = 30 + fnameSize;
			OSInt extraEnd = extraStart + extraSize;
			UInt16 extraHdr;
			UInt16 extraData;
			while (extraStart < extraEnd)
			{
				extraHdr = ReadUInt16(&buff[extraStart]);
				extraData = ReadUInt16(&buff[extraStart + 2]);
				if (extraHdr == 0x5455)
				{
					dt.SetUnixTimestamp(ReadUInt32(&buff[extraStart + 5]));
				}
				else if (extraHdr == 1)
				{
					if (extraData >= 16)
					{
						dataSize = ReadUInt64(&buff[extraStart + 4]);
//						decompSize = ReadUInt64(&buff[extraStart + 12]);
					}
				}
				else
				{
					modDate = 0;
				}
				extraStart += extraData + 4;
			}
			if (buff[30 + fnameSize - 1] == '/')
			{
				enc.UTF8FromBytes(sbuff, &buff[30], fnameSize - 1, 0);
				zipInfo = zipInfos.Get(sbuff);
				if (zipInfo && (zipInfo->compSize != 0xffffffff))
				{
					dataSize = zipInfo->compSize;
				}
				pf2 = pf;
				sptr = sbuff;
				sb.ClearStr();
				sb.Append(fd->GetFullName());
				while (true)
				{
					i = Text::StrIndexOf(sptr, '/');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb.Append(sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(sb.ToString()));
							pf2->AddPack(pf3, sptr, dt.ToTicks());
						}
						pf2 = pf3;
						sptr = &sptr[i + 1];
					}
					else
					{
						sb.Append((const UTF8Char*)"\\");
						sb.Append(sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(sb.ToString()));
							pf2->AddPack(pf3, sptr, dt.ToTicks());
						}
						break;
					}
				}
				currOfst += 30 + fnameSize + extraSize + dataSize;
			}
			else
			{
				IO::PackFileItem::CompressInfo compInfo;
				enc.UTF8FromBytes(sbuff, &buff[30], fnameSize, 0);
				ofsts.Put(sbuff, currOfst + 30 + fnameSize + extraSize);
				zipInfo = zipInfos.Get(sbuff);
				if (zipInfo && (zipInfo->compSize != 0xffffffff))
				{
					dataSize = zipInfo->compSize;
				}
				pf2 = pf;
				sptr = sbuff;
				sb.ClearStr();
				sb.Append(fd->GetFullName());
				while (true)
				{
					i = Text::StrIndexOf(sptr, '/');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
						sb.Append(sptr);
						pf3 = pf2->GetPackFile(sptr);
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(sb.ToString()));
							pf2->AddPack(pf3, sptr, dt.ToTicks());
						}
						pf2 = pf3;
						sptr = &sptr[i + 1];
					}
					else
					{
						break;
					}
				}

				if (compMeth == 0)
				{
					pf2->AddData(fd, currOfst + 30 + fnameSize + extraSize, dataSize, sptr, dt.ToTicks());
				}
				else
				{
					compInfo.checkMethod = Crypto::Hash::HT_CRC32R_IEEE;
					if (zipInfo)
					{
						WriteMInt32(compInfo.checkBytes, zipInfo->crc);
						compInfo.compExtras = 0;
						compInfo.compExtraSize = 0;
						compInfo.compFlags = 0;
						if (compMeth == 8)
						{
							compInfo.compMethod = Data::Compress::Decompressor::CM_DEFLATE;
						}
						else
						{
							compInfo.compMethod = Data::Compress::Decompressor::CM_UNKNOWN;
						}
						compInfo.decSize = zipInfo->decSize;
					}
					else
					{
						WriteMInt32(compInfo.checkBytes, ReadInt32(&buff[14]));
						compInfo.compExtras = 0;
						compInfo.compExtraSize = 0;
						compInfo.compFlags = 0;
						if (compMeth == 8)
						{
							compInfo.compMethod = Data::Compress::Decompressor::CM_DEFLATE;
						}
						else
						{
							compInfo.compMethod = Data::Compress::Decompressor::CM_UNKNOWN;
						}
						compInfo.decSize = ReadUInt32(&buff[22]);
					}
					pf2->AddCompData(fd, currOfst + 30 + fnameSize + extraSize, dataSize, &compInfo, sptr, dt.ToTicks());
				}
				currOfst += 30 + fnameSize + extraSize + dataSize;
			}
		}
		else if (ReadInt32(buff) == 0x02014b50)
		{
			UInt32 fnameSize = ReadUInt16(&buff[28]);
			UInt32 extraSize = ReadUInt16(&buff[30]);
			UInt32 commentSize = ReadUInt16(&buff[32]);
//			UInt32 dataSize = ReadUInt32(&buff[20]);
			currOfst += 46 + fnameSize + extraSize + commentSize;
		}
		else if (ReadInt32(buff) == 0x06054b50)
		{
			break;
		}
		else if (ReadInt32(buff) == 0x08074b50)
		{
			currOfst += 16;
		}
		else
		{
			currOfst = 0;
			break;
		}
	}
	Data::ArrayList<ZIPInfoEntry*> *zipInfoList = zipInfos.GetValues();
	UOSInt ui = zipInfoList->GetCount();
	while (ui-- > 0)
	{
		zipInfo = zipInfoList->GetItem(ui);
		MemFree(zipInfo);
	}

	if (targetType == IO::ParserType::MapLayer || targetType == IO::ParserType::Unknown)
	{
		ui = pf->GetCount();
		while (ui-- > 0)
		{
			pf->GetItemName(sbuff, ui);
			if (Text::StrEquals(sbuff, (const UTF8Char*)"doc.kml") && pf->GetItemType(ui) == IO::PackageFile::POT_STREAMDATA)
			{
				IO::IStreamData *stmData = pf->GetItemStmData(0);
				Parser::FileParser::XMLParser xmlParser;
				xmlParser.SetParserList(this->parsers);
				xmlParser.SetEncFactory(this->encFact);
				IO::ParsedObject *pobj = xmlParser.ParseFile(stmData, pf, IO::ParserType::MapLayer);
				DEL_CLASS(stmData);
				if (pobj)
				{
					if (pobj->GetParserType() == IO::ParserType::MapLayer)
					{
						DEL_CLASS(pf);
						return pobj;
					}
					else
					{
						DEL_CLASS(pobj);
					}
				}
			}
		}
	}
	return pf;
}
