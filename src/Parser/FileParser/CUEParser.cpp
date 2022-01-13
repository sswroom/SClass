#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "IO/StmData/FileData.h"
#include "Media/MediaFile.h"
#include "Parser/FileParser/CUEParser.h"
#include "Parser/ParserList.h"

Parser::FileParser::CUEParser::CUEParser()
{
	this->parsers = 0;
}

Parser::FileParser::CUEParser::~CUEParser()
{
}

Int32 Parser::FileParser::CUEParser::GetName()
{
	return *(Int32*)"CUEP";
}

void Parser::FileParser::CUEParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::CUEParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter((const UTF8Char*)"*.cue", (const UTF8Char*)"Cuesheet File");
	}
}

IO::ParserType Parser::FileParser::CUEParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::CUEParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	Media::MediaFile *mf = 0;
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	UOSInt currTrack;
	UOSInt maxTrack = 0;
	const UTF8Char *fileName = 0;
	const UTF8Char *artists[100];
	const UTF8Char *titles[100];
	UInt32 stmTime[100];
	UInt32 lastTime;
	UOSInt i;
	Bool errorFound = false;
	if (!fd->GetFullName()->EndsWithICase(UTF8STRC(".CUE")))
		return 0;

	i = 100;
	while (i-- > 0)
	{
		artists[i] = 0;
		titles[i] = 0;
		stmTime[i] = 0;
	}
	currTrack = 0;

	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, 0));
	while (reader->ReadLine(sbuff, 511))
	{
		sptr = Text::StrTrim(sbuff);
		if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("PERFORMER ")))
		{
			ReadString(sbuff2, &sbuff[10]);
			if (artists[currTrack] != 0)
			{
				errorFound = true;
				break;
			}
			artists[currTrack] = Text::StrCopyNew(sbuff2);
		}
		else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("TITLE ")))
		{
			ReadString(sbuff2, &sbuff[6]);
			if (titles[currTrack] != 0)
			{
				errorFound = true;
				break;
			}
			titles[currTrack] = Text::StrCopyNew(sbuff2);
		}
		else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FILE ")))
		{
			ReadString(sbuff2, &sbuff[5]);
			if (fileName != 0)
			{
				errorFound = true;
				break;
			}
			fileName = Text::StrCopyNew(sbuff2);
		}
		else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("TRACK ")))
		{
			ReadString(sbuff2, &sbuff[6]);
			currTrack = Text::StrToUInt32(sbuff2);
			if(currTrack == 0)
			{
				errorFound = true;
				break;
			}
			if (currTrack > maxTrack)
			{
				maxTrack = currTrack;
			}
		}
		else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("INDEX ")))
		{
			ReadString(sbuff2, &sbuff[6]);
			i = Text::StrToUInt32(sbuff2);
			if (i == 1)
			{
				if (currTrack == 0)
				{
					errorFound = true;
					break;
				}
				else
				{
					stmTime[currTrack] = ReadTime(&sbuff[9]);
				}
			}
		}
	}

	if (!errorFound && fileName)
	{
		IO::StmData::FileData *data;
		IO::ParserType pt;
		IO::ParsedObject *pobj;

		fd->GetFullName()->ConcatTo(sbuff);
		IO::Path::AppendPath(sbuff, fileName);
		NEW_CLASS(data, IO::StmData::FileData(sbuff, false));
		pobj = this->parsers->ParseFile(data, &pt);
		DEL_CLASS(data);
		if (pobj)
		{
			if (pt == IO::ParserType::MediaFile)
			{
				mf = (Media::MediaFile*)pobj;
				Media::ChapterInfo *chapters;
				NEW_CLASS(chapters, Media::ChapterInfo());
				lastTime = 0;
				i = 1;
				while (i <= maxTrack)
				{
					if (lastTime > stmTime[i])
					{
						DEL_CLASS(mf);
						mf = 0;
						break;
					}
					sptr = sbuff;
					if (titles[i])
					{
						sptr = Text::StrConcat(sptr, titles[i]);
					}
					else
					{
						sptr = Text::StrInt32(Text::StrConcatC(sptr, UTF8STRC("Track ")), (Int32)i);
					}
					chapters->AddChapter(stmTime[i], sbuff, artists[i]?artists[i]:artists[0]);

					lastTime = stmTime[i];
					i++;
				}
				mf->SetChapterInfo(chapters, true);
			}
			else
			{
				DEL_CLASS(pobj);
			}
		}
	}
	SDEL_TEXT(fileName);
	i = 100;
	while (i-- > 0)
	{
		SDEL_TEXT(artists[i]);
		SDEL_TEXT(titles[i]);
	}


	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return mf;
}

UTF8Char *Parser::FileParser::CUEParser::ReadString(UTF8Char *sbuff, const UTF8Char *cueStr)
{
	Bool isQuote = false;
	const UTF8Char *sptr = cueStr;
	UTF8Char c;
	while (true)
	{
		c = *sptr++;
		if (c == '"')
		{
			if (!isQuote)
			{
				isQuote = true;
			}
			else if (*sptr == '"')
			{
				*sbuff++ = c;
				sptr++;
			}
			else
			{
				isQuote = false;
			}
		}
		else if (c == 0)
		{
			break;
		}
		else if (c == ' ' && !isQuote)
		{
			break;
		}
		else
		{
			*sbuff++ = c;
		}
	}
	*sbuff = 0;
	return sbuff;
}

UInt32 Parser::FileParser::CUEParser::ReadTime(const UTF8Char *timeStr)
{
	UTF8Char sbuff[10];
	UTF8Char *sarr[4];
	if (Text::StrConcatS(sbuff, timeStr, 10) - sbuff == 8)
	{
		if (Text::StrSplit(sarr, 4, sbuff, ':') == 3)
		{
			return Text::StrToUInt32(sarr[0]) * 60000 + Text::StrToUInt32(sarr[1]) * 1000 + (Text::StrToUInt32(sarr[2]) * 1000 / 75);
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
