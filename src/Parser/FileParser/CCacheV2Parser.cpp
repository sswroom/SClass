#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/VirtualPackageFileFast.h"
#include "IO/StmData/FileData.h"
#include "Parser/FileParser/CCacheV2Parser.h"

Parser::FileParser::CCacheV2Parser::CCacheV2Parser()
{
}

Parser::FileParser::CCacheV2Parser::~CCacheV2Parser()
{
}

Int32 Parser::FileParser::CCacheV2Parser::GetName()
{
	return *(Int32*)"CCV2";
}

void Parser::FileParser::CCacheV2Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.bundle"), CSTR("Compact Cache V2 File"));
	}
}

IO::ParserType Parser::FileParser::CCacheV2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::CCacheV2Parser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->GetFullFileName()->EndsWith(CSTR(".bundle")))
	{
		return nullptr;
	}
	if (ReadUInt32(&hdr[0]) != 3)
		return nullptr;
	if (ReadUInt64(&hdr[24]) != fd->GetDataSize())
	{
		return nullptr;
	}
	if (ReadUInt32(&hdr[40]) != 0x20014)
	{
		return nullptr;
	}
	NN<IO::VirtualPackageFile> pkg;
	NEW_CLASSNN(pkg, IO::VirtualPackageFileFast(fd->GetFullFileName()));
	UIntOS rOfst = 0;
	UIntOS cOfst = 0;
	Text::CStringNN fileName;
	if (fd->GetShortName().SetTo(fileName))
	{
		if (fileName.leng == 17 && fileName.v[0] == 'R' && fileName.v[5] == 'C' && fileName.v[10] == '.')
		{
			rOfst = Text::StrHex2UInt16C(&fileName.v[1]);
			cOfst = Text::StrHex2UInt16C(&fileName.v[6]);
		}
	}
	if (!ParseAppend(fd, hdr, pkg, rOfst, cOfst))
	{
		pkg.Delete();
		return nullptr;
	}
	return pkg;
}

Bool Parser::FileParser::CCacheV2Parser::AppendFile(Text::CStringNN filePath, NN<IO::VirtualPackageFile> pkgFile)
{
	IO::StmData::FileData fd(filePath, false);
	if (!fd.IsError())
	{
		UInt8 hdr[256];
		if (fd.GetRealData(0, 256, BYTEARR(hdr)) == 256)
		{
			UIntOS rOfst = 0;
			UIntOS cOfst = 0;
			Text::CStringNN fileName;
			if (fd.GetShortName().SetTo(fileName))
			{
				if (fileName.leng == 17 && fileName.v[0] == 'R' && fileName.v[5] == 'C' && fileName.v[10] == '.')
				{
					rOfst = Text::StrHex2UInt16C(&fileName.v[1]);
					cOfst = Text::StrHex2UInt16C(&fileName.v[6]);
				}
			}
			return ParseAppend(fd, BYTEARR(hdr), pkgFile, rOfst, cOfst);
		}
	}
	return false;
}

Bool Parser::FileParser::CCacheV2Parser::ParseAppend(NN<IO::StreamData> fd, Data::ByteArrayR hdr, NN<IO::VirtualPackageFile> pkgFile, UIntOS rOfst, UIntOS cOfst)
{
	if (ReadUInt32(&hdr[0]) != 3)
		return false;
	if (ReadUInt64(&hdr[24]) != fd->GetDataSize())
	{
		return false;
	}
	if (ReadUInt32(&hdr[40]) != 0x20014)
	{
		return false;
	}
	UInt8 index[0x20000];
	if (fd->GetRealData(64, 0x20000, BYTEARR(index)) != 0x20000)
		return 0;
	Text::StringBuilderUTF8 sb;
	Optional<IO::VirtualPackageFile> colPkg;
	NN<IO::VirtualPackageFile> nncolPkg;
	NN<IO::PackageFile> nnpkg;
	Data::Timestamp now = Data::Timestamp::Now();
	UInt64 idx;
	UInt64 tileOfst;
	UIntOS tileSize;
	UIntOS i = 0;
	UIntOS j;
	while (i < 128)
	{
		colPkg = nullptr;
		j = 0;
		while (j < 128)
		{
			idx = ReadUInt64(&index[i * 1024 + j * 8]);
			tileOfst = idx % 0x10000000000LL;
			tileSize = (UIntOS)(idx / 0x10000000000LL);
			if (tileSize != 0)
			{
				sb.ClearStr();
				sb.AppendUIntOS(j + cOfst);
				if (!pkgFile->GetPackFile(sb.ToCString()).SetTo(nnpkg))
				{
					NEW_CLASSNN(nncolPkg, IO::VirtualPackageFileFast(sb.ToCString()));
					colPkg = nncolPkg;
					pkgFile->AddPack(nncolPkg, sb.ToCString(), now, now, now, 0);
				}
				else
				{
					nncolPkg = NN<IO::VirtualPackageFile>::ConvertFrom(nnpkg);
					colPkg = nncolPkg;
				}
				sb.ClearStr();
				sb.AppendUIntOS(i + rOfst);
				sb.Append(CSTR(".png"));
				nncolPkg->AddData(fd, tileOfst, tileSize, IO::PackFileItem::HeaderType::No, sb.ToCString(), now, now, now, 0);
			}
			j++;
		}
		i++;
	}
	return true;
}
