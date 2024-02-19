#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Data/StringUTF8Map.h"
#include "IO/Path.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/ParserList.h"
#include "Parser/FileParser/XMLParser.h"
#include "Parser/FileParser/ZIPParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

#include <stdio.h>

Parser::FileParser::ZIPParser::ZIPParser()
{
	this->codePage = 65001;
	this->parsers = 0;
	this->encFact = 0;
	this->browser = 0;
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

void Parser::FileParser::ZIPParser::SetWebBrowser(Net::WebBrowser *browser)
{
	this->browser = browser;
}

void Parser::FileParser::ZIPParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::ZIPParser::SetEncFactory(Text::EncodingFactory *encFact)
{
	this->encFact = encFact;
}

void Parser::FileParser::ZIPParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.zip"), CSTR("ZIP File"));
	}
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.kmz"), CSTR("KMZ File"));
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

IO::ParsedObject *Parser::FileParser::ZIPParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UInt8 buff[512];
	UInt8 recHdr[64];
	UInt8 z64eocdl[20];
	UInt8 z64eocd[56];
	UInt64 ofst;
	UInt32 recType;
	UOSInt i;
	UInt64 currOfst;
	UInt64 fileSize = fd->GetDataSize();

	if (ReadInt32(hdr) != 0x04034b50)
	{
		return 0;
	}
	NotNullPtr<IO::VirtualPackageFile> pf;
	IO::VirtualPackageFile *pf2;
	NotNullPtr<IO::PackageFile> pf3;
	Text::Encoding enc(this->codePage);
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	Data::StringUTF8Map<UInt64> ofsts;
	UOSInt ui;
	Bool parseFile = true;
	dt.ToLocalTime();
	NEW_CLASSNN(pf, IO::VirtualPackageFileFast(fd->GetFullName()));

	if (fd->GetRealData(fileSize - 22, 22, BYTEARR(recHdr)) == 22)
	{
		recType = ReadMUInt32(recHdr);
		if (recType == 0x504B0506)
		{
			UInt32 sizeOfDir = ReadUInt32(&recHdr[12]);
			UInt32 ofstOfDir = ReadUInt32(&recHdr[16]);
			if (sizeOfDir == 0xffffffff || ofstOfDir == 0xffffffff)
			{
				fd->GetRealData(fileSize - 42, 20, BYTEARR(z64eocdl));

				if (ReadMUInt32(z64eocdl) == 0x504B0607)
				{
					UInt64 z64eocdOfst = ReadUInt64(&z64eocdl[8]);
					fd->GetRealData(z64eocdOfst, 56, BYTEARR(z64eocd));
					if (ReadMUInt32(z64eocd) == 0x504B0606)
					{
						UInt64 cdSize = ReadUInt64(&z64eocd[40]);
						UInt64 cdOfst = ReadUInt64(&z64eocd[48]);
						if (cdSize <= 1048576)
						{
							Data::ByteBuffer cdBuff((UOSInt)cdSize);
							fd->GetRealData(cdOfst, (UOSInt)cdSize, cdBuff);
							ParseCentDir(pf, &enc, fd, cdBuff, cdOfst);
						}
						else
						{
							Data::ByteBuffer cdBuff(1048576);
							UOSInt buffSize = 0;
							ofst = 0;
							while (ofst < cdSize)
							{
								if (1048576 < cdSize - ofst)
								{
									i = fd->GetRealData(cdOfst + ofst + buffSize, 1048576 - buffSize, cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								else
								{
									i = fd->GetRealData(cdOfst + ofst + buffSize, (UOSInt)(cdSize - ofst), cdBuff.SubArray(buffSize));
									buffSize += i;
								}
								if (i == 0)
								{
									break;
								}
								i = ParseCentDir(pf, &enc, fd, cdBuff.WithSize(buffSize), cdOfst + ofst);
								if (i == 0)
								{
									break;
								}
								if (i == buffSize)
								{
									ofst += i;
									buffSize = 0;
								}
								else
								{
									ofst += i;
									cdBuff.CopyInner(0, i, buffSize - i);
									buffSize -= i;
								}
							}
						}

						parseFile = false;
					}
				}
			}
			else
			{
				Data::ByteBuffer centDir(sizeOfDir);
				if (fd->GetRealData(ofstOfDir, sizeOfDir, centDir) == sizeOfDir)
				{
					ParseCentDir(pf, &enc, fd, centDir, 0);
					parseFile = false;
				}
			}
		}
	}

	if (parseFile)
	{
		ZIPInfoEntry *zipInfo;
		ZIPInfoEntry *zipInfo2;
		Data::StringUTF8Map<ZIPInfoEntry*> zipInfos;

		printf("ZIPParser: Scan file\r\n");
		fd->GetRealData(fileSize - 22, 22, BYTEARR(buff));
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
					fd->GetRealData(currOfst, 512, BYTEARR(buff));
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
				pf.Delete();
				return 0;
			}
			fd->GetRealData(currOfst, 512, BYTEARR(buff));
			if (ReadInt32(buff) == 0x04034b50)
			{
				UInt32 fnameSize = ReadUInt16(&buff[26]);
				UInt32 extraSize = ReadUInt16(&buff[28]);
				UInt32 compMeth = ReadUInt16(&buff[8]);
				UInt64 dataSize = ReadUInt32(&buff[18]);
	//			UInt64 decompSize = ReadUInt32(&buff[22]);
				UInt16 modTime = ReadUInt16(&buff[10]);
				UInt16 modDate = ReadUInt16(&buff[12]);
				Data::Timestamp accTime = 0;
				Data::Timestamp createTime = 0;
				dt.ToLocalTime();
				dt.SetMSDOSTime(modDate, modTime);
				UOSInt extraStart = 30 + fnameSize;
				UOSInt extraEnd = extraStart + extraSize;
				UInt16 extraHdr;
				UInt16 extraData;
				UInt32 unixAttr = 0;
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
					else if (extraHdr == 10 && extraData >= 32 && ReadUInt16(&buff[extraStart + 8]) == 1)
					{
						dt.SetValueFILETIME(&buff[extraStart + 12]);
						accTime = Data::Timestamp::FromFILETIME(&buff[extraStart + 20], Data::DateTimeUtil::GetLocalTzQhr());
						createTime = Data::Timestamp::FromFILETIME(&buff[extraStart + 28], Data::DateTimeUtil::GetLocalTzQhr());
					}
					else
					{
						modDate = 0;
					}
					extraStart += (UOSInt)extraData + 4;
				}
				if (buff[30 + fnameSize - 1] == '/')
				{
					sptrEnd = enc.UTF8FromBytes(sbuff, &buff[30], fnameSize - 1, 0);
					zipInfo = zipInfos.Get(sbuff);
					if (zipInfo && (zipInfo->compSize != 0xffffffff))
					{
						dataSize = zipInfo->compSize;
					}
					pf2 = pf.Ptr();
					sptr = sbuff;
					sb.ClearStr();
					sb.Append(fd->GetFullName());
					while (true)
					{
						i = Text::StrIndexOfChar(sptr, '/');
						if (i != INVALID_INDEX)
						{
							sptr[i] = 0;
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendC(sptr, i);
							if (!pf3.Set(pf2->GetPackFile({sptr, i})))
							{
								NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
								pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), accTime, createTime, unixAttr);
							}
							pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
							sptr = &sptr[i + 1];
						}
						else
						{
							sb.AppendC(UTF8STRC("\\"));
							sb.AppendC(sptr, (UOSInt)(sptrEnd - sptr));
							if (!pf3.Set(pf2->GetPackFile({sptr, (UOSInt)(sptrEnd - sptr)})))
							{
								NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
								pf2->AddPack(pf3, CSTRP(sptr, sptrEnd), Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), accTime, createTime, unixAttr);
							}
							break;
						}
					}
					currOfst += 30 + fnameSize + extraSize + dataSize;
				}
				else
				{
					IO::PackFileItem::CompressInfo compInfo;
					sptrEnd = enc.UTF8FromBytes(sbuff, &buff[30], fnameSize, 0);
					ofsts.Put(sbuff, currOfst + 30 + fnameSize + extraSize);
					zipInfo = zipInfos.Get(sbuff);
					if (zipInfo && (zipInfo->compSize != 0xffffffff))
					{
						dataSize = zipInfo->compSize;
					}
					pf2 = pf.Ptr();
					sptr = sbuff;
					sb.ClearStr();
					sb.Append(fd->GetFullName());
					while (true)
					{
						i = Text::StrIndexOfChar(sptr, '/');
						if (i != INVALID_INDEX)
						{
							sptr[i] = 0;
							sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
							sb.AppendC(sptr, i);
							if (!pf3.Set(pf2->GetPackFile({sptr, i})))
							{
								NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
								pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), accTime, createTime, unixAttr);
							}
							pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
							sptr = &sptr[i + 1];
						}
						else
						{
							break;
						}
					}

					if (compMeth == 0)
					{
						pf2->AddData(fd, currOfst, dataSize, IO::PackFileItem::HeaderType::Zip, 30 + fnameSize + extraSize, CSTRP(sptr, sptrEnd), Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), accTime, createTime, unixAttr);
					}
					else
					{
						compInfo.checkMethod = Crypto::Hash::HashType::CRC32R_IEEE;
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
						pf2->AddCompData(fd, currOfst, dataSize, IO::PackFileItem::HeaderType::Zip, 30 + fnameSize + extraSize, &compInfo, CSTRP(sptr, sptrEnd), Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), accTime, createTime, unixAttr);
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
		NotNullPtr<const Data::ArrayList<ZIPInfoEntry*>> zipInfoList = zipInfos.GetValues();
		ui = zipInfoList->GetCount();
		while (ui-- > 0)
		{
			zipInfo = zipInfoList->GetItem(ui);
			MemFree(zipInfo);
		}
	}

	if (targetType == IO::ParserType::MapLayer || targetType == IO::ParserType::Unknown)
	{
		ui = pf->GetCount();
		while (ui-- > 0)
		{
			sptr = pf->GetItemName(sbuff, ui);
			if (Text::StrEndsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".kml")) && pf->GetItemType(ui) == IO::PackageFile::PackObjectType::StreamData)
			{
				NotNullPtr<IO::StreamData> stmData;
				IO::ParsedObject *pobj = 0;
				if (pf->GetItemStmDataNew(ui).SetTo(stmData))
				{
					Parser::FileParser::XMLParser xmlParser;
					xmlParser.SetParserList(this->parsers);
					xmlParser.SetEncFactory(this->encFact);
					xmlParser.SetWebBrowser(this->browser);
					pobj = xmlParser.ParseFile(stmData, pf.Ptr(), IO::ParserType::MapLayer);
					stmData.Delete();
				}
				if (pobj)
				{
					if (pobj->GetParserType() == IO::ParserType::MapLayer)
					{
						pf.Delete();
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
	if (targetType != IO::ParserType::PackageFile && this->parsers)
	{
		IO::ParsedObject *newObj = this->parsers->ParseObjectType(pf, targetType);
		if (newObj)
		{
			pf.Delete();
			return newObj;
		}
	}
	return pf.Ptr();
}

UOSInt Parser::FileParser::ZIPParser::ParseCentDir(NotNullPtr<IO::VirtualPackageFile> pf, Text::Encoding *enc, NotNullPtr<IO::StreamData> fd, Data::ByteArrayR buff, UInt64 ofst)
{
	IO::VirtualPackageFile *pf2;
	NotNullPtr<IO::PackageFile> pf3;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UInt16 flags;
	UInt16 compMeth;
	UInt64 compSize;
	UInt64 uncompSize;
	UInt16 fnameLen;
	UInt16 extraLen;
	UInt16 commentLen;
	UInt32 recType;
	UInt32 extAttr;
	UInt32 unixAttr;
	Data::Timestamp modTime;
	Data::Timestamp accTime;
	Data::Timestamp createTime;
	UOSInt i;
	UOSInt j;
	i = 0;
	while (i < buff.GetSize())
	{
		if (i + 46 > buff.GetSize())
		{
			return i;
		}
		recType = ReadMUInt32(&buff[i]);
		if (recType != 0x504B0102)
		{
			break;
		}
		flags = ReadUInt16(&buff[i + 8]);
		compMeth = ReadUInt16(&buff[i + 10]);
		compSize = ReadUInt32(&buff[i + 20]);
		uncompSize = ReadUInt32(&buff[i + 24]);
		fnameLen = ReadUInt16(&buff[i + 28]);
		extraLen = ReadUInt16(&buff[i + 30]);
		commentLen = ReadUInt16(&buff[i + 32]);
		extAttr = ReadUInt32(&buff[i + 38]);
		if (buff[i + 7] == 3)
		{
			unixAttr = extAttr >> 16;
		}
		else
		{
			unixAttr = 0x124;
			if ((extAttr & 1) == 0)
				unixAttr |= 0x92;
			if (extAttr & 16)
				unixAttr |= 0x4000 | 0x49;
		}
		ofst = ReadUInt32(&buff[i + 42]);
		modTime = Data::Timestamp::FromMSDOSTime(ReadUInt16(&buff[i + 14]), ReadUInt16(&buff[i + 12]), Data::DateTimeUtil::GetLocalTzQhr());
		accTime = 0;
		createTime = 0;

		if (i + 46 + (UOSInt)fnameLen + extraLen + commentLen > buff.GetSize())
		{
			return i;
		}
		if (extraLen > 0)
		{
			const UInt8 *extraBuff = &buff[i + 46 + (UOSInt)fnameLen];
			j = 0;
			UInt16 extraTag;
			UInt16 extraSize;
			while (j + 4 <= extraLen)
			{
				extraTag = ReadUInt16(&extraBuff[j]);
				extraSize = ReadUInt16(&extraBuff[j + 2]);
				if (extraTag == 1)
				{
					const UInt8 *zip64Info = &extraBuff[j + 4];
					if (uncompSize == 0xffffffff)
					{
						uncompSize = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
					if (compSize == 0xffffffff)
					{
						compSize = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
					if (ofst == 0xffffffff)
					{
						ofst = ReadUInt64(zip64Info);
						zip64Info += 8;
					}
				}
				else if (extraTag == 0x5455)
				{
					modTime = Data::Timestamp(ReadUInt32(&extraBuff[j + 5]), Data::DateTimeUtil::GetLocalTzQhr());
				}
				else if (extraTag == 10 && extraSize >= 32 && ReadUInt16(&extraBuff[j + 8]) == 1)
				{
					modTime = Data::Timestamp::FromFILETIME(&extraBuff[j + 12], Data::DateTimeUtil::GetLocalTzQhr());
					accTime = Data::Timestamp::FromFILETIME(&extraBuff[j + 20], Data::DateTimeUtil::GetLocalTzQhr());
					createTime = Data::Timestamp::FromFILETIME(&extraBuff[j + 20], Data::DateTimeUtil::GetLocalTzQhr());
				}
				j += 4 + (UOSInt)extraSize;
			}
		}
		if (flags & 0x800)
		{
			sptrEnd = Text::StrConcatC(sbuff, &buff[i + 46], fnameLen);
		}
		else
		{
			sptrEnd = enc->UTF8FromBytes(sbuff, &buff[i + 46], fnameLen, 0);
		}
		if (sptrEnd[-1] == '/')
		{
			sptrEnd--;
			*sptrEnd = 0;
			pf2 = pf.Ptr();
			sptr = sbuff;
			while (true)
			{
				j = Text::StrIndexOfChar(sptr, '/');
				if (j != INVALID_INDEX)
				{
					sptr[j] = 0;
					if (!pf3.Set(pf2->GetPackFile({sptr, j})))
					{
						NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(sbuff, &sptr[j])));
						pf2->AddPack(pf3, {sptr, j}, modTime, accTime, createTime, unixAttr);
					}
					pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
					sptr[j] = '/';
					sptr = &sptr[j + 1];
				}
				else
				{
					if (!pf3.Set(pf2->GetPackFile({sptr, (UOSInt)(sptrEnd - sptr)})))
					{
						NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(sbuff, sptrEnd)));
						pf2->AddPack(pf3, CSTRP(sptr, sptrEnd), modTime, accTime, createTime, unixAttr);
					}
					break;
				}
			}
		}
		else
		{
			IO::PackFileItem::CompressInfo compInfo;
			pf2 = pf.Ptr();
			sptr = sbuff;
			while (true)
			{
				j = Text::StrIndexOfChar(sptr, '/');
				if (j != INVALID_INDEX)
				{
					sptr[j] = 0;
					if (!pf3.Set(pf2->GetPackFile({sptr, j})))
					{
						NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(CSTRP(sbuff, &sptr[j])));
						pf2->AddPack(pf3, {sptr, j}, modTime, accTime, createTime, unixAttr);
					}
					pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
					sptr[j] = '/';
					sptr = &sptr[j + 1];
				}
				else
				{
					break;
				}
			}
			UOSInt hdrLen = 30 + fnameLen;
			if (compSize >= 0xffffffffLL || uncompSize >= 0xffffffffLL)
			{
				hdrLen += 4;
				if (compSize >= 0xffffffffLL)
					hdrLen += 8;
				if (uncompSize >= 0xffffffffLL)
					hdrLen += 8;
			}
			if (compMeth == 0)
			{
				pf2->AddData(fd, ofst, compSize, IO::PackFileItem::HeaderType::Zip, hdrLen, CSTRP(sptr, sptrEnd), modTime, accTime, createTime, unixAttr);
			}
			else
			{
				compInfo.checkMethod = Crypto::Hash::HashType::CRC32R_IEEE;
				WriteMInt32(compInfo.checkBytes, ReadInt32(&buff[i + 16]));
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
				compInfo.decSize = uncompSize;
				pf2->AddCompData(fd, ofst, compSize, IO::PackFileItem::HeaderType::Zip, hdrLen, &compInfo, CSTRP(sptr, sptrEnd), modTime, accTime, createTime, unixAttr);
			}
		}

		i += 46 + (UOSInt)fnameLen + extraLen + commentLen;
	}
	return i;
}
