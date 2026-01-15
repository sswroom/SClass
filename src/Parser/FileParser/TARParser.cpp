#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/TARParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

Parser::FileParser::TARParser::TARParser()
{
	this->codePage = 65001;
}

Parser::FileParser::TARParser::~TARParser()
{
}

Int32 Parser::FileParser::TARParser::GetName()
{
	return *(Int32*)"TARP";
}

void Parser::FileParser::TARParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::TARParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.tar"), CSTR("Tar File"));
	}
}

IO::ParserType Parser::FileParser::TARParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::TARParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UInt8 buff[512];
	UIntOS i;
	UInt64 currOfst;
	UInt64 fileSize;
	UInt64 itemSize;
	Int64 t;
	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".TAR")))
	{
		return nullptr;
	}
	fileSize = fd->GetDataSize();
	if (fileSize & 511)
	{
		return nullptr;
	}

	currOfst = 0;
	IO::VirtualPackageFile *pf;
	IO::VirtualPackageFile *pf2;
	NN<IO::PackageFile> pf3;
	Text::StringBuilderUTF8 sb;
	Text::Encoding enc(this->codePage);
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));

	while (true)
	{
		if (currOfst >= fileSize)
		{
			DEL_CLASS(pf);
			return nullptr;
		}
		fd->GetRealData(currOfst, 512, BYTEARR(buff));
		currOfst += 512;

		itemSize = (UInt64)Text::StrOct2Int64(&buff[124]);
		t = Text::StrOct2Int64(&buff[136]);
		sptrEnd = enc.UTF8FromBytes(sbuff, buff, 100, 0);
		if (itemSize == 0)
		{
			if (buff[0] == 0)
				return pf;
			if (buff[156] == '5')
			{
				sptr = sbuff;
				pf2 = pf;
				sb.ClearStr();
				sb.Append(fd->GetFullName());
				while (true)
				{
					i = Text::StrIndexOfChar(sptr, '/');
					if (i != INVALID_INDEX)
					{
						sptr[i] = 0;
						sb.AppendC(UTF8STRC("\\"));
						sb.AppendC(sptr, i);
						if (!pf2->GetPackFile({sptr, i}).SetTo(pf3))
						{
							NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
							pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(t * 1000LL, 0), nullptr, nullptr, 0);
						}
						pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
						sptr = &sptr[i + 1];
					}
					else 
					{
						if (sptr[0] != 0)
						{
							sb.AppendC(UTF8STRC("\\"));
							sb.AppendC(sptr, (UIntOS)(sptrEnd - sptr));
							if (!pf2->GetPackFile({sptr, (UIntOS)(sptrEnd - sptr)}).SetTo(pf3))
							{
								NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
								pf2->AddPack(pf3, CSTRP(sptr, sptrEnd), Data::Timestamp(t * 1000LL, 0), nullptr, nullptr, 0);
							}
						}
						break;
					}
				}
				continue;
			}
			DEL_CLASS(pf);
			return nullptr;
		}
		pf2 = pf;
		sptr = sbuff;
		sb.ClearStr();
		sb.Append(fd->GetFullName());
		while (true)
		{
			i = Text::StrIndexOfChar(sptr, '/');
			if (i != INVALID_INDEX)
			{
				sptr[i] = 0;
				sb.AppendC(UTF8STRC("\\"));
				sb.AppendC(sptr, i);
				if (!pf2->GetPackFile({sptr, i}).SetTo(pf3))
				{
					NEW_CLASSNN(pf3, IO::VirtualPackageFileFast(sb.ToCString()));
					pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(t * 1000LL, 0), nullptr, nullptr, 0);
				}
				pf2 = (IO::VirtualPackageFile*)pf3.Ptr();
				sptr = &sptr[i + 1];
			}
			else
			{
				break;
			}
		}
		pf2->AddData(fd, currOfst, itemSize, IO::PackFileItem::HeaderType::No, CSTRP(sptr, sptrEnd), Data::Timestamp(t * 1000LL, 0), nullptr, nullptr, 0);
		if (itemSize & 511)
		{
			currOfst += itemSize + 512 - (itemSize & 511);
		}
		else
		{
			currOfst += itemSize;
		}
	}
	return nullptr;
}
