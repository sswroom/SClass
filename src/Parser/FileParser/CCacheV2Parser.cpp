#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/VirtualPackageFileFast.h"
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
	if (!ParseAppend(fd, hdr, pkg, 0, 0))
	{
		pkg.Delete();
		return nullptr;
	}
	return pkg;
}

Bool Parser::FileParser::CCacheV2Parser::ParseAppend(NN<IO::StreamData> fd, Data::ByteArrayR hdr, NN<IO::VirtualPackageFile> pkgFile, UOSInt rOfst, UOSInt cOfst)
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
	Optional<IO::VirtualPackageFile> rowPkg;
	NN<IO::VirtualPackageFile> nnrowPkg;
	NN<IO::PackageFile> nnpkg;
	Data::Timestamp now = Data::Timestamp::Now();
	UInt64 idx;
	UInt64 tileOfst;
	UOSInt tileSize;
	UOSInt i = 0;
	UOSInt j;
	while (i < 128)
	{
		rowPkg = nullptr;
		j = 0;
		while (j < 128)
		{
			idx = ReadUInt64(&index[i * 1024 + j * 8]);
			tileOfst = idx % 0x10000000000LL;
			tileSize = (UOSInt)(idx / 0x10000000000LL);
			if (tileSize == 0)
				break;
			if (!rowPkg.SetTo(nnrowPkg))
			{
				sb.ClearStr();
				sb.AppendUOSInt(i + rOfst);
				if (!pkgFile->GetPackFile(sb.ToCString()).SetTo(nnpkg))
				{
					NEW_CLASSNN(nnrowPkg, IO::VirtualPackageFileFast(sb.ToCString()));
					rowPkg = nnrowPkg;
					pkgFile->AddPack(nnrowPkg, sb.ToCString(), now, now, now, 0);
				}
				else
				{
					nnrowPkg = NN<IO::VirtualPackageFile>::ConvertFrom(nnpkg);
					rowPkg = nnrowPkg;
				}
			}
			sb.ClearStr();
			sb.AppendUOSInt(j + cOfst);
			sb.Append(CSTR(".png"));
			nnrowPkg->AddData(fd, tileOfst, tileSize, IO::PackFileItem::HeaderType::No, sb.ToCString(), now, now, now, 0);
			j++;
		}
		if (j == 0)
			break;
		i++;
	}
	return true;
}
