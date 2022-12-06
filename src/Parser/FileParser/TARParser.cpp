#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
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

void Parser::FileParser::TARParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::TARParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UInt8 buff[512];
	UOSInt i;
	UInt64 currOfst;
	UInt64 fileSize;
	UInt64 itemSize;
	Int64 t;
	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".TAR")))
	{
		return 0;
	}
	fileSize = fd->GetDataSize();
	if (fileSize & 511)
	{
		return 0;
	}

	currOfst = 0;
	IO::PackageFile *pf;
	IO::PackageFile *pf2;
	IO::PackageFile *pf3;
	Text::StringBuilderUTF8 sb;
	Text::Encoding enc(this->codePage);
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (true)
	{
		if (currOfst >= fileSize)
		{
			DEL_CLASS(pf);
			return 0;
		}
		fd->GetRealData(currOfst, 512, buff);
		currOfst += 512;

		itemSize = (UInt64)Text::StrOct2Int64((Char*)&buff[124]);
		t = Text::StrOct2Int64((Char*)&buff[136]);
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
						pf3 = pf2->GetPackFile({sptr, i});
						if (pf3 == 0)
						{
							NEW_CLASS(pf3, IO::PackageFile(sb.ToCString()));
							pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(t * 1000LL, 0));
						}
						pf2 = pf3;
						sptr = &sptr[i + 1];
					}
					else 
					{
						if (sptr[0] != 0)
						{
							sb.AppendC(UTF8STRC("\\"));
							sb.AppendC(sptr, (UOSInt)(sptrEnd - sptr));
							pf3 = pf2->GetPackFile({sptr, (UOSInt)(sptrEnd - sptr)});
							if (pf3 == 0)
							{
								NEW_CLASS(pf3, IO::PackageFile(sb.ToCString()));
								pf2->AddPack(pf3, CSTRP(sptr, sptrEnd), Data::Timestamp(t * 1000LL, 0));
							}
						}
						break;
					}
				}
				continue;
			}
			DEL_CLASS(pf);
			return 0;
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
				pf3 = pf2->GetPackFile({sptr, i});
				if (pf3 == 0)
				{
					NEW_CLASS(pf3, IO::PackageFile(sb.ToCString()));
					pf2->AddPack(pf3, {sptr, i}, Data::Timestamp(t * 1000LL, 0));
				}
				pf2 = pf3;
				sptr = &sptr[i + 1];
			}
			else
			{
				break;
			}
		}
		pf2->AddData(fd, currOfst, itemSize, CSTRP(sptr, sptrEnd), Data::Timestamp(t * 1000LL, 0));
		if (itemSize & 511)
		{
			currOfst += itemSize + 512 - (itemSize & 511);
		}
		else
		{
			currOfst += itemSize;
		}
	}
	return 0;
}
